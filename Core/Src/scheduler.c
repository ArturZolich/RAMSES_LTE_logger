
#include "UART_DMA.h"
#include "string.h"

#include <stdio.h>

#include "scheduler.h"

const uint8_t g_RAMSES_Query[] = { 0x23, 0x00, 0x00, 0x80, 0xB0, 0x00, 0x00, 0x01 };
const uint8_t g_RAMSES_Sample[] = { 0x23, 0x00, 0x00, 0x00, 0xA8, 0x00, 0x81, 0x01 };


void Send_LTE(uint8_t* cmd, uint16_t len){
	if (len == 0){ // lenghts of arrays of chars can be computed here
		len = strlen((char*)cmd);
	}
	//printf("%s, %d - SIZEOF\r\n", (char*)cmd, sizeof(cmd));
	//	strcat(cmd, "\r\n");
	//printf("%s, %d - SIZEOF\r\n", cmd, strlen(cmd));
	HAL_UART_Transmit(&huart2,cmd,len,250);
	//HAL_UART_Transmit(&huart2,(uint8_t*)"\r\n",2,10);
}

void Get_LTE_data(){
	UARTDMA_IsDataReady(&hLTE_uart2dma6);
}

void Clear_Array(uint8_t* array, unsigned int size) {
	for (int i = 0; i < size; i++) {
		array[i] = 0;
	}
}

enum State Do_Action_RAMSES(const command *cmd) {
	// DO RAMSES ACTION
	if (strcmp((char*)cmd->cmd, "RAMSES_1_2_SAMPLE") == 0)
	{
		HAL_UART_Transmit(&huart1, g_RAMSES_Sample, SAMPLE_CMD_SIZE, 10);
		g_RAM_1_timer = HAL_GetTick();
		HAL_UART_Transmit(&huart3, g_RAMSES_Sample, SAMPLE_CMD_SIZE, 10);
		g_RAM_2_timer = HAL_GetTick();
		return READY;
	} else if (strcmp((char*)cmd->cmd, "RAMSES_1_SAMPLE") == 0)
	{
		HAL_UART_Transmit(&huart1, g_RAMSES_Sample, SAMPLE_CMD_SIZE, 10);
		g_RAM_1_timer = HAL_GetTick();
		return READY;

	} else if (strcmp((char*)cmd->cmd, "RAMSES_2_SAMPLE") == 0)
	{
		HAL_UART_Transmit(&huart3, g_RAMSES_Sample, SAMPLE_CMD_SIZE, 10);
		g_RAM_2_timer = HAL_GetTick();
		return READY;

	} else if (strcmp((char*)cmd->cmd, "RAMSES_1_QUERY") == 0)
	{
		HAL_UART_Transmit(&huart1, g_RAMSES_Query, QUERY_CMD_SIZE, 10);
		g_RAM_1_timer = HAL_GetTick();
		return READY;
	} else if (strcmp((char*)cmd->cmd, "RAMSES_2_QUERY") == 0)
	{
		HAL_UART_Transmit(&huart3, g_RAMSES_Query, QUERY_CMD_SIZE, 10);
		g_RAM_2_timer = HAL_GetTick();
		return READY;
	}

	return READY;
	// if send, check if there's data, if not check if timout has passed, if yes, send timeout, if not send data, CHECKING_ANSWER can be the mode to send while waiting

}

enum State Do_Action(command cmd, uint8_t* uart_data){
	static enum State state = READY;
	static unsigned int retry;
	static int start_time;

	if(strstr((char*)uart_data, "RAMSES") != NULL){ // DO RAMSES ACTION
		state = Do_Action_RAMSES(&cmd);
		return state;

	}else{ // DO LTE ACTION


		if(strcmp((char*)cmd.cmd, "RESET_ACTION") == 0){
			Clear_Array(uart_data, BUFFER_SIZE);  // ZOLICH _ modified without test
			retry = 0;
			state = READY;
			return state;
		}

		if(state == READY){ 					// ready to send new command
			start_time = HAL_GetTick();		// save send time
			state = CHECKING_ANSWER;			// notify that now we wait for a reply
			retry = 0;
			Clear_Array(uart_data, BUFFER_SIZE);  // ZOLICH _ modified without test
			Send_LTE(cmd.cmd, cmd.length);				// send new command
			return state;

		}else if(state == CHECKING_ANSWER){	// if we expect a reply now

			if((start_time + cmd.timeout) < HAL_GetTick()){	// if we wait too long, report timeout
				state = TIMEOUT;
				return state;
			}

			if((strcmp((char*)cmd.good_answer, (char*)uart_data) == 0) || strcmp((char*)cmd.good_answer, "ANY") == 0){ 		// if the reply is OK, return success
				state = GOOD_ANSWER;
				return state;
			}else if(strcmp((char*)cmd.bad_answer, (char*)uart_data) == 0){	// if the reply suggest an error...
				state = BAD_ANSWER;
				if(retry < cmd.retry_counter){				// retry if structure suggest that behaviour
					retry++;
					Do_Action(cmd, uart_data);
				}else{										// if retries still give an error, or no replies suggested, report error
					retry = 0;
					state = BAD_ANSWER;
					return state;
				}
				return state;
			}
		}
	}

	return state;
}

#include "scheduler.h"


void Send_LTE(char* cmd){
	//printf("%s, %d - SIZEOF\r\n", (char*)cmd, sizeof(cmd));
	//	strcat(cmd, "\r\n");
	//printf("%s, %d - SIZEOF\r\n", cmd, strlen(cmd));
	HAL_UART_Transmit(&huart2,(uint8_t*)cmd,strlen(cmd),250);
	//HAL_UART_Transmit(&huart2,(uint8_t*)"\r\n",2,10);
}

void Get_LTE_data(){
	UARTDMA_IsDataReady(&huartdma6);
}


enum State Do_Action(command cmd, uint8_t* uart_data){
	static enum State state = READY;
	static unsigned int retry;
	static int start_time;

	if(strcmp(cmd.cmd, "FTP_ACTIVE") == 0){
		state = FTP_ACTIVE;
		retry = 0;
		for(int i = 0; i< BUFFER_SIZE; i++){
			uart_data[i] ='\0';
		}
		return state;
	}


	if(strcmp(cmd.cmd, "RESET_ACTION") == 0){
		state = READY;
		retry = 0;
		for(int i = 0; i< BUFFER_SIZE; i++){
			uart_data[i] ='\0';
		}
		return state;
	}

	if(state == READY){ 					// ready to send new command
		start_time = HAL_GetTick();		// save send time
		state = CHECKING_ANSWER;			// notify that now we wait for a reply
		Send_LTE(cmd.cmd);				// send new command
		return state;

	}else if(state == CHECKING_ANSWER){	// if we expect a reply now

		if((start_time + cmd.timeout) < HAL_GetTick()){	// if we wait too long, report timeout
			state = TIMEOUT;
			return state;
		}

		if((strcmp(cmd.good_answer, (char*)uart_data) == 0) || strcmp(cmd.good_answer, "ANY") == 0){ 		// if the reply is OK, return success
			state = GOOD_ANSWER;
			return state;
		}else if(strcmp(cmd.bad_answer, (char*)uart_data) == 0){	// if the reply suggest an error...
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

	return state;
}

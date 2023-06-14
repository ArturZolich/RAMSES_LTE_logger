/*
 * scheduler.h
 *
 *  Created on: May 19, 2023
 *      Author: arturz
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_




#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif




#define BUFFER_SIZE 1024

#define RAMSES_TIMEOUT 12000


#define QUERY_CMD_SIZE 8
#define SAMPLE_CMD_SIZE 8
#define SAMPLE_RADIANCE_CMD_SIZE 16


extern long g_RAM_1_timer;
extern long g_RAM_2_timer;

extern UART_HandleTypeDef huart2; // LTE
extern UARTDMA_HandleTypeDef hLTE_uart2dma6;

extern UART_HandleTypeDef huart1; // RAMSES 1
extern UARTDMA_HandleTypeDef hRAM_1_uart1dma5;

extern UART_HandleTypeDef huart3; // RAMSES 2
extern UARTDMA_HandleTypeDef hRAM_2_uart3dma3;


void Clear_Array(uint8_t* array, unsigned int size);

enum Action { RETRY, RESET_PROCESSOR, INIT, MOVE_ON };

enum State { READY, CHECKING_ANSWER, GOOD_ANSWER, BAD_ANSWER, TIMEOUT, FTP_ACTIVE, SEND_RAM_1, SEND_RAM_2 };


struct{				// structure to define available actions
	uint8_t* cmd;
	uint16_t length;
	uint8_t* good_answer;
	int timeout;
	uint8_t* bad_answer;
	enum Action act_on_error;
	int retry_counter;
} typedef command;


enum State Do_Action(command cmd, uint8_t* uart_data);

void Send_LTE(uint8_t* cmd, uint16_t len); // sends data to LTE
void Get_LTE_data();

#endif /* INC_SCHEDULER_H_ */

/*
 * scheduler.h
 *
 *  Created on: May 19, 2023
 *      Author: arturz
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_


#include "main.h"
#include "UART_DMA.h"
#include "string.h"

#include <stdio.h>

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif




#define BUFFER_SIZE 1024


extern UART_HandleTypeDef huart2;
extern UARTDMA_HandleTypeDef huartdma6;

enum Action { RETRY, RESET_PROCESSOR, INIT, MOVE_ON };

enum State { READY, CHECKING_ANSWER, GOOD_ANSWER, BAD_ANSWER, TIMEOUT, FTP_ACTIVE };


struct{
	char* cmd;
	char* good_answer;
	int timeout;
	char* bad_answer;
	enum Action act_on_error;
	int retry_counter;
} typedef command;


enum State Do_Action(command cmd, uint8_t* uart_data);

void Send_LTE(char* cmd);
void Get_LTE_data();

#endif /* INC_SCHEDULER_H_ */

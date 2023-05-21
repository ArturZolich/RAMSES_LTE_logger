/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "UART_DMA.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


UARTDMA_HandleTypeDef hLTE_uart2dma6;
UARTDMA_HandleTypeDef hRAM_1_uart1dma5;
UARTDMA_HandleTypeDef hRAM_2_uart3dma3;


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */


uint8_t g_LTE_ParseBuffer[BUFFER_SIZE];
uint8_t g_RAM_1_ParseBuffer[BUFFER_SIZE];
uint8_t g_RAM_2_ParseBuffer[BUFFER_SIZE];


uint8_t g_fileName[60] = "AT+FTPPUTNAME=\"unknown_";

uint8_t g_dataSize[20] = " AT+FTPPUT=2,0";

uint8_t g_dataSizeResponse[20] = "+FTPPUT=2,0";


uint8_t gps_data_latest[BUFFER_SIZE] = {};


uint8_t RAM_1_data_latest[BUFFER_SIZE] = {};
uint16_t g_RAM_1_data_size = 0;
uint8_t g_RAM_1_data_ready = 0;
uint8_t g_RAM_1_dataSize[20] = " AT+FTPPUT=2,0"; // buffer to prepare FTP for data
uint8_t g_RAM_1_dataSizeResponse[20] = "+FTPPUT=2,0";
int g_RAM_1_timer = 0;

uint8_t RAM_2_data_latest[BUFFER_SIZE] = {};
uint16_t g_RAM_2_data_size = 0;
uint8_t g_RAM_2_data_ready = 0;
uint8_t g_RAM_2_dataSize[20] = " AT+FTPPUT=2,0"; // buffer to prepare FTP for data
uint8_t g_RAM_2_dataSizeResponse[20] = "+FTPPUT=2,0";
int g_RAM_2_timer = 0;






command LTE_sim_check_active = {
		.cmd = (uint8_t*)"AT\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RETRY,
		.retry_counter = 3
};

command LTE_disable_echo = {
		.cmd = (uint8_t*)"ATE0\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RETRY,
		.retry_counter = 3
};



command LTE_deactivate_gprs = {
		.cmd = (uint8_t*)"AT+CIPSHUT\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"SHUT OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RETRY,
		.retry_counter = 3
};


command LTE_attach_gprs = {
		.cmd = (uint8_t*)"AT+CGATT=1\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RETRY,
		.retry_counter = 3
};

command LTE_set_apn = {
		.cmd = (uint8_t*)"AT+CSTT=\"internet\",\"\",\"\"\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 5000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RETRY,
		.retry_counter = 3
};

command LTE_gprs_up = {
		.cmd = (uint8_t*)"AT+CIICR\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 5000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RETRY,
		.retry_counter = 3
};

command LTE_get_ip = {
		.cmd = (uint8_t*)"AT+CIFSR\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"ANY",
		.timeout = 5000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RETRY,
		.retry_counter = 3
};


command LTE_init_gps = {
		.cmd = (uint8_t*)"AT+CGNSPWR=1\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RETRY,
		.retry_counter = 3
};

command LTE_get_position = {
		.cmd = (uint8_t*)"AT+CGNSINF\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RETRY,
		.retry_counter = 0
};


command LTE_ftp_quit = {
		.cmd = (uint8_t*)"AT+FTPQUIT\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"ANY",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};


command LTE_bearer_1 = {
		.cmd = (uint8_t*)"AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_bearer_2 = {
		.cmd = (uint8_t*)"AT+SAPBR=3,1,\"APN\",\"internet\"\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_bearer_3 = {
		.cmd = (uint8_t*)"AT+SAPBR=1,1\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_set_dns = {
		.cmd = (uint8_t*)"AT+CDNSCFG=\"8.8.8.8\",\"8.8.4.4\"\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_ftp_bearer = {
		.cmd = (uint8_t*)"AT+FTPCID=1\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_ftp_type_binary = {
		.cmd = (uint8_t*)"AT+FTPTYPE=\"I\"\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_ftp_append = {
		.cmd = (uint8_t*)"AT+FTPPUTOPT=\"APPE\"\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_ftp_ip = {
		.cmd = (uint8_t*)"AT+FTPSERV=\"188.210.221.82\"\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_ftp_user = {
		.cmd = (uint8_t*)"AT+FTPUN=\"ftp@unmanned.solutions\"\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_ftp_pass = {
		.cmd = (uint8_t*)"AT+FTPPW=\"cIzCm9jQ\"\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_ftp_port = {
		.cmd = (uint8_t*)"AT+FTPPORT=21\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};


command LTE_ftp_path = {
		.cmd = (uint8_t*)"AT+FTPPUTPATH=\"/\"\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 2000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_get_time = {
		.cmd = (uint8_t*)"AT+CCLK?\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 5000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_ftp_filename = {
		.cmd = g_fileName,
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 5000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = MOVE_ON,
		.retry_counter = 0
};

command LTE_ftp_start_put = {
		.cmd = (uint8_t*)"AT+FTPPUT=1\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"+FTPPUT: 1,1,1360",
		.timeout = 10000,
		.bad_answer = (uint8_t*)"+FTPPUT: 1,66",
		.act_on_error = RESET_PROCESSOR,
		.retry_counter = 0
};

command LTE_ftp_put_data_size = {
		.cmd = g_dataSize,
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = g_dataSizeResponse,
		.timeout = 4000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RESET_PROCESSOR,
		.retry_counter = 0
};

command LTE_ftp_put_data = {
		.cmd = gps_data_latest,
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 4000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RESET_PROCESSOR,
		.retry_counter = 0
};


command LTE_ftp_end_put = {
		.cmd = (uint8_t*)"AT+FTPPUT=2,0\r\n",
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = (uint8_t*)"OK",
		.timeout = 5000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RESET_PROCESSOR,
		.retry_counter = 0
};

// SIGNAL commands
command LTE_ftp_active = {
		.cmd = (uint8_t*)"FTP_ACTIVE"
};

command LTE_reset_action = {
		.cmd = (uint8_t*)"RESET_ACTION"
};


command RAM_1_query = {
		.cmd = (uint8_t*)"RAMSES_1_QUERY"
};


command RAM_1_sample = {
		.cmd = (uint8_t*)"RAMSES_1_SAMPLE"
};

command RAM_2_query = {
		.cmd = (uint8_t*)"RAMSES_2_QUERY"
};

command RAM_2_sample = {
		.cmd = (uint8_t*)"RAMSES_2_SAMPLE"
};

command RAM_1_2_sample = {
		.cmd = (uint8_t*)"RAMSES_1_2_SAMPLE"
};


command LTE_ftp_put_RAM_1_data_size = {
		.cmd = g_RAM_1_dataSize,
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = g_RAM_1_dataSizeResponse,
		.timeout = 4000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RESET_PROCESSOR,
		.retry_counter = 0
};

command LTE_ftp_put_RAM_2_data_size = {
		.cmd = g_dataSize,
		.length = 0, // zero indicate's it's a char. LTE_Send function handles it automatically
		.good_answer = g_RAM_2_dataSizeResponse,
		.timeout = 4000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RESET_PROCESSOR,
		.retry_counter = 0
};

command LTE_ftp_put_RAM_1_data = {  // sizeof will not work here
		.cmd = RAM_1_data_latest,
		.length = -1, // that's a bitfield
		.good_answer = (uint8_t*)"OK",
		.timeout = 4000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RESET_PROCESSOR,
		.retry_counter = 0
};

command LTE_ftp_put_RAM_2_data = { // sizeof will not work here
		.cmd = RAM_2_data_latest,
		.length = -1, // that's a bitfield
		.good_answer = (uint8_t*)"OK",
		.timeout = 4000,
		.bad_answer = (uint8_t*)"ERROR",
		.act_on_error = RESET_PROCESSOR,
		.retry_counter = 0
};




/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_UART4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */


	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	srand(time(NULL));

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART2_UART_Init();
	MX_UART4_Init();
	MX_USART1_UART_Init();
	MX_USART3_UART_Init();
	/* USER CODE BEGIN 2 */

	UARTDMA_Init(&hLTE_uart2dma6, &huart2); // receives LTE modem serial
	UARTDMA_Init(&hRAM_1_uart1dma5, &huart1); // receives RAMSES 1 serial
	UARTDMA_Init(&hRAM_2_uart3dma3, &huart3); // receives RAMSES 2 serial

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */


	char random[20]; 				// create an initial file name, in case LTE won't get time fix
	itoa(rand(), random, 10);
	strcat((char*)g_fileName, random);
	strcat((char*)g_fileName, ".txt\"\r\n");


	printf("\t\tFILE_RAND: %s\r\n", g_fileName);



	// query, sample, dataSize, data


	uint8_t current_action = 0; // currently executed action

	uint8_t filename_time_set = 0; // flag to see if random file name was updated with LTE time

	//uint8_t transfer_status = 0;

	command action_list[255]; // list of actions to perform

	uint8_t last_action = 34; // number of the last action we want to execute

	uint8_t startup = 0; // flag to see if we just reset the processor to check LTE modem - if it is On or Off

	int resolution = 0; // reduces amount of printf, helpful in case there's no delay between loops


	action_list[0] = LTE_sim_check_active;
	action_list[1] = LTE_disable_echo;
	action_list[2] = LTE_deactivate_gprs;
	action_list[3] = LTE_attach_gprs;
	action_list[4] = LTE_set_apn;
	action_list[5] = LTE_gprs_up;
	action_list[6] = LTE_get_ip;
	action_list[7] = LTE_init_gps;
	action_list[8] = LTE_reset_action;
	action_list[9] = LTE_bearer_1;
	action_list[10] = LTE_bearer_2;
	action_list[11] = LTE_bearer_3;
	action_list[12] = LTE_set_dns;

	// action_list[] = RAM_1_query;
	// action_list[] = RAM_2_query;

	// action_list[] = LTE_ftp_put_RAM_1_data_size;
	// action_list[] = LTE_ftp_put_RAM_2_data_size;

	// action_list[] = RAM_1_send_LTE;
	// action_list[] = RAM_2_send_LTE;

	// action_list[] = RAM_1_sample;
	// action_list[] = RAM_2_sample;

	// action_list[] = LTE_ftp_put_RAM_1_data_size;
	// action_list[] = LTE_ftp_put_RAM_2_data_size;

	// action_list[] = RAM_1_send_LTE;
	// action_list[] = RAM_2_send_LTE;

	action_list[13] = LTE_get_position;
	action_list[14] = LTE_get_time;
	action_list[15] = LTE_ftp_quit;
	action_list[16] = LTE_ftp_bearer;
	action_list[17] = LTE_ftp_type_binary;
	action_list[18] = LTE_ftp_append;
	action_list[19] = LTE_ftp_ip;
	action_list[20] = LTE_ftp_user;
	action_list[21] = LTE_ftp_pass;
	action_list[22] = LTE_ftp_port;
	action_list[23] = LTE_ftp_path;
	action_list[24] = LTE_ftp_filename;
	action_list[25] = LTE_ftp_start_put;
	action_list[26] = LTE_ftp_put_data_size;
	action_list[27] = LTE_ftp_put_data;
	action_list[28] = LTE_ftp_end_put;

	action_list[29] = LTE_get_position;
	action_list[30] = LTE_get_time;
	action_list[31] = LTE_ftp_start_put;
	action_list[32] = LTE_ftp_put_data_size;
	action_list[33] = LTE_ftp_put_data;
	action_list[34] = LTE_ftp_end_put;

	action_list[35] = LTE_reset_action;


	//uint8_t ftp_transfer_active = 0;


	Clear_Array(g_LTE_ParseBuffer, BUFFER_SIZE); // let's start with clean LTE buffer

	while (1)
	{

		if(current_action > last_action){
			Clear_Array(g_LTE_ParseBuffer, BUFFER_SIZE);
			current_action = 29;
		}


		if((resolution % 50) == 0){
			printf("TRYING: >>%s<<\r\n", action_list[current_action].cmd);
		}

		enum State state = Do_Action(action_list[current_action], g_LTE_ParseBuffer);
		resolution++;

		switch (state){
		case GOOD_ANSWER:
			printf("%s - %s\r\n", action_list[current_action].cmd, action_list[current_action].good_answer);
			printf("\tREPLY: >>%s<<\r\n", (char*)g_LTE_ParseBuffer);
			Do_Action(LTE_reset_action, g_LTE_ParseBuffer);
			current_action++;
			HAL_Delay(action_list[current_action].timeout / 2);
			startup = 5;
			resolution = 0;
			break;
		case READY:
			//printf("%s - READY\r\n", action_list[current_action].cmd);
			//printf("BUFFER: >>%s<<\r\n", (char*)LTE_ParseBuffer);
			current_action++;
			resolution = 0;
			break;
		case TIMEOUT:
			printf("%s - Timeout\r\n", action_list[current_action].cmd);
			if(startup < 2){
				printf("INIT\r\n");
				printf("TURN ON\r\n");
				HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_6, GPIO_PIN_SET);
				HAL_Delay(1500);
				HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_6, GPIO_PIN_RESET);
				HAL_Delay(5000);
				startup++;
			}
			Do_Action(LTE_reset_action, (uint8_t*)g_LTE_ParseBuffer);

			HAL_Delay(action_list[current_action].timeout);
			current_action=0;
			resolution = 0;
			break;
		case BAD_ANSWER:
			printf("%s - %s\r\n", action_list[current_action].cmd, action_list[current_action].bad_answer);
			Do_Action(LTE_reset_action, (uint8_t*)g_LTE_ParseBuffer);
			HAL_Delay(action_list[current_action].timeout);
			startup = 5;
			resolution = 0;
			break;
		case FTP_ACTIVE:

			break;
		default:
			break;
		};


		if(hRAM_1_uart1dma5.UartTransferCompleted == 1)
		{
			// remember about timeout


//			char TempChar;
//			int count = 0;
//			uint8_t* LinePointer = RAM_1_data_latest;
//
//			while(hRAM_1_uart1dma5.UartTransferCompleted != 0){
//				TempChar = UARTDMA_GetCharFromBuffer(&hRAM_1_uart1dma5);
//
//				*LinePointer = TempChar;
//				LinePointer++;
//				count++;
//			}
//
//			g_RAM_1_data_size = count;
//
//			char save[100] = "AT+FTPPUT=2,";
//			char len[5];
//			itoa(g_RAM_1_data_size, len, 10);
//			strcat(save, len);
//			strcat(save, "\r\n");
//
//			strcpy((char*)g_RAM_1_dataSize, save);
//
//			char save2[100] = "+FTPPUT: 2,";
//			strcat(save2, len);
//
//			strcpy((char*)g_RAM_1_dataSizeResponse, save2);
//
//			g_RAM_1_data_ready = 1;
		}

		if(hRAM_2_uart3dma3.UartTransferCompleted)
		{

		}

		if(UARTDMA_IsDataReady(&hLTE_uart2dma6))
		{
			UARTDMA_GetLineFromBuffer(&hLTE_uart2dma6, (char*)g_LTE_ParseBuffer);
			printf("\tBUFFER: >>%s<<\r\n", (char*)g_LTE_ParseBuffer);

			// if message is time, update FTP filename
			char* ptr;
			if((ptr = strstr((char*)g_LTE_ParseBuffer, "+CCLK: ")) != NULL){
				printf("\t\tTIME: %s\r\n", (char*)g_LTE_ParseBuffer);

				strcat((char*)gps_data_latest,(char*)g_LTE_ParseBuffer); // append GPS data with LTE time
				//strcat(gps_data_latest,"\r\n");

				printf("GPS BUFFER AT TIME: %s\r\n", gps_data_latest);

				if(filename_time_set == 0){ // it's the first time I receive, and I will set FTP file name based on it


					filename_time_set = 1; // do not repeat

					char* timeString = ptr+8;

					for (int i = 0; i < strlen(timeString); i++)
					{
						if ((timeString[i] > '9') || (timeString[i] < '0')){
							timeString[i] = '_';
						}
						if(i>19){
							timeString[i] = '\0';
						}
					}

					char fileNameLocal[60] = "AT+FTPPUTNAME=\"";
					strcat(fileNameLocal, timeString);
					strcat(fileNameLocal, ".txt\"\r\n");


					printf("\tFILENAME_TIME_CHANGE: %s\r\n", fileNameLocal);

					strcpy((char*)g_fileName, fileNameLocal);

				}

				char save[100] = "AT+FTPPUT=2,";
				char len[5];
				itoa(strlen((char*)gps_data_latest), len, 10);
				strcat(save, len);
				strcat(save, "\r\n");

				strcpy((char*)g_dataSize, save);

				char save2[100] = "+FTPPUT: 2,";
				strcat(save2, len);

				strcpy((char*)g_dataSizeResponse, save2);

				printf("\tASSERT_EXPECTED_REPLY: %s\r\n", g_dataSizeResponse);


				//				// LTE_ftp_put_data referst to gps_data_latest already
				//				char* tmp2 = (char*) calloc(strlen(gps_data_latest), sizeof(char));
				//				if(tmp2 != NULL){
				//					strcpy(tmp2, gps_data_latest);
				//					free(LTE_ftp_put_data.cmd);
				//					LTE_ftp_put_data.cmd = tmp2;
				//					printf("FTP SAVE DATA: %s\r\n", LTE_ftp_put_data.cmd);
				//				}else{
				//					printf("\t\t\t\tCALLOC_FAIL_DATA");
				//				}


			}else if((ptr = strstr((char*)g_LTE_ParseBuffer, "+FTPPUT: 1,1,")) != NULL){ // else, if message is FTP server ready for data response
				//ftp_transfer_active = 1;
				//transfer_status++;
				printf("FTP #: %d\r\n", atoi(ptr+13));
			}else if((ptr = strstr((char*)g_LTE_ParseBuffer, "+CGNSINF")) != NULL){ // save GPS data
				//ftp_transfer_active = 0;
				//transfer_status = 0; // I receive new GPS data, which means I don't transmit

				Clear_Array(gps_data_latest, BUFFER_SIZE); // clean old entries

				gps_data_latest[0] = '#'; // preamble
				for(int i=0; i<strlen((char*)g_LTE_ParseBuffer)-1; i++){ // -1 because I added preamble
					gps_data_latest[i+1] = g_LTE_ParseBuffer[i];
				}

				//char fake[BUFFER_SIZE] = "+CGNSINF: 5109.0262308,N,11401.8407342,203522.00,A,5109.0262308,N,11401.8407342,W,0.004,133.4,130522,0.0,E,D*2B";
				//				for(int i=0; i<strlen(fake); i++){
				//					gps_data_latest[i] = fake[i];
				//				}

				printf("GPS DATA LOCKED: %s\r\n", gps_data_latest);

			}

		}else{
			Clear_Array(g_LTE_ParseBuffer, BUFFER_SIZE);
		}




		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief UART4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART4_Init(void)
{

	/* USER CODE BEGIN UART4_Init 0 */

	/* USER CODE END UART4_Init 0 */

	/* USER CODE BEGIN UART4_Init 1 */

	/* USER CODE END UART4_Init 1 */
	huart4.Instance = UART4;
	huart4.Init.BaudRate = 115200;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart4) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN UART4_Init 2 */

	/* USER CODE END UART4_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 57600;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void)
{

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
	/* DMA1_Channel5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
	/* DMA1_Channel6_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_6, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LD2_Pin PA6 */
	GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

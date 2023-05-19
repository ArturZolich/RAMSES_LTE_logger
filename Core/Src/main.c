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


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
UARTDMA_HandleTypeDef huartdma6;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
char ParseBuffer[BUFFER_SIZE];
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

	srand(time(NULL));

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART2_UART_Init();
	MX_UART4_Init();
	MX_USART1_UART_Init();
	MX_USART3_UART_Init();
	/* USER CODE BEGIN 2 */

	UARTDMA_Init(&huartdma6, &huart2);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */


	command LTE_sim_check_active = {
			.cmd = "AT",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = RETRY,
			.retry_counter = 3
	};


	command LTE_deactivate_gprs = {
			.cmd = "AT+CIPSHUT",
			.good_answer = "SHUT OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = RETRY,
			.retry_counter = 3
	};


	command LTE_attach_gprs = {
			.cmd = "AT+CGATT=1",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = RETRY,
			.retry_counter = 3
	};

	command LTE_set_apn = {
			.cmd = "AT+CSTT=\"internet\",\"\",\"\"",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = RETRY,
			.retry_counter = 3
	};

	command LTE_gprs_up = {
			.cmd = "AT+CIICR",
			.good_answer = "OK",
			.timeout = 5000,
			.bad_answer = "ERROR",
			.act_on_error = RETRY,
			.retry_counter = 3
	};

	command LTE_get_ip = {
			.cmd = "AT+CIFSR",
			.good_answer = "AT+CIFSR",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = RETRY,
			.retry_counter = 3
	};


	command LTE_init_gps = {
			.cmd = "AT+CGNSPWR=1",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = RETRY,
			.retry_counter = 3
	};

	command LTE_get_position = {
			.cmd = "AT+CGNSINF",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};


	command LTE_ftp_quit = {
			.cmd = "AT+FTPQUIT",
			.good_answer = "ANY",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};


	command LTE_bearer_1 = {
			.cmd = "AT+SAPBR=3,1,\"Contype\",\"GPRS\"",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};

	command LTE_bearer_2 = {
			.cmd = "AT+SAPBR=3,1,\"APN\",\"internet\"",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};

	command LTE_bearer_3 = {
			.cmd = "AT+SAPBR=1,1",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};

	command LTE_set_dns = {
			.cmd = "AT+CDNSCFG=\"8.8.8.8\",\"8.8.4.4\"",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};

	command LTE_ftp_bearer = {
			.cmd = "AT+FTPCID=1",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};

	command LTE_ftp_type = {
			.cmd = "AT+FTPTYPE=\"A\"",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};

	command LTE_ftp_ip = {
			.cmd = "AT+FTPSERV=\"188.210.221.82\"",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};

	command LTE_ftp_user = {
			.cmd = "AT+FTPUN=\"ftp@unmanned.solutions\"",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};

	command LTE_ftp_pass = {
			.cmd = "AT+FTPPW=\"cIzCm9jQ\"",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};

	command LTE_ftp_port = {
			.cmd = "AT+FTPPORT=21",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};


	command LTE_ftp_path = {
			.cmd = "AT+FTPPUTPATH=\"/\"",
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};

	command LTE_get_time = {
			.cmd = "AT+CCLK?",
			.good_answer = "ANY",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};

	char fileName[60] = "AT+FTPPUTNAME=\"unknown_";
	char random[20];
	itoa(rand(), random, 10);
	strcat(fileName, random);
	strcat(fileName, ".txt\"");


	printf("\t\tFILE_RAND: %s\r\n", fileName);

	command LTE_ftp_filename = {
			.cmd = fileName,
			.good_answer = "OK",
			.timeout = 2000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};

	command LTE_ftp_put = {
			.cmd = "AT+FTPPUT=1",
			.good_answer = "+FTPPUT",
			.timeout = 3000,
			.bad_answer = "ERROR",
			.act_on_error = MOVE_ON,
			.retry_counter = 0
	};



	command LTE_reset_action = {
			.cmd = "RESET_ACTION"
	};

	uint8_t current_action = 0;

	command action_list[255];


	uint8_t last_action = 23;

	action_list[0] = LTE_sim_check_active;
	action_list[1] = LTE_deactivate_gprs;
	//action_list[1] = LTE_get_time;
	action_list[2] = LTE_attach_gprs;
	action_list[3] = LTE_set_apn;
	action_list[4] = LTE_gprs_up;
	action_list[5] = LTE_get_ip;
	action_list[6] = LTE_init_gps;
	action_list[7] = LTE_reset_action;
	action_list[8] = LTE_get_position;
	action_list[9] = LTE_ftp_quit;
	action_list[10] = LTE_bearer_1;
	action_list[11] = LTE_bearer_2;
	action_list[12] = LTE_bearer_3;
	action_list[13] = LTE_set_dns;
	action_list[14] = LTE_ftp_bearer;
	action_list[15] = LTE_ftp_type;
	action_list[16] = LTE_ftp_ip;
	action_list[17] = LTE_ftp_user;
	action_list[18] = LTE_ftp_pass;
	action_list[19] = LTE_ftp_port;
	action_list[20] = LTE_ftp_path;
	action_list[21] = LTE_get_time;
	action_list[22] = LTE_ftp_filename;
	action_list[23] = LTE_ftp_put;




	uint8_t startup = 0;

	for(int i = 0; i< BUFFER_SIZE; i++){
		ParseBuffer[i] ='\0';
	}

	while (1)
	{

		if(current_action >= last_action){
			current_action = last_action;
		}


		enum State state = Do_Action(action_list[current_action], (uint8_t*)ParseBuffer);

		switch (state){
		case GOOD_ANSWER:
			printf("%s - %s\r\n", action_list[current_action].cmd, action_list[current_action].good_answer);
			printf("\tREPLY: >>%s<<\r\n", (char*)ParseBuffer);
			Do_Action(LTE_reset_action, (uint8_t*)ParseBuffer);
			current_action++;
			HAL_Delay(1000);
			startup = 5;
			break;
		case READY:
			//printf("%s - READY\r\n", action_list[current_action].cmd);
			//printf("BUFFER: >>%s<<\r\n", (char*)ParseBuffer);
			current_action++;
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
			Do_Action(LTE_reset_action, (uint8_t*)ParseBuffer);

			HAL_Delay(1000);
			current_action=0;
			break;
		case BAD_ANSWER:
			printf("%s - %s\r\n", action_list[current_action].cmd, action_list[current_action].bad_answer);
			Do_Action(LTE_reset_action, (uint8_t*)ParseBuffer);
			HAL_Delay(1000);
			startup = 5;
			break;
		default:
			break;
		};

		if(UARTDMA_IsDataReady(&huartdma6))
		{
			UARTDMA_GetLineFromBuffer(&huartdma6, ParseBuffer);
			printf("BUFFER: >>%s<<\r\n", (char*)ParseBuffer);

			char* ptr;
			if((ptr = strstr((char*)ParseBuffer, "+CCLK: ")) != NULL){
				printf("\t\tTIME: %s\r\n", (char*)ParseBuffer);

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
				strcat(fileNameLocal, ".txt\"");

				strcpy(LTE_ftp_filename.cmd, fileNameLocal);

				printf("\t\tTIME_F: %s\r\n", fileNameLocal);

			}

		}else{
			for(int i = 0; i< BUFFER_SIZE; i++){
				ParseBuffer[i] ='\0';
			}
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

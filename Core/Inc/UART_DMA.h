/*
 * UART_DMA.h
 *
 *  Created on: May 17, 2023
 *      Author: arturz
 */

#ifndef INC_UART_DMA_H_
#define INC_UART_DMA_H_


/*
 * UART_DMA.h
 *
 *  Created on: 09.12.2019
 *      Author: Mateusz Salamon
 *		www.msalamon.pl
 *
 *      Website: https://msalamon.pl/odbieranie-uart-z-dma-na-f103-to-rowniez-jest-proste/
 *      GitHub:  https://github.com/lamik/UART_DMA_receiving_F103
 *      Contact: mateusz@msalamon.pl
*/
#include "main.h"

#define DMA_RX_BUFFER_SIZE          1024
#define UART_BUFFER_SIZE            2048

typedef struct
{
	UART_HandleTypeDef* huart;					// UART handler

	uint8_t DMA_RX_Buffer[DMA_RX_BUFFER_SIZE];	// DMA direct buffer
	uint8_t UART_Buffer[UART_BUFFER_SIZE];		// UART working circular buffer

	uint16_t UartBufferHead;
	uint16_t UartBufferTail;
	uint8_t  UartBufferLines;

	uint8_t UartTransferCompleted;
}UARTDMA_HandleTypeDef;

void UARTDMA_UartIrqHandler(UARTDMA_HandleTypeDef *huartdma);
void UARTDMA_DmaIrqHandler(UARTDMA_HandleTypeDef *huartdma);

uint8_t UARTDMA_IsDataReady(UARTDMA_HandleTypeDef *huartdma);
int UARTDMA_GetLineFromBuffer(UARTDMA_HandleTypeDef *huartdma, char *OutBuffer);

int UARTDMA_GetCharFromBuffer(UARTDMA_HandleTypeDef *huartdma);

void UARTDMA_Init(UARTDMA_HandleTypeDef *huartdma, UART_HandleTypeDef *huart);

#endif /* INC_UART_DMA_H_ */

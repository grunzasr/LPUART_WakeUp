/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "tx_api.h"
#include "console.h"
#include "main.h"


#include <stdio.h>	// for snprintf() prototype
#include <string.h>	// for memset() prototype

// Message being received
uint8_t         message[ MAX_MSG_LEN ];
uint16_t        msg_len;

// Circular input buffers
CHAR_BUFFER_T   rxData;

// Circular output buffer
CHAR_BUFFER_T	txData;

// Defined in main.c
extern char		DBG_msg_buf[];

// Byte received
uint8_t         rx_byte;

// Command Q to console task
QUEUE_T		*cmdQ;

/* USER CODE END 0 */

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart1;

/* LPUART1 init function */

void MX_LPUART1_UART_Init(void)
{

	/* USER CODE BEGIN LPUART1_Init 0 */

	/* USER CODE END LPUART1_Init 0 */

	/* USER CODE BEGIN LPUART1_Init 1 */

	/* USER CODE END LPUART1_Init 1 */
	hlpuart1.Instance = LPUART1;
	hlpuart1.Init.BaudRate = 115200;
	hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
	hlpuart1.Init.StopBits = UART_STOPBITS_1;
	hlpuart1.Init.Parity = UART_PARITY_NONE;
	hlpuart1.Init.Mode = UART_MODE_TX_RX;
	hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
	if (HAL_UART_Init(&hlpuart1) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN LPUART1_Init 2 */

	/* USER CODE END LPUART1_Init 2 */

}
/* USART1 init function */

void MX_USART1_UART_Init(void)
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
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	if(uartHandle->Instance==LPUART1)
	{
		/* USER CODE BEGIN LPUART1_MspInit 0 */

		/* USER CODE END LPUART1_MspInit 0 */

		/** Initializes the peripherals clock
		 */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
		PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_HSI;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Error_Handler();
		}

		/* LPUART1 clock enable */
		__HAL_RCC_LPUART1_CLK_ENABLE();

		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**LPUART1 GPIO Configuration
    PC0     ------> LPUART1_RX
    PB11     ------> LPUART1_TX
		 */
		GPIO_InitStruct.Pin = CLI_TO_MICRO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
		HAL_GPIO_Init(CLI_TO_MICRO_GPIO_Port, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = CLI_FROM_MICRO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
		HAL_GPIO_Init(CLI_FROM_MICRO_GPIO_Port, &GPIO_InitStruct);

		/* LPUART1 interrupt Init */
		HAL_NVIC_SetPriority(LPUART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(LPUART1_IRQn);
		/* USER CODE BEGIN LPUART1_MspInit 1 */

		/* USER CODE END LPUART1_MspInit 1 */
	}
	else if(uartHandle->Instance==USART1)
	{
		/* USER CODE BEGIN USART1_MspInit 0 */

		/* USER CODE END USART1_MspInit 0 */

		/** Initializes the peripherals clock
		 */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
		PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Error_Handler();
		}

		/* USART1 clock enable */
		__HAL_RCC_USART1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
		 */
		GPIO_InitStruct.Pin = USART1_TX_Pin|USART1_RX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USER CODE BEGIN USART1_MspInit 1 */

		/* USER CODE END USART1_MspInit 1 */
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

	if(uartHandle->Instance==LPUART1)
	{
		/* USER CODE BEGIN LPUART1_MspDeInit 0 */

		/* USER CODE END LPUART1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_LPUART1_CLK_DISABLE();

		/**LPUART1 GPIO Configuration
    PC0     ------> LPUART1_RX
    PB11     ------> LPUART1_TX
		 */
		HAL_GPIO_DeInit(CLI_TO_MICRO_GPIO_Port, CLI_TO_MICRO_Pin);

		HAL_GPIO_DeInit(CLI_FROM_MICRO_GPIO_Port, CLI_FROM_MICRO_Pin);

		/* LPUART1 interrupt Deinit */
		HAL_NVIC_DisableIRQ(LPUART1_IRQn);
		/* USER CODE BEGIN LPUART1_MspDeInit 1 */

		/* USER CODE END LPUART1_MspDeInit 1 */
	}
	else if(uartHandle->Instance==USART1)
	{
		/* USER CODE BEGIN USART1_MspDeInit 0 */

		/* USER CODE END USART1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART1_CLK_DISABLE();

		/**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
		 */
		HAL_GPIO_DeInit(GPIOA, USART1_TX_Pin|USART1_RX_Pin);

		/* USER CODE BEGIN USART1_MspDeInit 1 */

		/* USER CODE END USART1_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */

//*********************************************************************
// Remove the last entry.  Useful for BackSpace handling
//
// Return values:
//      1 = success
//      0 = failure
//*********************************************************************
int8_t
charBufferRemoveLastOne(
		CHAR_BUFFER_T     *buffer )
{

	int8_t               result;

	if( buffer == NULL )
	{
		result = 0;
	}
	else
	{
		// Delete the received character
		buffer->data[ buffer->head ] = '\0';

		// Fix the index
		if( buffer->head > 0 )
		{
			buffer->head = buffer->head - 1;
		}
		else
		{
			// The character to be deleted is at the end of the buffer
			buffer->head = MAX_RX_RING_SZ - 1;
		}
		result = 1;
	}

	return( result );
}  // end of charBufferRemoveLastOne()

//*********************************************************************
// Put the given data in the buffer structure
//
// Return values:
//      1 = success
//      0 = failure
//*********************************************************************
int8_t
charBufferPushOne(
		CHAR_BUFFER_T     *buffer,
		uint8_t           *data)
{

	int8_t               ok;
	uint16_t             nextIndex;

	ok = 1;

	if( buffer == NULL )
	{
		ok = 0;
	}

	if( data  == NULL )
	{
		ok = 0;
	}

	nextIndex = (buffer->head + 1) % MAX_RX_RING_SZ;
	if( nextIndex == buffer->tail )
	{
		// no space left;
		ok = 0;

		// process some of the data in here
		buffer->newData = 1;
	}

	if( ok )
	{
		buffer->data[buffer->head] = *data;
		buffer->head = nextIndex;
		buffer->newData = 1;
	}

	return( ok );
} // end of charBufferPushOne()

//*********************************************************************
// Put the given data in the buffer structure
//
// Return values:
//      1 = success
//      0 = failure
//*********************************************************************
int8_t
charBufferPush(
		CHAR_BUFFER_T     *buffer,
		uint8_t           *data,
		uint8_t           len )
{

	int8_t               ok;

	ok = 1;
	if( buffer == NULL )
	{
		ok = 0;
	}

	if( data  == NULL )
	{
		ok = 0;
	}

	if( len == 0 )
	{
		ok = 0;
	}

	do
	{
		ok = charBufferPushOne( buffer, data++);
		len--;
	} while( ok && (len > 0) );

	return( ok );
} // end of charBufferPush()

//*********************************************************************
// Get a byte from the buffer structure
//
// Return values:
//      1 = success
//      0 = failure
//*********************************************************************
int8_t
charBufferPopOne(
		CHAR_BUFFER_T     *buffer,
		uint8_t           *data )
{

	int8_t               ok;
	uint8_t              charsAvailable;

	ok = 1;
	if( buffer == NULL )
	{
		ok = 0;
	}

	if( data  == NULL )
	{
		ok = 0;
	}

	if( buffer->tail == buffer->head )
	{
		charsAvailable = 0;
	}
	else
	{
		charsAvailable = 1;
	}

	if( charsAvailable )
	{
		*data = buffer->data[buffer->tail];
		buffer->tail = (buffer->tail + 1) % MAX_RX_RING_SZ;
	}

	return( ok );
} // end of charBufferPopOne()

//*********************************************************************
// Check if given charBuffer is empty
// 1 = empty
// 0 = not empty
//*********************************************************************
uint8_t
charBufferEmpty(
		CHAR_BUFFER_T     *buffer )
{
	uint8_t              result;

	if( buffer->tail == buffer->head )
	{
		result = 1;
	}
	else
	{
		result = 0;
	}

	return result;
} // end of charBufferEmpty()


//*********************************************************************
// This is the UART task.  It takes data received at interrupt
// level and processes it at user level.
//*********************************************************************
void
uartTask(
		ULONG   pvParameters )
{

	CHAR_BUFFER_T        *rcvBuf_p;

	rcvBuf_p = &rxData;

	uint8_t      c;
	char		*p;
	char         str[ MAX_MSG_LEN ];

	// Start the receiver
	HAL_UART_Receive_IT( &CONSOLE_DEVICE, &rx_byte, 1);

	// Initialize message reception buffer
	msg_len = 0;
	memset( message, 0, MAX_MSG_LEN );

	if( cmdQ == NULL )
	{
		cmdQ = consoleGetCmdQ();
	}

	while( 1 )
	{
		if( !charBufferEmpty( &txData ) )
		{
			p = str;
			c = 0;
			while( !charBufferEmpty( &txData ) )
			{
				charBufferPopOne( &txData, (uint8_t *) p );
				p++;
				c++;
			}

			// Send to console device
			HAL_UART_Transmit( &CONSOLE_DEVICE,
					(uint8_t *)str,
					c,
					HAL_MAX_DELAY);
		}

		if( rcvBuf_p->newData )
		{
			// There's new data to be processed
			do
			{
				charBufferPopOne( rcvBuf_p, &c );
				if( c == '\r' )
				{
					// User pressed enter
					if( msg_len == 0 )
					{
						// on a blank line
						// Just print a blank line on console

						snprintf( str, MAX_MSG_LEN, "\r\n> " );

						// Blocking transmit
						//
						// need blocking since the storage for str is only
						// ...allocated during this function
						HAL_UART_Transmit( &CONSOLE_DEVICE,
								(uint8_t *) str,
								strlen(str),
								HAL_MAX_DELAY );

					}
					else
					{
						// Send message to command interpreter
						//process_msg( (char *)message );
						if( cmdQ )
						{
							//                     xQueueGenericSend(
							//                        cmdQ,
							//                        message,
							//                        (100 * portTICK_PERIOD_MS),     // 100 ms
							//                        queueSEND_TO_BACK );
							fifo_push( cmdQ, (char *)message );

						}
						msg_len = 0;
						memset( message, 0, MAX_MSG_LEN );
					}
				}
				else
				{
					// another character of the message
					message[msg_len++] = c;
					if( msg_len > MAX_MSG_LEN )
					{
						// TODO : deal with too long of a message
						msg_len = MAX_MSG_LEN - 1;
						message[ msg_len ] = '\0'; // Null termination
					}
				}
			} while( !charBufferEmpty( rcvBuf_p ));
			rcvBuf_p->newData = 0;
		}
		else
		{
			// Nothing to do so sleep
#ifdef ENABLE_SLEEPING
			tx_thread_sleep( 10 );
#endif
		}
	} // while(1)


	return;
} // end of uartTask()

//*********************************************************************
void
vSerialPutString(
		char              *msg )
{

	uint16_t		len;
	HAL_StatusTypeDef	status;
	uint8_t		loops = 5;


	len = strlen( msg );


	while( loops-- > 0 )
	{
		printf( "%s", msg );
		status = HAL_UART_Transmit( &CONSOLE_DEVICE,
				(uint8_t *) msg,
				len,
				HAL_MAX_DELAY );
		if( status == HAL_OK )
		{
			loops = 0;
		}
		else
		{
#ifdef ENABLE_SLEEPING
			tx_thread_sleep( 10 );
#endif
		}
	}

	return;
}

//*********************************************************************
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		// USART1
	}
	else if (huart->Instance == USART2  )
	{
		//  USART2
	}
	else if (huart->Instance == LPUART1 )
	{
		// LPUART1

		uint8_t             storeChar;

		// Default to saving the character
		storeChar = 1;

		// Handle Backspace
		if( rx_byte == '\b' )
		{
			// Do not store and send backspace, space, backspace sequence
			storeChar = 0;
			charBufferRemoveLastOne( &rxData );
			// Blocking transmit
			HAL_UART_Transmit( &CONSOLE_DEVICE,
					(uint8_t *)"\b \b",
					3,
					HAL_MAX_DELAY );
		}

		if( (huart->ErrorCode & HAL_UART_ERROR_FE) == HAL_UART_ERROR_FE )
		{
			// Framing error probably from a received BREAK
			storeChar = 0;
		}

		if( storeChar )
		{
			// Push onto receive FIFO
			charBufferPushOne( &rxData, &rx_byte );

			// Send character back out to console for echo-back
			charBufferPushOne( &txData, &rx_byte );
		}

		/* Receive the next byte */
		HAL_UART_Receive_IT( &CONSOLE_DEVICE, &rx_byte, 1);
	}
	else if (huart->Instance == USART3)
	{
		// USART3
	}
	else if (huart->Instance == UART4)
	{
		// UART4
	}

	return;
} // end of HAL_UART_RxCpltCallback()


/* USER CODE END 1 */

/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.h
 * @brief   This file contains all the function prototypes for
 *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef hlpuart1;

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */


   // Size of the receiver ring buffer for the console
#define MAX_RX_RING_SZ    128

   typedef struct CHAR_BUFFER_T
   {
      uint16_t             head;   // add to the head
      uint16_t             tail;   // read brings tail up to head
      uint8_t              newData;        // 1 = new data written
      uint8_t              data[ MAX_RX_RING_SZ ];
   } CHAR_BUFFER_T;



/* USER CODE END Private defines */

void MX_LPUART1_UART_Init(void);
void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
   int8_t
   charBufferPush(
	    CHAR_BUFFER_T     *buffer,
	    uint8_t           *data,
	    uint8_t           len );

   int8_t
   charBufferPushOne(
	    CHAR_BUFFER_T     *buffer,
	    uint8_t           *data);

   uint8_t
   charBufferEmpty(
	    CHAR_BUFFER_T     *buffer );

   int8_t
   charBufferPopOne(
	    CHAR_BUFFER_T     *buffer,
	    uint8_t           *data );

   void vSerialPutString( char * );

   int8_t
   charBufferRemoveLastOne(
	    CHAR_BUFFER_T     *buffer );

   void
   uartTask(
	    ULONG		pvParameters );


/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */


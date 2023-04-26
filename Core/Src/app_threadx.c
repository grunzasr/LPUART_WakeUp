/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_threadx.c
 * @author  MCD Application Team
 * @brief   ThreadX applicative file
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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include "usart.h"
#include "console.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

// Pointer to the command queue
extern TX_QUEUE		*consoleCmdQ;

TX_THREAD		consoleThread;
TX_THREAD		uartThread;

uint8_t			consoleStack[ CONSOLE_APP_STACK_SZ ];
uint8_t			uartThreadStack[ APP_STACK_SIZE ];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

void SystemClock_Restore(void);

/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

   /* USER CODE BEGIN App_ThreadX_MEM_POOL */
   (void)byte_pool;
  /* USER CODE END App_ThreadX_MEM_POOL */

  /* USER CODE BEGIN App_ThreadX_Init */


   const ULONG		timeSlice = 20;

   // Create the console thread

   // Create the actual thread
   ret = tx_thread_create( &consoleThread,			// Thread control block
		   "Console Thread",		// Thread name
		   consoleTask,			// Thread entry point
		   0,						// 32-bit input to thread
		   consoleStack,				// pointer to stack
		   CONSOLE_APP_STACK_SZ,	// stack size in bytes
		   THREAD_PRIO,			// priority
		   THREAD_PREEMPTION_THRESHOLD,	// preemption threshold
		   timeSlice,				// thread time-slice value
		   TX_AUTO_START);			// automatic start selection
   if( ret != TX_SUCCESS )
   {
	   return( ret );
   }

   // Start the UART thread to handle character reception

   // Create the actual thread
   ret = tx_thread_create( &uartThread,		// Thread control block
			   "UART Thread",		// Thread name
			   uartTask,			// Thread entry point
			   0,					// 32-bit input to thread
			   uartThreadStack,			// pointer to stack
			   APP_STACK_SIZE,		// stack size in bytes
			   THREAD_PRIO,		// priority
			   THREAD_PREEMPTION_THRESHOLD,	// preemption threshold
			   timeSlice,			// thread time-slice value
			   TX_AUTO_START);		// automatic start selection
   if( ret != TX_SUCCESS )
   {
      return( ret );
   }
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  MX_ThreadX_Init
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/**
  * @brief  App_ThreadX_LowPower_Timer_Setup
  * @param  count : TX timer count
  * @retval None
  */
void App_ThreadX_LowPower_Timer_Setup(ULONG count)
{
  /* USER CODE BEGIN  App_ThreadX_LowPower_Timer_Setup */

  /* USER CODE END  App_ThreadX_LowPower_Timer_Setup */
}

/**
  * @brief  App_ThreadX_LowPower_Enter
  * @param  None
  * @retval None
  */
void App_ThreadX_LowPower_Enter(void)
{
	/* USER CODE BEGIN  App_ThreadX_LowPower_Enter */
	UART_WakeUpTypeDef 	WakeUpSelection;

	HAL_StatusTypeDef	status;

	/* Enable Power Clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* Configuration of the LPM read mode */
	if (HAL_FLASHEx_ConfigLowPowerRead(FLASH_LPM_ENABLE) != HAL_OK)
	{
		Error_Handler();
	}

	WakeUpSelection.WakeUpEvent = UART_WAKEUP_ON_READDATA_NONEMPTY;
	status = HAL_UARTEx_StopModeWakeUpSourceConfig( &hlpuart1, WakeUpSelection );
	if( status != HAL_OK )
	{
		Error_Handler();
	}

	status = HAL_UARTEx_EnableStopMode( &hlpuart1 );
	if( status != HAL_OK )
	{
		Error_Handler();
	}

	__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXFNE );	//  'L4 had UART_IT_WUF);

	__HAL_RCC_LPUART1_CLK_SLEEP_ENABLE();

	HAL_SuspendTick();

	/* Enter to the stop mode */
	HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);

	/* USER CODE END  App_ThreadX_LowPower_Enter */
}

/**
  * @brief  App_ThreadX_LowPower_Exit
  * @param  None
  * @retval None
  */
void App_ThreadX_LowPower_Exit(void)
{
  /* USER CODE BEGIN  App_ThreadX_LowPower_Exit */

	SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
	HAL_ResumeTick();

	  /* Enable AHB APB Peripheral Clock */
	  __HAL_RCC_AHB1_CLK_ENABLE();
	  __HAL_RCC_AHB2_1_CLK_ENABLE();
	  __HAL_RCC_AHB2_2_CLK_ENABLE();
	  __HAL_RCC_AHB3_CLK_ENABLE();

	  __HAL_RCC_APB1_CLK_ENABLE();
	  __HAL_RCC_APB2_CLK_ENABLE();
	  __HAL_RCC_APB3_CLK_ENABLE();

	  /* Reconfigure the system clock*/
	  SystemClock_Restore();

	  // Wake up the CLI thread
	  tx_thread_resume( &uartThread );
	  tx_thread_resume( &consoleThread );


  /* USER CODE END  App_ThreadX_LowPower_Exit */
}

/**
  * @brief  App_ThreadX_LowPower_Timer_Adjust
  * @param  None
  * @retval Amount of time (in ticks)
  */
ULONG App_ThreadX_LowPower_Timer_Adjust(void)
{
  /* USER CODE BEGIN  App_ThreadX_LowPower_Timer_Adjust */
   return 0;
  /* USER CODE END  App_ThreadX_LowPower_Timer_Adjust */
}

/* USER CODE BEGIN 1 */

//*********************************************************************
//  * @brief  Restore system clock after wake-up from STOP: enable MSI, PLL
//  *         and select PLL as system clock source.
//  * @param  None
//  * @retval None
//*********************************************************************
void SystemClock_Restore(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Deinitialize the RCC */
  HAL_RCC_DeInit();

  /* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_LSE
                              |RCC_OSCILLATORTYPE_MSI|RCC_OSCILLATORTYPE_MSIK;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_0;
  RCC_OscInitStruct.LSIDiv = RCC_LSI_DIV1;
  RCC_OscInitStruct.MSIKClockRange = RCC_MSIKRANGE_0;
  RCC_OscInitStruct.MSIKState = RCC_MSIK_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV4;
  RCC_OscInitStruct.PLL.PLLM = 3;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 1;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  //__HAL_RCC_PWR_CLK_DISABLE();

}
/* USER CODE END 1 */

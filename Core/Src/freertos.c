/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can.h"

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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for messageReceive */
osThreadId_t messageReceiveHandle;
const osThreadAttr_t messageReceive_attributes = {
  .name = "messageReceive",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for updateDisplay */
osThreadId_t updateDisplayHandle;
const osThreadAttr_t updateDisplay_attributes = {
  .name = "updateDisplay",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void messageReceiveTask(void *argument);
void updateDisplayTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of messageReceive */
  messageReceiveHandle = osThreadNew(messageReceiveTask, NULL, &messageReceive_attributes);

  /* creation of updateDisplay */
  updateDisplayHandle = osThreadNew(updateDisplayTask, NULL, &updateDisplay_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_messageReceiveTask */
/**
  * @brief  Function implementing the messageReceive thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_messageReceiveTask */
void messageReceiveTask(void *argument)
{
	uint8_t msg_data[8];

	while (1) {
		if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0)) {
			// there are multiple CAN IDs being passed through the filter, check if the message is the SOC
			HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &CAN_rx_header, msg_data);
			if (CAN_rx_header.StdId == 0x626) {

				// if the battery SOC is out of range, assume it is at 100% as a safety measure
				if (msg_data[0] < 0 || msg_data[0] > 100) {

				} else {

				}

			}
		}

		osDelay(1);
	}
}

/* USER CODE BEGIN Header_updateDisplayTask */
/**
* @brief Function implementing the updateDisplay thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_updateDisplayTask */
void updateDisplayTask(void *argument)
{
  /* USER CODE BEGIN updateDisplayTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END updateDisplayTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

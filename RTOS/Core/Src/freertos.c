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
#include "LCD.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define MESSAGE_RECEIVE_TASK_DELAY 1
#define UPDATE_DISPLAY_TASK_DELAY 1
#define UPDATE_EVENT_FLAGS_TASK_DELAY 1

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
/* Definitions for updateEventFlag */
osThreadId_t updateEventFlagHandle;
const osThreadAttr_t updateEventFlag_attributes = {
  .name = "updateEventFlag",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for eventFlagsHandle */
osEventFlagsId_t eventFlagsHandleHandle;
const osEventFlagsAttr_t eventFlagsHandle_attributes = {
  .name = "eventFlagsHandle"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void messageReceiveTask(void *argument);
void updateDisplayTask(void *argument);
void updateEventFlagsTask(void *argument);

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

  /* creation of updateEventFlag */
  updateEventFlagHandle = osThreadNew(updateEventFlagsTask, NULL, &updateEventFlag_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of eventFlagsHandle */
  eventFlagsHandleHandle = osEventFlagsNew(&eventFlagsHandle_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_messageReceiveTask */
/**
  * @brief  Reads CAN message ID received and updates the corresponding structure for the global data array.
  * 		Currently, parses the data for specific IDs that require manipulation of the rx_data.
  *
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_messageReceiveTask */
void messageReceiveTask(void *argument)
{
  /* USER CODE BEGIN messageReceiveTask */
	/* Infinite loop */
	uint8_t CAN_rx_data[8];
	while(1)
	{
		// Wait for the CAN FIFO0 interrupt to invoke and set the event flag

		// event flag code

		// Check if message is available
		if (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) != 0)
		{
			// Populate CAN header and data variables
			HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_rx_header, CAN_rx_data);

			// Find the matching data that needs to be updated
			for(uint8_t i = 0; i < NUM_CAN_IDS; i++)
			{
				if(DisplayData[i].can_id == CAN_rx_header.StdId)
				{
					DisplayData[i].raw_data = CAN_rx_data;
				}
			}

			// Set event flag to run the updateDisplay task
			event_flags.update_display = 0x01;
		}
		osDelay(MESSAGE_RECEIVE_TASK_DELAY);
	}
  /* USER CODE END messageReceiveTask */
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
	while(1)
	{
		// Wait for the CAN FIFO0 interrupt to invoke and set the event flag

		// event flag code

		// Update display

		// new function from LCD.c

		osDelay(UPDATE_DISPLAY_TASK_DELAY);
	}
  /* USER CODE END updateDisplayTask */
}

/* USER CODE BEGIN Header_updateEventFlagsTask */
/**
* @brief Function implementing the updateEventFlag thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_updateEventFlagsTask */
void updateEventFlagsTask(void *argument)
{
	/* USER CODE BEGIN updateEventFlagsTask */
	/* Infinite loop */
	while(1)
	{
		if (event_flags.message_receive)
		{
			osEventFlagsSet(, )
		}
		else if (event_flags.update_display)
		{
			osEventFlagsSet(, )
		}

		osDelay(UPDATE_EVENT_FLAGS_TASK_DELAY);
	}
	/* USER CODE END updateEventFlagsTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

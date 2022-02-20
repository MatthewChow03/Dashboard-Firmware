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
#define PAGE_CHANGE_TASK_DELAY 1
#define PAGE_TIMEOUT_TASK_DELAY 1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/**
 * The dashboard can only be doing one of these actions at a time.
 * States are used to set the state event flag which is used in starting tasks.
 */
enum DisplayStates {
	IDLE = (uint32_t) 0x0000, // Default state when no task needs to be executed

	MESSAGE_RECEIVE = (uint32_t) 0x0001, // Data has been received, store the data

	UPDATE_DISPLAY = (uint32_t) 0x0002,	// After data has been stored, update the display
										// with the new data

	PAGE_CHANGE = (uint32_t) 0x0004,	 // Button has been pressed, change page

	PAGE_TIMEOUT = (uint32_t) 0x0008, // User has been on a page other than page 1
									   // for a long time, go back to page 1
} state;

/* USER CODE END Variables */

osThreadId_t messageReceiveHandle;
const osThreadAttr_t messageReceive_attributes = { .name = "messageReceive",
		.stack_size = 128 * 4, .priority = (osPriority_t) osPriorityNormal, };

osThreadId_t updateDisplayHandle;
const osThreadAttr_t updateDisplay_attributes = { .name = "updateDisplay",
		.stack_size = 128 * 4, .priority = (osPriority_t) osPriorityLow, };

osThreadId_t updateEventFlagHandle;
const osThreadAttr_t updateEventFlag_attributes =
		{ .name = "updateEventFlag", .stack_size = 128 * 4, .priority =
				(osPriority_t) osPriorityAboveNormal, };

osThreadId_t pageChangeHandle;
const osThreadAttr_t pageChange_attributes = { .name = "pageChange",
		.stack_size = 128 * 4, .priority = (osPriority_t) osPriorityLow, };

osThreadId_t pageTimeoutHandle;
const osThreadAttr_t pageTimeout_attributes = { .name = "pageTimeout",
		.stack_size = 128 * 4, .priority = (osPriority_t) osPriorityLow, };

osEventFlagsId_t stateEventFlagsHandle;
const osEventFlagsAttr_t stateEventFlags_attributes = { .name =
		"stateEventFlags" };

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void messageReceiveTask(void *argument);
void updateDisplayTask(void *argument);
void updateEventFlagsTask(void *argument);
void pageChangeTask(void *argument);
void pageTimeoutTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {

	messageReceiveHandle = osThreadNew(messageReceiveTask, NULL,
			&messageReceive_attributes);

	updateDisplayHandle = osThreadNew(updateDisplayTask, NULL,
			&updateDisplay_attributes);

	updateEventFlagHandle = osThreadNew(updateEventFlagsTask, NULL,
			&updateEventFlag_attributes);

	pageChangeHandle = osThreadNew(pageChangeTask, NULL,
			&pageChange_attributes);

	pageTimeoutHandle = osThreadNew(pageTimeoutTask, NULL,
			&pageTimeout_attributes);

	stateEventFlagsHandle = osEventFlagsNew(&stateEventFlags_attributes);
}

/* USER CODE BEGIN Header_messageReceiveTask */
/**
 * @brief  Reads CAN message ID received and updates the corresponding structure for the global data array.
 *         Currently, parses the data for specific IDs that require manipulation of the rx_data.
 */
/* USER CODE END Header_messageReceiveTask */
void messageReceiveTask(void *argument) {
	/* USER CODE BEGIN messageReceiveTask */
	uint8_t CAN_rx_data[8];
	while (1) {
		// Wait for the CAN FIFO0 interrupt to invoke and set the event flag

		osEventFlagsWait(stateEventFlagsHandle, MESSAGE_RECEIVE, osFlagsWaitAll,
				osWaitForever);

		// Check if message is available
		if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) != 0) {
			// Populate CAN header and data variables
			HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &CAN_rx_header, CAN_rx_data);

			// Find the matching data that needs to be updated
			for (uint8_t struct_index = 0; struct_index < NUM_CAN_IDS; struct_index++) {
				if (DisplayDataArray[struct_index].can_id == CAN_rx_header.StdId) {
					for (int data_index = 0; data_index < 8; data_index++) {
						DisplayDataArray[struct_index].raw_data[data_index] = CAN_rx_data[data_index];
					}
				}
			}

			// Set event flag to run the updateDisplay task
			event_flags.update_display = 0x01;

			// Reset the event flag after the task has finished
			event_flags.message_receive = 0x00;
		}
		osDelay(MESSAGE_RECEIVE_TASK_DELAY);
	}
	/* USER CODE END messageReceiveTask */
}

/* USER CODE BEGIN Header_updateDisplayTask */
/**
 * @brief 	Updates the display. Only called after new data has been received
 */
/* USER CODE END Header_updateDisplayTask */
void updateDisplayTask(void *argument) {
	/* USER CODE BEGIN updateDisplayTask */
	while (1) {
		// Wait for the CAN FIFO0 interrupt to invoke and set the event flag

		osEventFlagsWait(stateEventFlagsHandle, UPDATE_DISPLAY, osFlagsWaitAll,
		osWaitForever);

		DisplayScreen();

		// Reset the event flag after the task has finished
		event_flags.update_display = 0x00;

		osDelay(UPDATE_DISPLAY_TASK_DELAY);
	}
	/* USER CODE END updateDisplayTask */
}

/* USER CODE BEGIN Header_updateEventFlagsTask */
/**
 * @brief 	Sets the display state event flag based on the event_flags value
 */
/* USER CODE END Header_updateEventFlagsTask */
void updateEventFlagsTask(void *argument) {
	/* USER CODE BEGIN updateEventFlagsTask */
	while (1) {
		if (event_flags.message_receive) {
			state = MESSAGE_RECEIVE;
		} else if (event_flags.update_display) {
			state = UPDATE_DISPLAY;
		} else if (event_flags.page_change) {
			state = PAGE_CHANGE;
		} else if (event_flags.page_timeout) {
			state = PAGE_TIMEOUT;
		} else {
			state = IDLE;
		}

		osEventFlagsSet(stateEventFlagsHandle, state);

		osDelay(UPDATE_EVENT_FLAGS_TASK_DELAY);
	}
	/* USER CODE END updateEventFlagsTask */
}

/* USER CODE BEGIN Header_pageChangeTask */
/**
 * @brief   Changes the page number when the dashboard button is pressed.
 *          The button press will be recognized via a CAN message.
 */
/* USER CODE END Header_pageChangeTask */
void pageChangeTask(void *argument) {
	/* USER CODE BEGIN pageChangeTask */
	while (1) {
		osEventFlagsWait(stateEventFlagsHandle, PAGE_CHANGE, osFlagsWaitAll,
		osWaitForever);

		osDelay(PAGE_CHANGE_TASK_DELAY);
	}
	/* USER CODE END pageChangeTask */
}

/* USER CODE BEGIN Header_pageTimeoutTask */
/**
 * @brief   Returns to page 1 after spending a set time on any other page
 */
/* USER CODE END Header_pageTimeoutTask */
void pageTimeoutTask(void *argument) {
	/* USER CODE BEGIN pageTimeoutTask */
	while (1) {
		osEventFlagsWait(stateEventFlagsHandle, PAGE_TIMEOUT, osFlagsWaitAll,
		osWaitForever);

		osDelay(PAGE_TIMEOUT_TASK_DELAY);
	}
	/* USER CODE END pageTimeoutTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

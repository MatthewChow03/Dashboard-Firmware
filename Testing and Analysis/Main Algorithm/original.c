/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "LCD.h"

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

// CAN filtering
CAN_FilterTypeDef CAN_filter0;
CAN_FilterTypeDef CAN_filter1;

// CAN receiving
uint8_t receive_flag = 0x00;
CAN_RxHeaderTypeDef CAN_rx_header;
uint8_t CAN_rx_data[8];

// Changing pages
uint8_t button_pressed = 0;
uint8_t timer_at_max = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// CAN interrupt callback function
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	// Set message flag to 1 so that the update display task can execute
	receive_flag = 0x01;
}

// TIM5 interrupt function for period elapsing (HAL page 1018)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// EXPECTED PERIOD: the callback will be invoked every X seconds

	// Check if the correct timer is triggering the interrupt
	if (htim == &htim5)
	{
		// Blink on board LED
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}

	// current_page = 1;
}

// Blink LED function for testing purposes
void blink_A6(int num, int delay)
{
	for (int i = 0; i < num; i++)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
		HAL_Delay(delay);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
		HAL_Delay(delay);
	}
}

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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */

  	// Start timer for using interrupts (HAL page 1015)
  	HAL_TIM_Base_Start_IT(&htim5);

    // CAN initialization
    CanFilterSetup();
    HAL_CAN_Start(&hcan);

    // LCD initialization
    ScreenSetup();
    ClearScreen();
    DisplayScreen();


    // --- TESTING START
    // Blink to indicate that the microcontroller is running
    blink_A6(3,2000);
    // --- TESTING END






  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {

    	//uint32_t t = __HAL_TIM_GET_COUNTER(&htim5);


    	/*
    	if (receive_flag == 0x01)
    	{
			// Set the flag to 0 to allow the next interrupt to be recognized
			receive_flag = 0x00;


		*/




			// Check if message is available
			if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) != 0)
			{
				// Populate CAN header and data variables
				HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &CAN_rx_header, CAN_rx_data);

				// Find the matching data that needs to be updated
				for (uint8_t struct_index = 0; struct_index < NUM_CAN_IDS; struct_index++)
				{
					if (DisplayDataArray[struct_index].can_id == CAN_rx_header.StdId)
					{
						// Populate data for the specific CAN ID in DisplayDataArray identified in the loop above
						for (int data_index = 0; data_index < 8; data_index++)
						{
							DisplayDataArray[struct_index].raw_data[data_index] = CAN_rx_data[data_index];
						}

						// Check for next page button press
						if (CAN_rx_header.StdId == 0x400)
						{
							// Check the first bit of the first byte
							if (DisplayDataArray[0].raw_data[0] & 0B0000001)
							{
								// Increase page by 1 or bring back to first page
								if (++current_page > PAGE_NUM_MAX)
								{
									current_page++;

									// --- TESTING START
									blink_A6(1,10000); // stay on when page change is pressed
								    // --- TESTING END

								}
							}
						}
					}
				}
				// Display screen with updated parameters
				DisplayScreen();

				// --- TESTING START
				blink_A6(1,1000);
			    // --- TESTING END
			}




		/*
		}
		*/



    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "key.h"
#include "logic.h"
#include "heart.h"
#include "lcd_drive.h"
#include "setting.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "logic.h"
#include "ch340e.h"
#include "rtc.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
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

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}



static uint8_t USB_Vol_Flag = 0;//上电的USB充电口状态
void USB_Battery_Show(void)
{
	GUI_ClearSCR(0x00);       // 初始化缓冲区为0x00，并输出屏幕(清屏)
	for(uint8_t i=0; i<200;i++)GUI_UpdateDisplay();//刷新下屏幕	
	if(!HAL_GPIO_ReadPin(USB_JOIN_GPIO_Port, USB_JOIN_Pin))
	{
	  USB_Vol_Flag=1;
	}
	while(!HAL_GPIO_ReadPin(USB_JOIN_GPIO_Port, USB_JOIN_Pin) && Key_Off_Flag == 0 && (!setting_GetBattertType()) )
	{
		key_scan();        //先扫描按键再刷屏幕就有更快的响应
		
		if(get_RtcFlag())
		{
			clear_RtcFlag();
			logic_BatteryAD_Big(); //获取ad值	
		}
		if(get_LcdFlag()) //将显存中的内容刷到屏幕上去
		{
			clear_LcdFlag();
			GUI_UpdateDisplay();
		}	
	  ch340_DataHandle();			
		if(get_RtcFlag())
		{
			clear_RtcFlag();
			HYM8563_UpdateTime();
		}
		
	}
	GUI_ClearSCR(0x00);       // 初始化缓冲区为0x00，并输出屏幕(清屏)
	for(uint8_t i=0; i<200;i++)GUI_UpdateDisplay();//刷新下屏幕		

	
	if(USB_Vol_Flag ==1  && Key_Off_Flag == 0)
	{
    HAL_GPIO_WritePin(LCD_BLC_GPIO_Port, LCD_BLC_Pin, GPIO_PIN_RESET);  //关闭背光
		SHUTDOWN();		//关闭电源		
	}
	software_init();	
}

/* USER CODE BEGIN 4 */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
	
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_IWDG_Init();
	
	hardware_init();

  USB_Battery_Show();  //增加的USB充电未开机显示正在充电图标

  while (1)
  { 
		main_loop();		 
  }
}
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

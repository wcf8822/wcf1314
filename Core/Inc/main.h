/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef union{
	float   value_f;
	uint8_t value_arr[4];
}float_u;

typedef union{
	uint32_t   value_f;
	uint8_t value_arr[4];
}uint32_u;

typedef union{
	uint16_t   value_f;
	uint8_t value_arr[2];
}uint16_u;

typedef union{
	int16_t   value_f;
	int8_t value_arr[2];
}int16_u;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USB_JOIN_Pin GPIO_PIN_4    //判断USB供电
#define USB_JOIN_GPIO_Port GPIOC

#define GPS_EN_PIN  GPIO_PIN_11  //GPS供电使能引脚
#define GPS_EN_PORT GPIOA

#define LED_Pin GPIO_PIN_2
#define LED_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_6
#define BUZZER_GPIO_Port GPIOA

#define BAT_OFF_Pin GPIO_PIN_7
#define BAT_OFF_GPIO_Port GPIOE

#define BAT_AD_Pin GPIO_PIN_8
#define BAT_AD_GPIO_Port GPIOE

#define KEY_BLU_Pin GPIO_PIN_9
#define KEY_BLU_GPIO_Port GPIOE
#define KEY_UP_Pin GPIO_PIN_10
#define KEY_UP_GPIO_Port GPIOE
#define KEY_DOWN_Pin GPIO_PIN_11
#define KEY_DOWN_GPIO_Port GPIOE
#define KEY_OK_Pin GPIO_PIN_12
#define KEY_OK_GPIO_Port GPIOE
#define KEY_CAL_Pin GPIO_PIN_13
#define KEY_CAL_GPIO_Port GPIOE
#define KEY_ESC_Pin GPIO_PIN_14
#define KEY_ESC_GPIO_Port GPIOE
#define KEY_MENU_Pin GPIO_PIN_15
#define KEY_MENU_GPIO_Port GPIOE

#define KEY_OFF_Pin GPIO_PIN_7
#define KEY_OFF_GPIO_Port GPIOA

#define GPIO_Ver_0_Pin GPIO_PIN_12
#define GPIO_Ver_0_GPIO_Port GPIOB
#define GPIO_Ver_1_Pin GPIO_PIN_13
#define GPIO_Ver_1_GPIO_Port GPIOB
#define LCD_D7_Pin GPIO_PIN_8
#define LCD_D7_GPIO_Port GPIOD
#define LCD_D6_Pin GPIO_PIN_9
#define LCD_D6_GPIO_Port GPIOD
#define LCD_D5_Pin GPIO_PIN_10
#define LCD_D5_GPIO_Port GPIOD
#define LCD_D4_Pin GPIO_PIN_11
#define LCD_D4_GPIO_Port GPIOD
#define LCD_D3_Pin GPIO_PIN_12
#define LCD_D3_GPIO_Port GPIOD
#define LCD_D2_Pin GPIO_PIN_13
#define LCD_D2_GPIO_Port GPIOD
#define LCD_D1_Pin GPIO_PIN_14
#define LCD_D1_GPIO_Port GPIOD
#define LCD_D0_Pin GPIO_PIN_15
#define LCD_D0_GPIO_Port GPIOD
#define LCD_RST_Pin GPIO_PIN_6
#define LCD_RST_GPIO_Port GPIOC
#define LCD_WR_Pin GPIO_PIN_7
#define LCD_WR_GPIO_Port GPIOC
#define LCD_RD_Pin GPIO_PIN_8
#define LCD_RD_GPIO_Port GPIOC
#define LCD_A0_Pin GPIO_PIN_9
#define LCD_A0_GPIO_Port GPIOC
#define LCD_CS0_Pin GPIO_PIN_8
#define LCD_CS0_GPIO_Port GPIOA
#define CH340_TX_Pin GPIO_PIN_9
#define CH340_TX_GPIO_Port GPIOA
#define CH340_RX_Pin GPIO_PIN_10
#define CH340_RX_GPIO_Port GPIOA
#define LCD_BLC_Pin GPIO_PIN_15
#define LCD_BLC_GPIO_Port GPIOA
#define RS485_TX_Pin GPIO_PIN_10
#define RS485_TX_GPIO_Port GPIOC
#define RS485_RX_Pin GPIO_PIN_11
#define RS485_RX_GPIO_Port GPIOC

#define RS485_DE_Pin GPIO_PIN_12
#define RS485_DE_GPIO_Port GPIOC
// #define RS485_DE_Pin GPIO_PIN_4
// #define RS485_DE_GPIO_Port GPIOD

#define GPS_TX_Pin GPIO_PIN_5
#define GPS_TX_GPIO_Port GPIOD
#define GPS_RX_Pin GPIO_PIN_6
#define GPS_RX_GPIO_Port GPIOD
#define FLASH_CS_Pin GPIO_PIN_7
#define FLASH_CS_GPIO_Port GPIOD
#define RTC_SCL_Pin GPIO_PIN_6
#define RTC_SCL_GPIO_Port GPIOB
#define RTC_SDA_Pin GPIO_PIN_7
#define RTC_SDA_GPIO_Port GPIOB
#define BMP280_CS_Pin GPIO_PIN_8
#define BMP280_CS_GPIO_Port GPIOB
#define FLASH_WP_Pin GPIO_PIN_0
#define FLASH_WP_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

#define DEBUG 1

#if DEBUG
#define STATIC 
#else
#define STATIC static
#endif



#define SETTING_FIRSTRUN_JUDGE 0xDE     //版本维护

#define SETTING_FIRSTRUN_ADDR  0x000000 //判断是否是第一次上电的判断对比位 地址
#define SETTING_CNORENG_ADDR   0x000100 //判断恢复出厂设置是英文还是中文的数据位 地址

#define SETTING_START_ADDR     0x000300 //配置信息存储地址
#define SETTING_LOGO_ADDR    	 0x000400 //开机动画选择位 地址
#define SETTING_HAVEGPS_ADDR   0x000500 //是否有gps功能位 地址
#define SETTING_CONTRAST_ADDR  0x000600 //屏幕初始化对比度数据存储地址
#define BATTERT_TYPE_ADDR  	   0x000700 //电池类型（干电池和锂电池）数据存储地址   0：锂电池    1：干电池   默认：0
#define B580_D702_TYPE_ADDR  	 0x000800 //设备类型 702 还是580             0: 702 1:580


#define LOG_MAX_COUNT        10000//最大存储个数
#define LOG_FIRST_ADDR         0x3CB000 //DO 第一条记录的存储地址
#define LOG_FIRST_pH_ADDR         0x3CB000+1*LOG_MAX_COUNT*sizeof(log_t)   //pH 第一条记录的存储地址
#define LOG_FIRST_Tur_ADDR         0x3CB000+2*LOG_MAX_COUNT*sizeof(log_t)   //Tur 第一条记录的存储地址
#define LOG_FIRST_SAL_ADDR         0x3CB000+3*LOG_MAX_COUNT*sizeof(log_t)   //SAL 第一条记录的存储地址
#define LOG_FIRST_EC_ADDR         0x3CB000+4*LOG_MAX_COUNT*sizeof(log_t)   //EC 第一条记录的存储地址
#define LOG_FIRST_ORP_ADDR         0x3CB000+5*LOG_MAX_COUNT*sizeof(log_t)   //ORP 第一条记录的存储地址	
#define LOG_FIRST_NH4_ADDR         0x3CB000+6*LOG_MAX_COUNT*sizeof(log_t)   //NH4 第一条记录的存储地址	
#define LOG_FIRST_F_ADDR         0x3CB000+7*LOG_MAX_COUNT*sizeof(log_t)   //F 第一条记录的存储地址
#define LOG_FIRST_CL_ADDR         0x3CB000+8*LOG_MAX_COUNT*sizeof(log_t)   //CL 第一条记录的存储地址
#define LOG_FIRST_Chl_ADDR         0x3CB000+9*LOG_MAX_COUNT*sizeof(log_t)   //Chl 第一条记录的存储地址
#define LOG_FIRST_Bga_ADDR         0x3CB000+10*LOG_MAX_COUNT*sizeof(log_t)   //Bga 第一条记录的存储地址
#define LOG_FIRST_COD_ADDR         0x3CB000+11*LOG_MAX_COUNT*sizeof(log_t)   //COD 第一条记录的存储地址
#define LOG_FIRST_PPM_ADDR         0x3CB000+12*LOG_MAX_COUNT*sizeof(log_t)   //水中油 第一条记录的存储地址
#define LOG_FIRST_MLSS_ADDR        0x3CB000+13*LOG_MAX_COUNT*sizeof(log_t)   //污泥浓度 第一条记录的存储地址
#define LOG_FIRST_OIW_ADDR         0x3CB000+14*LOG_MAX_COUNT*sizeof(log_t)   //水中油 第一条记录的存储地址
#define LOG_FIRST_TSS_ADDR         0x3CB000+15*LOG_MAX_COUNT*sizeof(log_t)   //悬浮物 第一条记录的存储地址

#define LOG_COUNT_ADDR         0x000200 //记录条数储存位起始地址  

#define MAX_EPS_K 0.5
#define MAX_EPS_B 0.15

#define CAL_ZERO_VALUE 0.001

#define SHUTDOWN() HAL_GPIO_WritePin(BAT_OFF_GPIO_Port, BAT_OFF_Pin, GPIO_PIN_SET)
// #define SHUTDOWN() HAL_GPIO_WritePin(BAT_OFF_GPIO_Port, BAT_OFF_Pin, GPIO_PIN_RESET)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

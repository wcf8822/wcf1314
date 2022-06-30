#ifndef __BMP280_H
#define __BMP280_H

#include "main.h"

#define BMP280_CS_H()    HAL_GPIO_WritePin(BMP280_CS_GPIO_Port, BMP280_CS_Pin, GPIO_PIN_SET)
#define BMP280_CS_L()    HAL_GPIO_WritePin(BMP280_CS_GPIO_Port, BMP280_CS_Pin, GPIO_PIN_RESET)

#define BMP280_RESET_VALUE                   0xB6

#define BMP280_CHIPID_REG                    0xD0  /*Chip ID Register */
#define BMP280_RESET_REG                     0xE0  /*Softreset Register */

#define BMP280_STATUS_REG                    0xF3  /*Status Register */
#define BMP280_CTRLMEAS_REG                  0xF4  /*Ctrl Measure Register */
#define BMP280_CONFIG_REG                    0xF5  /*Configuration Register */

#define BMP280_PRESSURE_MSB_REG              0xF7  /*Pressure MSB Register */
#define BMP280_PRESSURE_LSB_REG              0xF8  /*Pressure LSB Register */
#define BMP280_PRESSURE_XLSB_REG             0xF9  /*Pressure XLSB Register */

#define BMP280_TEMPERATURE_MSB_REG           0xFA  /*Temperature MSB Reg */
#define BMP280_TEMPERATURE_LSB_REG           0xFB  /*Temperature LSB Reg */
#define BMP280_TEMPERATURE_XLSB_REG          0xFC  /*Temperature XLSB Reg */

//状态寄存器转换标志
#define	BMP280_MEASURING					           0x01
#define	BMP280_IM_UPDATE					           0x08




#define BMP280_DIG_T1_LSB_REG                0x88
#define BMP280_DIG_T1_MSB_REG                0x89
#define BMP280_DIG_T2_LSB_REG                0x8A
#define BMP280_DIG_T2_MSB_REG                0x8B
#define BMP280_DIG_T3_LSB_REG                0x8C
#define BMP280_DIG_T3_MSB_REG                0x8D
#define BMP280_DIG_P1_LSB_REG                0x8E
#define BMP280_DIG_P1_MSB_REG                0x8F
#define BMP280_DIG_P2_LSB_REG                0x90
#define BMP280_DIG_P2_MSB_REG                0x91
#define BMP280_DIG_P3_LSB_REG                0x92
#define BMP280_DIG_P3_MSB_REG                0x93
#define BMP280_DIG_P4_LSB_REG                0x94
#define BMP280_DIG_P4_MSB_REG                0x95
#define BMP280_DIG_P5_LSB_REG                0x96
#define BMP280_DIG_P5_MSB_REG                0x97
#define BMP280_DIG_P6_LSB_REG                0x98
#define BMP280_DIG_P6_MSB_REG                0x99
#define BMP280_DIG_P7_LSB_REG                0x9A
#define BMP280_DIG_P7_MSB_REG                0x9B
#define BMP280_DIG_P8_LSB_REG                0x9C
#define BMP280_DIG_P8_MSB_REG                0x9D
#define BMP280_DIG_P9_LSB_REG                0x9E
#define BMP280_DIG_P9_MSB_REG                0x9F


typedef			long signed int				BMP280_S32_t;	//有符号 64位！
typedef			long unsigned int			BMP280_U32_t;	//无符号 32位！
typedef			long long signed int	BMP280_S64_t;


//BMP温度过采样因子
typedef enum {
	BMP280_T_MODE_SKIP = 0x0,	/*skipped*/
	BMP280_T_MODE_1,			/*x1*/
	BMP280_T_MODE_2,			/*x2*/
	BMP280_T_MODE_3,			/*x4*/
	BMP280_T_MODE_4,			/*x8*/
	BMP280_T_MODE_5			    /*x16*/
} BMP280_T_OVERSAMPLING;

typedef enum {
	BMP280_SLEEP_MODE  = 0x00,
	BMP280_FORCED_MODE = 0x01,	//可以说0x2
	BMP280_NORMAL_MODE = 0x03
} BMP280_WORK_MODE;

//BMP压力过采样因子
typedef enum 
{
	BMP280_P_MODE_SKIP = 0x0,	/*skipped*/
	BMP280_P_MODE_1,			/*x1*/
	BMP280_P_MODE_2,			/*x2*/
	BMP280_P_MODE_3,			/*x4*/
	BMP280_P_MODE_4,			/*x8*/
	BMP280_P_MODE_5			    /*x16*/
} BMP280_P_OVERSAMPLING;	


									
//IIR滤波器时间常数
typedef enum {
	BMP280_FILTER_OFF = 0x0,	/*filter off*/
	BMP280_FILTER_MODE_1,		/*0.223*ODR*/	/*x2*/
	BMP280_FILTER_MODE_2,		/*0.092*ODR*/	/*x4*/
	BMP280_FILTER_MODE_3,		/*0.042*ODR*/	/*x8*/
	BMP280_FILTER_MODE_4		/*0.021*ODR*/	/*x16*/
} BMP280_FILTER_COEFFICIENT;

//保持时间
typedef enum {
	BMP280_T_SB1 = 0x0,	    /*0.5ms*/
	BMP280_T_SB2,			/*62.5ms*/
	BMP280_T_SB3,			/*125ms*/
	BMP280_T_SB4,			/*250ms*/
	BMP280_T_SB5,			/*500ms*/
	BMP280_T_SB6,			/*1000ms*/
	BMP280_T_SB7,			/*2000ms*/
	BMP280_T_SB8,			/*4000ms*/
} BMP280_T_SB;


typedef struct  
{
	/* T1~P9 为补偿系数 */
	uint16_t T1;
	int16_t	 T2;
	int16_t	 T3;
	uint16_t P1;
	int16_t	 P2;
	int16_t	 P3;
	int16_t	 P4;
	int16_t	 P5;
	int16_t	 P6;
	int16_t	 P7;
	int16_t	 P8;
	int16_t	 P9;
} BMP280_Compensation_t;


typedef struct
{
	BMP280_P_OVERSAMPLING P_Osample;
	BMP280_T_OVERSAMPLING T_Osample;
	BMP280_WORK_MODE		  WORKMODE;
} BMP_OVERSAMPLE_MODE;
	
typedef struct
{
	BMP280_T_SB 				       T_SB;
	BMP280_FILTER_COEFFICIENT  FILTER_COEFFICIENT;
	FunctionalState				     SPI_EN;
} BMP_CONFIG;




typedef struct{
	SPI_HandleTypeDef *hspi;//接的是哪个spi口
	
	void (*init)(SPI_HandleTypeDef *hspi);
	
	double pressure;
	double temperature;
	
	
}bmp280_t;


extern bmp280_t bmp280; //外部调用结构体

uint8_t bmp280_readId(void); //测试用 用于读取id

void bmp280_UpdateValue(void);

double bmp280_GetTemp(void);

double bmp280_GetPress(void);

#endif




#ifndef __SPA06_H
#define __SPA06_H

#include "main.h"

/* SPA06-003 与 BMP280 是 pin-to-pin 封装，共用同一根片选线 */
#define SPA06_CS_H()    HAL_GPIO_WritePin(BMP280_CS_GPIO_Port, BMP280_CS_Pin, GPIO_PIN_SET)
#define SPA06_CS_L()    HAL_GPIO_WritePin(BMP280_CS_GPIO_Port, BMP280_CS_Pin, GPIO_PIN_RESET)

/* ------------------------- 寄存器地址 ------------------------- */
#define SPA06_REG_PRS_B2            0x00  /*压力 MSB   PRS[23:16] */
#define SPA06_REG_PRS_B1            0x01  /*压力 LSB   PRS[15:8]  */
#define SPA06_REG_PRS_B0            0x02  /*压力 XLSB  PRS[7:0]   */
#define SPA06_REG_TMP_B2            0x03  /*温度 MSB   TMP[23:16] */
#define SPA06_REG_TMP_B1            0x04  /*温度 LSB   TMP[15:8]  */
#define SPA06_REG_TMP_B0            0x05  /*温度 XLSB  TMP[7:0]   */

#define SPA06_REG_PRS_CFG           0x06  /*压力测量配置 PM_RATE[7:4]/PM_PRC[3:0] */
#define SPA06_REG_TMP_CFG           0x07  /*温度测量配置 TMP_RATE[7:4]/TMP_PRC[3:0] */
#define SPA06_REG_MEAS_CFG          0x08  /*工作模式与状态 */
#define SPA06_REG_CFG_REG           0x09  /*中断/FIFO/移位配置 */
#define SPA06_REG_INT_STS           0x0A  /*中断状态(读清除) */
#define SPA06_REG_FIFO_STS          0x0B  /*FIFO状态 */
#define SPA06_REG_RESET             0x0C  /*软复位/FIFO刷新 */
#define SPA06_REG_ID                0x0D  /*产品与版本ID REV_ID[7:4]/PROD_ID[3:0] */

#define SPA06_REG_COEF_BASE         0x10  /*校准系数起始地址 0x10~0x24 共21字节 */

/* ------------------------- 寄存器位定义 ------------------------- */
/* MEAS_CFG */
#define SPA06_MEAS_CFG_COEF_RDY     0x80  /*校准系数就绪 */
#define SPA06_MEAS_CFG_SENSOR_RDY   0x40  /*传感器自初始化完成 */
#define SPA06_MEAS_CFG_TMP_RDY      0x20  /*温度测量就绪 */
#define SPA06_MEAS_CFG_PRS_RDY      0x10  /*压力测量就绪 */
#define SPA06_MEAS_CFG_BACKGROUND_PT 0x07 /*后台模式：连续测压+测温 */

/* CFG_REG */
#define SPA06_CFG_REG_P_SHIFT       0x04  /*压力结果右移1位(过采样>8必须置1) */
#define SPA06_CFG_REG_T_SHIFT       0x08  /*温度结果右移1位(过采样>8必须置1) */

/* RESET */
#define SPA06_SOFT_RESET_CMD        0x09  /*SOFT_RST = 1001 */

/* ------------------------- 测量配置 ------------------------- */
/* 压力：1Hz × 16次过采样(标准精度)，配合 P_SHIFT=1，kP=253952 */
#define SPA06_PRS_CFG_VALUE         0x04
/* 温度：1Hz × 单次过采样，无移位，kT=524288 */
#define SPA06_TMP_CFG_VALUE         0x00
#define SPA06_CFG_REG_VALUE         SPA06_CFG_REG_P_SHIFT

/* 补偿换算系数(过采样>8的结果已右移，手册表4) */
#define SPA06_SCALE_FACTOR_P        253952.0
#define SPA06_SCALE_FACTOR_T        524288.0

/* ------------------------- 校准系数 ------------------------- */
typedef struct
{
	int16_t  c0;    /*12bit 有符号*/
	int16_t  c1;    /*12bit 有符号*/
	int32_t  c00;   /*20bit 有符号*/
	int32_t  c10;   /*20bit 有符号*/
	int16_t  c01;   /*16bit 有符号*/
	int16_t  c11;   /*16bit 有符号*/
	int16_t  c20;   /*16bit 有符号*/
	int16_t  c21;   /*16bit 有符号*/
	int16_t  c30;   /*16bit 有符号*/
	int16_t  c31;   /*12bit 有符号*/
	int16_t  c40;   /*12bit 有符号*/
} SPA06_Coefficients_t;

typedef struct
{
	SPI_HandleTypeDef *hspi;   /*接的是哪个spi口*/

	double pressure;           /*气压值 Pa*/
	double temperature;        /*温度值 ℃*/
} spa06_t;

extern spa06_t spa06;   /*外部调用结构体*/

uint8_t spa06_Init(SPI_HandleTypeDef *hspi);   /*初始化，成功返回1，失败返回0*/

void spa06_UpdateValue(void);                  /*更新气压与温度值*/

double spa06_GetTemp(void);                    /*获取温度 ℃*/

double spa06_GetPress(void);                   /*获取气压 kPa(与bmp280_GetPress语义一致)*/

uint8_t spa06_ReadId(void);                    /*读取ID寄存器0x0D*/

#endif

#include "spa06.h"

/**
 * SPA06-003 数字气压传感器驱动 (SPI 4线, mode 3)
 * SPI帧格式: 首字节 = bit7(R/W: 1读0写) + bit6:0(寄存器地址)
 * 压力/温度为24bit二补码；补偿公式见手册4.6节
 */

spa06_t spa06 = {
	.hspi = NULL,
	.pressure = 0.0,
	.temperature = 0.0
};

static SPA06_Coefficients_t spa06_coef;

/*调试观察变量: 供调试器查看实际读到的原始值(0xEE=尚未读取)*/
volatile uint8_t spa06_dbg_id       = 0xEE;
volatile uint8_t spa06_dbg_meas_cfg = 0xEE;

/* ---------------- SPI 底层收发(直接寄存器操作，与bmp280.c风格一致) ---------------- */

static uint8_t spa06_SendByte(uint8_t byte)
{
	while (__HAL_SPI_GET_FLAG(spa06.hspi, SPI_FLAG_TXE) == RESET);
	WRITE_REG(spa06.hspi->Instance->DR, byte);
	while (__HAL_SPI_GET_FLAG(spa06.hspi, SPI_FLAG_RXNE) == RESET);
	return READ_REG(spa06.hspi->Instance->DR);
}

/* 读单个寄存器 */
static uint8_t spa06_ReadReg(uint8_t reg)
{
	uint8_t ret;

	SPA06_CS_L();

	spa06_SendByte(reg | 0x80);   /*bit7=1 读*/
	ret = spa06_SendByte(0xFF);

	SPA06_CS_H();

	return ret;
}

/* 写单个寄存器 */
static void spa06_WriteReg(uint8_t reg, uint8_t data)
{
	SPA06_CS_L();

	spa06_SendByte(reg & 0x7F);   /*bit7=0 写*/
	spa06_SendByte(data);

	SPA06_CS_H();
}

/* 突发读连续寄存器(地址自动递增) */
static void spa06_ReadBurst(uint8_t reg, uint8_t *buf, uint16_t size)
{
	uint16_t i;

	SPA06_CS_L();

	spa06_SendByte(reg | 0x80);   /*bit7=1 读*/
	for (i = 0; i < size; i++)
	{
		buf[i] = spa06_SendByte(0xFF);
	}

	SPA06_CS_H();
}

uint8_t spa06_ReadId(void)
{
	return spa06_ReadReg(SPA06_REG_ID);
}

/* ---------------- 符号扩展与系数读取 ---------------- */

/* 读取0x10~0x24共21字节，拼装系数并做符号扩展(见手册7.11 Table 10) */
static uint8_t spa06_ReadCoefficients(void)
{
	uint8_t raw[21];
	uint32_t v;

	spa06_ReadBurst(SPA06_REG_COEF_BASE, raw, sizeof(raw));

	/* c0[11:4]=raw[0], c0[3:0]=raw[1]高4位 */
	v = ((uint16_t)raw[0] << 4) | (raw[1] >> 4);
	spa06_coef.c0 = (v & 0x0800) ? (int16_t)(v - 0x1000) : (int16_t)v;

	/* c1[11:8]=raw[1]低4位, c1[7:0]=raw[2] */
	v = ((uint16_t)(raw[1] & 0x0F) << 8) | raw[2];
	spa06_coef.c1 = (v & 0x0800) ? (int16_t)(v - 0x1000) : (int16_t)v;

	/* c00[19:12]=raw[3], c00[11:4]=raw[4], c00[3:0]=raw[5]高4位 */
	v = ((uint32_t)raw[3] << 12) | ((uint32_t)raw[4] << 4) | (raw[5] >> 4);
	spa06_coef.c00 = (v & 0x080000UL) ? (int32_t)(v - 0x100000UL) : (int32_t)v;

	/* c10[19:16]=raw[5]低4位, c10[15:8]=raw[6], c10[7:0]=raw[7] */
	v = ((uint32_t)(raw[5] & 0x0F) << 16) | ((uint32_t)raw[6] << 8) | raw[7];
	spa06_coef.c10 = (v & 0x080000UL) ? (int32_t)(v - 0x100000UL) : (int32_t)v;

	/* 16bit系数 */
	v = ((uint16_t)raw[8] << 8) | raw[9];
	spa06_coef.c01 = (v & 0x8000) ? (int16_t)(v - 0x10000) : (int16_t)v;

	v = ((uint16_t)raw[10] << 8) | raw[11];
	spa06_coef.c11 = (v & 0x8000) ? (int16_t)(v - 0x10000) : (int16_t)v;

	v = ((uint16_t)raw[12] << 8) | raw[13];
	spa06_coef.c20 = (v & 0x8000) ? (int16_t)(v - 0x10000) : (int16_t)v;

	v = ((uint16_t)raw[14] << 8) | raw[15];
	spa06_coef.c21 = (v & 0x8000) ? (int16_t)(v - 0x10000) : (int16_t)v;

	v = ((uint16_t)raw[16] << 8) | raw[17];
	spa06_coef.c30 = (v & 0x8000) ? (int16_t)(v - 0x10000) : (int16_t)v;

	/* c31[11:4]=raw[18], c31[3:0]=raw[19]高4位 */
	v = ((uint16_t)raw[18] << 4) | (raw[19] >> 4);
	spa06_coef.c31 = (v & 0x0800) ? (int16_t)(v - 0x1000) : (int16_t)v;

	/* c40[11:8]=raw[19]低4位, c40[7:0]=raw[20] */
	v = ((uint16_t)(raw[19] & 0x0F) << 8) | raw[20];
	spa06_coef.c40 = (v & 0x0800) ? (int16_t)(v - 0x1000) : (int16_t)v;

	return 1;
}

/* ---------------- 初始化(按手册4.6.4流程) ---------------- */

uint8_t spa06_Init(SPI_HandleTypeDef *hspi)
{
	uint8_t id;
	uint8_t cfg;
	uint8_t timeout;
	uint8_t retry;

	spa06.hspi = hspi;



	/* 1. ID读取(带重试): 上电后首个CSB下降沿被芯片用于接口模式切换, 首次读数可能无效;
	   同时读MEAS_CFG(上电复位值0xC0)做交叉验证 */
	for (retry = 0; retry < 5; retry++)
	{
	/* 0. 接口切换窗口: SPA06上电默认I2C, 需CSB出一个干净的低电平并保持SCK稳定,
	   让芯片从I2C切换到SPI 4-wire(手册4.3.2: CSB跳变时SCL必须稳定) */
		SPA06_CS_L();
		HAL_Delay(1);
		SPA06_CS_H();
		HAL_Delay(1);
		
		id = spa06_ReadId();
		cfg = spa06_ReadReg(SPA06_REG_MEAS_CFG);
		spa06_dbg_id = id;
		spa06_dbg_meas_cfg = cfg;

		if (id == 0x10 || id == 0x11)
		{
			break;	/*ID符合手册值*/
		}
		if (cfg == 0xC0 && id != 0x00 && id != 0xFF)
		{
			break;	/*ID与手册不符但MEAS_CFG为复位值, 视为兼容的克隆芯片*/
		}
		HAL_Delay(2);
	}
	if (retry >= 5)
	{
		return 0;	/*失败时用调试器观察 spa06_dbg_id / spa06_dbg_meas_cfg 定位*/
	}

	/* 2. 软复位 */
	spa06_WriteReg(SPA06_REG_RESET, SPA06_SOFT_RESET_CMD);
	HAL_Delay(20);

	/* 2.1 复位后MEAS_CFG应为复位值0xC0，二次确认不是误判 */
	if (spa06_ReadReg(SPA06_REG_MEAS_CFG) != 0xC0)
	{
		return 0;
	}

	/* 3. 等待系数就绪+传感器自初始化完成(带超时，通讯异常时不挂死) */
	for (timeout = 0; timeout < 50; timeout++)
	{
		cfg = spa06_ReadReg(SPA06_REG_MEAS_CFG);
		if ((cfg & (SPA06_MEAS_CFG_COEF_RDY | SPA06_MEAS_CFG_SENSOR_RDY))
		        == (SPA06_MEAS_CFG_COEF_RDY | SPA06_MEAS_CFG_SENSOR_RDY))
		{
			break;
		}
		HAL_Delay(1);
	}
	if (timeout >= 50)
	{
		return 0;
	}

	/* 4. 读取校准系数 */
	spa06_ReadCoefficients();

	/* 5. 配置压力/温度测量与移位 */
	spa06_WriteReg(SPA06_REG_PRS_CFG, SPA06_PRS_CFG_VALUE);
	spa06_WriteReg(SPA06_REG_TMP_CFG, SPA06_TMP_CFG_VALUE);
	spa06_WriteReg(SPA06_REG_CFG_REG, SPA06_CFG_REG_VALUE);

	/* 6. 启动后台连续测量(压力+温度) */
	spa06_WriteReg(SPA06_REG_MEAS_CFG, SPA06_MEAS_CFG_BACKGROUND_PT);

	/* 等首次测量完成(16次过采样约28ms) */
	HAL_Delay(50);

	return 1;
}

/* ---------------- 数据更新与补偿 ---------------- */

void spa06_UpdateValue(void)
{
	uint8_t b[6];
	int32_t praw, traw;
	double psc, tsc, p2, p3, p4;

	/* 突发读压力+温度 6字节 (0x00~0x05) */
	spa06_ReadBurst(SPA06_REG_PRS_B2, b, 6);

	/* 24bit二补码 */
	praw = ((int32_t)b[0] << 16) | ((int32_t)b[1] << 8) | b[2];
	if (praw & 0x800000) praw -= 0x1000000;

	traw = ((int32_t)b[3] << 16) | ((int32_t)b[4] << 8) | b[5];
	if (traw & 0x800000) traw -= 0x1000000;

	/* 换算 */
	psc = (double)praw / SPA06_SCALE_FACTOR_P;
	tsc = (double)traw / SPA06_SCALE_FACTOR_T;

	/* 温度补偿: Tcomp = c0*0.5 + c1*Traw_sc */
	spa06.temperature = (double)spa06_coef.c0 * 0.5 + (double)spa06_coef.c1 * tsc;

	/* 压力补偿: Pcomp = c00 + c10*Psc + c20*Psc^2 + c30*Psc^3 + c40*Psc^4
	   + Tsc*(c01 + c11*Psc + c21*Psc^2 + c31*Psc^3) */
	p2 = psc * psc;
	p3 = p2 * psc;
	p4 = p2 * p2;
	spa06.pressure = (double)spa06_coef.c00
	                 + (double)spa06_coef.c10 * psc
	                 + (double)spa06_coef.c20 * p2
	                 + (double)spa06_coef.c30 * p3
	                 + (double)spa06_coef.c40 * p4
	                 + tsc * ((double)spa06_coef.c01
	                          + (double)spa06_coef.c11 * psc
	                          + (double)spa06_coef.c21 * p2
	                          + (double)spa06_coef.c31 * p3);
}

double spa06_GetTemp(void)
{
	return spa06.temperature;
}

double spa06_GetPress(void)
{
	return spa06.pressure / 1000.0;   /*Pa -> kPa, 与bmp280_GetPress语义一致*/
}

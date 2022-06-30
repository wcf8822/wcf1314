#include "bmp280.h"

void bmp280_Init(SPI_HandleTypeDef *hspi);

bmp280_t bmp280 = {
	.init = bmp280_Init,
	.pressure = 0.0,
	.temperature = 0.0
};

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t bmp280_SendByte(uint8_t byte)
{
  while (__HAL_SPI_GET_FLAG(bmp280.hspi, SPI_FLAG_TXE) == RESET);
  WRITE_REG(bmp280.hspi->Instance->DR, byte);
  while (__HAL_SPI_GET_FLAG( bmp280.hspi, SPI_FLAG_RXNE ) == RESET);
  return READ_REG(bmp280.hspi->Instance->DR);
}

uint8_t bmp280_ReadByte(void)
{
  return (bmp280_SendByte(0xFF));
}


void SPI_Read(uint8_t* ReadBuf, uint8_t ReadAddr, uint16_t size)
{
	uint16_t i;
	
	BMP280_CS_L();
	
	bmp280_SendByte(ReadAddr);
	
	for(i = 0; i < size; i++)
	{
		ReadBuf[i] = bmp280_ReadByte();
	}
	
	BMP280_CS_H();
	
}

void SPI_Write(uint8_t* WriteBuf, uint8_t WriteAddr, uint16_t size)
{
	uint16_t i;
	
	BMP280_CS_L();
	
	bmp280_SendByte(WriteAddr);
	for(i = 0; i < size; i++)
	{
		bmp280_SendByte(WriteBuf[i]);
	}
	
	BMP280_CS_H();
}

void BMP280_Write_Byte(uint8_t WriteAddr, uint8_t data)
{
	
	BMP280_CS_L();
	
	bmp280_SendByte(WriteAddr);
	bmp280_SendByte(data);
	
	BMP280_CS_H();
}

uint8_t BMP280_Read_Byte(uint8_t RegAddr)
{
	uint8_t ret;
	
	BMP280_CS_L();
	
	bmp280_SendByte(RegAddr);

	ret = bmp280_ReadByte();
	
	BMP280_CS_H();
	
	return ret;
}


uint8_t bmp280_readId(void)
{	
	return BMP280_Read_Byte(BMP280_CHIPID_REG);
}

BMP280_Compensation_t bmp280_Compensation;

void GetCompensation(void)
{
	uint8_t Lsb,Msb;
	
	/********************接下来读出矫正参数*********************/
	//温度传感器的矫正值
	Lsb = BMP280_Read_Byte(BMP280_DIG_T1_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_T1_MSB_REG);
	bmp280_Compensation.T1 = (((uint16_t)Msb)<<8) + Lsb;			//高位加低位
	
	Lsb = BMP280_Read_Byte(BMP280_DIG_T2_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_T2_MSB_REG);
	bmp280_Compensation.T2 = (((uint16_t)Msb)<<8) + Lsb;
	
	Lsb = BMP280_Read_Byte(BMP280_DIG_T3_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_T3_MSB_REG);
	bmp280_Compensation.T3 = (((uint16_t)Msb)<<8) + Lsb;		
	
	//大气压传感器的矫正值
	Lsb = BMP280_Read_Byte(BMP280_DIG_P1_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P1_MSB_REG);
	bmp280_Compensation.P1 = (((uint16_t)Msb)<<8) + Lsb;
	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P2_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P2_MSB_REG);
	bmp280_Compensation.P2 = (((uint16_t)Msb)<<8) + Lsb;
	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P3_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P3_MSB_REG);
	bmp280_Compensation.P3 = (((uint16_t)Msb)<<8) + Lsb;
	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P4_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P4_MSB_REG);
	bmp280_Compensation.P4 = (((uint16_t)Msb)<<8) + Lsb;
	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P5_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P5_MSB_REG);
	bmp280_Compensation.P5 = (((uint16_t)Msb)<<8) + Lsb;
	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P6_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P6_MSB_REG);
	bmp280_Compensation.P6 = (((uint16_t)Msb)<<8) + Lsb;
	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P7_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P7_MSB_REG);
	bmp280_Compensation.P7 = (((uint16_t)Msb)<<8) + Lsb;
	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P8_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P8_MSB_REG);
	bmp280_Compensation.P8 = (((uint16_t)Msb)<<8) + Lsb;
	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P9_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P9_MSB_REG);
	bmp280_Compensation.P9 = (((uint16_t)Msb)<<8) + Lsb;	
	/******************************************************/
}





//设置BMP过采样因子 MODE 
//BMP280_SLEEP_MODE||BMP280_FORCED_MODE||BMP280_NORMAL_MODE
void BMP280_Set_TemOversamp(BMP_OVERSAMPLE_MODE * Oversample_Mode)
{
	uint8_t Regtmp;
	Regtmp = ((Oversample_Mode->T_Osample)<<5)|
			 ((Oversample_Mode->P_Osample)<<2)|
			 ((Oversample_Mode)->WORKMODE);
	
	BMP280_Write_Byte(BMP280_CTRLMEAS_REG,Regtmp);
}

//设置保持时间和滤波器分频因子
void BMP280_Set_Standby_FILTER(BMP_CONFIG * BMP_Config)
{
	uint8_t Regtmp;
	Regtmp = ((BMP_Config->T_SB)<<5)|
			 ((BMP_Config->FILTER_COEFFICIENT)<<2)|
			 ((BMP_Config->SPI_EN));
	
	BMP280_Write_Byte(BMP280_CONFIG_REG,Regtmp);
}



BMP280_S32_t t_fine;			//用于计算补偿

#define	dig_T1			bmp280_Compensation.T1	
#define	dig_T2			bmp280_Compensation.T2	
#define	dig_T3			bmp280_Compensation.T3	

#define	dig_P1			bmp280_Compensation.P1
#define	dig_P2			bmp280_Compensation.P2
#define	dig_P3			bmp280_Compensation.P3
#define	dig_P4			bmp280_Compensation.P4
#define	dig_P5			bmp280_Compensation.P5
#define	dig_P6			bmp280_Compensation.P6
#define	dig_P7			bmp280_Compensation.P7
#define	dig_P8			bmp280_Compensation.P8
#define	dig_P9			bmp280_Compensation.P9



/**************************传感器值转定点值*************************************/
// Returns temperature in DegC, double precision. Output value of “51.23” equals 51.23 DegC.
// t_fine carries fine temperature as global value
double bmp280_compensate_T_double(BMP280_S32_t adc_T)
{
	double var1, var2, T;
	var1 = (((double)adc_T)/16384.0 - ((double)dig_T1)/1024.0) * ((double)dig_T2);
	var2 = ((((double)adc_T)/131072.0 - ((double)dig_T1)/8192.0) *
	(((double)adc_T)/131072.0 - ((double) dig_T1)/8192.0)) * ((double)dig_T3);
	t_fine = (BMP280_S32_t)(var1 + var2);
	T = (var1 + var2) / 5120.0;
	return T;
}

// Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
double bmp280_compensate_P_double(BMP280_S32_t adc_P)
{
	double var1, var2, p;
	var1 = ((double)t_fine/2.0) - 64000.0;
	var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double)dig_P5) * 2.0;
	var2 = (var2/4.0)+(((double)dig_P4) * 65536.0);
	var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0)*((double)dig_P1);
	if (var1 == 0.0)
	{
	return 0; // avoid exception caused by division by zero
	}
	p = 1048576.0 - (double)adc_P;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double)dig_P9) * p * p / 2147483648.0;
	var2 = p * ((double)dig_P8) / 32768.0;
	p = p + (var1 + var2 + ((double)dig_P7)) / 16.0;
	return p;
}





//获取BMP当前状态
//status_flag = BMP280_MEASURING ||
//			 	BMP280_IM_UPDATE
uint8_t  BMP280_GetStatus(uint8_t status_flag)
{
	uint8_t flag;
	flag = BMP280_Read_Byte(BMP280_STATUS_REG);
	if(flag&status_flag)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

uint8_t XLsb,Lsb, Msb;
//大气压值-Pa
double BMP280_Get_Pressure(void)
{
	
	long signed Bit32;
	double pressure;
	XLsb = BMP280_Read_Byte(BMP280_PRESSURE_XLSB_REG);
	Lsb	 = BMP280_Read_Byte(BMP280_PRESSURE_LSB_REG);
	Msb	 = BMP280_Read_Byte(BMP280_PRESSURE_MSB_REG);
	Bit32 = ((long)(Msb << 12))|((long)(Lsb << 4))|(XLsb>>4);	//寄存器的值,组成一个浮点数
	pressure = bmp280_compensate_P_double(Bit32);
	return pressure;
}

//温度值-℃
double BMP280_Get_Temperature(void)
{
	uint8_t XLsb,Lsb, Msb;
	long signed Bit32;
	double temperature;
	XLsb = BMP280_Read_Byte(BMP280_TEMPERATURE_XLSB_REG);
	Lsb	 = BMP280_Read_Byte(BMP280_TEMPERATURE_LSB_REG);
	Msb	 = BMP280_Read_Byte(BMP280_TEMPERATURE_MSB_REG);
	Bit32 = ((long)(Msb << 12))|((long)(Lsb << 4))|(XLsb>>4);	//寄存器的值,组成一个浮点数
	temperature = bmp280_compensate_T_double(Bit32);
	return temperature;
}



void bmp280_Init(SPI_HandleTypeDef *hspi)
{
	bmp280.hspi = hspi;	
	
	GetCompensation();
	
	BMP280_Write_Byte(BMP280_RESET_REG,BMP280_RESET_VALUE);	//往复位寄存器写入给定值	
	
	HAL_Delay(20);
	
	BMP_OVERSAMPLE_MODE			BMP_OVERSAMPLE_MODEStructure;
	BMP_OVERSAMPLE_MODEStructure.P_Osample = BMP280_P_MODE_3;
	BMP_OVERSAMPLE_MODEStructure.T_Osample = BMP280_T_MODE_1;
	BMP_OVERSAMPLE_MODEStructure.WORKMODE  = BMP280_NORMAL_MODE;
	BMP280_Set_TemOversamp(&BMP_OVERSAMPLE_MODEStructure);
	
	BMP_CONFIG					BMP_CONFIGStructure;
	BMP_CONFIGStructure.T_SB = BMP280_T_SB1;
	BMP_CONFIGStructure.FILTER_COEFFICIENT = BMP280_FILTER_MODE_4;
	BMP_CONFIGStructure.SPI_EN = ENABLE;
	BMP280_Set_Standby_FILTER(&BMP_CONFIGStructure);
	
	BMP280_Write_Byte(0x74, 0xff); //一定要加这个不然数据不会变
	
	HAL_Delay(10);
}

void bmp280_UpdateValue(void)
{
	while(BMP280_GetStatus(BMP280_MEASURING) != RESET);
	while(BMP280_GetStatus(BMP280_IM_UPDATE) != RESET);
	bmp280.temperature = BMP280_Get_Temperature();
	bmp280.pressure = BMP280_Get_Pressure();
}

double bmp280_GetTemp(void)
{
	return bmp280.temperature;
}

double bmp280_GetPress(void)
{
	return bmp280.pressure/1000.0;
}



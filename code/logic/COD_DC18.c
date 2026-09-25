#include "COD_DC18.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"
#include "heart.h"
#include "font_lib.h"
// #include "interfacial.h"
#include "lcd_drive.h"

/*获取探头Modbus通讯ID*/
void COD_DC18_rs485_GetModbusId(uint8_t modbus_id)
{
	rs485_usart.tx_buf[0] = modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x05;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x08;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_usart.tx_size = 8;
	
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}


/*获取 COD 温度 浊度*/
void COD_DC18_rs485_GetValue(PtrToDOProbe ptd,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x05;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x08;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	

	rs485_SetSentType(DO_SendType_GetTempTwoDO);
	
}

/*获取sn*/
void COD_DC18_rs485_GetSN(PtrToDOProbe ptd,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	if(modbus_id == COD_DC17_ModbusID)
	{
		rs485_usart.tx_buf[0] = COD_DC17_ModbusID;//10
		rs485_usart.tx_buf[1] = 0x03;
		rs485_usart.tx_buf[2] = 0x05;
		rs485_usart.tx_buf[3] = 0xC0;
		rs485_usart.tx_buf[4] = 0x00;
		rs485_usart.tx_buf[5] = 0x10;
	}
	else if(modbus_id == COD_DC18_ModbusID)
	{
		rs485_usart.tx_buf[0] = COD_DC18_ModbusID;
		rs485_usart.tx_buf[1] = 0x03;
		rs485_usart.tx_buf[2] = 0x05;
		rs485_usart.tx_buf[3] = 0x00;
		rs485_usart.tx_buf[4] = 0x00;
		rs485_usart.tx_buf[5] = 0x08;
	}
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSN);
	
}

/*获取软硬件版本号*/
void COD_DC18_rs485_GetSHWVersion(PtrToDOProbe ptd,uint8_t modbus_id)
{
	// if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x05;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x08;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
	
}


/*获取补偿系数U*/
void COD_DC18_rs485_Get_U(PtrToDOProbe ptd,uint8_t modbus_id)
{
	// if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x05;
	rs485_usart.tx_buf[3] = 0x10;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetKB);
	
}

/*获取测量模式和测量时间间隔*/
void COD_DC17_rs485_Get_Mes_Para(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = COD_DC17_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x05;
	rs485_usart.tx_buf[3] = 0x1D;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetMesParameter);
	
}


//浊度 零点校准
void COD_DC18_Tur_rs485_ZeroCal(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;
	Cal_Value.value_f=Value*10;	
	rs485_usart.tx_buf[0]  = modbus_id;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x05;
	rs485_usart.tx_buf[3]  = 0x85;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_SetKB);
	
}


//浊度 斜率校准
void COD_DC18_Tur_rs485_SlopeCal(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;
	Cal_Value.value_f=Value*10;	
	rs485_usart.tx_buf[0]  = modbus_id;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x05;
	rs485_usart.tx_buf[3]  = 0x86;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();	
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}


//COD  零点校准
void COD_DC18_rs485_ZeroCal(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;
	Cal_Value.value_f=Value*10;	
	rs485_usart.tx_buf[0]  = modbus_id;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x05;
	rs485_usart.tx_buf[3]  = 0x87;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();	
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}

//COD  内部浊度零点校准
void COD_DC17_rs485_IN_tur_ZeroCal(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	rs485_usart.tx_buf[0]  = COD_DC17_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x05;
	rs485_usart.tx_buf[3]  = 0x34;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10]  = Cal_Value.value_arr[2];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	rs485_SetCircularSentStatus();	
	
	rs485_SetSentType(DO_SendType_SetKB);
}


//COD  内部零点校准
void COD_DC17_rs485_INZeroCal(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	rs485_usart.tx_buf[0]  = COD_DC17_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x05;
	rs485_usart.tx_buf[3]  = 0x44;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	rs485_usart.tx_buf[7]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10]  = Cal_Value.value_arr[2];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	rs485_SetCircularSentStatus();	
	
	rs485_SetSentType(DO_SendType_SetKB);
}

//COD 斜率校准
void COD_DC18_rs485_SlopeCal(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;
	Cal_Value.value_f=Value*10;	
	rs485_usart.tx_buf[0]  = modbus_id;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x05;
	rs485_usart.tx_buf[3]  = 0x88;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();	
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}


//温度校准  写入偏移量 例如：25.8-25=0.8  0.8*100=80  写入80
void COD_DC18_rs485_SetTemp(PtrToDOProbe ptd, int16_t temp ,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	int16_u Cal_Value;
	Cal_Value.value_f=temp;	
	rs485_usart.tx_buf[0]  = modbus_id;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x05;
	rs485_usart.tx_buf[3]  = 0x07;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_SetTemp);
}

//曲线系数即浊度补偿COD的系数 范围（0~1）*100
void COD_DC18_rs485_Coefficient(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;
	Cal_Value.value_f=Value;	
	rs485_usart.tx_buf[0]  = modbus_id;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x05;
	rs485_usart.tx_buf[3]  = 0x10;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_SetTemp);
	
}


//清洗
void COD_DC18_rs485_Cleanse(PtrToDOProbe ptd,uint16_t Value,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	uint16_u Cal_Value;
	Cal_Value.value_f=Value;	
	rs485_usart.tx_buf[0]  = modbus_id;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x05;
	rs485_usart.tx_buf[3]  = 0x14;
	rs485_usart.tx_buf[4]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = Cal_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_SetTemp);
	
}


//恢复出厂设置
void COD_DC18_rs485_ClearCal(PtrToDOProbe ptd,uint8_t modbus_id)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0]  = modbus_id;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x05;
	rs485_usart.tx_buf[3]  = 0x15;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x79;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}


//设置测量模式---连续测量还是单次测量
void COD_DC17_rs485_Set_Mes(PtrToDOProbe ptd,uint16_t Value)
{
	if(ptd == NULL) return;
	uint16_u write_Value;
	write_Value.value_f=Value;	
	rs485_usart.tx_buf[0]  = COD_DC17_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x05;
	rs485_usart.tx_buf[3]  = 0x1d;
	rs485_usart.tx_buf[4]  = write_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = write_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_Set_Mes_mode);
}


//设置单次测量模式的测量间隔
void COD_DC17_rs485_Set_Mes_Time(PtrToDOProbe ptd,uint16_t Value)
{
	if(ptd == NULL) return;
	uint16_u write_Value;
	write_Value.value_f=Value;	
	rs485_usart.tx_buf[0]  = COD_DC17_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x05;
	rs485_usart.tx_buf[3]  = 0x1e;
	rs485_usart.tx_buf[4]  = write_Value.value_arr[1];
	rs485_usart.tx_buf[5]  = write_Value.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();	
	rs485_SetSentType(DO_SendType_Set_Mes_Time);
}

void COD_DC18_SetSN(PtrToDOProbe ptd,uint8_t* buff, uint8_t len)
{
	if(ptd == NULL) return;
	if(len == 0)
	{
		ptd->SN[0]='L';
		ptd->SN[1]='H';
		ptd->SN[2]='-';
		ptd->SN[3]='D';	
		ptd->SN[4]='C';
		ptd->SN[5]='1';	
		ptd->SN[6]='8';
		ptd->SN[7]='_';	
		ptd->SN[8]='C';
		ptd->SN[9]='O';	
		ptd->SN[10]='D';
		ptd->SN[11]=' ';	

	}
	else
	{
		// for(uint8_t i =0; i<len; i++)
		// {
		// 	ptd->SN[i] = *(buff + i);
		// }
		ptd->SN[0]='L';
		ptd->SN[1]='H';
		ptd->SN[2]='-';
		ptd->SN[3]='D';	
		ptd->SN[4]='C';
		ptd->SN[5]='1';	
		ptd->SN[6]='7';
		ptd->SN[7]='_';	
		ptd->SN[8]='C';
		ptd->SN[9]='O';	
		ptd->SN[10]='D';
		ptd->SN[11]=' ';	
	}		
}
void COD_DC18_SetSHWVersion(PtrToDOProbe ptd, uint8_t* data)
{
	if(ptd == NULL) return;
	if(data == NULL)
  	{	
		ptd->HWV[0] = '1';
		ptd->HWV[1] = '.';
		ptd->HWV[2] = '0';
		ptd->HWV[3] = '\0';

		ptd->SWV[0] = '1';
		ptd->SWV[1] = '.';
		ptd->SWV[2] = '0';
		ptd->SWV[3] = '\0';

	}
	else
	{
		ptd->HWV[0] = (*data) + '0';
		ptd->HWV[1] = '.';
		ptd->HWV[2] = (*(data+1))+'0';
		ptd->HWV[3] = '\0';
		
		ptd->SWV[0] = (*(data+2))+'0';
		ptd->SWV[1] = '.';
		ptd->SWV[2] = (*(data+3))+'0';
		ptd->SWV[3] = '\0';	
	}

}



void COD_DC18_Set_U(PtrToDOProbe ptd, uint8_t* data)
{
	if(ptd == NULL) return;
	if(data == NULL)
  {
    ptd->Measure_Range.value_f=50;	
	}
	else
	{		
		ptd->Measure_Range.value_arr[0]=data[1];
		ptd->Measure_Range.value_arr[1]=data[0];	
	}
}



void COD_DC17_Set_Mes_Para(PtrToDOProbe ptd, uint8_t* data)
{
	if(ptd == NULL) return;
	if(data == NULL)
  	{
    	ptd->DC17_Mes_Para.value_f=1;	
		ptd->DC17_Mes_Time.value_f=10;	
	}
	else
	{		
		ptd->DC17_Mes_Para.value_arr[0]=data[1];
		ptd->DC17_Mes_Para.value_arr[1]=data[0];
		if(ptd->DC17_Mes_Para.value_f == 0)
		{
			ptd->DC17_Mes_Para.value_f = 2;
		}
		else
		{
			ptd->DC17_Mes_Para.value_f = ptd->DC17_Mes_Para.value_f;
		}

		ptd->DC17_Mes_Time.value_arr[0]=data[3];
		ptd->DC17_Mes_Time.value_arr[1]=data[2];

		TimeValue = 0; 
		setting_Setdanciceliangtime(ptd->DC17_Mes_Time.value_f);
		if(ptd->DC17_Mes_Para.value_f == 1)//连续测量
		{
			gui_ClearMes();
		}
		else if(ptd->DC17_Mes_Para.value_f == 2)//单次测量
		{
			
		}		
		snprintf(ptd->Mes_Timearr, 4, "%3d", ptd->DC17_Mes_Time.value_f);
	}
}


void COD_DC17_UPdata_Mes_Para(PtrToDOProbe ptd, uint8_t* data)
{
	if(ptd == NULL) return;
	if(data == NULL)
  	{
    	ptd->DC17_Mes_Para.value_f=1;	
	}
	else
	{		
		ptd->DC17_Mes_Para.value_arr[0]=data[1];
		ptd->DC17_Mes_Para.value_arr[1]=data[0];
		if(ptd->DC17_Mes_Para.value_f == 0)
		{
			ptd->DC17_Mes_Para.value_f = 2;
		}
		else
		{
			ptd->DC17_Mes_Para.value_f = ptd->DC17_Mes_Para.value_f;
		}
		TimeValue = 0; 
		if(ptd->DC17_Mes_Para.value_f == 1)//连续测量
		{
			// gui_ClearMes();
		}
	}
}

	// float DO_mgl_toc = 0.0;
	float tocmgl_sum = 0.0;
void COD_DC18_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_Percent_temp = 0.0;
	float DO_mgl_temp = 0.0;	
	float DO_mgl_toc = 0.0;	
	// float tocmgl_sum = 0.0;
	uint16_u uint16_u_DOpercent;
	uint16_u uint16_u_DOmgl;
	int16_u int16_u_temperature;
	
	if(ptd == NULL) return;
	
	//浊度
	uint16_u_DOpercent.value_arr[0]=dat[1];
	uint16_u_DOpercent.value_arr[1]=dat[0];
	
	//cod
	uint16_u_DOmgl.value_arr[0]=dat[3];
	uint16_u_DOmgl.value_arr[1]=dat[2];
	
	//温度
	int16_u_temperature.value_arr[0]=dat[5];
	int16_u_temperature.value_arr[1]=dat[4];
	
	//浊度
  	ptd->DOpercent.value_f=uint16_u_DOpercent.value_f;
	ptd->DOpercent.value_f=ptd->DOpercent.value_f/10.0;

	//cod
  	ptd->DOmgl.value_f=uint16_u_DOmgl.value_f;
	ptd->DOmgl.value_f=ptd->DOmgl.value_f/10.0;

	//温度
  	ptd->temperature.value_f=int16_u_temperature.value_f;
	ptd->temperature.value_f=ptd->temperature.value_f/100.0;
	
	//TOC
	uint16_u_DOmgl.value_arr[0]=dat[11];
	uint16_u_DOmgl.value_arr[1]=dat[10];
	ptd->pH_Vol.value_f=uint16_u_DOmgl.value_f /10.0;

	//浊度校准值
	int16_u_temperature.value_arr[0]=dat[15];
	int16_u_temperature.value_arr[1]=dat[14];
	
	//浊度校准值
  	ptd->sal.value_f=int16_u_temperature.value_f;

	if(setting_Get_Temp_Unit())//温度单位为华氏度时需要做以下换算
	{
		ptd->temperature.value_f = ptd->temperature.value_f*1.8+32;//摄氏度转华氏度公式
		if(ptd->temperature.value_f > 140)
		{
			ptd->temperature.value_f = 140;
		}
		else if(ptd->temperature.value_f < 32)
		{
			ptd->temperature.value_f = 32;
		}
	}
	
	if(!setting_GetIsKey_DC18_Set())
	{
	  ptd->DOpercent.value_f=0.0;
	}
	
	if(ptd->is_FirstGetValue)//如果是第一次获取到数据的话给它一个值
	{
		ptd->is_FirstGetValue = 0;
		
		ptd->last_DOmgl = ptd->DOmgl.value_f;
		
		snprintf(ptd->temperature_arr, 6, "%5.1f", ptd->temperature.value_f);
		snprintf(ptd->DOpercent_arr,   7, "%6.1f", (ptd->DOpercent.value_f ));
		snprintf(ptd->DOmgl_arr,       7, "%6.1f", ptd->DOmgl.value_f);
		snprintf(ptd->tocmgl_Vol_arr,  7, "%6.1f", ptd->pH_Vol.value_f);
	}
	else
	{
		ptd->DOmgl_sum += ptd->DOmgl.value_f;
		ptd->DOpercent_sum += (ptd->DOpercent.value_f);
		ptd->temperature_sum += ptd->temperature.value_f;
		tocmgl_sum += ptd->pH_Vol.value_f;
		
		CheckValueLock(ptd);//自动锁定直接做在读数这里的
		
		ptd->last_DOmgl = ptd->DOmgl.value_f;//更新一下上次的值
		
		if(ptd->modbus_id == COD_DC17_ModbusID)
		{
			if(++ptd->update_count >= 3 || get_CurDo()->DC17_Mes_Para.value_f == 2)
			{
				temperature_temp = ptd->temperature_sum / ((float)ptd->update_count);
				DO_Percent_temp = ptd->DOpercent_sum / ((float)ptd->update_count);
				DO_mgl_temp = ptd->DOmgl_sum / ((float)ptd->update_count);
				DO_mgl_toc = tocmgl_sum / ((float)ptd->update_count);		
				
				if(setting_GetIsOpen_SlideAvg_COD() && setting_GetSlideAvgTimes_COD()>=2)//开启并且次数最起码为2次
				{
					if(is_FirstFilter)
					{
						is_FirstFilter = 0;
						
						filter_clear(&(ptd->queue_domgl));
						filter_clear(&(ptd->queue_dopercent));
						filter_clear(&(ptd->queue_temp));
						filter_clear(&(ptd->queue_value));
					}
					
					filter_inset2arr(&(ptd->queue_domgl), DO_mgl_temp);
					filter_inset2arr(&(ptd->queue_dopercent), DO_Percent_temp);
					filter_inset2arr(&(ptd->queue_temp), temperature_temp);
					filter_inset2arr(&(ptd->queue_value), DO_mgl_toc);

					temperature_temp  = filter_get_avg(&(ptd->queue_temp)); 
					DO_mgl_temp  = filter_get_avg(&(ptd->queue_domgl)); 
					DO_Percent_temp  = filter_get_avg(&(ptd->queue_dopercent));
					DO_mgl_toc = filter_get_avg(&(ptd->queue_value)); 
				}
				else
				{
					is_FirstFilter = 1;
				}
				
				if(ptd->DC17_Mes_Para.value_f == 1)
				{
					if(!DO_GetValueLocked(ptd))
					{
					
					snprintf(ptd->temperature_arr, 6, "%5.1f", temperature_temp);
					snprintf(ptd->DOpercent_arr,   7, "%6.1f", DO_Percent_temp);
					snprintf(ptd->DOmgl_arr,       7, "%6.1f", DO_mgl_temp);
					snprintf(ptd->tocmgl_Vol_arr,       7, "%6.1f", DO_mgl_toc);
					}
				}
				else
				{
					// if(!DO_GetValueLocked(ptd))
					// {
					// snprintf(ptd->temperature_arr, 6, "%5.1f", temperature_temp);
					// snprintf(ptd->DOpercent_arr,   7, "%6.1f", DO_Percent_temp);
					// snprintf(ptd->DOmgl_arr,       7, "%6.1f", DO_mgl_temp);
					// snprintf(ptd->tocmgl_Vol_arr,       7, "%6.1f", DO_mgl_toc);
					// }	
					if(get_SinMesFlag() >= (setting_Getdanciceliangtime() + 5))
					{
						MesRun = 0;
					}
					else
					{
						if(!DO_GetValueLocked(ptd))
						{
						snprintf(ptd->temperature_arr, 6, "%5.1f", temperature_temp);
						snprintf(ptd->DOpercent_arr,   7, "%6.1f", DO_Percent_temp);
						snprintf(ptd->DOmgl_arr,       7, "%6.1f", DO_mgl_temp);
						snprintf(ptd->tocmgl_Vol_arr,       7, "%6.1f", DO_mgl_toc);
						}
					}
				}
				
				ptd->update_count = 0;
				ptd->DOmgl_sum =0.0;
				ptd->DOpercent_sum = 0.0;
				ptd->temperature_sum = 0.0;		
				tocmgl_sum = 0.0;
				
				
			}
		}
		if(ptd->modbus_id == COD_DC18_ModbusID)
		{
			if(++ptd->update_count >= 3)
			{
				temperature_temp = ptd->temperature_sum / ((float)ptd->update_count);
				DO_Percent_temp = ptd->DOpercent_sum / ((float)ptd->update_count);
				DO_mgl_temp = ptd->DOmgl_sum / ((float)ptd->update_count);
				DO_mgl_toc = tocmgl_sum / ((float)ptd->update_count);		
				
				if(setting_GetIsOpen_SlideAvg_COD() && setting_GetSlideAvgTimes_COD()>=2)//开启并且次数最起码为2次
				{
					if(is_FirstFilter)
					{
						is_FirstFilter = 0;
						
						filter_clear(&(ptd->queue_domgl));
						filter_clear(&(ptd->queue_dopercent));
						filter_clear(&(ptd->queue_temp));
						filter_clear(&(ptd->queue_value));
					}
					
					filter_inset2arr(&(ptd->queue_domgl), DO_mgl_temp);
					filter_inset2arr(&(ptd->queue_dopercent), DO_Percent_temp);
					filter_inset2arr(&(ptd->queue_temp), temperature_temp);
					filter_inset2arr(&(ptd->queue_value), DO_mgl_toc);

					temperature_temp  = filter_get_avg(&(ptd->queue_temp)); 
					DO_mgl_temp  = filter_get_avg(&(ptd->queue_domgl)); 
					DO_Percent_temp  = filter_get_avg(&(ptd->queue_dopercent));
					DO_mgl_toc = filter_get_avg(&(ptd->queue_value)); 
				}
				else
				{
					is_FirstFilter = 1;
				}
				
				if(!DO_GetValueLocked(ptd))
				{
					snprintf(ptd->temperature_arr, 6, "%5.1f", temperature_temp);
					snprintf(ptd->DOpercent_arr,   7, "%6.1f", DO_Percent_temp);
					snprintf(ptd->DOmgl_arr,       7, "%6.1f", DO_mgl_temp);
					snprintf(ptd->tocmgl_Vol_arr,       7, "%6.1f", DO_mgl_toc);
				}					
				ptd->update_count = 0;
				ptd->DOmgl_sum =0.0;
				ptd->DOpercent_sum = 0.0;
				ptd->temperature_sum = 0.0;		
				tocmgl_sum = 0.0;
			}
		}
	}
}
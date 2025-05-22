#include "OiW_Guohong.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"

uint16_t Mes_mode,Clear_Time,Lvbo,Temp_mode;
float Temp_K_value;

/*获取探头Modbus通讯ID*/
void OiW_Guohong_Get_Modbus_ID(void)	
{
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x2B;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
}

/*获取水中油和温度和光强  */
void OiW_Guohong_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x02;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x06;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
}


/*获取0x24后面7个寄存器*/
void OiW_Guohong_rs485_GetSN(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x24;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x07;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSN);
}


/*获取0x0c后面18个寄存器*/
void OiW_Guohong_rs485_Get_Cal_value(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x0c;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x14;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetDOPercent);
}

/*设置0x24后面7个寄存器,用来显示读取*/
void OiW_Guohong_rs485_SetSN(PtrToDOProbe ptd,uint8_t *dat)
{
	if(ptd == NULL) return;
	uint16_u data_value_one;
	uint16_u data_value_two;

	//测量档位
	data_value_one.value_arr[0] = dat[1];
	data_value_one.value_arr[1] = dat[0];
	Set_OIW_Mes_Mode(data_value_one.value_f);
	//清洁周期
	data_value_one.value_arr[0] = dat[3];
	data_value_one.value_arr[1] = dat[2];
	Set_OIW_Clear_Time(data_value_one.value_f);
	snprintf(ptd->DOmgl_Vol_arr, 6, "%4d", data_value_one.value_f);
	//命令寄存器
	// data_value_one.value_arr[0] = dat[5];
	// data_value_one.value_arr[1] = dat[4];
	//数位滤波
	data_value_one.value_arr[0] = dat[7];
	data_value_one.value_arr[1] = dat[6];
	Set_OIW_Lvbo(data_value_one.value_f);

	//温度模式
	data_value_one.value_arr[0] = dat[9];
	data_value_one.value_arr[1] = dat[8];
	Set_OIW_Temp_mode(data_value_one.value_f);

	//生产批次,传感器序号
	data_value_one.value_arr[0] = dat[11];
	data_value_one.value_arr[1] = dat[10];

	data_value_two.value_arr[0] = dat[13];
	data_value_two.value_arr[1] = dat[12];
	snprintf(ptd->HWV, 5, "%d", data_value_one.value_f);
	snprintf(ptd->SWV, 5, "%f", 1.0);
	snprintf(ptd->SN, 17, "%d", data_value_two.value_f);
}


/*设置0x0c后面18个寄存器,用来显示读取*/
void OiW_Guohong_rs485_Set_Cal_info(PtrToDOProbe ptd,uint8_t *dat)
{
	if(ptd == NULL) return;
	float_u value_data_buff;

	//水中油b值
	value_data_buff.value_arr[0] =	dat[1];
	value_data_buff.value_arr[1] =	dat[0];
	value_data_buff.value_arr[2] =	dat[3];
	value_data_buff.value_arr[3] =	dat[2];
	ptd->NH4_Vol.value_f = value_data_buff.value_f;

	//温度b值
	value_data_buff.value_arr[0] =	dat[5];
	value_data_buff.value_arr[1] =	dat[4];
	value_data_buff.value_arr[2] =	dat[7];
	value_data_buff.value_arr[3] =	dat[6];
	ptd->pH_Vol.value_f = value_data_buff.value_f;

	//水中油零点系数
	value_data_buff.value_arr[0] =	dat[13];
	value_data_buff.value_arr[1] =	dat[12];
	value_data_buff.value_arr[2] =	dat[15];
	value_data_buff.value_arr[3] =	dat[14];
	ptd->compensate_k.value_f = value_data_buff.value_f;
	
	//水中油斜率系数
	value_data_buff.value_arr[0] =	dat[17];
	value_data_buff.value_arr[1] =	dat[16];
	value_data_buff.value_arr[2] =	dat[19];
	value_data_buff.value_arr[3] =	dat[18];
	ptd->compensate_b.value_f = value_data_buff.value_f;

	//水中油k值
	value_data_buff.value_arr[0] =	dat[21];
	value_data_buff.value_arr[1] =	dat[20];
	value_data_buff.value_arr[2] =	dat[23];
	value_data_buff.value_arr[3] =	dat[22];
	ptd->press.value_f = value_data_buff.value_f;

	//空白电位
	value_data_buff.value_arr[0] =	dat[29];
	value_data_buff.value_arr[1] =	dat[28];
	value_data_buff.value_arr[2] =	dat[31];
	value_data_buff.value_arr[3] =	dat[30];
	snprintf(ptd->DOpercent_Vol_arr,       7, "%6.1f", value_data_buff.value_f);

	//温度补偿系数
	value_data_buff.value_arr[0] =	dat[37];
	value_data_buff.value_arr[1] =	dat[36];
	value_data_buff.value_arr[2] =	dat[39];
	value_data_buff.value_arr[3] =	dat[38];
	snprintf(ptd->tocmgl_Vol_arr,       7, "%6.1f", value_data_buff.value_f);
	Set_OIW_Temp_value( value_data_buff.value_f);
}

/*获取探头状态*/
void OiW_Guohong_rs485_Get_Status(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_Get_Tur_Status);
}


// 水中油 标定
void OiW_Guohong_rs485_Cal(PtrToDOProbe ptd,float Value,uint8_t Cal_num)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0]  = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x00;

	if(Cal_num == 1)
	{
		rs485_usart.tx_buf[3]  = 0x2f;
		rs485_usart.tx_buf[4]  = 0x00;
		rs485_usart.tx_buf[5]  = 0x03;	
		rs485_usart.tx_buf[6]  = 0x06;
		rs485_usart.tx_buf[7]  = 0x00;
		rs485_usart.tx_buf[8]  = 0x11;
	}
	else if(Cal_num == 2)
	{
		rs485_usart.tx_buf[3]  = 0x32;
		rs485_usart.tx_buf[4]  = 0x00;
		rs485_usart.tx_buf[5]  = 0x03;	
		rs485_usart.tx_buf[6]  = 0x06;
		rs485_usart.tx_buf[7]  = 0x00;
		rs485_usart.tx_buf[8]  = 0x21;
	}
	else
	{
		rs485_usart.tx_buf[3]  = 0x35;
		rs485_usart.tx_buf[4]  = 0x00;
		rs485_usart.tx_buf[5]  = 0x03;	
		rs485_usart.tx_buf[6]  = 0x06;
		rs485_usart.tx_buf[7]  = 0x00;
		rs485_usart.tx_buf[8]  = 0x31;
	}

	rs485_usart.tx_buf[9]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[10]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[11]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[12] = Cal_Value.value_arr[2];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 15);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
}

/*设置温度模式*/
void OiW_Guohong_rs485_Set_Temp_Model(PtrToDOProbe ptd,uint8_t model)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x28;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = model;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetPressure);
}

/*设置刮刷启动*/
void OiW_Guohong_rs485_Set_Clear_move(PtrToDOProbe ptd,uint8_t model)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x26;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = model;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSalinity);
}

/*零点校准*/
void OiW_Guohong_rs485_Set_Zero_Cal(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x26;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x29;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetPressure);
}

/*单点校准*/
void OiW_Guohong_rs485_Set_OIW_B(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x0c;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	rs485_usart.tx_buf[6] = 0x04;

	rs485_usart.tx_buf[7] = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8] = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9] = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_Set_Mes_Time);
}

/*设置水中油斜率k值*/
void OiW_Guohong_rs485_Set_OIW_K(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x16;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	rs485_usart.tx_buf[6] = 0x04;

	rs485_usart.tx_buf[7] = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8] = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9] = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_Set_Mes_Time);
}

/*设置水中k恢复出厂设置*/
void OiW_Guohong_rs485_Set_OIW_K_Reset(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x16;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	rs485_usart.tx_buf[6] = 0x04;

	rs485_usart.tx_buf[7] = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8] = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9] = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}

/*b恢复出场校准*/
void OiW_Guohong_rs485_Set_OIW_B_Reset(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x0c;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	rs485_usart.tx_buf[6] = 0x04;

	rs485_usart.tx_buf[7] = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8] = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9] = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
}

/*数位滤波设置*/
void OiW_Guohong_rs485_Set_Filter_Model(PtrToDOProbe ptd,uint8_t model)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x27;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = model;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetSalinity);
}

/*清洁周期设置*/
void OiW_Guohong_rs485_Set_Clear_Model(PtrToDOProbe ptd,uint16_t model)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x25;
	rs485_usart.tx_buf[4] = model >> 8;
	rs485_usart.tx_buf[5] = model;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetSensorCap);
}

/*测量挡位设置*/
void OiW_Guohong_rs485_Set_Mes_Model(PtrToDOProbe ptd,uint8_t model)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x24;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = model;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetFullCal);
}

/*温度系数补偿设置*/
void OiW_Guohong_rs485_Set_Temp_k_Model(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	float_u Cal_Value;
	Cal_Value.value_f=Value;
	rs485_usart.tx_buf[0] = OiW_guohong_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x1e;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	rs485_usart.tx_buf[6] = 0x04;

	rs485_usart.tx_buf[7] = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[8] = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[9] = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[10] = Cal_Value.value_arr[2];

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_Set_Mes_mode);
}


//数据解析
void OiW_Guohong_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_Percent_temp = 0.0;
	float DO_mgl_temp = 0.0;

	if(ptd == NULL) return;
	
	//水中油
	ptd->DOmgl.value_arr[0] =	dat[1];
	ptd->DOmgl.value_arr[1] =	dat[0];
	ptd->DOmgl.value_arr[2] =	dat[3];
	ptd->DOmgl.value_arr[3] =	dat[2];
	
	//温度
	ptd->temperature.value_arr[0] =	dat[5];
	ptd->temperature.value_arr[1] =	dat[4];
	ptd->temperature.value_arr[2] =	dat[7];
	ptd->temperature.value_arr[3] =	dat[6];

	//MV
	ptd->DOpercent.value_arr[0] =	dat[9];
	ptd->DOpercent.value_arr[1] =	dat[8];
	ptd->DOpercent.value_arr[2] =	dat[11];
	ptd->DOpercent.value_arr[3] =	dat[10];
	
	if(ptd->is_FirstGetValue)//如果是第一次获取到数据的话给它一个值
	{
		ptd->is_FirstGetValue = 0;
		
		ptd->last_DOmgl = ptd->DOmgl.value_f;
		
		snprintf(ptd->temperature_arr, 6, "%5.1f", ptd->temperature.value_f);
		snprintf(ptd->DOpercent_arr,   7, "%6.1f", ptd->DOpercent.value_f);
		if(ptd->DOmgl.value_f >= 100.00f)
		{
			snprintf(ptd->DOmgl_arr,       7, "%6.3f", ptd->DOmgl.value_f);
		}
		else if(ptd->DOmgl.value_f >= 10.00f)
		{
			snprintf(ptd->DOmgl_arr,       7, "%6.2f", ptd->DOmgl.value_f);
		}
		else
		{
			snprintf(ptd->DOmgl_arr,       7, "%6.3f", ptd->DOmgl.value_f);
		}
	}
	else
	{
		ptd->DOmgl_sum += ptd->DOmgl.value_f;
		ptd->DOpercent_sum += (ptd->DOpercent.value_f);
		ptd->temperature_sum += ptd->temperature.value_f;
		
		CheckValueLock(ptd);//自动锁定直接做在读数这里的
		
		ptd->last_DOmgl = ptd->DOmgl.value_f;//更新一下上次的值
		
		if(++ptd->update_count >= 3)
		{
			temperature_temp = ptd->temperature_sum / ((float)ptd->update_count);
			DO_Percent_temp = ptd->DOpercent_sum / ((float)ptd->update_count);
			DO_mgl_temp = ptd->DOmgl_sum / ((float)ptd->update_count);
			
			if(setting_GetIsOpen_SlideAvg_OIW() && setting_GetSlideAvgTimes_OIW()>=2)//开启并且次数最起码为2次
			{
				if(is_FirstFilter)
				{
					is_FirstFilter = 0;
					
					filter_clear(&(ptd->queue_domgl));
					filter_clear(&(ptd->queue_dopercent));
					filter_clear(&(ptd->queue_temp));
				}
				
				filter_inset2arr(&(ptd->queue_domgl), DO_mgl_temp);
				filter_inset2arr(&(ptd->queue_dopercent), DO_Percent_temp);
				filter_inset2arr(&(ptd->queue_temp), temperature_temp);
				
				temperature_temp  = filter_get_avg(&(ptd->queue_temp)); 
				DO_mgl_temp  = filter_get_avg(&(ptd->queue_domgl)); 
				DO_Percent_temp  = filter_get_avg(&(ptd->queue_dopercent));
			}
			else
			{
				is_FirstFilter = 1;
			}
			
			if(!DO_GetValueLocked(ptd))
			{
				snprintf(ptd->temperature_arr, 6, "%5.1f", temperature_temp);
				snprintf(ptd->DOpercent_arr,   7, "%6.1f", DO_Percent_temp);

				if(DO_mgl_temp >= 100.0f)
				{
					snprintf(ptd->DOmgl_arr,       7, "%6.1f", DO_mgl_temp);
				}
				else if(DO_mgl_temp >= 10.0f)
				{
					snprintf(ptd->DOmgl_arr,       7, "%6.2f", DO_mgl_temp);
				}
				else
				{
					snprintf(ptd->DOmgl_arr,       7, "%6.3f", DO_mgl_temp);
				}
			}
			
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->DOpercent_sum = 0.0;
			ptd->temperature_sum = 0.0;		
		}
	}
}

/**
 * @brief 获取测量档位
 * 0/1:自动  2:10ppm  3:100ppm
 */  
uint16_t Get_OIW_Mes_Mode(void)
{
	return Mes_mode;
}

/**
 * @brief 设置测量档位
 * 0/1:自动  2:10ppm  3:100ppm
 */  
void Set_OIW_Mes_Mode(uint16_t value)
{
	Mes_mode = value;
}


/**
 * @brief 获取清洁周期
 * 
 */  
uint16_t Get_OIW_Clear_Time(void)
{
	return Clear_Time;
}

/**
 * @brief 设置清洁周期
 *
 */  
void Set_OIW_Clear_Time(uint16_t value)
{
	Clear_Time = value;
}

/**
 * @brief 获取数位滤波
 * 
 */  
uint16_t Get_OIW_Lvbo(void)
{
	return Lvbo;
}

/**
 * @brief 设置数位滤波
 *
 */  
void Set_OIW_Lvbo(uint16_t value)
{
	Lvbo = value;
}

/**
 * @brief 获取温度模式
 * 
 */  
uint16_t Get_OIW_Temp_mode(void)
{
	return Temp_mode;
}

/**
 * @brief 设置温度模式
 *
 */  
void Set_OIW_Temp_mode(uint16_t value)
{
	Temp_mode = value;
}

/**
 * @brief 获取温度补偿系数
 * 
 */  
float Get_OIW_Temp_value(void)
{
	return Temp_K_value;
}

/**
 * @brief 设置温度补偿系数
 *
 */  
void Set_OIW_Temp_value(float value)
{
	Temp_K_value = value;
}


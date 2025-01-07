#include "EC_shenghui.h"
#include "setting.h"
#include "calculate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtc.h"
#include "dissolved_oxygen.h"
#include "interfacial.h"

/*获取探头Modbus通讯ID*/
void EC_shenghui_rs485_GetModbusId(void)	
{
	rs485_usart.tx_buf[0] = EC_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x69;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

/*获取 电导率  温度 */
void EC_shenghui_rs485_GetValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = EC_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x69;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
	
}



/*获取sn*/
void EC_shenghui_rs485_GetSN(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = EC_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x30;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x06;

	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSN);
	
}

/*获取软硬件版本号*/
void EC_shenghui_rs485_GetSHWVersion(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = EC_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x04;
	rs485_usart.tx_buf[2] = 0x75;
	rs485_usart.tx_buf[3] = 0x45;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
	
}




// 电导率1 标定 1413μS/cm
void EC_shenghui_rs485_Frist(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	uint32_u timestamp;
	float_u Cal_Value;
	datetime_t temp_time;
	temp_time=RTC_GetTime();
	Cal_Value.value_f=Value;
	timestamp.value_f=fml_time_to_stamp(temp_time);
	rs485_usart.tx_buf[0]  = EC_shenghui_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x9c;
	rs485_usart.tx_buf[3]  = 0x5b;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x04;
	rs485_usart.tx_buf[6]  = 0x08;
 
	
	rs485_usart.tx_buf[7]  = timestamp.value_arr[1];
	rs485_usart.tx_buf[8]  = timestamp.value_arr[0];
	rs485_usart.tx_buf[9]  = timestamp.value_arr[3];
	rs485_usart.tx_buf[10] = timestamp.value_arr[2];
	
	rs485_usart.tx_buf[11]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[12]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[13]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[14] = Cal_Value.value_arr[2];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 17);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}

//电导率 2 标定
void EC_shenghui_rs485_Second(PtrToDOProbe ptd,float Value)
{
	if(ptd == NULL) return;
	uint32_u timestamp;
	float_u Cal_Value;
	datetime_t temp_time;
	temp_time=RTC_GetTime();
	Cal_Value.value_f=Value;
	timestamp.value_f=fml_time_to_stamp(temp_time);
	rs485_usart.tx_buf[0]  = EC_shenghui_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x9c;
	rs485_usart.tx_buf[3]  = 0x57;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x04;
	rs485_usart.tx_buf[6]  = 0x08;
 
	
	rs485_usart.tx_buf[7]  = timestamp.value_arr[1];
	rs485_usart.tx_buf[8]  = timestamp.value_arr[0];
	rs485_usart.tx_buf[9]  = timestamp.value_arr[3];
	rs485_usart.tx_buf[10] = timestamp.value_arr[2];
	
	rs485_usart.tx_buf[11]  = Cal_Value.value_arr[1];
	rs485_usart.tx_buf[12]  = Cal_Value.value_arr[0];
	rs485_usart.tx_buf[13]  = Cal_Value.value_arr[3];
	rs485_usart.tx_buf[14] = Cal_Value.value_arr[2];  
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 17);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}

// 标定点数
void EC_shenghui_rs485_CalNumber(PtrToDOProbe ptd, uint16_t Value)
{
	if(ptd == NULL) return;
	uint16_u timestamp;
  timestamp.value_f=Value;
	rs485_usart.tx_buf[0]  = EC_shenghui_ModbusID;
	rs485_usart.tx_buf[1]  = 0x06;
	rs485_usart.tx_buf[2]  = 0x9c;
	rs485_usart.tx_buf[3]  = 0x6b;
	rs485_usart.tx_buf[4]  = timestamp.value_arr[1];
	rs485_usart.tx_buf[5]  = timestamp.value_arr[0];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	
}


void EC_shenghui_rs485_SetTemp(PtrToDOProbe ptd, float temp)
{//设置温度
	if(ptd == NULL) return;
	ptd->temperature.value_f = temp;
	
	rs485_usart.tx_buf[0] = EC_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x9C;
	rs485_usart.tx_buf[3] = 0x59;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	rs485_usart.tx_buf[6] = 0x04;
	
	rs485_usart.tx_buf[7]  = ptd->temperature.value_arr[1];
	rs485_usart.tx_buf[8]  = ptd->temperature.value_arr[0];
	rs485_usart.tx_buf[9]  = ptd->temperature.value_arr[3];
	rs485_usart.tx_buf[10] = ptd->temperature.value_arr[2];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetTemp);
}


void EC_shenghui_rs485_ClearCal(PtrToDOProbe ptd)
{//发送清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = EC_shenghui_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x9C;
	rs485_usart.tx_buf[3] = 0x51;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01; 
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}


void EC_shenghui_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)
{
	float temperature_temp = 0.0;
	float DO_Percent_temp = 0.0;
	float DO_mgl_temp = 0.0;
  static unsigned char EC_unit_Flag=0;	
	
	if(ptd == NULL) return;
	
  ptd->DOmgl.value_arr[0] =	dat[1];
  ptd->DOmgl.value_arr[1] =	dat[0];
	ptd->DOmgl.value_arr[2] =	dat[3];
	ptd->DOmgl.value_arr[3] =	dat[2];
	
  ptd->temperature.value_arr[0] =	dat[5];
  ptd->temperature.value_arr[1] =	dat[4];
	ptd->temperature.value_arr[2] =	dat[7];
	ptd->temperature.value_arr[3] =	dat[6];	

	ptd->DOpercent.value_f=0.0;
	
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
	if(ptd->is_FirstGetValue)//如果是第一次获取到数据的话给它一个值
	{
		ptd->is_FirstGetValue = 0;
		
		ptd->last_DOmgl = ptd->DOmgl.value_f;
		
		snprintf(ptd->temperature_arr, 6, "%5.2f", ptd->temperature.value_f);
		snprintf(ptd->DOpercent_arr,   7, "%6.2f", (ptd->DOpercent.value_f ));
		//snprintf(ptd->DOmgl_arr,       7, "%5.2f", ptd->DOmgl.value_f);
		
		if(ptd->DOmgl.value_f >= 10000.0)
		{
			//snprintf(ptd->DOmgl_arr,       7, "%5.2f", ptd->DOmgl.value_f/1000.0);
			snprintf(ptd->DOmgl_arr,       7, "%d",(unsigned int)ptd->DOmgl.value_f);	
//			if(EC_unit_Flag == 0){
//				EC_unit_Flag =1;
//				rs485_SetIsChangeSenesor();
//				interfacial_ClearLabel();                                  //清所有的数据标签  不加的话当显示数据（PAGE_0_START）的两个界面切换 cur_interfacial.label_head会一直添加到链表中导致溢出，无法显示数据内容
//	    }
			
		}
		else if(ptd->DOmgl.value_f >= 1000.0)
		{
			snprintf(ptd->DOmgl_arr,       6, "%d",(unsigned int)ptd->DOmgl.value_f);		
			EC_unit_Flag =0;			
		}
		else if(ptd->DOmgl.value_f >= 100.0)
		{
			snprintf(ptd->DOmgl_arr,       7, "%5.1f",ptd->DOmgl.value_f);	
			EC_unit_Flag =0;				
		}
		else
		{
			snprintf(ptd->DOmgl_arr,       7, "%5.2f",ptd->DOmgl.value_f);
			EC_unit_Flag =0;				
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
			
			
			
			if(setting_GetIsOpen_SlideAvg_EC() && setting_GetSlideAvgTimes_EC()>=2)//开启并且次数最起码为2次
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

				snprintf(ptd->temperature_arr, 6, "%5.2f", temperature_temp);
				snprintf(ptd->DOpercent_arr,   7, "%6.2f", DO_Percent_temp);
				

				if(DO_mgl_temp >= 10000.0)
				{
				  //snprintf(ptd->DOmgl_arr,       7, "%5.2f", DO_mgl_temp/1000.0);
			    snprintf(ptd->DOmgl_arr,       7, "%d",(unsigned int)ptd->DOmgl.value_f);						
//					if(EC_unit_Flag == 0){
//						EC_unit_Flag =1;
//						rs485_SetIsChangeSenesor();
//						interfacial_ClearLabel();                                  //清所有的数据标签  不加的话当显示数据（PAGE_0_START）的两个界面切换 cur_interfacial.label_head会一直添加到链表中导致溢出，无法显示数据内容
//					}                                 //清所有的数据标签  不加的话当显示数据（PAGE_0_START）的两个界面切换 cur_interfacial.label_head会一直添加到链表中导致溢出，无法显示数据内容					
			
				}
				else if(DO_mgl_temp >= 1000.0)
				{
				  snprintf(ptd->DOmgl_arr,       6, "%d",(unsigned int)DO_mgl_temp);	
			    EC_unit_Flag =0;						
				}
				else if(DO_mgl_temp >= 100.0)
				{
				  snprintf(ptd->DOmgl_arr,       7, "%5.1f",DO_mgl_temp);		
			    EC_unit_Flag =0;						
				}
				else
			  {
				  snprintf(ptd->DOmgl_arr,       7, "%5.2f",DO_mgl_temp);		
			    EC_unit_Flag =0;						
				}

			}
			
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->DOpercent_sum = 0.0;
			ptd->temperature_sum = 0.0;		
			
			
		}
	}
}



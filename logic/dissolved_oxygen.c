#include "dissolved_oxygen.h"
#include "setting.h"
#include "DO_shenghui.h"
#include "bmp280.h"
#include "setting.h"
#include "calculate.h"
#include "interfacial.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "log.h"
#include "logic.h"
#include "DO_HaiFa_DY12.h"
#include "DO_HaiFa_DY56.h"
#include "DY05.h"
#include "DO59.h"
#include "math.h"
/////////////////////////////////////////////////////////////////////////////////////////////好像可以直接在485指令里直接改对应设备的值就不用改结构体里的值

const float press_error = 0.3;    //气压差值
const float EPSILON = 1.0E-6;


connected_probe_t cur_DO = {
	.DO_list = NULL,
	.current_sensor_type = TYPE_NONE
};

connected_probe_t comA_DO = {
	.DO_list = NULL,
	.current_sensor_type = TYPE_NONE
};

connected_probe_t comB_DO = {
	.DO_list = NULL,
	.current_sensor_type = TYPE_NONE
};

uint8_t is_FirstFilter = 1;

//#define DO_EPS 0.0145//0.015

const float DO_AutoLock_eps[3] = {0.1, 0.3, 0.5};//{0.03, 0.05, 0.1};

const float ORP_AutoLock_eps[3] = {1, 3, 5};//{0.03, 0.05, 0.1};

const float DO_702AutoLock_eps[3] = {50, 40, 30};//{0.03, 0.05, 0.1};
//要不要按照modbusid 来对设备进行操作 就是先find一下然后如果没找到重新创建一个

/*获取当前显示设备的指针*/
PtrToDOProbe get_CurDo(void)
{
	return cur_DO.DO_list;
}


/*获取COMA 连接设备的指针*/
PtrToDOProbe get_COMADo(void)
{
	return comA_DO.DO_list;
}

/*获取COMB 连接设备的指针*/
PtrToDOProbe get_COMBDo(void)
{
	return comB_DO.DO_list;
}




void DO_ClearCurDO(void)
{
	cur_DO.DO_list = NULL;
	cur_DO.current_sensor_type=TYPE_NONE;
}
void DO_ClearCOMADO(void)
{
	comA_DO.DO_list = NULL;
	comA_DO.current_sensor_type=TYPE_NONE;
}
void DO_ClearCOMBDO(void)
{
	comB_DO.DO_list = NULL;
	comB_DO.current_sensor_type=TYPE_NONE;
}


uint8_t DO_GetValueLocked(PtrToDOProbe ptd)
{
	return ptd->is_ValueLocked;
}
void DO_SetValueUnlocked(PtrToDOProbe ptd)
{
	ptd->is_ValueLocked = 0;
}
void DO_SetValueLocked(PtrToDOProbe ptd)
{
	ptd->is_ValueLocked = 1;
}





void DO_zero_buf_mgl(PtrToDOProbe p,uint8_t modbus_Id)
{
	p->tocmgl_Vol_arr[0] = ' ';
	p->tocmgl_Vol_arr[1] = ' ';
	p->tocmgl_Vol_arr[2] = '0';
	p->tocmgl_Vol_arr[3] = '.';
	p->tocmgl_Vol_arr[4] = '0';
	p->tocmgl_Vol_arr[5] = '0';
	p->tocmgl_Vol_arr[6] = '\0';
	if(modbus_Id == Chl_shenghui_ModbusID)
	{
		p->DOmgl_arr[0] = ' ';
		p->DOmgl_arr[1] = ' ';
		p->DOmgl_arr[2] = '0';
		p->DOmgl_arr[3] = '.';
		p->DOmgl_arr[4] = '0';
		p->DOmgl_arr[5] = '\0';
		p->DOmgl_arr[6] = '\0';
	}
	else
	{
		p->DOmgl_arr[0] = ' ';
		p->DOmgl_arr[1] = ' ';
		p->DOmgl_arr[2] = '0';
		p->DOmgl_arr[3] = '.';
		p->DOmgl_arr[4] = '0';
		p->DOmgl_arr[5] = '0';
		p->DOmgl_arr[6] = '\0';
	}
	if(modbus_Id == ORP_DR31_ModbusID)
	{
		p->DOmgl_arr[0] = ' ';
		p->DOmgl_arr[1] = ' ';
		p->DOmgl_arr[2] = ' ';
		p->DOmgl_arr[3] = ' ';
		p->DOmgl_arr[4] = '0';
		p->DOmgl_arr[5] = '\0';
		p->DOmgl_arr[6] = '\0';
	}
	if(modbus_Id == MLSS_Tianjian_ModbusID)
	{
		p->DOpercent_Vol_arr[0] = ' ';
		p->DOpercent_Vol_arr[1] = ' ';
		p->DOpercent_Vol_arr[2] = ' ';
		p->DOpercent_Vol_arr[3] = '0';
		p->DOpercent_Vol_arr[4] = '\0';
		p->DOpercent_Vol_arr[5] = '\0';
		p->DOpercent_Vol_arr[6] = '\0';
		p->DOpercent_Vol_arr[7] = '\0';
	}
}

void DO_zero_buf_percent(PtrToDOProbe p)
{
	p->DOpercent_arr[0] = ' ';
	p->DOpercent_arr[1] = ' ';
	p->DOpercent_arr[2] = '0';
	p->DOpercent_arr[3] = '.';
	p->DOpercent_arr[4] = '0';
	p->DOpercent_arr[5] = '0';
	p->DOpercent_arr[6] = '\0';
}

void DO_AddProbe(uint8_t ModbusId)//这里得添加名字
{  
  SENSOR_TYPE add_Type=TYPE_NONE;
	PtrToDOProbe p = NULL;
	p = (PtrToDOProbe)malloc(sizeof(DOProbe_t));//分配空间
	
	if(p == NULL)//安全性判断
	{
		return;
	}
    //将新节点指针指向新生成的节点方便后面修改添加设备
	
	memset(&(p->queue_domgl), 0, sizeof(filter_t));
	memset(&(p->queue_dopercent), 0, sizeof(filter_t));
	memset(&(p->queue_temp), 0, sizeof(filter_t));
	switch(ModbusId)
	{
		case DO_shenghui_ModbusID:
		case DO_HF1012_ModbusID:
		case DO_HF_DY56_ModbusID:
		case DO_HF_DY12_ModbusID:
		case DO_DY05_ModbusID:
		case DO_DO59_ModbusID:
			snprintf(p->name, 6, "DO %02d", ModbusId); //生成名字
		  add_Type=TYPE_DO;
			if(setting_GetIsOpen_SlideAvg_DO())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_DO());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_DO());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_DO());       //初始化一下℃    数值指针
			}
			break;
		
		case pH_DpH07_ModbusID :
		case pH_shenghui_ModbusID :
			snprintf(p->name, 6, "pH %02d", ModbusId); //生成名字
		  add_Type=TYPE_pH;
			if(setting_GetIsOpen_SlideAvg_pH())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_pH());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_pH());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_pH());       //初始化一下℃    数值指针
			}
			break;
		
		case Tur_shenghui_ModbusID:
		case ZS_DZ09_ModbusID:
			snprintf(p->name, 7, "Tur %02d", ModbusId); //生成名字
		  add_Type=TYPE_Tur;
			if(setting_GetIsOpen_SlideAvg_Tur())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_Tur());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_Tur());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_Tur());       //初始化一下℃    数值指针
			}
			break;

		case EC_shenghui_ModbusID:		
		case EC_DE26_ModbusID:
		case EC_DE40_ModbusID:
			snprintf(p->name, 7, "EC %02d", ModbusId); //生成名字
		  add_Type=TYPE_EC;
			if(setting_GetIsOpen_SlideAvg_EC())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_EC());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_EC());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_EC());       //初始化一下℃    数值指针
			}
			break;

		case EC_DS46_ModbusID:
			snprintf(p->name, 7, "SAL %02d", ModbusId); //生成名字
		  add_Type=TYPE_SAL;
			if(setting_GetIsOpen_SlideAvg_SAL())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_SAL());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_SAL());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_SAL());       //初始化一下℃    数值指针
			}
			break;
		
		case ORP_DR31_ModbusID:
			snprintf(p->name, 7, "ORP %02d", ModbusId); //生成名字
		  add_Type=TYPE_ORP;
			if(setting_GetIsOpen_SlideAvg_ORP())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_ORP());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_ORP());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_ORP());       //初始化一下℃    数值指针
			}
			break;
		
		case NH3N_DN02_ModbusID :
		case NH3N_shenghui_ModbusID :
			snprintf(p->name, 8, "NH3N %02d", ModbusId); //生成名字
		  add_Type=TYPE_NH4;
			if(setting_GetIsOpen_SlideAvg_NH4())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_NH4());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_NH4());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_NH4());       //初始化一下℃    数值指针
			}
			break;	
		
		case Chl_shenghui_ModbusID:
			snprintf(p->name, 7, "Chl %02d", ModbusId); //生成名字
		  add_Type=TYPE_Chl;
			if(setting_GetIsOpen_SlideAvg_Chl())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_Chl());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_Chl());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_Chl());       //初始化一下℃    数值指针
			}
			break;
		
		case Bga_shenghui_ModbusID:
			snprintf(p->name, 7, "Bga %02d", ModbusId); //生成名字
		  add_Type=TYPE_Bga;
			if(setting_GetIsOpen_SlideAvg_Bga())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_Bga());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_Bga());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_Bga());       //初始化一下℃    数值指针
			}
			break;
		
		case COD_DC17_ModbusID :
		case COD_DC18_ModbusID :
		case COD_shenghui_ModbusID :
			snprintf(p->name, 7, "COD %02d", ModbusId); //生成名字
		  add_Type=TYPE_CODuv;
			if(setting_GetIsOpen_SlideAvg_COD())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_COD());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_COD());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_COD());       //初始化一下℃    数值指针
				filter_init(&(p->queue_value), setting_GetSlideAvgTimes_COD());       //初始化一下℃    数值指针
			}
			break;
			
		case MLSS_Tianjian_ModbusID:
			snprintf(p->name, 8, "MLSS %02d", ModbusId); //生成名字
		  add_Type=TYPE_MLSS;
			if(setting_GetIsOpen_SlideAvg_MLSS())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_MLSS());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_MLSS());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_MLSS());       //初始化一下℃    数值指针
							
			}
			break;

		case OiW_guohong_ModbusID:
		case OiW_yushan_ModbusID:
			snprintf(p->name, 8, "OiW %02d", ModbusId); //生成名字
		  add_Type=TYPE_Oiw;
			if(setting_GetIsOpen_SlideAvg_OIW())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_OIW());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_OIW());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_OIW());       //初始化一下℃    数值指针			
			}
			break;

		case OiW_yushan_DA511_ModbusID:
			snprintf(p->name, 8, "OiW %02d", ModbusId); //生成名字
		  add_Type=TYPE_Oiw;
			if(setting_GetIsOpen_SlideAvg_OIW_ppm())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_OIW_ppm());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_OIW_ppm());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_OIW_ppm());       //初始化一下℃    数值指针			
			}
			break;

		case LH_DX01_ModbusID:
			snprintf(p->name, 8, "TSS %02d", ModbusId); //生成名字
		  add_Type=TYPE_TSS;
			if(setting_GetIsOpen_SlideAvg_TSS())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_TSS());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_TSS());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_TSS());       //初始化一下℃    数值指针			
			}
			break;

		case TDS_DT49_Modbus:
			snprintf(p->name, 8, "TDS %02d", ModbusId); //生成名字
		  add_Type=TYPE_TDS;
			if(setting_GetIsOpen_SlideAvg_TDS())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_TDS());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_TDS());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_TDS());       //初始化一下℃    数值指针			
			}
			break;
			
		case Cl_DL312_ModbusID:
			snprintf(p->name, 8, "CL %02d", ModbusId); //生成名字
		  add_Type=TYPE_CL;
			if(setting_GetIsOpen_SlideAvg_CL())
			{//如果开启了滑动平均就直接添加下 没开的话就等开的时候再初始化
				filter_init(&(p->queue_domgl), setting_GetSlideAvgTimes_CL());      //初始化一下mg/l 数值指针
				filter_init(&(p->queue_dopercent), setting_GetSlideAvgTimes_CL());  //初始化一下%    数值指针
				filter_init(&(p->queue_temp), setting_GetSlideAvgTimes_CL());       //初始化一下℃    数值指针			
			}
			break;
			
		default:
			break;
		
	}
	
	p->SN[12] = '\0';
	p->is_init = 0;                            //设置成没有初始化
	p->is_GetedValue = 0;                      //设置成没有获取到数据
	
	p->is_FirstGetValue = 1;
	
	p->is_ValueLocked = 0;
	
	p->modbus_id = ModbusId;
	
	//传感器的值初始化一下都成0
	p->DOmgl.value_f = 0.0;
	p->DOpercent.value_f = 0.0;
	p->temperature.value_f = 0.0;
	
	//显示buff初始化一下全都显示成0
		//显示buff初始化一下全都显示成0
	if (ModbusId == Bga_shenghui_ModbusID)
	{
		p->DOmgl_arr[0] = '0';
		p->DOmgl_arr[1] = '.';
		p->DOmgl_arr[2] = '0';
		p->DOmgl_arr[3] = '0';
		p->DOmgl_arr[4] = '\0';
	}
	else
	{
		DO_zero_buf_mgl(p,ModbusId);
		DO_zero_buf_percent(p);
	}
	
	p->temperature_arr[0] = ' ';
	p->temperature_arr[1] = '0';
	p->temperature_arr[2] = '.';
	p->temperature_arr[3] = '0';
	p->temperature_arr[4] = '0';
	p->temperature_arr[5] = '\0';
	if(cur_DO.DO_list == NULL)
	{
	 	comA_DO.DO_list=p;
	 	cur_DO.DO_list = comA_DO.DO_list; 
   		comA_DO.current_sensor_type=add_Type;		
	 	cur_DO.current_sensor_type=add_Type;		
	}
	else
	{
		comB_DO.DO_list=p; 		
   		comB_DO.current_sensor_type=add_Type;			
	}	
}




/*通过名字查找溶解氧设备并返回指针*/
PtrToDOProbe DO_FindByName(uint8_t* name, PtrToDOProbe *DO_head)
{
	PtrToDOProbe p = (*DO_head);
	if((*DO_head) == NULL)
	{
		return NULL;
	}
	return p;
}


/*获取探头Modbus通讯ID*/
void DO_rs485_GetModbusId(void)
{
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
//	rs485_usart.tx_buf[1] = 0x03;
//	rs485_usart.tx_buf[2] = 0x30;
//	rs485_usart.tx_buf[3] = 0x00;
//	rs485_usart.tx_buf[4] = 0x00;
//	rs485_usart.tx_buf[5] = 0x01;
//	rs485_usart.tx_buf[6] = 0x9E;
//	rs485_usart.tx_buf[7] = 0xD4;
//	rs485_usart.tx_size = 8;
	
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x26;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x06;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetModbusId);
}


/*获取温度 DO% DOmg/L*/
void DO_rs485_GetTempTwoDO(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x26;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x06;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
	
}


/*设置电机的ModbusID*/
void DO_rs485_SetAddr(PtrToDOProbe ptd, uint8_t NewId) //这里要校验一下是不是写进去了，然后要将这个结构体里面的modbus id 也改掉
{
	if(ptd == NULL) return;
	uint8_t temp = 0;
	
	temp = NewId>>4;        //更改设备名字
	ptd->name[3] = ((temp>=10) ? ((temp - 10) + 'A'): (temp+'0'));
	temp = NewId & 0x0f;
	ptd->name[4] = ((temp>=10) ? ((temp - 10) + 'A') : (temp+'0'));
	
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x30;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	rs485_usart.tx_buf[6] = 0x02;
	rs485_usart.tx_buf[7] = NewId;
	rs485_usart.tx_buf[8] = 0x00;
	
	SetCrc(	rs485_usart.tx_buf, rs485_usart.tx_size = 11);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetAddr);
	
	ptd->new_ModbusID = NewId; //更改设备id

}


/*获取sn*/
void DO_rs485_GetSN(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x09;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x07;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSN);
	

}




/*获取温度*/
void DO_rs485_GetTemperature(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x26;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetTemperature);
	

}

/*获取DO %*/
void DO_rs485_GetDOPercent(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x26;
	rs485_usart.tx_buf[3] = 0x02;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	
	rs485_SetSentType(DO_SendType_GetDOPercent);
	

}
/*获取DO mg/L*/
void DO_rs485_GetDOmgL(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x26;
	rs485_usart.tx_buf[3] = 0x04;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetSentType(DO_SendType_GetDOmgL);
	

}

/*获取软硬件版本号*/
void DO_rs485_GetSHWVersion(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x07;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
	
}


/*获取用户校准参数*/
void DO_rs485_GetKB(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x11;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x04;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetKB);
	

}

void DO_rs485_SetKBValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0]  = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x11;
	rs485_usart.tx_buf[3]  = 0x00;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x04;
	rs485_usart.tx_buf[6]  = 0x08;
	
	rs485_usart.tx_buf[7]  = ptd->compensate_k.value_arr[0];
	rs485_usart.tx_buf[8]  = ptd->compensate_k.value_arr[1];
	rs485_usart.tx_buf[9]  = ptd->compensate_k.value_arr[2];
	rs485_usart.tx_buf[10] = ptd->compensate_k.value_arr[3];
	
	rs485_usart.tx_buf[11] = ptd->compensate_b.value_arr[0];
	rs485_usart.tx_buf[12] = ptd->compensate_b.value_arr[1];
	rs485_usart.tx_buf[13] = ptd->compensate_b.value_arr[2];
	rs485_usart.tx_buf[14] = ptd->compensate_b.value_arr[3];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 17);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetKB);
	

}


//0x01	0x10	0x11   	0x00	0x00	0x04	0x08
/*设置校准参数*/
void DO_rs485_SetKB(PtrToDOProbe ptd, float k, float b)
{
	ptd->compensate_k.value_f = k;
	ptd->compensate_b.value_f = b;
	DO_rs485_SetKBValue(ptd);
}



void DO_rs485_SetK(PtrToDOProbe ptd, float k)
{
	ptd->compensate_k.value_f = k;
	DO_rs485_SetKBValue(ptd);
}

void DO_rs485_SetB(PtrToDOProbe ptd, float b)
{
	ptd->compensate_b.value_f = b;
	DO_rs485_SetKBValue(ptd);
}





//0x01	0x10	0x27   	0x00	0x00	0x10	0x20	K0~ K7
void DO_rs485_SetSensorCap(PtrToDOProbe ptd, SensorCap_t *sc)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0]  = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x27;
	rs485_usart.tx_buf[3]  = 0x00;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x10;
	rs485_usart.tx_buf[6]  = 0x20;
	
	rs485_usart.tx_buf[7]  = sc->k0[0];
	rs485_usart.tx_buf[8]  = sc->k0[1];
	rs485_usart.tx_buf[9]  = sc->k0[2];
	rs485_usart.tx_buf[10] = sc->k0[3];
	
	rs485_usart.tx_buf[11] = sc->k1[0];
	rs485_usart.tx_buf[12] = sc->k1[1];
	rs485_usart.tx_buf[13] = sc->k1[2];
	rs485_usart.tx_buf[14] = sc->k1[3];
	
	rs485_usart.tx_buf[15] = sc->k2[0];
	rs485_usart.tx_buf[16] = sc->k2[1];
	rs485_usart.tx_buf[17] = sc->k2[2];
	rs485_usart.tx_buf[18] = sc->k2[3];
	
	rs485_usart.tx_buf[19] = sc->k3[0];
	rs485_usart.tx_buf[20] = sc->k3[1];
	rs485_usart.tx_buf[21] = sc->k3[2];
	rs485_usart.tx_buf[22] = sc->k3[3];

	rs485_usart.tx_buf[23] = sc->k4[0];
	rs485_usart.tx_buf[24] = sc->k4[1];
	rs485_usart.tx_buf[25] = sc->k4[2];
	rs485_usart.tx_buf[26] = sc->k4[3];
	
	rs485_usart.tx_buf[27] = sc->k5[0];
	rs485_usart.tx_buf[28] = sc->k5[1];
	rs485_usart.tx_buf[29] = sc->k5[2];
	rs485_usart.tx_buf[30] = sc->k5[3];
	
	rs485_usart.tx_buf[31] = sc->k6[0];
	rs485_usart.tx_buf[32] = sc->k6[1];
	rs485_usart.tx_buf[33] = sc->k6[2];
	rs485_usart.tx_buf[34] = sc->k6[3];
	
	rs485_usart.tx_buf[35] = sc->k7[0];
	rs485_usart.tx_buf[36] = sc->k7[1];
	rs485_usart.tx_buf[37] = sc->k7[2];
	rs485_usart.tx_buf[38] = sc->k7[3];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 41);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetSensorCap);
}
void DO_rs485_GetSensorCap(PtrToDOProbe ptd)//获取帽膜设置
{
	
}




void DO_rs485_GetSalinity(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x15;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSalinity);
	

}

//0x01	0x10	0x15   	0x00	0x00	0x02	0x04
void DO_rs485_SetSalinity(PtrToDOProbe ptd, float sal)
{
	if(ptd == NULL) return;
	ptd->sal.value_f = sal;
	
	rs485_usart.tx_buf[0]  = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x15;
	rs485_usart.tx_buf[3]  = 0x00;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	
	rs485_usart.tx_buf[7]  = ptd->sal.value_arr[0];
	rs485_usart.tx_buf[8]  = ptd->sal.value_arr[1];
	rs485_usart.tx_buf[9]  = ptd->sal.value_arr[2];
	rs485_usart.tx_buf[10] = ptd->sal.value_arr[3];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetSalinity);
}

void DO_rs485_GetPressure(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x24;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetPressure);
	

}
//0x01	0x10	0x24   	0x00	0x00	0x02	0x04
void DO_rs485_SetPressure(PtrToDOProbe ptd, float press)
{
	if(ptd == NULL) return;
	ptd->press.value_f = press;
	
	rs485_usart.tx_buf[0]  = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1]  = 0x10;
	rs485_usart.tx_buf[2]  = 0x24;
	rs485_usart.tx_buf[3]  = 0x00;
	rs485_usart.tx_buf[4]  = 0x00;
	rs485_usart.tx_buf[5]  = 0x02;
	rs485_usart.tx_buf[6]  = 0x04;
	
	rs485_usart.tx_buf[7]  = ptd->press.value_arr[0];
	rs485_usart.tx_buf[8]  = ptd->press.value_arr[1];
	rs485_usart.tx_buf[9]  = ptd->press.value_arr[2];
	rs485_usart.tx_buf[10] = ptd->press.value_arr[3];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetPressure);
}

void DO_rs485_SetTemp(PtrToDOProbe ptd, float temp)
{//海发传感器设置温度
	if(ptd == NULL) return;
	ptd->temperature.value_f = temp;
	
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x76;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	rs485_usart.tx_buf[6] = 0x04;
	
	rs485_usart.tx_buf[7]  = ptd->temperature.value_arr[0];
	rs485_usart.tx_buf[8]  = ptd->temperature.value_arr[1];
	rs485_usart.tx_buf[9]  = ptd->temperature.value_arr[2];
	rs485_usart.tx_buf[10] = ptd->temperature.value_arr[3];
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 13);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetTemp);
}

void DO_rs485_hyphiveClearCal(PtrToDOProbe ptd)
{//发送海发溶解氧清除所有校准参数
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = DO_HF1012_ModbusID;
	rs485_usart.tx_buf[1] = 0x06;
	rs485_usart.tx_buf[2] = 0x00;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0xFC;
	rs485_usart.tx_buf[5] = 0xFC;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_HyphiveClearCal);
}

void DO_SetSN(PtrToDOProbe ptd, uint8_t* buff, uint8_t len)
{
	if(ptd == NULL) return;
	for(uint8_t i =0; i<len; i++)
	{
		ptd->SN[i] = *(buff + i);
	}
	switch(ptd->SN[0])
	{
		case 'Y':
			if(ptd->SN[1] == 'L' && ptd->SN[1] == '0' && ptd->SN[1] == '1')
			{
				ptd->manufacturer = manufacturer_yosemitech;//生产厂商是禹山
			}
			break;
		
		case 'H':
			if(ptd->SN[1] == 'F')
			{
				ptd->manufacturer = manufacturer_hyphive;//生产厂商是海发
			}
			break;
	}
	
	
}
void DO_SetSHWVersion(PtrToDOProbe ptd, uint8_t* data)
{
	if(ptd == NULL) return;
	ptd->HWV[0] = (*data) + '0';
	ptd->HWV[1] = '.';
	ptd->HWV[2] = (*(data+1))+'0';
	ptd->HWV[3] = '\0';
	
	ptd->SWV[0] = (*(data+2))+'0';
	ptd->SWV[1] = '.';
	ptd->SWV[2] = (*(data+3))+'0';
	ptd->SWV[3] = '\0';
	
}
void DO_SetKBArr(PtrToDOProbe ptd, uint8_t* data)
{
	if(ptd == NULL) return;
	uint8_t i;
	for(i = 0; i < 4; i++)
	{
		ptd->compensate_k.value_arr[i] = *(data+i);
	}
	data += 4;
	for(i = 0; i < 4; i++)
	{
		ptd->compensate_b.value_arr[i] = *(data+i);
	}
}
void DO_SetKBFloat(PtrToDOProbe ptd, float k, float b)
{
	ptd->compensate_k.value_f = k;
	ptd->compensate_b.value_f = b;
}

void DO_SetKFloat(PtrToDOProbe ptd, float k)
{
	if(ptd == NULL) return;
	ptd->compensate_k.value_f = k;
}
float DO_GetKFloat(PtrToDOProbe ptd)
{
	return ptd->compensate_k.value_f;
}


void DO_SetBFloat(PtrToDOProbe ptd, float b)
{
	if(ptd == NULL) return;
	ptd->compensate_b.value_f = b;
}
float DO_GetBFloat(PtrToDOProbe ptd)
{
	return ptd->compensate_b.value_f;
}


void DO_SetPressureArr(PtrToDOProbe ptd, uint8_t* dat)
{
	if(ptd == NULL) return;
	
	ptd->press.value_arr[0] = *dat;
	ptd->press.value_arr[1] = *(dat+1);
	ptd->press.value_arr[2] = *(dat+2);
	ptd->press.value_arr[3] = *(dat+3);
}
float DO_GetPressure(PtrToDOProbe ptd)
{
	return ptd->press.value_f;
}

void DO_SetSalinityArr(PtrToDOProbe ptd, uint8_t* dat)
{
	if(ptd == NULL) return;
	
	ptd->sal.value_arr[0] = *dat;
	ptd->sal.value_arr[1] = *(dat+1);
	ptd->sal.value_arr[2] = *(dat+2);
	ptd->sal.value_arr[3] = *(dat+3);
}
float DO_GetSalinity(PtrToDOProbe ptd)
{
	return ptd->sal.value_f;
}

uint8_t DO_GetIsInit(PtrToDOProbe ptd)
{
	return ptd->is_init;
}
void DO_SetIsInit(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	ptd->is_init = 1;
}

uint8_t DO_GetIsGetedValue(PtrToDOProbe ptd)
{
	return ptd->is_GetedValue;
}
void DO_SetIsGetedValue(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	ptd->is_GetedValue = 1;
}

float DO_GetDOPercent(PtrToDOProbe ptd)
{
	return ptd->DOpercent.value_f;
}

float DO_GetDOmgL(PtrToDOProbe ptd)
{
	return ptd->DOmgl.value_f;
}

float DO_GetTemperature(PtrToDOProbe ptd)
{
	return ptd->temperature.value_f;
}







//static uint8_t shake_count = 0;

void clear_DOShakeCount(void)
{
	get_CurDo()->shake_count = 0;
}


uint8_t lock_num=0;//锁定计数值
void CheckValueLock(PtrToDOProbe ptd)
{
	float eps = 0.0;
	uint8_t GetAutoLock_Flag=0;
	double difference = 0.0;//差值
	uint8_t lock_max =0;

	if(interfacial_GetCurPage() == PAGE_5_DO_ONE_First|| 
		 interfacial_GetCurPage() == PAGE_5_DO_TWO_FIRST||
		 interfacial_GetCurPage() == PAGE_5_NH3N_pH_ONE ||
	   interfacial_GetCurPage() == PAGE_5_NH3N_pH_TWO || 
	   interfacial_GetCurPage() == PAGE_5_NH3N_pH_THREE||
		 interfacial_GetCurPage() == PAGE_5_shenghui_Tur_ONE|| 
	   interfacial_GetCurPage() == PAGE_5_shenghui_Tur_TWO|| 
	   interfacial_GetCurPage() == PAGE_5_shenghui_Tur_THREE||    
		 interfacial_GetCurPage() == PAGE_5_DE26_EC_Zero|| 
	   interfacial_GetCurPage() == PAGE_5_shenghui_EC_ONE||
		 interfacial_GetCurPage() == PAGE_5_NH3N_ONE|| 
	   interfacial_GetCurPage() == PAGE_5_NH3N_TWO||
		 interfacial_GetCurPage() == PAGE_5_COD_DC18_Tur_Zero|| 
		interfacial_GetCurPage() ==  PAGE_5_COD_DC17_IN_Tur_Zero||
	   interfacial_GetCurPage() == PAGE_5_COD_DC18_Tur_Slope|| 
	   interfacial_GetCurPage() == PAGE_5_COD_DC18_Zero|| 
	   interfacial_GetCurPage() == PAGE_5_COD_DC17_IN_Zero||
	   interfacial_GetCurPage() == PAGE_5_COD_DC18_Slope||
		 interfacial_GetCurPage() == PAGE_5_COD_shenghui_Zero|| 
	   interfacial_GetCurPage() == PAGE_5_COD_shenghui_Tur_ONE|| 
	   interfacial_GetCurPage() == PAGE_5_COD_shenghui_Tur_TWO|| 
     interfacial_GetCurPage() == PAGE_5_COD_shenghui_ONE||
	   interfacial_GetCurPage() == PAGE_5_COD_shenghui_TWO||	
	   interfacial_GetCurPage() == PAGE_5_COD_shenghui_THREE||
	   interfacial_GetCurPage() == PAGE_5_shenghui_BGA_ONE||
		interfacial_GetCurPage() == PAGE_5_shenghui_BGA_TWO||
	   interfacial_GetCurPage() == PAGE_5_DR31_ORP_ONE|| 
	   interfacial_GetCurPage() == PAGE_5_MLSS_zero_signal
		)   //在具体校准界面中，锁定功能失效
	{
		clear_DOShakeCount();              //清除抖动计数
		if(DO_GetValueLocked(get_CurDo()))
		{
			DO_SetValueUnlocked(get_CurDo());  //解锁
		}	
	}
	else
  	{
		switch(rs485_GetSensorType())
		{
			case TYPE_DO://如果当前查询的设备是do的话
				eps = DO_702AutoLock_eps[setting_GetAutoLockLevel_DO()];
				GetAutoLock_Flag=setting_GetAutoLock_DO();
				break;		
			case TYPE_pH:	
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_pH()];
				GetAutoLock_Flag=setting_GetAutoLock_pH();					
				break;
			case TYPE_Tur:	
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_Tur()];
				GetAutoLock_Flag=setting_GetAutoLock_Tur();	
				break;
			case TYPE_FCL:		
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_FCL()];
				GetAutoLock_Flag=setting_GetAutoLock_FCL();	
				break;
			case TYPE_EC:	
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_EC()];
				GetAutoLock_Flag=setting_GetAutoLock_EC();						
				break;
			case TYPE_ORP:	
				eps = ORP_AutoLock_eps[setting_GetAutoLockLevel_ORP()];
				GetAutoLock_Flag=setting_GetAutoLock_ORP();	
				break;
			case TYPE_NH4:
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_NH4()];
				GetAutoLock_Flag=setting_GetAutoLock_NH4();	
				break;
			case TYPE_F:	
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_F()];			
				GetAutoLock_Flag=setting_GetAutoLock_F();	
				break;
			case TYPE_CL:
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_CL()];
				GetAutoLock_Flag=setting_GetAutoLock_CL();	
				break;
			case TYPE_Chl:
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_Chl()];
				GetAutoLock_Flag=setting_GetAutoLock_Chl();						
				break;
			case TYPE_Bga:
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_Bga()];
				GetAutoLock_Flag=setting_GetAutoLock_Bga();						
				break;
			case TYPE_CODuv:
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_COD()];
				GetAutoLock_Flag=setting_GetAutoLock_COD();						
				break;
			case TYPE_MLSS:
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_MLSS()];
				GetAutoLock_Flag=setting_GetAutoLock_MLSS();	
				break;
			case TYPE_Oiw:
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_OIW()];
				GetAutoLock_Flag=setting_GetAutoLock_OIW();	
				break;
			case TYPE_TSS:
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_TSS()];
				GetAutoLock_Flag=setting_GetAutoLock_TSS();	
				break;
			case TYPE_SAL:
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_SAL()];
				GetAutoLock_Flag=setting_GetAutoLock_SAL();	
				break;
			case TYPE_TDS:
				eps = DO_AutoLock_eps[setting_GetAutoLockLevel_TDS()];
				GetAutoLock_Flag=setting_GetAutoLock_TDS();	
				break;
			default:
				break;
		}					
		
		if(GetAutoLock_Flag == AUTOLOCK_AUTO)//如果开启了自动锁定功能
		{
			if(rs485_GetSensorType() ==TYPE_DO)
			{			
				if(get_CurDo()->modbus_id == DO_DY05_ModbusID || get_CurDo()->modbus_id == DO_DO59_ModbusID)
				{
					difference = ptd->DOmgl.value_f - ptd->last_DOmgl;
					if(fabs(difference) <= 0.015)
					{
						ptd->shake_count++;
					}
					lock_num++;
					setting_SetAutoLock_num(lock_num);

					if(setting_AutoLock_num() == eps || fabs(difference) > 0.06)
					{
						lock_num =0;
						setting_SetAutoLock_num(0);
						ptd->shake_count =0;
					}
          lock_max= eps*2/4;									
				}				
				else
				{
					difference = ptd->DOpercent.value_f - ptd->last_DOmgl;//取差值
					if(fabs(difference) <= 0.0015)
					{
						ptd->shake_count++;
					}
					lock_num++;
					setting_SetAutoLock_num(lock_num);

					if(setting_AutoLock_num() == eps || fabs(difference) > 0.005)
					{
						lock_num =0;
						setting_SetAutoLock_num(0);
						ptd->shake_count =0;
					}
          lock_max= eps*3/4;					
				}

				
			}
			else
			{
				difference = ptd->DOmgl.value_f - ptd->last_DOmgl;
				if(difference >= 0)//这次是上涨
				{
					ptd->down_count = 0;//清连续下降次数
					
					if(ptd->last_trend == TREND_UP)//上次是上涨
					{
						if(++ptd->up_count >= SAME_TIMES)
						{
							ptd->shake_count = 0;
							ptd->up_count = 0;
							ptd->down_count = 0;
						}
					}
					else//上次是跌 \/
					{
							ptd->shake_count++;
					}
					ptd->last_trend = TREND_UP;
				}
				else//这次是跌
				{
					ptd->up_count = 0;//清连续上涨次数
					
					if(ptd->last_trend == TREND_DOWN)//上次是跌的话
					{
						if(++ptd->down_count >= SAME_TIMES)
						{
							ptd->shake_count = 0;
							ptd->up_count = 0;
							ptd->down_count = 0;
						}
					}
					else//上次是涨
					{
						ptd->shake_count++;
					}
					ptd->last_trend = TREND_DOWN;
				}
				
				
				if(fabs(difference) >= eps)//如果值变换幅度超过了设定的阈值清除计数
				{
					ptd->shake_count = 0;
					ptd->up_count = 0;
					ptd->down_count = 0;
				}

				lock_max= SHAKE_TIMES;
			}
			
			if(ptd->shake_count >=lock_max)
			{
				ptd->shake_count = 0;
				if(setting_GetLockSave() && ptd->is_ValueLocked == 0 )
				{	
					if(ptd->modbus_id == get_CurDo()->modbus_id)
					{	
						 log_SaveData(rs485_GetSensorType());								
					}
					else
					{
					   if(ptd->modbus_id == get_COMADo()->modbus_id)
						 {
							 cur_DO.DO_list = comA_DO.DO_list; 
							 cur_DO.current_sensor_type=comA_DO.current_sensor_type;
							 log_SaveData(rs485_GetSensorType());
							 cur_DO.DO_list = comB_DO.DO_list; 
							 cur_DO.current_sensor_type=comB_DO.current_sensor_type;								 
						 }
						 else
						 {
							 cur_DO.DO_list = comB_DO.DO_list; 
							 cur_DO.current_sensor_type=comB_DO.current_sensor_type;
							 log_SaveData(rs485_GetSensorType());
							 cur_DO.DO_list = comA_DO.DO_list; 
							 cur_DO.current_sensor_type=comA_DO.current_sensor_type;							 
						 }			
					}												
					generate_MessageBox(MESSAGE_SAVELOG, 1);	
	
				}
				ptd->is_ValueLocked = 1;//上锁
			}
		} 
  	}
}

void DO_UpdateTemp2DO(PtrToDOProbe ptd, uint8_t *dat)//要添加数字滤波
{
	float temperature_temp = 0.0;
	float DO_Percent_temp = 0.0;
	float DO_mgl_temp = 0.0;
	
	
	if(ptd == NULL) return;
	
	uint8_t i;
	for(i = 0; i < 4; i++)
	{
		ptd->temperature.value_arr[i] = *(dat++);
	}
	for(i = 0; i < 4; i++)
	{
		ptd->DOpercent.value_arr[i] = *(dat++);
	}
	for(i = 0; i < 4; i++)
	{
		ptd->DOmgl.value_arr[i] = *(dat++);
	}
	
	//ptd->DOpercent.value_f = fabs(ptd->DOpercent.value_f);//为了屏蔽负值这里变成绝对值
	
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
		
		ptd->last_DOmgl = ptd->DOpercent.value_f;
		
		snprintf(ptd->temperature_arr, 6, "%5.1f", ptd->temperature.value_f);
		snprintf(ptd->DOpercent_arr,   7, "%6.2f", (ptd->DOpercent.value_f * 100.0));
		snprintf(ptd->DOmgl_arr,       7, "%6.2f", ptd->DOmgl.value_f);
	}
	else
	{
		ptd->DOmgl_sum += ptd->DOmgl.value_f;
		ptd->DOpercent_sum += (ptd->DOpercent.value_f * 100.0);
		ptd->temperature_sum += ptd->temperature.value_f;
		
		CheckValueLock(ptd);//自动锁定直接做在读数这里的
		
		
		ptd->last_DOmgl = ptd->DOpercent.value_f;//更新一下上次的值
		
		if(++ptd->update_count >= 3)
		{
			
			temperature_temp = ptd->temperature_sum / ((float)ptd->update_count);
			DO_Percent_temp = ptd->DOpercent_sum / ((float)ptd->update_count);
			DO_mgl_temp = ptd->DOmgl_sum / ((float)ptd->update_count);
			
			
			
			if(setting_GetIsOpen_SlideAvg_DO() && setting_GetSlideAvgTimes_DO()>=2)//开启并且次数最起码为2次
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
				snprintf(ptd->DOpercent_arr,   7, "%6.2f", DO_Percent_temp);
				snprintf(ptd->DOmgl_arr,       7, "%6.2f", DO_mgl_temp);
			}
			
			ptd->update_count = 0;
			ptd->DOmgl_sum =0.0;
			ptd->DOpercent_sum = 0.0;
			ptd->temperature_sum = 0.0;	
			
			
		}
	}
}

void DO_UpdatePressSal(PtrToDOProbe *DO_head) //更新DO设备的气压值和盐度值（如果有需要的话）
{
	PtrToDOProbe p = (*DO_head);
	float press, sal;
	
	press = bmp280_GetPress()+setting_GetAirCompensate();
	sal = setting_GetSalinity();
	
	if((*DO_head) == NULL) //安全性判断
	{
		return;
	}
	

	if(p->is_init == 1)
	{
		if(fabs(press - p->press.value_f) >= press_error && !rs485_GetCircularSentStatus())//气压值跟当前值不一样
		{
			switch(get_CurDo()->modbus_id)
			{						
				case DO_HF1012_ModbusID:  
			    DO_rs485_SetPressure(get_CurDo(), press);//更新do设备的气压值
					break;
				
				case DO_shenghui_ModbusID:  
					DO_shenghui_rs485_SetPressure(get_CurDo(),press);	            
					break;	

				case DO_HF_DY12_ModbusID:  
					Set_DY12_Press_Flag(1);       
					Set_DY12_Press_Value((int16_t)(press * 100));
					DO_HaiFa_DY12_rs485_Set_Cmd_open(get_CurDo());
					break;

				case DO_HF_DY56_ModbusID:  
					Set_DY56_Press_Flag(1);       
					Set_DY56_Press_Value((int16_t)(press * 100));
					DO_HaiFa_DY56_rs485_Set_Cmd_open(get_CurDo());
					break;
				
				case DO_DY05_ModbusID:  
          DY05_rs485_SetPressure(get_CurDo(),press);	 				
					break;	

				case DO_DO59_ModbusID:  
          DO59_rs485_SetPressure(get_CurDo(),press);	 				
					break;	
				
				default:
					break;
			}
			return;
		}
		if(fabs(sal - p->sal.value_f) >= EPSILON && !rs485_GetCircularSentStatus())//如果盐度值跟当前设置的值不一样
		{
			
			switch(get_CurDo()->modbus_id)
			{						
				case DO_HF1012_ModbusID:  
			    DO_rs485_SetSalinity(get_CurDo(), sal);
					break;
				
				case DO_shenghui_ModbusID:  
					DO_shenghui_rs485_SetSalinity(get_CurDo(),sal);	            
					break;	
				
				case DO_HF_DY12_ModbusID:
					Set_DY12_Sal_Flag(1);       
					Set_DY12_Sal_Value((int16_t)(sal * 100));
					DO_HaiFa_DY12_rs485_Set_Cmd_open(get_CurDo());
					break;

				case DO_HF_DY56_ModbusID:
					Set_DY56_Sal_Flag(1);       
					Set_DY56_Sal_Value((int16_t)(sal * 100));
					DO_HaiFa_DY56_rs485_Set_Cmd_open(get_CurDo());
					break;
				
				case DO_DY05_ModbusID:  
					DY05_rs485_SetSalinity(get_CurDo(),sal);	            
					break;

				case DO_DO59_ModbusID:  
					DO59_rs485_SetSalinity(get_CurDo(),sal);	            
					break;
				
				default:
					break;
			}
			return;
		}
	}

	
}


/*用来检测数据是否合理能否被写入 实际值real  写入区间 （real/2）- (2*real)  */
uint8_t DO_ValueCheckFirst(PtrToDOProbe ptd, float data)
{

	return 1;
}

void DO_ClearCalPara(PtrToDOProbe ptd)
{
//	switch(ptd->manufacturer)
//	{
//		case manufacturer_hyphive:
//			DO_rs485_hyphiveClearCal(ptd);//海发清除校准参数
//			break;
//		
//		case manufacturer_yosemitech:
//			DO_rs485_SetKB(ptd, 1.0, 0.0);//禹山设置校准kb为1 和 0
//			break;
//	}
		DO_rs485_SetKB(ptd, 1.0, 0.0);//设置校准kb为1 和 0
}




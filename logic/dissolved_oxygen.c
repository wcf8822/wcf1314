#include "dissolved_oxygen.h"
#include "setting.h"

#include "bmp280.h"

#include "setting.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/////////////////////////////////////////////////////////////////////////////////////////////好像可以直接在485指令里直接改对应设备的值就不用改结构体里的值

const float press_error = 0.3;    //气压差值
const float EPSILON = 1.0E-6;

STATIC PtrToDOProbe cur_DO;

//要不要按照modbusid 来对设备进行操作 就是先find一下然后如果没找到重新创建一个

/*获取DO设备的指针*/
PtrToDOProbe get_CurDo(void)
{
	return cur_DO;
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


//根据modbusid 来设置当前溶解氧设备
void DO_SetCurDO(uint8_t ModbusId, PtrToDOProbe DO_head)
{
	PtrToDOProbe p = DO_head;
	if(DO_head == NULL)
	{
		return;
	}
	while(p != NULL && p->modbus_id != ModbusId)
	{
		p = p->next_DO;
	}
	cur_DO = p;
}

void DO_ClearCueDO(void)
{
	cur_DO = NULL;
}

void DO_AddProbe(uint8_t ModbusId, PtrToDOProbe *DO_head)//这里得添加名字
{
	PtrToDOProbe p = NULL;
	p = (PtrToDOProbe)malloc(sizeof(DOProbe_t));//分配空间
	
	if(p == NULL)//安全性判断
	{
		return;
	}
	
	cur_DO = p;                                //将新节点指针指向新生成的节点方便后面修改添加设备
	
	snprintf(p->name, 6, "DO-%02X", ModbusId); //生成名字
	
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
	p->DOmgl_arr[0] = ' ';
	p->DOmgl_arr[1] = '0';
	p->DOmgl_arr[2] = '.';
	p->DOmgl_arr[3] = '0';
	p->DOmgl_arr[4] = '0';
	p->DOmgl_arr[5] = '\0';
	
	p->DOpercent_arr[0] = ' ';
	p->DOpercent_arr[1] = ' ';
	p->DOpercent_arr[2] = '0';
	p->DOpercent_arr[3] = '.';
	p->DOpercent_arr[4] = '0';
	p->DOpercent_arr[5] = '0';
	p->DOpercent_arr[6] = '\0';
	
	p->temperature_arr[0] = ' ';
	p->temperature_arr[1] = '0';
	p->temperature_arr[2] = '.';
	p->temperature_arr[3] = '0';
	p->temperature_arr[4] = '0';
	p->temperature_arr[5] = '\0';
	
	p->next_DO = *DO_head;
	*DO_head = p;
}

//清除所有设备
void DO_Destory(PtrToDOProbe *DO_head)
{
	PtrToDOProbe p = NULL, temp = NULL;
	p = *DO_head;                          //指向头指针
	if((*DO_head) == NULL)//安全性检查
	{
		return;
	}
	
	*DO_head = NULL;	
	while(p != NULL)
	{
		temp = p->next_DO;
		free(p);
		p = temp;
	}
}

/*删除指定modbusid的设备*/
void DO_DelProbe(uint8_t ModbusId, PtrToDOProbe *DO_head) //好像要对头指针操作只能用这种办法了
{                                                         //这里要改成快慢指针 不应该局限于两个设备
	PtrToDOProbe cur = (*DO_head);
	
	if((*DO_head) == NULL)//安全性检查
	{
		return;
	}

	
	if(cur->modbus_id == ModbusId)//如果头指针指的就是
	{
		*DO_head = (*DO_head)->next_DO;
		free(cur);
	}
	if(cur->next_DO->modbus_id == ModbusId)//如果第二个节点是
	{
		(*DO_head)->next_DO = (*DO_head)->next_DO->next_DO;
		free(cur->next_DO);
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
	while(p != NULL && (uint8_t *)(p->name) != name)
	{
		p = p->next_DO;
	}
	return p;
}



/*设置电机的ModbusID*/
void DO_rs485_SetAddr(PtrToDOProbe ptd, uint8_t NewId) //这里要校验一下是不是写进去了，然后要将这个结构体里面的modbus id 也改掉
{
	if(ptd == NULL) return;
	uint8_t temp = 0;
	
	
	
	temp = NewId>>4;        //更改设备名字
	ptd->name[3] = temp>=10 ? (temp - 10) + 'A': temp+'0';
	temp = NewId & 0x0f;
	ptd->name[4] = temp>=10 ? (temp - 10) + 'A': temp+'0';
	
	rs485_usart.tx_buf[0] = ptd->modbus_id;
	rs485_usart.tx_buf[1] = 0x10;
	rs485_usart.tx_buf[2] = 0x30;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	rs485_usart.tx_buf[6] = 0x02;
	rs485_usart.tx_buf[7] = NewId;
	rs485_usart.tx_buf[8] = 0x00;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 11);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_SetAddr);
	
	ptd->new_ModbusID = NewId; //更改设备id

}


/*获取sn*/
void DO_rs485_GetSN(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ptd->modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x09;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x07;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSN);
	

}

/*开始测量*/
void DO_rs485_Start(PtrToDOProbe ptd)//默认上电就开始测量
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ptd->modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x25;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_Start);
	

}

/*获取温度 DO% DOmg/L*/
void DO_rs485_GetTempTwoDO(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ptd->modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x26;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x06;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetTempTwoDO);
	

}

/*获取温度*/
void DO_rs485_GetTemperature(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ptd->modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x26;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetTemperature);
	

}

/*获取DO %*/
void DO_rs485_GetDOPercent(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ptd->modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x26;
	rs485_usart.tx_buf[3] = 0x02;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetDOPercent);
	

}
/*获取DO mg/L*/
void DO_rs485_GetDOmgL(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ptd->modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x26;
	rs485_usart.tx_buf[3] = 0x04;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetDOmgL);
	

}

/*获取软硬件版本号*/
void DO_rs485_GetSHWVersion(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ptd->modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x07;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x02;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_GetSHWVersion);
	

}

void DO_rs485_Stop(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ptd->modbus_id;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x2E;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	
	SetCrc(rs485_usart.tx_buf, rs485_usart.tx_size = 8);
	
	rs485_SetCircularSentStatus();
	
	rs485_SetSentType(DO_SendType_Stop);
	

}

/*获取用户校准参数*/
void DO_rs485_GetKB(PtrToDOProbe ptd)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0] = ptd->modbus_id;
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
	rs485_usart.tx_buf[0]  = ptd->modbus_id;
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








/*获取探头Modbus通讯ID*/
void DO_rs485_GetModbusId(void)
{
	rs485_usart.tx_buf[0] = 0xFF;
	rs485_usart.tx_buf[1] = 0x03;
	rs485_usart.tx_buf[2] = 0x30;
	rs485_usart.tx_buf[3] = 0x00;
	rs485_usart.tx_buf[4] = 0x00;
	rs485_usart.tx_buf[5] = 0x01;
	rs485_usart.tx_buf[6] = 0x9E;
	rs485_usart.tx_buf[7] = 0xD4;
	
	rs485_usart.tx_size = 8;
	
//	rs485_SetNeedSendStatus();
	
	rs485_SetSentType(DO_SendType_GetModbusId);
	
}

//0x01	0x10	0x27   	0x00	0x00	0x10	0x20	K0~ K7
void DO_rs485_SetSensorCap(PtrToDOProbe ptd, SensorCap_t *sc)
{
	if(ptd == NULL) return;
	rs485_usart.tx_buf[0]  = ptd->modbus_id;
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
	rs485_usart.tx_buf[0] = ptd->modbus_id;
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
	
	rs485_usart.tx_buf[0]  = ptd->modbus_id;
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
	rs485_usart.tx_buf[0] = ptd->modbus_id;
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
	
	rs485_usart.tx_buf[0]  = ptd->modbus_id;
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


void DO_SetSN(PtrToDOProbe ptd, uint8_t* buff, uint8_t len)
{
	if(ptd == NULL) return;
	for(uint8_t i =0; i<len; i++)
	{
		ptd->SN[i] = *(buff + i);
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

static uint8_t update_count = 0;
static float DOmgl_sum =0.0;
static float DOpercent_sum = 0.0;
static float temperature_sum = 0.0;

void DO_SetTempZero(void)
{
	update_count = 0;
	DOmgl_sum =0.0;
	DOpercent_sum = 0.0;
	temperature_sum = 0.0;
}


#define DO_EPS 0.0145//0.015
#define SHAKE_TIMES 8//8
#define SAME_TIMES 3

static uint8_t shake_count = 0;

void clear_DOShakeCount(void)
{
	shake_count = 0;
}

float last_DOmgl=0.0;

//uint8_t n=0;

void CheckValueLock(PtrToDOProbe ptd)
{
	double difference = 0.0;//差值
	static uint8_t last_trend = 1;//1 上涨 0 下降
	static uint8_t up_count = 0;
	static uint8_t down_count = 0;
	
	if(setting_GetAutoLock() == AUTOLOCK_AUTO)
	{
		
		difference = ptd->DOmgl.value_f - last_DOmgl;
		
//		if(++n <= 50)
//		{
//			printf("%.4f,", difference);
//		}
//		else
//		{
//			n = 101;
//		}
//		
		if(difference >= 0)//这次是上涨
		{
			down_count = 0;
			if(last_trend == 1)//上次是上涨
			{
				if(fabs(difference) >= DO_EPS)//连续上涨一定次数
				{
					shake_count = 0;
					up_count = 0;
				}
				else
				{
					if(++up_count >= SAME_TIMES)
					{
						shake_count = 0;
						up_count = 0;
					}
				}
			}
			else//上次是跌 \/
			{
					if(fabs(difference) < DO_EPS)
					{
						shake_count++;
					}
			}
			last_trend = 1;
		}
		else//这次是跌
		{
			up_count = 0;
			if(last_trend == 0)//上次是跌的话
			{
				if(fabs(difference) >= DO_EPS)//连续跌一定次数
				{
					shake_count = 0;
					down_count = 0;
				}
				else
				{
					if(++down_count >= SAME_TIMES)
					{
						down_count = 0;
						shake_count = 0;
					}
				}
			}
			else//上次是涨
			{
				if(fabs(difference) < DO_EPS)
				{
					shake_count++;
				}
			}
			last_trend = 0;
		}
		
		
		if(shake_count >= SHAKE_TIMES)
		{
			shake_count = 0;
		  ptd->is_ValueLocked = 1;//上锁
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
	
	if(ptd->is_FirstGetValue)
	{
		ptd->is_FirstGetValue = 0;
		
		last_DOmgl = ptd->DOmgl.value_f;
		
		snprintf(ptd->temperature_arr, 6, "%5.2f", ptd->temperature.value_f);
		snprintf(ptd->DOpercent_arr,   7, "%6.2f", (ptd->DOpercent.value_f * 100.0));
		snprintf(ptd->DOmgl_arr,       6, "%5.2f", ptd->DOmgl.value_f);
	}
	else
	{
		DOmgl_sum += ptd->DOmgl.value_f;
		DOpercent_sum += (ptd->DOpercent.value_f * 100.0);
		temperature_sum += ptd->temperature.value_f;
		
		
		CheckValueLock(ptd);
		
		
		last_DOmgl = ptd->DOmgl.value_f;//更新一下上次的值
		if(++update_count >= 3)
		{
			
			temperature_temp = temperature_sum / ((float)update_count);
			DO_Percent_temp = DOpercent_sum / ((float)update_count);
			DO_mgl_temp = DOmgl_sum / ((float)update_count);
			
			if(!DO_GetValueLocked(ptd))
			{
				snprintf(ptd->temperature_arr, 6, "%5.2f", temperature_temp);
				snprintf(ptd->DOpercent_arr,   7, "%6.2f", DO_Percent_temp);
				snprintf(ptd->DOmgl_arr,       6, "%5.2f", DO_mgl_temp);
			}
			
			update_count = 0;
			DOmgl_sum =0.0;
			DOpercent_sum = 0.0;
			temperature_sum = 0.0;
			
			
		}
	}
}

void DO_UpdatePressSal(PtrToDOProbe *DO_head) //更新DO链表上所有DO设备的气压值和盐度值（如果有需要的话）
{
	PtrToDOProbe p = (*DO_head);
	float press, sal;
	
	press = bmp280_GetPress()+setting_GetAirCompensate();
	sal = setting_GetSalinity();
	
	if((*DO_head) == NULL) //安全性判断
	{
		return;
	}
	
	while(p != NULL)//遍历更新所有已连接的溶解氧   可以通过判断是否在循环写入去写入  写一个然后直接return
	{
		if(fabs(press - p->press.value_f) >= press_error && !rs485_GetCircularSentStatus())//气压值跟当前值不一样
		{
			DO_rs485_SetPressure(get_CurDo(), press);
			return;
		}
		if(fabs(sal - p->sal.value_f) >= EPSILON && !rs485_GetCircularSentStatus())//如果盐度值跟当前设置的值不一样
		{
			DO_rs485_SetSalinity(get_CurDo(), sal);
			return;
		}
		
		p = p->next_DO;
	}
	
}


/*用来检测数据是否合理能否被写入 实际值real  写入区间 （real/2）- (2*real)  */
uint8_t DO_ValueCheckFirst(PtrToDOProbe ptd, float data)
{
//	float real = 0.0;
//	real = (DO_GetDOPercent(ptd) - DO_GetBFloat(ptd)) / DO_GetKFloat(ptd);//获得当前实际的值 b为0 k为1
//	real *= 100.0;
//	if(data != 0)
//	{
//		if( data <= (2.0*real) && data >= (real/2.0))
//		{
//			return 1;
//		}
//	}
//	else
//	{
//		return 1;
//	}
	
	return 1;
}






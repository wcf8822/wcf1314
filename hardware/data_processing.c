#include "data_processing.h"
#include "rs485.h"
#include "gps.h"
#include "ch340e.h"
#include "dissolved_oxygen.h"

#include "label.h"

#include "interfacial.h"

#include "setting.h"

#include <stdio.h>

#include "font_lib.h"



/*所有串口的接收中断，表示有接收到数据*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//这里放接收回调
{
	rs485_RxCallBack(huart);
	gps_RxCallBack(huart);
	ch340e_RxCallBack(huart);
}

/*所有串口的空闲中断，表示接受满一帧数据*/
void HAL_UART_IDLE_HANDLER(UART_HandleTypeDef *huart)//这里放空闲回调
{
	if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))
	{
		rs485_IDLECallBack(huart);
		gps_IDLECallBack(huart);
		ch340e_IDLECallBack(huart);
	}
}

/*所有串口的错误中断，表示串口出错*/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) //这里放串口错误回调
{
}

void close_circle(void)
{
	rs485_ClearCircularSentStatus(); //关闭循环发送
	rs485_ClearResendCount();        //清空重发计数
}

//这是在main里处理的
void rs485_DataHandle(void)
{
	if(rs485_GetRxFlag())//如果接收到数据 并 通过crc校验了的话
	{
		rs485_ClearRxFlag();
		if(CheckCrc(rs485_usart.rx_buf, rs485_usart.rx_size))
		{
			switch(rs485_GetSentType())
			{
				case DO_SendType_GetModbusId:   //溶解氧获取modbus id
					if(rs485_usart.rx_buf[0] == 0xFF && rs485_usart.rx_buf[1] == 0x03)
					{
						close_circle();
						
						uint8_t DO_ID = rs485_usart.rx_buf[3];
						
						DO_AddProbe(DO_ID, &connected_probe.DO_list);
						
						rs485_SetSensorType(TYPE_DO);
						
						
//						if(get_CurPage() == PAGE_3_SENSORSSEARCH) //搜索设备界面上添加设备
//						{
//							//这里添加节点
//							LabelList_Add(0, 32 + LabelList_count(interfacial_ReturnCurrent()->label_head) * 16, NULL, 0, (uint8_t *)rs485_GetDeviceName(rs485_GetDeviceCount()), NOT_NUMBER, IS_STR, &(interfacial_ReturnCurrent()->label_head));
//						}
						
						
						
						rs485_DevicePlus();
						
						
						if(!DO_GetIsInit(get_CurDo()))
						{
							DO_rs485_GetSN(get_CurDo());
						}
					}
					break;
				
				case DO_SendType_GetSN:         //获取溶解氧设备编码
					if(rs485_usart.rx_buf[0] == get_CurDo()->modbus_id && rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x0E)
					{
						close_circle();
						DO_SetSN(get_CurDo(), &(rs485_usart.rx_buf[4]), 12);
						rs485_SetSensorType(TYPE_DO);
						if(!DO_GetIsInit(get_CurDo()))
						{
							DO_rs485_GetSHWVersion(get_CurDo());
						}
					}
					break;					
				
				case DO_SendType_GetSHWVersion: //溶解氧获取软硬件版本号
					if(rs485_usart.rx_buf[0] == get_CurDo()->modbus_id && rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x04)
					{
						close_circle();
						DO_SetSHWVersion(get_CurDo(), &(rs485_usart.rx_buf[3]));
						rs485_SetSensorType(TYPE_DO);
						if(!DO_GetIsInit(get_CurDo()))
						{
							DO_rs485_GetKB(get_CurDo());
						}
					}
					break;
					
				case DO_SendType_GetKB:        //获取溶解氧的kb数值
					if(rs485_usart.rx_buf[0] == get_CurDo()->modbus_id && rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x08)
					{
						close_circle();
						DO_SetKBArr(get_CurDo(), &(rs485_usart.rx_buf[3]));
						rs485_SetSensorType(TYPE_DO);
						if(!DO_GetIsInit(get_CurDo()))
						{
							DO_rs485_GetSalinity(get_CurDo());
						}
					}
					break;
					
				case DO_SendType_GetSalinity:  //获取溶解氧设置的盐度值
					if(rs485_usart.rx_buf[0] == get_CurDo()->modbus_id && rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x04)
					{
						close_circle();
						DO_SetSalinityArr(get_CurDo(), &(rs485_usart.rx_buf[3]));
						rs485_SetSensorType(TYPE_DO);
						if(!DO_GetIsInit(get_CurDo()))
						{
							DO_rs485_GetPressure(get_CurDo());
						}
					}
					break;
					
				case DO_SendType_GetPressure: //获取溶解氧设置的气压值
					if(rs485_usart.rx_buf[0] == get_CurDo()->modbus_id && rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x04)
					{
						close_circle();
						DO_SetPressureArr(get_CurDo(), &(rs485_usart.rx_buf[3]));
						rs485_SetSensorType(TYPE_DO);
						if(!DO_GetIsInit(get_CurDo()))
						{
							DO_SetIsInit(get_CurDo());
						}
					}
					break;
				
				case DO_SendType_SetAddr:
					if(rs485_usart.rx_buf[0] == get_CurDo()->modbus_id && rs485_usart.rx_buf[2] == 0x30)
					{
						close_circle();
						get_CurDo()->modbus_id = get_CurDo()->new_ModbusID;
						rs485_SetSensorType(TYPE_DO);////////////////////////////////
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
					}
					break;
				
				case DO_SendType_GetTempTwoDO:
					if(rs485_usart.rx_buf[0] == get_CurDo()->modbus_id && rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x0c)
					{
						close_circle();
						DO_UpdateTemp2DO(get_CurDo(), &(rs485_usart.rx_buf[3]));
						DO_SetIsGetedValue(get_CurDo());//设置do设备已经有数据了
						rs485_SetSensorType(TYPE_DO);
					}
					break;
				
				case DO_SendType_SetKB:
					if(rs485_usart.rx_buf[0] == get_CurDo()->modbus_id && rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x11)//校准返回
					{
						close_circle();
						switch(interfacial_GetCurPage())
						{
//								interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
//								interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)chenggong_en;
//								interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
//								break;
							case PAGE_1_RESETCAL:
							case PAGE_5_ONE:
							case PAGE_5_TWOSECOND:
								interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
								interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)chenggong_en;
								interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
								generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
								break;
							
							
							
							default:
								break;
						}
					}
					break;
					
				default:
					break;
			}
		}
	}
}






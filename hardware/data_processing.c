#include "data_processing.h"
#include "rs485.h"
#include "gps.h"
#include "ch340e.h"
#include "label.h"
#include "interfacial.h"
#include "setting.h"
#include <stdio.h>
#include "font_lib.h"
#include "rs485.h"
#include "heart.h"
#include "dissolved_oxygen.h"
#include "DO_shenghui.h"
#include "NH3N_shenghui.h"
#include "COD_shenghui.h"
#include "NH3N_DN02.h"
#include "COD_DC18.h"
#include "pH_shenghui.h"
#include "Tur_shenghui.h"
#include "EC_shenghui.h"
#include "EC_DE26.h"
#include "pH_Dph07.h"
#include "Chl_shenghui.h"
#include "ORP_DR31.h"
#include "BGA_shenghui.h"

uint8_t DC18_DC17_flag;		//17,18切换标志位

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




/*海发禹山溶解氧传感器  串口数据处理*/
void DO_HF1012_DataHandle(void){
	switch(rs485_GetSentType())
	{
		case DO_SendType_GetModbusId:   //溶解氧获取modbus id
			if( rs485_usart.rx_buf[1] == 0x03)
			{			
				uint8_t DO_ID =DO_HF1012_ModbusID;
        				
				DO_AddProbe(DO_ID);//在所有设备树中添加溶解氧节点
					
				rs485_DevicePlus();//添加下设备树上的设备个数
				
				DO_rs485_GetSN(get_COMADo());

			}
			break;
		
		case DO_SendType_GetSN:         //获取溶解氧设备编码
			if( rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x0E)
			{
				close_circle();
				DO_SetSN(get_COMADo()->modbus_id == DO_HF1012_ModbusID ? get_COMADo() : get_COMBDo(),  &(rs485_usart.rx_buf[4]), 12);
				DO_rs485_GetSHWVersion(get_COMADo());

			}
			break;					
		
		case DO_SendType_GetSHWVersion: //溶解氧获取软硬件版本号
			if(rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x04)
			{
				close_circle();
				DO_SetSHWVersion(get_COMADo()->modbus_id == DO_HF1012_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_rs485_GetKB(get_COMADo());

			}
			break;
			
		case DO_SendType_GetKB:        //获取溶解氧的kb数值
			if(rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x08)
			{
				close_circle();
				DO_SetKBArr(get_COMADo()->modbus_id == DO_HF1012_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_SetIsInit(get_COMADo()->modbus_id == DO_HF1012_ModbusID ? get_COMADo() : get_COMBDo());
			}
			break;
			
		case DO_SendType_GetSalinity:  //获取溶解氧设置的盐度值
			if( rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x04)
			{
				close_circle();
				DO_SetSalinityArr(get_COMADo()->modbus_id == DO_HF1012_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_rs485_GetPressure(get_COMADo());

			}
			break;
			
		case DO_SendType_GetPressure: //获取溶解氧设置的气压值
			if(rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x04)
			{
				close_circle();
				DO_SetPressureArr(get_COMADo()->modbus_id == DO_HF1012_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
			}
			break;
		
		case DO_SendType_SetAddr:
			if( rs485_usart.rx_buf[2] == 0x30)
			{
				close_circle();
				get_COMADo()->modbus_id = get_COMADo()->new_ModbusID;
				generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
			}
			break;
		
		case DO_SendType_GetTempTwoDO:
			if(rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x0c)
			{
				close_circle();
				DO_UpdateTemp2DO(get_COMADo()->modbus_id == DO_HF1012_ModbusID ? get_COMADo() : get_COMBDo(),  &(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == DO_HF1012_ModbusID ? get_COMADo() : get_COMBDo());//设置do设备已经有数据了
			}
			break;
		
		case DO_SendType_SetKB:
			if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x11)//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{
					case PAGE_1_RESETCAL:
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
					case PAGE_5_DO_ONE_First:
					case PAGE_5_DO_TWO_SECOND:
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)chenggong_en;
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						break;
					
					default:
						break;
				}
			}
			break;
		
		case DO_SendType_SetTemp:
			if( rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x76)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_5_TEMP)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;

		case DO_SendType_SetPressure:
			if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x24)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_3_PRESSURE)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;
		
		case DO_SendType_SetSalinity:
			if( rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x15)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_3_SALT)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;
			
		case DO_SendType_HyphiveClearCal:
			if( rs485_usart.rx_buf[2] == 0x00 && rs485_usart.rx_buf[4] == 0xFC)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;	
		
  	default:
			break;
	}

}





/*昇辉溶解氧传感器  串口数据处理*/
void DO_shenghui_DataHandle(void){
	switch(rs485_GetSentType())
	{
		case DO_SendType_GetModbusId:   //溶解氧获取modbus id
			if(rs485_usart.rx_buf[1] == 0x04)
			{	
				uint8_t DO_ID = DO_shenghui_ModbusID;
				
				DO_AddProbe(DO_ID);
					
				rs485_DevicePlus();//添加下设备树上的设备个数
							
				GetCircularSent_Flag=0;
				DO_shenghui_rs485_GetSN(get_COMADo());

			}
			break;
		
		case DO_SendType_GetSN:         //获取溶解氧设备编码
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x0C)
			{
				close_circle();
				shenghui_SetSN(get_COMADo()->modbus_id == DO_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]), 12);
				DO_shenghui_rs485_GetSHWVersion(get_COMADo());

			}
			break;					
		
		case DO_SendType_GetSHWVersion: //溶解氧获取软硬件版本号
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x02)
			{
				close_circle();
				shenghui_SetSHWVersion(get_COMADo()->modbus_id == DO_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),&(rs485_usart.rx_buf[3]));
				DO_SetIsInit(get_COMADo()->modbus_id == DO_shenghui_ModbusID ? get_COMADo() : get_COMBDo());
			}
			break;
			
			
		case DO_SendType_GetSalinity:  //获取溶解氧设置的盐度值
			if(rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x04)
			{
				close_circle();
				DO_shenghui_SetSalinityArr(get_COMADo()->modbus_id == DO_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_shenghui_rs485_GetPressure(get_COMADo());
			}
			break;
			
		case DO_SendType_GetPressure: //获取溶解氧设置的气压值
			if(rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x04)
			{
				close_circle();
				DO_shenghui_SetPressureArr(get_COMADo()->modbus_id == DO_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),&(rs485_usart.rx_buf[3]));
			}
			break;
			
		case DO_SendType_GetTempTwoDO:
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x08)
			{
				close_circle();
				DO_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == DO_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),&(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == DO_shenghui_ModbusID ? get_COMADo() : get_COMBDo());//设置do设备已经有数据了
			}
			break;
			
			
		case DO_SendType_GetDOPercent:
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x04)
			{
				close_circle();
				DO_shenghui_UpdateTemp2DO_Percent(get_COMADo()->modbus_id == DO_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),&(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == DO_shenghui_ModbusID ? get_COMADo() : get_COMBDo());//设置do设备已经有数据了

			}
			break;
		
		case DO_SendType_SetKB:
			if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{
					case PAGE_1_RESETCAL:
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
					case PAGE_5_DO_ONE_First:
					case PAGE_5_DO_TWO_SECOND:
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)chenggong_en;
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						break;
					
					default:
						break;
				}
			}
			break;
		
		case DO_SendType_SetTemp:
			if( rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_5_TEMP)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;

		case DO_SendType_SetPressure:
			if( rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_3_PRESSURE)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;
		
		case DO_SendType_SetSalinity:
			if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_3_SALT)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;
			
		case DO_SendType_HyphiveClearCal:
			if(rs485_usart.rx_buf[2] == 0x9C && rs485_usart.rx_buf[3] == 0x51)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;								
		default:
			break;
	}
}

void NH3N_shenghui_DataHandle(void){
			switch(rs485_GetSentType())
			{
				case DO_SendType_GetModbusId:   //溶解氧获取modbus id
					if(rs485_usart.rx_buf[1] == 0x04)
					{						
						uint8_t DO_ID = NH3N_shenghui_ModbusID;
						
            DO_AddProbe(DO_ID);
						NH3N_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == NH3N_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));											
						rs485_DevicePlus();//添加下设备树上的设备个数
											
						NH3N_shenghui_rs485_GetSN(get_COMADo());

					}
					break;
				
				case DO_SendType_GetSN:         //获取氨氮设备编码
					if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x0C)
					{
						close_circle();
						shenghui_SetSN(get_COMADo()->modbus_id == NH3N_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]), 12);

						NH3N_shenghui_rs485_GetSHWVersion(get_COMADo());

					}
					break;					
				
				case DO_SendType_GetSHWVersion: //氨氮获取软硬件版本号
					if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x02)
					{
						close_circle();
						shenghui_SetSHWVersion(get_COMADo()->modbus_id == NH3N_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
						DO_SetIsInit(get_COMADo()->modbus_id == NH3N_shenghui_ModbusID ? get_COMADo() : get_COMBDo());
					}
					break;
					
						
				case DO_SendType_GetTempTwoDO:
					if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x14)
					{
						close_circle();
						NH3N_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == NH3N_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
						DO_SetIsGetedValue(get_COMADo()->modbus_id == NH3N_shenghui_ModbusID ? get_COMADo() : get_COMBDo());//设置设备已经有数据了
					}
					break;
						
				case DO_SendType_SetKB:
					if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
					{
						close_circle();
						switch(interfacial_GetCurPage())
						{
							case PAGE_1_RESETCAL:
								generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
							case PAGE_5_DO_ONE_First:
							case PAGE_5_DO_TWO_SECOND:
								
							case PAGE_5_NH3N_ONE:
							case PAGE_5_NH3N_TWO:													
							case PAGE_5_NH3N_pH_ONE:
							case PAGE_5_NH3N_pH_TWO:
							case PAGE_5_NH3N_pH_THREE:		
								generate_MessageBox(MESSAGE_SUCCESSFUL, 1);				
								interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
								interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_eng = (uint8_t *)chenggong_en;
								interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
								break;
							
							default:
								break;
						}
					}
					break;
				
				case DO_SendType_SetTemp:
					if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
					{
						close_circle();
						if(interfacial_GetCurPage() == PAGE_5_TEMP)
						{
								generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
						}
					}
					break;
				
				case DO_SendType_HyphiveClearCal:
					if(rs485_usart.rx_buf[2] == 0x9C && rs485_usart.rx_buf[3] == 0x51)
					{
						close_circle();
						if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
						{
								generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
						}
					}
				
			    break;							
				default:
					break;
			}
}



void NH3N_DN02_DataHandle(void){
			switch(rs485_GetSentType())
			{
				case DO_SendType_GetModbusId:   //溶解氧获取modbus id
					if(rs485_usart.rx_buf[1] == 0x03)
					{	
						uint8_t DO_ID = NH3N_DN02_ModbusID;						
            DO_AddProbe(DO_ID);
						NH3N_DN02_UpdateTemp2DO(get_COMADo()->modbus_id == NH3N_DN02_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));				
						rs485_DevicePlus();//添加下设备树上的设备个数												
						NH3N_DN02_rs485_GetSN(get_COMADo());

					}
					break;
				
				case DO_SendType_GetSN:         //获取氨氮设备编码
					if(rs485_usart.rx_buf[1] == 0x03)
					{
						close_circle();
						NH3N_DN02_SetSN(get_COMADo()->modbus_id == NH3N_DN02_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]), 12);				
						NH3N_DN02_rs485_GetSHWVersion(get_COMADo());

					}
					break;					
				
				case DO_SendType_GetSHWVersion: //氨氮获取软硬件版本号
					if(rs485_usart.rx_buf[1] == 0x03)
					{
						close_circle();
						NH3N_DN02_SetSHWVersion(get_COMADo()->modbus_id == NH3N_DN02_ModbusID ? get_COMADo() : get_COMBDo(),  &(rs485_usart.rx_buf[3]));
						DO_SetIsInit(get_COMADo()->modbus_id == NH3N_DN02_ModbusID ? get_COMADo() : get_COMBDo());
					}
					break;
					
						
				case DO_SendType_GetTempTwoDO:
					if(rs485_usart.rx_buf[1] == 0x03)
					{
						close_circle();
						NH3N_DN02_UpdateTemp2DO(get_COMADo()->modbus_id == NH3N_DN02_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
						DO_SetIsGetedValue(get_COMADo()->modbus_id == NH3N_DN02_ModbusID ? get_COMADo() : get_COMBDo());//设置设备已经有数据了
					}
					break;
					
			
				case DO_SendType_SetKB:
					if(rs485_usart.rx_buf[1] == 0x10)//校准返回
					{
						close_circle();
						
						switch(interfacial_GetCurPage())
						{
							case PAGE_1_RESETCAL:
								generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
							case PAGE_5_DO_ONE_First:
							case PAGE_5_DO_TWO_SECOND:
							case PAGE_5_NH3N_ONE:
							case PAGE_5_NH3N_TWO:
							case PAGE_5_NH3N_pH_ONE:
							case PAGE_5_NH3N_pH_TWO:
							case PAGE_5_NH3N_pH_THREE:
								generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
								interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
								interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_eng = (uint8_t *)chenggong_en;
								interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
								break;
							
							default:
								break;
						}
					}
					break;
				
				case DO_SendType_SetTemp:
					if(rs485_usart.rx_buf[1] == 0x10)//校准返回
					{
						close_circle();
						if(interfacial_GetCurPage() == PAGE_5_TEMP)
						{
								generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
						}
					}
					break;

					
				case DO_SendType_HyphiveClearCal:
					if(rs485_usart.rx_buf[2] == 0x63 && rs485_usart.rx_buf[3] == 0x12)
					{
						close_circle();
						if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
						{
								generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
						}
					}
				break;
						
				default:
					break;
			}
}


void COD_shenghui_DataHandle(void){

	switch(rs485_GetSentType())
	{
		case DO_SendType_GetModbusId:   //
			if(rs485_usart.rx_buf[1] == 0x04)
			{
				
				uint8_t DO_ID = COD_shenghui_ModbusID;
        DO_AddProbe(DO_ID);			
				rs485_DevicePlus();//添加下设备树上的设备个数
				COD_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == COD_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));				
				COD_shenghui_rs485_GetSN(get_COMADo());

			}
			break;
		
		case DO_SendType_GetSN:         //获取设备编码
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x0C)
			{
				close_circle();
				shenghui_SetSN(get_COMADo()->modbus_id == COD_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]), 12);
				COD_shenghui_rs485_GetSHWVersion(get_COMADo());

			}
			break;					
		
		case DO_SendType_GetSHWVersion: //获取软硬件版本号
			if(rs485_usart.rx_buf[1] == 0x04)
			{
				close_circle();
				shenghui_SetSHWVersion(get_COMADo()->modbus_id == COD_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),  &(rs485_usart.rx_buf[3]));
				DO_SetIsInit(get_COMADo()->modbus_id == COD_shenghui_ModbusID ? get_COMADo() : get_COMBDo());
			}
			break;
			
		
		case DO_SendType_GetTempTwoDO:
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x1C)
			{
				close_circle();
				COD_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == COD_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == COD_shenghui_ModbusID ? get_COMADo() : get_COMBDo());//设置设备已经有数据了
			}
			break;
			
	
		case DO_SendType_SetKB:
			if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{
						case PAGE_1_RESETCAL:
						case PAGE_5_DO_ONE_First:
						case PAGE_5_DO_TWO_SECOND:
						
						case PAGE_5_COD_shenghui_Tur_ONE:
						case PAGE_5_COD_shenghui_Tur_TWO:
						case PAGE_5_COD_shenghui_ONE:
						case PAGE_5_COD_shenghui_TWO:
						case PAGE_5_COD_shenghui_THREE:
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
			  			interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)chenggong_en;
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						break;
										
					default:
						break;
				}
			}
			break;
		
		case DO_SendType_SetTemp:
			if(rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{			
				close_circle();
				if(interfacial_GetCurPage() == PAGE_5_TEMP ||  interfacial_GetCurPage() ==  PAGE_5_COD_Cleanse || 			interfacial_GetCurPage() == PAGE_5_COD_shenghui_Zero  )
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;

			
		case DO_SendType_HyphiveClearCal:
			if(rs485_usart.rx_buf[2] == 0x9C && rs485_usart.rx_buf[3] == 0x51)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;		
			
		default:
			break;
	}

}

void COD_DC17_DataHandle(void){

	switch(rs485_GetSentType())
	{
		case DO_SendType_GetModbusId:   //
			if(rs485_usart.rx_buf[1] == 0x03)
			{		
				uint8_t DO_ID = COD_DC17_ModbusID;
        		DO_AddProbe(DO_ID);			
				// DC18_DC17_flag = 17;
				rs485_DevicePlus();//添加下设备树上的设备个数
				COD_DC18_UpdateTemp2DO(get_COMADo()->modbus_id == COD_DC17_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));		
				// COD_DC18_rs485_GetSN(get_COMADo());
				COD_DC17_rs485_Get_Mes_Para(get_COMADo());
			}
			break;

		case DO_SendType_GetMesParameter://DC17测量参数
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				COD_DC17_Set_Mes_Para(get_COMADo()->modbus_id == COD_DC17_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				COD_DC18_rs485_GetSN(get_COMADo(),COD_DC17_ModbusID);
			}
			break;
		
		case DO_SendType_GetSN:         //获取设备编码
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				COD_DC18_SetSN(get_COMADo()->modbus_id == COD_DC17_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]), 1);
				COD_DC18_rs485_GetSHWVersion(get_COMADo(),COD_DC17_ModbusID);

			}
			break;					
		
		case DO_SendType_GetSHWVersion: //获取软硬件版本号
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				COD_DC18_SetSHWVersion(get_COMADo()->modbus_id == COD_DC17_ModbusID ? get_COMADo() : get_COMBDo(),NULL);
				COD_DC18_rs485_Get_U(get_COMADo(),COD_DC17_ModbusID);
			}
			break;
	
		case DO_SendType_GetKB: //获取补偿系数U
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				COD_DC18_Set_U(get_COMADo()->modbus_id == COD_DC17_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				// COD_DC17_rs485_Get_Mes_Para(get_COMADo());
				DO_SetIsInit(get_COMADo()->modbus_id == COD_DC17_ModbusID ? get_COMADo() : get_COMBDo());
			}
			break;
			

		case DO_SendType_Set_Mes_mode://设置测量模式
			if(rs485_usart.rx_buf[1] == 0x06 )//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{
					case PAGE_4_Mes_Set:
					MesRun = 1;//闪Mes的标志位
					generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
					interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
					interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)chenggong_en;
					interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
					break;
					default:
					break;
				}
				COD_DC17_UPdata_Mes_Para(get_COMADo()->modbus_id == COD_DC17_ModbusID ? get_COMADo() : get_COMBDo() ,&(rs485_usart.rx_buf[4]));
			}
			break;
			
		case DO_SendType_Set_Mes_Time:
			if(rs485_usart.rx_buf[1] == 0x06 )//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{
					case PAGE_4_Mes_Time_Set:
					generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
					interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
					interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)chenggong_en;
					interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
					break;
					default:
						break;
				}
				COD_DC17_rs485_Get_Mes_Para(get_COMADo());
			}
			break;
		case DO_SendType_GetTempTwoDO:
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				COD_DC18_UpdateTemp2DO(get_COMADo()->modbus_id == COD_DC17_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == COD_DC17_ModbusID ? get_COMADo() : get_COMBDo());//设置设备已经有数据了
			}
			break;
			
	
		case DO_SendType_SetKB:
			if(rs485_usart.rx_buf[1] == 0x06 || rs485_usart.rx_buf[1] == 0x10)//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{
						case PAGE_1_RESETCAL:
						case PAGE_5_DO_ONE_First:
						case PAGE_5_DO_TWO_SECOND:
		
						case PAGE_5_COD_DC18_Tur_Zero:
						case PAGE_5_COD_DC18_Tur_Slope:
						case PAGE_5_COD_DC18_Zero:
						case PAGE_5_COD_DC18_Slope:
						case PAGE_5_COD_DC17_IN_Tur_Zero:
						case PAGE_5_COD_DC17_IN_Zero:
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)chenggong_en;
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						break;
					
					default:
						break;
				}
			}
			break;
		
		case DO_SendType_SetTemp:
			if(rs485_usart.rx_buf[1] == 0x06)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_5_TEMP ||  interfacial_GetCurPage() ==  PAGE_5_COD_Cleanse ||  interfacial_GetCurPage() ==  PAGE_5_COD_DC18_Coefficient )
				{
					generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
				
				
			}
			break;

	
		case DO_SendType_HyphiveClearCal:
			if(rs485_usart.rx_buf[1] == 0x06)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
				{
					generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;		
					
		default:
			break;
	}

}

void COD_DC18_DataHandle(void){

	switch(rs485_GetSentType())
	{
		case DO_SendType_GetModbusId:   //
			if(rs485_usart.rx_buf[1] == 0x03)
			{		
				uint8_t DO_ID = COD_DC18_ModbusID;
        		DO_AddProbe(DO_ID);		
				// DC18_DC17_flag = 18;
				rs485_DevicePlus();//添加下设备树上的设备个数
				COD_DC18_UpdateTemp2DO(get_COMADo()->modbus_id == COD_DC18_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));		
				COD_DC18_rs485_GetSN(get_COMADo(),COD_DC18_ModbusID);
			}
			break;
		
		case DO_SendType_GetSN:         //获取设备编码
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				COD_DC18_SetSN(get_COMADo()->modbus_id == COD_DC18_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]), 0);
				COD_DC18_rs485_GetSHWVersion(get_COMADo(),COD_DC18_ModbusID);

			}
			break;					
		
		case DO_SendType_GetSHWVersion: //获取软硬件版本号
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				COD_DC18_SetSHWVersion(get_COMADo()->modbus_id == COD_DC18_ModbusID ? get_COMADo() : get_COMBDo(), NULL);
				COD_DC18_rs485_Get_U(get_COMADo(),COD_DC18_ModbusID);
			}
			break;
	
		case DO_SendType_GetKB: //获取补偿系数U
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				COD_DC18_Set_U(get_COMADo()->modbus_id == COD_DC18_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_SetIsInit(get_COMADo()->modbus_id == COD_DC18_ModbusID ? get_COMADo() : get_COMBDo());
			}
			break;

			
		case DO_SendType_GetTempTwoDO:
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				COD_DC18_UpdateTemp2DO(get_COMADo()->modbus_id == COD_DC18_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == COD_DC18_ModbusID ? get_COMADo() : get_COMBDo());//设置设备已经有数据了
			}
			break;
			
	
		case DO_SendType_SetKB:
			if(rs485_usart.rx_buf[1] == 0x06 )//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{
						case PAGE_1_RESETCAL:
						case PAGE_5_DO_ONE_First:
						case PAGE_5_DO_TWO_SECOND:
		
						case PAGE_5_COD_DC18_Tur_Zero:
						case PAGE_5_COD_DC18_Tur_Slope:
						case PAGE_5_COD_DC18_Zero:
						case PAGE_5_COD_DC18_Slope:
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)chenggong_en;
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						break;
										
					default:
						break;
				}
			}
			break;
		
		case DO_SendType_SetTemp:
			if(rs485_usart.rx_buf[1] == 0x06)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_5_TEMP ||  interfacial_GetCurPage() ==  PAGE_5_COD_Cleanse ||  interfacial_GetCurPage() ==  PAGE_5_COD_DC18_Coefficient )
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
				
				
			}
			break;

	
		case DO_SendType_HyphiveClearCal:
			if(rs485_usart.rx_buf[1] == 0x06)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;		
					
		default:
			break;
	}
}

void pH_shenghui_DataHandle(void){

	switch(rs485_GetSentType())
	{
		case DO_SendType_GetModbusId:   //
			if(rs485_usart.rx_buf[1] == 0x04)
			{
				
				uint8_t DO_ID = pH_shenghui_ModbusID;
        DO_AddProbe(DO_ID);			
				rs485_DevicePlus();//添加下设备树上的设备个数
				pH_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == pH_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));				
				pH_shenghui_rs485_GetSN(get_COMADo());

			}
			break;
		
		case DO_SendType_GetSN:         //获取设备编码
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x0C)
			{
				close_circle();
				shenghui_SetSN(get_COMADo()->modbus_id == pH_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]), 12);
				pH_shenghui_rs485_GetSHWVersion(get_COMADo());

			}
			break;					
		
		case DO_SendType_GetSHWVersion: //获取软硬件版本号
			if(rs485_usart.rx_buf[1] == 0x04)
			{
				close_circle();
				shenghui_SetSHWVersion(get_COMADo()->modbus_id == pH_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),  &(rs485_usart.rx_buf[3]));
				DO_SetIsInit(get_COMADo()->modbus_id == pH_shenghui_ModbusID ? get_COMADo() : get_COMBDo());
			}
			break;
			
		
		case DO_SendType_GetTempTwoDO:
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x10)
			{
				close_circle();
				pH_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == pH_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == pH_shenghui_ModbusID ? get_COMADo() : get_COMBDo());//设置设备已经有数据了
			}
			break;
			
	
		case DO_SendType_SetKB:
			if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{
						case PAGE_5_NH3N_pH_ONE:
						case PAGE_5_NH3N_pH_TWO:
						case PAGE_5_NH3N_pH_THREE:
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
			  			interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
						interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_eng = (uint8_t *)chenggong_en;
						interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						break;
										
					default:
						break;
				}
			}
			break;
		
		case DO_SendType_SetTemp:
			if(rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_5_TEMP )
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;

			
		case DO_SendType_HyphiveClearCal:
			if(rs485_usart.rx_buf[2] == 0x9C && rs485_usart.rx_buf[3] == 0x51)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;		
			
		default:
			break;
	}

}


void Tur_shenghui_DataHandle(void){

	switch(rs485_GetSentType())
	{
		case DO_SendType_GetModbusId:   //
			if(rs485_usart.rx_buf[1] == 0x04)
			{
				
				uint8_t DO_ID = Tur_shenghui_ModbusID;
        DO_AddProbe(DO_ID);			
				rs485_DevicePlus();//添加下设备树上的设备个数
				Tur_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == Tur_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));				
				Tur_shenghui_rs485_GetSN(get_COMADo());

			}
			break;
		
		case DO_SendType_GetSN:         //获取设备编码
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x0C)
			{
				close_circle();
				shenghui_SetSN(get_COMADo()->modbus_id == Tur_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]), 12);
				Tur_shenghui_rs485_GetSHWVersion(get_COMADo());

			}
			break;					
		
		case DO_SendType_GetSHWVersion: //获取软硬件版本号
			if(rs485_usart.rx_buf[1] == 0x04)
			{
				close_circle();
				shenghui_SetSHWVersion(get_COMADo()->modbus_id == Tur_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),  &(rs485_usart.rx_buf[3]));
				DO_SetIsInit(get_COMADo()->modbus_id == Tur_shenghui_ModbusID ? get_COMADo() : get_COMBDo());
			}
			break;
			
		
		case DO_SendType_GetTempTwoDO:
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x08)
			{
				close_circle();
				Tur_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == Tur_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == Tur_shenghui_ModbusID ? get_COMADo() : get_COMBDo());//设置设备已经有数据了
			}
			break;
			
	
		case DO_SendType_SetKB:
			if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{
						case PAGE_1_RESETCAL:			
							generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
							break;			
						case PAGE_5_shenghui_Tur_ONE:
						case PAGE_5_shenghui_Tur_TWO:
						case PAGE_5_shenghui_Tur_THREE:		
							// generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
							// interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
							// interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_eng = (uint8_t *)chenggong_en;
							// interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
							Tur_shenghui_rs485_Get_Tur_Status(get_COMADo());
						break;
										
					default:
						break;
				}
			}
			break;

		case DO_SendType_Get_Tur_Status://探头状态
			if(rs485_usart.rx_buf[1] == 0x04)//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{		
					case PAGE_5_shenghui_Tur_ONE:
					case PAGE_5_shenghui_Tur_TWO:
					case PAGE_5_shenghui_Tur_THREE:		
						if(rs485_usart.rx_buf[4] == 0x01)
						{
							generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
							interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
							interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_eng = (uint8_t *)chenggong_en;
							interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						}
						else
						{
							Tur_shenghui_rs485_Get_Tur_Status(get_COMADo());
						}
					break;
									
				default:
					break;
				}
			}
			break;
		
		case DO_SendType_SetTemp:
			if(rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_5_TEMP  )
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;

			
		case DO_SendType_HyphiveClearCal:
			if(rs485_usart.rx_buf[2] == 0x9C && rs485_usart.rx_buf[3] == 0x51)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;		
			
		default:
			break;
	}

}

void EC_shenghui_DataHandle(void){

	switch(rs485_GetSentType())
	{
		case DO_SendType_GetModbusId:   //
			if(rs485_usart.rx_buf[1] == 0x04)
			{
				
				uint8_t DO_ID = EC_shenghui_ModbusID;
        DO_AddProbe(DO_ID);			
				rs485_DevicePlus();//添加下设备树上的设备个数
				EC_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == EC_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));				
				EC_shenghui_rs485_GetSN(get_COMADo());

			}
			break;
		
		case DO_SendType_GetSN:         //获取设备编码
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x0C)
			{
				close_circle();
				shenghui_SetSN(get_COMADo()->modbus_id == EC_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]), 12);
				EC_shenghui_rs485_GetSHWVersion(get_COMADo());

			}
			break;					
		
		case DO_SendType_GetSHWVersion: //获取软硬件版本号
			if(rs485_usart.rx_buf[1] == 0x04)
			{
				close_circle();
				shenghui_SetSHWVersion(get_COMADo()->modbus_id == EC_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),  &(rs485_usart.rx_buf[3]));
				DO_SetIsInit(get_COMADo()->modbus_id == EC_shenghui_ModbusID ? get_COMADo() : get_COMBDo());
			}
			break;
			
		
		case DO_SendType_GetTempTwoDO:
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x08)
			{
				close_circle();
				EC_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == EC_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == EC_shenghui_ModbusID ? get_COMADo() : get_COMBDo());//设置设备已经有数据了
			}
			break;
			
	
		case DO_SendType_SetKB:
			if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{
					case PAGE_1_RESETCAL:
					case PAGE_5_shenghui_EC_ONE:
			 			generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)chenggong_en;
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						break;
										
					default:
						break;
				}
			}
			break;
		
		case DO_SendType_SetTemp:
			if(rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_5_TEMP  )
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;

			
		case DO_SendType_HyphiveClearCal:
			if(rs485_usart.rx_buf[2] == 0x9C && rs485_usart.rx_buf[3] == 0x51)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;		
			
		default:
			break;
	}

}


void EC_DE26_DataHandle(void){

	switch(rs485_GetSentType())
	{
		case DO_SendType_GetModbusId:   //
			if(rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x14)
			{
				
				uint8_t DO_ID = EC_DE26_ModbusID;
        DO_AddProbe(DO_ID);			
				rs485_DevicePlus();//添加下设备树上的设备个数
				EC_DE26_UpdateTemp2DO(get_COMADo()->modbus_id == EC_DE26_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));				
				EC_DE26_rs485_GetSN(get_COMADo());

			}
			break;
		
		case DO_SendType_GetSN:         //获取设备编码
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				EC_DE26_SetSN(get_COMADo()->modbus_id == EC_DE26_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]), 12);
				EC_DE26_rs485_GetSHWVersion(get_COMADo());

			}
			break;					
		
		case DO_SendType_GetSHWVersion: //获取软硬件版本号
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				EC_DE26_SetSHWVersion(get_COMADo()->modbus_id == EC_DE26_ModbusID ? get_COMADo() : get_COMBDo());
				DO_SetIsInit(get_COMADo()->modbus_id == EC_DE26_ModbusID ? get_COMADo() : get_COMBDo());
			}
			break;
			
		
		case DO_SendType_GetTempTwoDO:
			if(rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x14)
			{
				close_circle();
				EC_DE26_UpdateTemp2DO(get_COMADo()->modbus_id == EC_DE26_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == EC_DE26_ModbusID ? get_COMADo() : get_COMBDo());//设置设备已经有数据了
			}
			break;
			
	
		case DO_SendType_SetKB:
			if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x06)//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{
					case PAGE_5_DE26_EC_Zero:		
					case PAGE_5_shenghui_EC_ONE:
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
			  			interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
						interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)chenggong_en;
						interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						break;
										
					default:
						break;
				}
			}
			break;
		
		case DO_SendType_SetTemp:
	  	if(rs485_usart.rx_buf[1] == 0x10)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_5_TEMP  )
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;

			
		case DO_SendType_HyphiveClearCal:
			if(rs485_usart.rx_buf[2] == 0x06 && (rs485_usart.rx_buf[3] == 0xf4 || rs485_usart.rx_buf[3] == 0xf5 ))
			{
				close_circle();
				if(rs485_usart.rx_buf[3] == 0xf4 && rs485_usart.rx_buf[5] == 0x30)
				{
				  EC_DE26_rs485_ClearCal(get_COMADo());					
				}
				else
				{
					if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
					{
							generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
					}				
				}				

			}
			break;		
			
		default:
			break;
	}

}


void pH_DpH07_DataHandle(void){

	switch(rs485_GetSentType())
	{
		case DO_SendType_GetModbusId:   //
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				
				uint8_t DO_ID = pH_DpH07_ModbusID;
        DO_AddProbe(DO_ID);			
				rs485_DevicePlus();//添加下设备树上的设备个数
				pH_DpH07_UpdateTemp2DO(get_COMADo()->modbus_id == pH_DpH07_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));				
				pH_DpH07_rs485_GetSN(get_COMADo());

			}
			break;
		
		case DO_SendType_GetSN:         //获取设备编码
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				pH_DpH07_SetSN(get_COMADo()->modbus_id == pH_DpH07_ModbusID ? get_COMADo() : get_COMBDo());
				pH_DpH07_rs485_GetSHWVersion(get_COMADo());

			}
			break;					
		
		case DO_SendType_GetSHWVersion: //获取软硬件版本号
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				COD_DC18_SetSHWVersion(get_COMADo()->modbus_id == pH_DpH07_ModbusID ? get_COMADo() : get_COMBDo(), NULL);
				DO_SetIsInit(get_COMADo()->modbus_id == pH_DpH07_ModbusID ? get_COMADo() : get_COMBDo());
			}
			break;
			
		
		case DO_SendType_GetTempTwoDO:
			if(rs485_usart.rx_buf[1] == 0x03 && rs485_usart.rx_buf[2] == 0x06)
			{
				close_circle();
				pH_DpH07_UpdateTemp2DO(get_COMADo()->modbus_id == pH_DpH07_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == pH_DpH07_ModbusID ? get_COMADo() : get_COMBDo());//设置设备已经有数据了
			}
			break;
			
	
		case DO_SendType_SetKB:
			if(rs485_usart.rx_buf[1] == 0x06 && rs485_usart.rx_buf[2] == 0x08 && rs485_usart.rx_buf[3] == 0x08)//校准返回
			{
				close_circle();
				switch(interfacial_GetCurPage())
				{
					case PAGE_1_RESETCAL:
					case PAGE_5_NH3N_pH_ONE:
					case PAGE_5_NH3N_pH_TWO:
					case PAGE_5_NH3N_pH_THREE:
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
						interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
						interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_eng = (uint8_t *)chenggong_en;
						interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						break;
										
					default:
						break;
				}
			}
			break;
		
		case DO_SendType_SetTemp:
			if(rs485_usart.rx_buf[1] == 0x06 && rs485_usart.rx_buf[2] == 0x08 && rs485_usart.rx_buf[3] == 0x0b)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_5_TEMP )
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;

			
		case DO_SendType_HyphiveClearCal:
			if(rs485_usart.rx_buf[1] == 0x06 && rs485_usart.rx_buf[2] == 0x08 && rs485_usart.rx_buf[3] == 0x33)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;		
			
		default:
			break;
	}

}




/*昇辉叶绿素传感器  串口数据处理*/
void Chl_shenghui_DataHandle(void){
	switch(rs485_GetSentType())
	{
		case DO_SendType_GetModbusId:   //获取modbus id
			if(rs485_usart.rx_buf[1] == 0x04)
			{	
				uint8_t DO_ID = Chl_shenghui_ModbusID;
				
				DO_AddProbe(DO_ID);
					
				rs485_DevicePlus();//添加下设备树上的设备个数
							
				GetCircularSent_Flag=0;
				Chl_shenghui_rs485_GetSN(get_COMADo());

			}
			break;
		
		case DO_SendType_GetSN:         //获取设备编码
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x0C)
			{
				close_circle();
				shenghui_SetSN(get_COMADo()->modbus_id == Chl_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]), 12);
				Chl_shenghui_rs485_GetSHWVersion(get_COMADo());
			}
			break;					
		
		case DO_SendType_GetSHWVersion: //获取软硬件版本号
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x02)
			{
				close_circle();
				shenghui_SetSHWVersion(get_COMADo()->modbus_id == Chl_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),&(rs485_usart.rx_buf[3]));
				Chl_shenghui_rs485_GetMeasure(get_COMADo());
			}
			break;	
			
		case DO_SendType_GetSalinity:  //获取量程
			if(rs485_usart.rx_buf[1] == 0x03)
			{
				close_circle();
				Chl_shenghui_Set_Measure_Range(get_COMADo()->modbus_id == Chl_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_SetIsInit(get_COMADo()->modbus_id == Chl_shenghui_ModbusID ? get_COMADo() : get_COMBDo());
			}
			break;
			
			
		case DO_SendType_GetTempTwoDO:
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x08)
			{
				close_circle();
				Chl_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == Chl_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),&(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == Chl_shenghui_ModbusID ? get_COMADo() : get_COMBDo());//设置do设备已经有数据了
			}
			break;
			
			
		case DO_SendType_GetDOPercent:
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x04)
			{
				close_circle();
				Chl_shenghui_UpdateTemp2DO_Vol(get_COMADo()->modbus_id == Chl_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),&(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == Chl_shenghui_ModbusID ? get_COMADo() : get_COMBDo());//设置do设备已经有数据了

			}
			break;
		
		case DO_SendType_SetKB:
			if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{		
		  	close_circle();
				switch(interfacial_GetCurPage())
				{
					case PAGE_5_DE26_EC_Zero:		
					case PAGE_5_shenghui_EC_ONE:
							generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
							interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
							interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_eng = (uint8_t *)chenggong_en;
							interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						break;
										
					default:
						break;
				}
			}
			break;
		
		case DO_SendType_SetTemp:
			if( rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_5_TEMP ||  interfacial_GetCurPage() ==PAGE_4_Measure_Switch)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;

		case DO_SendType_HyphiveClearCal:
			if(rs485_usart.rx_buf[2] == 0x9C && rs485_usart.rx_buf[3] == 0x51)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;								
		default:
			break;
	}
}


/*ORP DR31传感器  串口数据处理*/
void ORP_DR31_DataHandle(void){
	switch(rs485_GetSentType())
	{	
		case DO_SendType_GetModbusId:   //获取modbus id
			if(rs485_usart.rx_buf[1] == 0x04)
			{	
				uint8_t DO_ID = ORP_DR31_ModbusID;
				
				DO_AddProbe(DO_ID);
					
				rs485_DevicePlus();//添加下设备树上的设备个数
				DO_SetIsInit(get_COMADo()->modbus_id == ORP_DR31_ModbusID ? get_COMADo() : get_COMBDo());
				GetCircularSent_Flag=0;
			}
			break;

		case DO_SendType_GetTempTwoDO:  //获取测量值
			if(rs485_usart.rx_buf[1] == 0x04)
			{
				close_circle();
				ORP_DR31_UpdateTemp2DO(get_COMADo()->modbus_id == ORP_DR31_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == ORP_DR31_ModbusID ? get_COMADo() : get_COMBDo());//设置设备已经有数据了
			}
			break;

		case DO_SendType_HyphiveClearCal: //恢复出厂设置
			if(rs485_usart.rx_buf[2] == 0x0B && rs485_usart.rx_buf[3] == 0x33)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
				{
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;	

		case DO_SendType_SetKB://ORP单点校准
			if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{		
		  		close_circle();
				switch(interfacial_GetCurPage())
				{
					case PAGE_5_DR31_ORP_ONE:		
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
						interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
						interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->content_eng = (uint8_t *)chenggong_en;
						interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						break;
										
					default:
						break;
				}
			}
			break;
	}
}


/*昇辉BGA传感器  串口数据处理*/
void BGA_shenghui_DataHandle(void){
	switch(rs485_GetSentType())
	{
		case DO_SendType_GetModbusId:   //获取modbus id
			if(rs485_usart.rx_buf[1] == 0x04)
			{	
				uint8_t DO_ID = Bga_shenghui_ModbusID;
				
				DO_AddProbe(DO_ID);
					
				rs485_DevicePlus();//添加下设备树上的设备个数
							
				GetCircularSent_Flag=0;
				BGA_shenghui_rs485_GetSN(get_COMADo());
			}
			break;
		
		case DO_SendType_GetSN:         //获取设备编码
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x0C)
			{
				close_circle();
				shenghui_SetSN(get_COMADo()->modbus_id == Bga_shenghui_ModbusID ? get_COMADo() : get_COMBDo(), &(rs485_usart.rx_buf[3]), 12);
				BGA_shenghui_rs485_GetSHWVersion(get_COMADo());
			}
			break;					
		
		case DO_SendType_GetSHWVersion: //获取软硬件版本号
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x02)
			{
				close_circle();
				shenghui_SetSHWVersion(get_COMADo()->modbus_id == Bga_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),&(rs485_usart.rx_buf[3]));
				DO_SetIsInit(get_COMADo()->modbus_id == Bga_shenghui_ModbusID ? get_COMADo() : get_COMBDo());
			}
			break;	
			
		case DO_SendType_GetTempTwoDO://更新数据
			if(rs485_usart.rx_buf[1] == 0x04 && rs485_usart.rx_buf[2] == 0x08)
			{
				close_circle();
				BGA_shenghui_UpdateTemp2DO(get_COMADo()->modbus_id == Bga_shenghui_ModbusID ? get_COMADo() : get_COMBDo(),&(rs485_usart.rx_buf[3]));
				DO_SetIsGetedValue(get_COMADo()->modbus_id == Bga_shenghui_ModbusID ? get_COMADo() : get_COMBDo());//设置do设备已经有数据了
			}
			break;
		
		case DO_SendType_SetKB://标定点1,2校准
			if(rs485_usart.rx_buf[1] == 0x10 && rs485_usart.rx_buf[2] == 0x9C)//校准返回
			{		
		  		close_circle();
				switch(interfacial_GetCurPage())
				{
					case PAGE_5_shenghui_BGA_ONE:
					case PAGE_5_shenghui_BGA_TWO:
						generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
						interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->content_chn = (uint8_t *)jiaozhunchenggong_cn;//校准成功
						interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->content_eng = (uint8_t *)chenggong_en;
						interfacial_GetCurrentInterfacial()->label_head->next_label->next_label->next_label->ChnContent_size = sizeof(jiaozhunchenggong_cn);
						break;
										
					default:
						break;
				}
			}
			break;
		
		case DO_SendType_SetTemp://温度校准
			if( rs485_usart.rx_buf[2] == 0x9C && rs485_usart.rx_buf[3] == 0x57)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_5_TEMP)
				{
					generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;

		case DO_SendType_HyphiveClearCal://恢复出厂设置
			if(rs485_usart.rx_buf[2] == 0x9C && rs485_usart.rx_buf[3] == 0x51)
			{
				close_circle();
				if(interfacial_GetCurPage() == PAGE_1_RESETCAL)
				{
					generate_MessageBox(MESSAGE_SUCCESSFUL, 1);
				}
			}
			break;								
		default:
			break;
	}
}

//这是在main里处理的
void rs485_DataHandle(void)
{
	if(rs485_GetRxFlag())//如果接收到数据 并 通过crc校验了的话
	{
		rs485_ClearRxFlag();
		
		if(rs485_usart.rx_size < 5)
		{
			return;
		}
		
		if(CheckCrc(rs485_usart.rx_buf, rs485_usart.rx_size))
		{
			switch(rs485_usart.rx_buf[0])
			{
				case DO_HF1012_ModbusID:  
          	  DO_HF1012_DataHandle();					
			    break;
				
				case NH3N_DN02_ModbusID:  
          	  NH3N_DN02_DataHandle();				
			    break;
				
				case COD_DC18_ModbusID:
					COD_DC18_DataHandle();
			    break;
			
				case COD_DC17_ModbusID:
					COD_DC17_DataHandle();
				break;
			
				case DO_shenghui_ModbusID:  
          	  DO_shenghui_DataHandle();					
			    break;
				
				case NH3N_shenghui_ModbusID:  
		      NH3N_shenghui_DataHandle();
			    break;
				
				case COD_shenghui_ModbusID:  
		      COD_shenghui_DataHandle();
			    break;	
			
				case pH_shenghui_ModbusID:  
		      pH_shenghui_DataHandle();
			    break;	
				
				case Tur_shenghui_ModbusID:  
		      Tur_shenghui_DataHandle();
			    break;

				case EC_shenghui_ModbusID:  
		      EC_shenghui_DataHandle();
			    break;

				case EC_DE26_ModbusID:  
		      EC_DE26_DataHandle();
			    break;
				
				case pH_DpH07_ModbusID:  
		      pH_DpH07_DataHandle();
			    break;	

				case Chl_shenghui_ModbusID:  
		      Chl_shenghui_DataHandle();
			    break;	

				case ORP_DR31_ModbusID:  
		      ORP_DR31_DataHandle();
			    break;	
				
				case Bga_shenghui_ModbusID:
			  BGA_shenghui_DataHandle();
				break;

				default:
					break;
			}

		}
	}
}






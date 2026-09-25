#include "ch340e.h"
#include "stdio.h"
#include "log.h"

#include "spi_flash.h"

#include "setting.h"

void ch340e_UartInit(UART_HandleTypeDef *huart);


 ch340e_t ch340e_usart = {
	.init = ch340e_UartInit,
	.receive_flag = 0
};
 
void ch340e_UartInit(UART_HandleTypeDef *huart)
{
	ch340e_usart.huart = huart;
	__HAL_UART_ENABLE_IT(ch340e_usart.huart, UART_IT_IDLE);      //开启空闲中断
	HAL_UART_Receive_DMA(ch340e_usart.huart, ch340e_usart.rx_buf, CH340E_RXBUFFSIZE);//开始dma接收
}


void ch340e_RxCallBack(UART_HandleTypeDef *huart)
{
		if(ch340e_usart.huart == huart)
	{
		
	}
}


void ch340e_SetRxFlag(void)
{
	ch340e_usart.receive_flag = 1;
}
uint8_t ch340e_GetRxFlag(void)
{
	return ch340e_usart.receive_flag;
}
void ch340e_ClearRxFlag(void)
{
	ch340e_usart.receive_flag = 0;
}


void ch340e_IDLECallBack(UART_HandleTypeDef *huart)
{
	
	if(huart->Instance == CH340E_USART)
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart);		//清中断

		HAL_UART_AbortReceive(huart);	//已经接收完一帧数据,所以这里要停止接收,然后再重新接收		
		
		ch340e_usart.rx_size = CH340E_RXBUFFSIZE - hdma_usart1_rx.Instance->CNDTR;  //接收到多少数据
		
		ch340e_SetRxFlag();

		HAL_UART_Receive_DMA(ch340e_usart.huart, ch340e_usart.rx_buf, CH340E_RXBUFFSIZE);//开始dma接收
	}
}




void clear_ch340erx_buf(void)
{
	clear_buf(ch340e_usart.rx_buf, CH340E_RXBUFFSIZE);
}

#ifdef __GNUC__
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(ch340e_usart.huart, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}


void ch340_DataHandle(void)
{
	uint16_t temp_index = 0; // 查询记录下标
	
	uint8_t check_byte = 0;  // 用来确认flash是否写入成功了
	
	datetime_t temp_time;    // 设置时间临时变量
	uint8_t setting_callback[6];
	
	if(ch340e_GetRxFlag())//如果接收到数据 并 通过crc校验了的话
	{
		ch340e_ClearRxFlag();
		if(CheckCrc(ch340e_usart.rx_buf, ch340e_usart.rx_size))
		{
			if(ch340e_usart.rx_buf[0] == 0xFE)
			{
				switch(ch340e_usart.rx_buf[1])
				{
					case 0x02://写数据
						switch(ch340e_usart.rx_buf[2])
						{
							case 0x03://恢复出厂设置中英文
								W25QXX_Write(&(ch340e_usart.rx_buf[3]), SETTING_CNORENG_ADDR, 1);
								
								setting_reset();
								
								setting_callback[0] = 0xFF;
								setting_callback[1] = 0x02;
								setting_callback[2] = 0x03;
								W25QXX_Read(&check_byte, SETTING_CNORENG_ADDR, 1);
								setting_callback[3] = ((check_byte == ch340e_usart.rx_buf[3]) ? 1 : 0);
								break;
							
							case 0x04://设置时间
								temp_time.years   = ch340e_usart.rx_buf[3];
								temp_time.month   = ch340e_usart.rx_buf[4];
								temp_time.day     = ch340e_usart.rx_buf[5];
								temp_time.hour    = ch340e_usart.rx_buf[6];
								temp_time.minute  = ch340e_usart.rx_buf[7];
								temp_time.seconds = ch340e_usart.rx_buf[8];
								HYM8563_SetTime(&temp_time);
								
								setting_callback[0] = 0xFF;
								setting_callback[1] = 0x02;
								setting_callback[2] = 0x04;
								if(RTC_GetYear() == ch340e_usart.rx_buf[3] && RTC_GetMonth() == ch340e_usart.rx_buf[4] && RTC_GetDay() == ch340e_usart.rx_buf[5] &&
									RTC_GetHour() == ch340e_usart.rx_buf[6] && RTC_GetMinute() == ch340e_usart.rx_buf[7])
								{
									setting_callback[3] = 1;
								}
								else
								{
									setting_callback[3] = 0;
								}
								break;
								
							case 0x05://定制时候切换logo
								W25QXX_Write(&(ch340e_usart.rx_buf[3]), SETTING_LOGO_ADDR, 1);
								setting_reset();
								setting_callback[0] = 0xFF;
								setting_callback[1] = 0x02;
								setting_callback[2] = 0x05;
								W25QXX_Read(&check_byte, SETTING_LOGO_ADDR, 1);
								setting_callback[3] = ((check_byte == ch340e_usart.rx_buf[3]) ? 1 : 0);
								break;
							
							case 0x06://清flash
								setting_callback[0] = 0xFF;
								setting_callback[1] = 0x02;
								setting_callback[2] = 0x06;
							
								W25QXX_Erase_Chip();
								HAL_Delay(300);
							
								setting_callback[3] = 0x01;
								first_write();
								break;
							
							case 0x07://写是否有gps
								W25QXX_Write(&(ch340e_usart.rx_buf[3]), SETTING_HAVEGPS_ADDR, 1);
								
								setting_reset();
								
								setting_callback[0] = 0xFF;
								setting_callback[1] = 0x02;
								setting_callback[2] = 0x07;
								W25QXX_Read(&check_byte, SETTING_HAVEGPS_ADDR, 1);
								setting_callback[3] = ((check_byte == ch340e_usart.rx_buf[3]) ? 1 : 0);
								break;
							
							case 0x08:  //设置屏幕对比度的
								W25QXX_Write(&(ch340e_usart.rx_buf[3]), SETTING_CONTRAST_ADDR, 1);
								
								setting_reset();
								
								setting_callback[0] = 0xFF;
								setting_callback[1] = 0x02;
								setting_callback[2] = 0x08;
								W25QXX_Read(&check_byte, SETTING_CONTRAST_ADDR, 1);
								setting_callback[3] = ((check_byte == ch340e_usart.rx_buf[3]) ? 1 : 0);
								break;

								case 0x09://写电池类型
									W25QXX_Write(&(ch340e_usart.rx_buf[3]), BATTERT_TYPE_ADDR, 1);
								
									setting_reset();
									
									setting_callback[0] = 0xFF;
									setting_callback[1] = 0x02;
									setting_callback[2] = 0x09;
									W25QXX_Read(&check_byte, BATTERT_TYPE_ADDR, 1);
									setting_callback[3] = ((check_byte == ch340e_usart.rx_buf[3]) ? 1 : 0);

									break;	
							
							default:
								break;							
						}
						
						SetCrc(setting_callback, sizeof(setting_callback));
						HAL_UART_Transmit(ch340e_usart.huart, setting_callback, sizeof(setting_callback),200);
						break;
					
					case 0x03://查数据
						switch(ch340e_usart.rx_buf[2])
						{
							case 0x01://查总共记录条数
							  log_SendCount();
                DO_Send_Count = 0;
								pH_Send_Count = 0;	
								Tur_Send_Count = 0;
								FCL_Send_Count = 0;		
								EC_Send_Count = 0;
								ORP_Send_Count = 0;
								NH4_Send_Count = 0;		
								F_Send_Count = 0;		
								CL_Send_Count = 0;
								Chl_Send_Count = 0;
								Bga_Send_Count = 0;	
								COD_Send_Count = 0;
								MLSS_Send_Count = 0;	
								OiW_Send_Count = 0;		
								OiW_YUSHAN_Send_Count = 0;
								TSS_Send_Count = 0;	
								SAL_Send_Count = 0;	
								TDS_Send_Count = 0;	
								break;
							
							case 0x02://查第几条记录
								temp_index = (ch340e_usart.rx_buf[3]<<8) | ch340e_usart.rx_buf[4];
								log_SendBytes();
								break;
							
							default:
								break;
						}
						break;
					default:
						break;
					
				}
			}
		}
	}
}










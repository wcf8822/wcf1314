#include "rtc.h"
#include "setting.h"
#include "lcd_drive.h"
#include "key.h"
STATIC datetime_t machine_time;//保护起来不让别的地方可以改只能调用接口来改动时间
STATIC uint8_t minute_ShutDown = 60;//设成61先防止开机直接拉闸

uint8_t dec2bcd(uint8_t DecData)
{
	return ((DecData/10)<<4)|((DecData%10));
}

uint8_t bcd2dec(uint8_t BcdData)
{
	return (((BcdData>>4)*10)+(BcdData&0x0F));
}

void HYM8563_Write_OneByte(uint8_t addr,uint8_t data)
{
	IIC_Start(); //产生IIC起始信号
	IIC_Send_Byte(0XA2); //发送写命令
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Send_Byte(addr); //发送地址
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Send_Byte(data); //发送字节
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Stop();//产生一个停止条件
}

void HYM8563_UpdateTime(void) //采用写地址读数据模式
{
	IIC_Start(); //产生IIC起始信号
	IIC_Send_Byte(0XA2); //发送写命令
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Send_Byte(0x02); //发送秒地址
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Start();
	IIC_Send_Byte(0XA3); //进入接收模式 读命令
	IIC_Wait_Ack(); //等待应答信号到来
	
	machine_time.seconds = bcd2dec(IIC_Read_Byte(1) & 0x7f);
	machine_time.minute = bcd2dec(IIC_Read_Byte(1) & 0x7f);
	machine_time.hour = bcd2dec(IIC_Read_Byte(1) & 0x3f);
	machine_time.day = bcd2dec(IIC_Read_Byte(1) & 0x3f);
	machine_time.week = bcd2dec(IIC_Read_Byte(1) & 0x07);
	machine_time.month = bcd2dec(IIC_Read_Byte(1) & 0x1f);
	machine_time.years = bcd2dec(IIC_Read_Byte(0));
	IIC_Stop();//产生一个停止条件
	
	
	RTC_AutoShut();
	/*if(Seconds >= 0x80) //VL标志 vl=1 电压过低 不保证准确时钟
	{//可重置时钟
		Seconds&=0x7f;
	}*/
}



void HYM8563_SetTime(datetime_t *p)
{
	//时间转换为BCD码后再进行写入时钟
	IIC_Start(); //产生IIC起始信号
	IIC_Send_Byte(0XA2); //发送写命令
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Send_Byte(0x02); //发送秒地址
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Send_Byte(dec2bcd(p->seconds)); //秒
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Send_Byte(dec2bcd(p->minute)); //分
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Send_Byte(dec2bcd(p->hour)); //时
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Send_Byte(dec2bcd(p->day)); //日
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Send_Byte(dec2bcd(p->week)); //周
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Send_Byte(dec2bcd(p->month)); //月
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Send_Byte(dec2bcd(p->years)); //年
	IIC_Wait_Ack(); //等待应答信号到来
	IIC_Stop();//产生一个停止条件
}

void HYM8563_init(void)
{
	SDA_OUT();
	HYM8563_Write_OneByte(0x00,0x00);
	HYM8563_Write_OneByte(0x01,0x1f);
	HYM8563_Write_OneByte(0x0d,0x82);
	HYM8563_Write_OneByte(0x0e,0x00);
	HYM8563_UpdateTime();
	
	if(machine_time.month == 0 || machine_time.day == 0)
	{
		machine_time.month = 1;
		machine_time.day = 1;
		HYM8563_SetTime(&machine_time);
		HYM8563_UpdateTime();
	}
}

uint8_t RTC_GetYear(void)
{
	return machine_time.years;
}
uint8_t RTC_GetMonth(void)
{
	return machine_time.month;
}
uint8_t RTC_GetDay(void)
{
	return machine_time.day;
}
uint8_t RTC_GetHour(void)
{
	return machine_time.hour;
}
uint8_t RTC_GetMinute(void)
{
	return machine_time.minute;
}
uint8_t RTC_GetSecond(void)
{
	return machine_time.seconds;
}
uint8_t RTC_GetWeek(void)
{
	return machine_time.week;
}
datetime_t RTC_GetTime(void){
	return machine_time;
}
void RTC_UpdateShutDownTime(uint8_t autoshut)
{
	uint8_t tim = autoshut + RTC_GetMinute();//现在的时间加上自动关机的时间
	minute_ShutDown = (tim >= 60 ? tim - 60 : tim);
}


void RTC_AutoShut(void)
{
	if(setting_GetAutoShut() && minute_ShutDown == machine_time.minute)
	{
    HAL_GPIO_WritePin(LCD_BLC_GPIO_Port, LCD_BLC_Pin, GPIO_PIN_RESET);  //关闭背光
		GUI_ClearSCR(0x00);       // 初始化缓冲区为0x00，并输出屏幕(清屏)
		for(uint8_t i=0; i<200;i++)GUI_UpdateDisplay();//刷新下屏幕
		
		if(!HAL_GPIO_ReadPin(USB_JOIN_GPIO_Port, USB_JOIN_Pin)) //充电过程中 关机了 软件复位再次进入充电显示状态
    { 
			if(KEY_OFF_STATUS != KEY_BURSTMODE)
			{
			  HAL_NVIC_SystemReset();
			}
		}
    else
    {
			SHUTDOWN();		//关闭电源
			while(1)
			{
			} 		
		}	
	}
}


 uint16_t month_days_table[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
/**
* @brief        判断是否为闰年
* @param        [in] year 年
* @retval       1 为闰年
* @retval       0 为平年
*/
uint16_t fml_leap_year(uint16_t year)
{
    return (((year % 4 == 0)&&(year % 100 != 0)) || (year % 400 == 0));
}
/**
* @brief        日期转时间戳
* @param        [in] date 日期值
* @retval       return:返回时间戳
*/

uint32_t fml_time_to_stamp(datetime_t date)
{
    uint16_t i;

	uint16_t yearZKX = 0;
	uint16_t leap_year_count = 0;
	uint32_t day_count = 0;
	uint32_t dax = 0; 
	
	yearZKX = date.years+2000;
    // 计算闰年数
    for (i = 1970; i < yearZKX; i++)
    {
        if (fml_leap_year(i))
        {
            leap_year_count++;
        }
    }
 
    // 计算年的总天数
    day_count = leap_year_count * 366 + (yearZKX - 1970 - leap_year_count) * 365;//zkx闰年天数+平年天数
 
    // 累加计算当年所有月的天数
    for (i = 1; i < date.month; i++)
    {
        if ((2 == i) && (fml_leap_year(yearZKX)))
        {
            day_count += 29;
        }
        else
        {
            day_count += month_days_table[i];
        }
    }
 
    // 累加计算当月的天数
    day_count += (date.day - 1);
 
    dax = (uint32_t)(day_count * 86400) + (uint32_t)((uint32_t)date.hour * 3600) + (uint32_t)((uint32_t)date.minute * 60) + (uint32_t)date.seconds;
 
    /* 北京时间补偿 */
    dax = dax - 8*60*60;
 
    return dax;
}








#include "battery.h"
#include "adc.h"
#include "loadbit.h"

static double sum=0.0;
static uint8_t count = 1;
static uint8_t flag_BlinkBattery = 0;

bat_flag_t bat_flags={//防止ad波动然后电量变多就只要出现一次另一档电压这档电压就关闭了
	.bat_lvlnone = 0,
	.bat_lvl1 = 0,
	.bat_lvl2 = 0,
	.bat_lvl3 = 0
};

uint32_t ADC_ReadData(void)
{
	static uint32_t AD_Data;
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 100);
	AD_Data = HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
	HAL_Delay(5);
	return AD_Data;
}

double get_bat(void)
{
	double ret;
	
	sum += ADC_ReadData();
	
	if(count >= BATTERY_TIMES)
	{
		ret=sum/count/2048.0 * 3.235;		 
		count = 1;
		sum = 0;
	}
	else
	{
		ret = -1.0;
		count++;
	}
	
	return ret;
}


double ADC_average(uint8_t times)
{
	double sum=0.0;
	for(uint8_t i=0; i<times; i++)
	{
		sum += ADC_ReadData();
	}
	return sum/times;

}

double get_bat_avg(uint8_t times)
{
   return ADC_average(times)/2048.0 * 3.235;	 
}

void battery_draw(float ad_value)
{
	static uint8_t BatteryCharge_count = 0;//充电	
	if(setting_GetBattertType())
	{
		if(ad_value >= BATTERY_LAVEL_3)
		{
			if(!bat_flags.bat_lvl3)
			{
				GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARR3, 22, 11); //显示三格电
			}
		}
		else if(ad_value >= BATTERY_LAVEL_2 && ad_value < BATTERY_LAVEL_3)
		{
			bat_flags.bat_lvl3 = 1;//关闭上一档入口
			if(!bat_flags.bat_lvl2)
			{
				GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARR2, 22, 11); //显示两格电
			}
		}
		else if(ad_value >= BATTERY_LAVEL_1 && ad_value < BATTERY_LAVEL_2)
		{
			bat_flags.bat_lvl2 = 1;//
			if(!bat_flags.bat_lvl1)
			{
				GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARR1, 22, 11); //显示一格电
			}
		}
		else if(ad_value >= BATTERY_LAVEL_USB && ad_value < BATTERY_LAVEL_1)
		{
			bat_flags.bat_lvl1 = 1;//
			bat_flags.bat_lvlnone = 1;
		}
		
		
		if(bat_flags.bat_lvlnone)
		{
			flag_BlinkBattery = !flag_BlinkBattery;                      //图标开始闪了
			if(flag_BlinkBattery)
			{
				GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARR0, 22, 11);//没格子了
			}
			else
			{
				GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARRNONE, 22, 11);//没格子了
			}
		}
		if(ad_value < BATTERY_LAVEL_USB)
		{
			GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARRUSB, 22, 11);
		}
	}
	else
 {
	if( !HAL_GPIO_ReadPin(USB_JOIN_GPIO_Port, USB_JOIN_Pin) && ad_value < BATTERY_VOLTAGE_MAX)//如果接了数据线并且电量没有达到满电要求,则为充电状态
	{
				if(BatteryCharge_count == 0)
				{
			    GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARR0, 22, 11);//没格子了
				}
				else if(BatteryCharge_count == 1)
				{
			  	GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARR1, 22, 11); //显示一格电
				}
				else if(BatteryCharge_count == 2)
				{
					GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARR2, 22, 11); //显示两格电
				}	
				else if(BatteryCharge_count == 3)
				{
					GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARR3, 22, 11); //显示三格电
				}					
				BatteryCharge_count++;
				if(BatteryCharge_count >= 4)
				{
					BatteryCharge_count = 0;
				}
	}
	else
	{
		if(ad_value >= BATTERY_LAVEL_3)//4.6	3格电
		{
			if(!bat_flags.bat_lvl3)
			{
				GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARR3, 22, 11); //显示三格电
			}
		}
		else if(ad_value >= BATTERY_LAVEL_2 && ad_value < BATTERY_LAVEL_3)//4.1~4.6		2格电
		{
			bat_flags.bat_lvl3 = 1;//关闭上一档入口
			if(!bat_flags.bat_lvl2)
			{
				GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARR2, 22, 11); //显示两格电
			}
		}
		else if(ad_value >= BATTERY_LAVEL_1 && ad_value < BATTERY_LAVEL_2)//3.6~4.1		1格电
		{
			bat_flags.bat_lvl2 = 1;//
			if(!bat_flags.bat_lvl1)
			{
				GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARR1, 22, 11); //显示一格电
			}
		}
		else if(ad_value < BATTERY_LAVEL_1)//0.6~3.6	0格电
		{
			bat_flags.bat_lvl1 = 1;
			GUI_LoadPic(130, 4, (uint8_t *)BATTERY_LAVEL_ARR0, 22, 11);//没格子了
		}	
	}			 
 }	
	
}
	
void battery_draw_big(float ad_value)
{
	static uint8_t BatteryCharge_count = 0;//充电
	if( ad_value < BATTERY_VOLTAGE_MAX)//如果接了数据线并且电量没有达到满电要求,则为充电状态
	{
			if(BatteryCharge_count == 0)
			{
				GUI_LoadPic(36, 58, (uint8_t *)BATTERY_LAVEL_ARR0_BIG, 88, 44);//没格子了
			}
			else if(BatteryCharge_count == 1)
			{
				GUI_LoadPic(36, 58, (uint8_t *)BATTERY_LAVEL_ARR1_BIG, 88, 44); //显示一格电
			}
			else if(BatteryCharge_count == 2)
			{
				GUI_LoadPic(36, 58, (uint8_t *)BATTERY_LAVEL_ARR2_BIG, 88, 44); //显示两格电
			}	
			else if(BatteryCharge_count == 3)
			{
				GUI_LoadPic(36, 58, (uint8_t *)BATTERY_LAVEL_ARR3_BIG, 88, 44); //显示三格电
			}					
			BatteryCharge_count++;
			if(BatteryCharge_count >= 4)
			{
				BatteryCharge_count = 0;
			}
	}
	else
  {
		GUI_LoadPic(36, 58, (uint8_t *)BATTERY_LAVEL_ARR3_BIG, 88, 44); //显示三格电	
	}
}







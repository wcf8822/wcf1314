#include "bee.h"
#include "setting.h"

STATIC uint8_t bee_count = 0;
STATIC uint8_t bee_flag = 0;

STATIC uint8_t BeeAlarm_flag = 0;
STATIC uint8_t BeeAlarm_count = 0;
STATIC uint8_t BeeAlarm_times = 0;

void set_BeeOn(void)
{
	bee_flag = 1;
}

void set_BeeAlarm(void)
{
	BeeAlarm_flag = 1;
}

void bee_handle(void)
{
	if(setting_GetKeyPadTone() && bee_flag)
	{
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
		if(++bee_count >= BEE_TIM)
		{
			bee_count = 0;
			bee_flag = 0;
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		}
	}
	else if(setting_GetAlarmTone() && BeeAlarm_flag)
	{
		if(BeeAlarm_times % 2)
		{
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
		}
		if(++BeeAlarm_count >= BEE_TIM)
		{
			BeeAlarm_count = 0;
			if(++BeeAlarm_times >= ALARM_TIMES)
			{
				BeeAlarm_times = 0;
				BeeAlarm_flag = 0; 
			}
			
		}
	}
	else
	{
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
	}
}

















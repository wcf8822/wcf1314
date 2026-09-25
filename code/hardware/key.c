#include "key.h"
#include "heart.h"

#include "interfacial.h"

uint8_t Key_Off_Flag=0;

//实例化按键flag
static volatile KeyFlag_t key_flag={
	.flag_KeyUp = 0,
	.flag_KeyDown = 0,
	.flag_KeyOk = 0,
	.flag_KeyEsc = 0,
	.flag_KeyMenu = 0,
	.flag_KeyCal = 0,
	.flag_KeyBlu = 0,
	.flag_KeyClicked = 0,
	
	.flag_KeyCalLong = 0,
	
	.flag_KeyOFF =0,
	.flag_KeyOFFLong=0,
	.flag_KeyCalLongLong = 0
};

//获取按键标志
uint8_t get_KeyUpFlag(void)
{
	return key_flag.flag_KeyUp;
}
uint8_t get_KeyDownFlag(void)
{
	return key_flag.flag_KeyDown;
}
uint8_t get_KeyOkFlag(void)
{
	return key_flag.flag_KeyOk;
}
uint8_t get_KeyOkLongFlag(void)
{
	return key_flag.flag_KeyOkLong;
}
uint8_t get_KeyEscFlag(void)
{
	return key_flag.flag_KeyEsc;
}
uint8_t get_KeyMenuFlag(void)
{
	return key_flag.flag_KeyMenu;
}
uint8_t get_KeyCalFlag(void)
{
	return key_flag.flag_KeyCal;
}
uint8_t get_KeyCalLongFlag(void)
{
	return key_flag.flag_KeyCalLong;
}
uint8_t get_KeyBluFlag(void)
{
	return key_flag.flag_KeyBlu;
}
uint8_t get_KeyClickedFlag(void)
{
	return key_flag.flag_KeyClicked;
}

uint8_t get_KeyOffFlag(void)
{
	return key_flag.flag_KeyOFF;
}
uint8_t get_KeyOffLongFlag(void)
{
	return key_flag.flag_KeyOFFLong;
}

uint8_t get_KeyCalLongLongFlag(void)
{
	return key_flag.flag_KeyCalLongLong;
}



//清除按键标志
void clear_KeyUpFlag(void)
{
	key_flag.flag_KeyUp = KEY_NORMAL;
}
void clear_KeyDownFlag(void)
{
	key_flag.flag_KeyDown = KEY_NORMAL;
}
void clear_KeyOkFlag(void)
{
	key_flag.flag_KeyOk = KEY_NORMAL;
}
void clear_KeyOkLongFlag(void)
{
	key_flag.flag_KeyOkLong = KEY_NORMAL;
}
void clear_KeyEscFlag(void)
{
	key_flag.flag_KeyEsc = KEY_NORMAL;
}
void clear_KeyMenuFlag(void)
{
	key_flag.flag_KeyMenu = KEY_NORMAL;
}
void clear_KeyCalFlag(void)
{
	key_flag.flag_KeyCal = KEY_NORMAL;
}
void clear_KeyCalLongFlag(void)
{
	key_flag.flag_KeyCalLong = KEY_NORMAL;
}
void clear_KeyBluFlag(void)
{
	key_flag.flag_KeyBlu = KEY_NORMAL;
}
void clear_KeyClickedFlag(void)
{
	key_flag.flag_KeyClicked = KEY_NORMAL;
}

void clear_KeyOffFlag(void)
{
	key_flag.flag_KeyOFF = KEY_NORMAL;
}
void clear_KeyOffLongFlag(void)
{
	key_flag.flag_KeyOFFLong = KEY_NORMAL;
}

void clear_KeyCalLongLongFlag(void)
{
	key_flag.flag_KeyCalLongLong = KEY_NORMAL;
}

void clear_KeyAllFlag(void)
{
	key_flag.flag_KeyBlu     = KEY_NORMAL;
	key_flag.flag_KeyCal     = KEY_NORMAL;
	key_flag.flag_KeyCalLong = KEY_NORMAL;
	key_flag.flag_KeyClicked = KEY_NORMAL;
	key_flag.flag_KeyDown    = KEY_NORMAL;
	key_flag.flag_KeyEsc     = KEY_NORMAL;
	key_flag.flag_KeyMenu    = KEY_NORMAL;
	key_flag.flag_KeyOk      = KEY_NORMAL;
	key_flag.flag_KeyUp      = KEY_NORMAL;
	key_flag.flag_KeyOFF      = KEY_NORMAL;
	key_flag.flag_KeyOFFLong      = KEY_NORMAL;
	key_flag.flag_KeyCalLongLong = KEY_NORMAL;
}

//设置按键接口
void set_KeyUpFlag(void)
{
	key_flag.flag_KeyUp = KEY_CLICKED;
}
void set_KeyDownFlag(void)
{
	key_flag.flag_KeyDown = KEY_CLICKED;
}
void set_KeyOkFlag(void)
{
	key_flag.flag_KeyOk = KEY_CLICKED;
}
void set_KeyOkLongFlag(void)
{
	key_flag.flag_KeyOkLong = KEY_CLICKED;
}
void set_KeyEscFlag(void)
{
	key_flag.flag_KeyEsc = KEY_CLICKED;
}
void set_KeyMenuFlag(void)
{
	key_flag.flag_KeyMenu = KEY_CLICKED;
}
void set_KeyCalFlag(void)
{
	key_flag.flag_KeyCal = KEY_CLICKED;
}
void set_KeyCalLongFlag(void)
{
	key_flag.flag_KeyCalLong = KEY_CLICKED;
}
void set_KeyBluFlag(void)
{
	key_flag.flag_KeyBlu = KEY_CLICKED;
}
void set_KeyClickedFlag(void)
{
	key_flag.flag_KeyClicked = KEY_CLICKED;
}
void set_KeyOffFlag(void)
{
	key_flag.flag_KeyOFF = KEY_CLICKED;
}
void set_KeyOffLongFlag(void)
{
	key_flag.flag_KeyOFFLong = KEY_CLICKED;
}

void set_KeyCalLongLongFlag(void)
{
	key_flag.flag_KeyCalLongLong = KEY_CLICKED;
}


void key_scan(void)
{
	if(KEY_UP_STATUS == KEY_BURSTMODE) //up按钮按下了
	{
		if(!get_KeyUpCountFlag())         //如果没有计数的话开始计数
			set_KeyUpCountFlag();
	}
	else                                //松开或者没按下
	{
		if(get_KeyUpClickedFlag())        //获取按键是否被按过
		{
			set_KeyUpFlag();                //up按钮被按下了
			set_KeyClickedFlag();           //有按键被按了
		}
		
		clear_KeyUpClickedFlag();         //清按键被按过标志
		clear_KeyUpCountFlag();           //清计数flag
		clear_KeyUpCount();               //清计数
	}
	
	if(KEY_DOWN_STATUS == KEY_BURSTMODE)
	{
		if(!get_KeyDownCountFlag()) 
			set_KeyDownCountFlag();
	}
	else
	{
		if(get_KeyDownClickedFlag()) 
		{
			set_KeyDownFlag();
			set_KeyClickedFlag();
		}
		
		clear_KeyDownClickedFlag();
		clear_KeyDownCountFlag();
		clear_KeyDownCount();
	}
	
	if(KEY_OK_STATUS == KEY_BURSTMODE)
	{
		if(!get_KeyOkCountFlag()) 
			set_KeyOkCountFlag();
	}
	else
	{
		/*
		if(get_KeyCalClickedFlag()) 
		{
			if(get_KeyCalCount() < KEY_TIM_LONG) //如果按下去的时间小于长按规定的时间的话
			{
				set_KeyCalFlag();                  //设置按键
			}
			set_KeyClickedFlag();
		}
		clear_KeyCalLongFlag();//清除校准长按标志
		clear_KeyCalClickedFlag();
		clear_KeyCalCountFlag();
		clear_KeyCalCount();
		*/
		
		if(get_KeyOkClickedFlag()) 
		{
			if(interfacial_GetCurPage() == PAGE_0_START)
			{
				if(get_KeyOkCount() < KEY_TIM_LONG)
				{
					set_KeyOkFlag();
				}
			}
			else
			{
				set_KeyOkFlag();
			}
			set_KeyClickedFlag();
		}
		clear_KeyOkLongFlag();//因为这里是长按途中进入不是长按结束之后进入所以要在这里清
		clear_KeyOkClickedFlag();
		clear_KeyOkCountFlag();
		clear_KeyOkCount();
	}
	
	if(KEY_BLU_STATUS == KEY_BURSTMODE)
	{
		if(!get_KeyBluCountFlag()) 
			set_KeyBluCountFlag();
	}
	else
	{
		if(get_KeyBluClickedFlag()) 
		{
			set_KeyBluFlag();
			set_KeyClickedFlag();
		}
		
		clear_KeyBluClickedFlag();
		clear_KeyBluCountFlag();
		clear_KeyBluCount();
	}
	
	if(KEY_CAL_STATUS == KEY_BURSTMODE)
	{
		if(!get_KeyCalCountFlag()) 
			set_KeyCalCountFlag();
	}
	else
	{
		if(get_KeyCalClickedFlag()) 
		{
//			if(get_KeyCalCount() < KEY_TIM_LONG) //如果按下去的时间小于长按规定的时间的话
//			{
//				set_KeyCalFlag();                  //设置按键
//			}
			set_KeyCalFlag();
			set_KeyClickedFlag();
		}
		clear_KeyCalLongFlag();//清除校准长按标志
		clear_KeyCalLongLongFlag();
		clear_KeyCalClickedFlag();
		clear_KeyCalCountFlag();
		clear_KeyCalCount();
	}
	
	if(KEY_ESC_STATUS == KEY_BURSTMODE)
	{
		if(!get_KeyEscCountFlag()) 
			set_KeyEscCountFlag();
	}
	else
	{
		if(get_KeyEscClickedFlag()) 
		{
			set_KeyEscFlag();
			set_KeyClickedFlag();
		}
		
		clear_KeyEscClickedFlag();
		clear_KeyEscCountFlag();
		clear_KeyEscCount();
	}
	
	if(KEY_MENU_STATUS == KEY_BURSTMODE)
	{
		if(!get_KeyMenuCountFlag()) 
			set_KeyMenuCountFlag();
	}
	else
	{
		if(get_KeyMenuClickedFlag()) 
		{
			set_KeyMenuFlag();
			set_KeyClickedFlag();
		}
		
		clear_KeyMenuClickedFlag();
		clear_KeyMenuCountFlag();
		clear_KeyMenuCount();
	}
	
	if(KEY_OFF_STATUS 	== KEY_BURSTMODE)
	{
		if(!get_KeyOffCountFlag()) 
			set_KeyOffCountFlag();
		Key_Off_Flag=1;
	}
	else
	{
		if(get_KeyOffClickedFlag()) 
		{
			set_KeyOffFlag();
			set_KeyClickedFlag();
		}
		clear_KeyOffLongFlag();//清除校准长按标志
		clear_KeyOffClickedFlag();
		clear_KeyOffCountFlag();
		clear_KeyOffCount();
	}
	

}



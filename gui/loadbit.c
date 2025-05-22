/****************************************************************************************
* 文件名：LOADBIT.C
* 功能：显示单色图形及汉字显示。先将图形转换为对应的点阵数组，然后即可调用此文件的函数输出驱动。
* 作者：黄绍斌
* 日期：2004.02.26
* 备注：使用GUI_SetBackColor()函数设置显示颜色及背景色。
****************************************************************************************/
#include  "loadbit.h"

/****************************************************************************
* 名称：GUI_LoadLine()
* 功能：输出单色图形的一行数据。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的数据。
*           no      要显示此行的点个数
* 出口参数：返回值为1时表示操作成功，为0时表示操作失败。
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
uint8_t  GUI_LoadLine(uint8_t x, uint8_t y, uint8_t *dat, uint8_t no, uint8_t IsInverse)
{  
	uint8_t   bit_dat;
	uint8_t   i;
	TCOLOR  bakc;
	

	/* 参数过滤 */
	if(x>=GUI_LCM_XMAX) return 0;
	if(y>=GUI_LCM_YMAX) return 0;

	for(i=0; i<no; i++)
	{  /* 判断是否要读取点阵数据 */
		if( (i%8)==0 ) bit_dat = *dat++;

		/* 设置相应的点为color或为back_color */
		
		if( (bit_dat&DCB2HEX_TAB[i & 0x07])==0 ) GUI_CopyColor(&bakc, (IsInverse ? disp_color : back_color)); 
		else  GUI_CopyColor(&bakc,(IsInverse? back_color : disp_color));
		GUI_Point(x, y, bakc);       

		if( (++x)>=GUI_LCM_XMAX ) return 0;
	}

	return 1;
}
/****************************************************************************
* 名称：GUI_LoadPic()
* 功能：输出单色图形数据。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的数据
*           hno     要显示此行的点个数
*           lno     要显示此列的点个数
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_LoadPic(uint8_t x, uint8_t y, uint8_t *dat, uint8_t hno, uint8_t lno)
{  
	uint8_t  i;

	for(i=0; i<lno; i++)
	{  
		GUI_LoadLine(x, y, dat, hno, 0);				// 输出一行数据
		y++;										// 显示下一行
		dat += (hno>>3);							// 计算下一行的数据
		if( (hno&0x07)!=0 ) dat++;
	}
}
/****************************************************************************
* 名称：GUI_LoadPic1()
* 功能：输出单色图形数据，反相显示。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的数据。
*           hno     要显示此行的点个数
*           lno     要显示此列的点个数
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_LoadPic1(uint8_t x, uint8_t y, uint8_t *dat, uint8_t hno, uint8_t lno)
{ 
	uint8_t  i;

	GUI_ExchangeColor();									// 显示色与背景色交换
	for(i=0; i<lno; i++)
	{ 
		GUI_LoadLine(x, y, dat, hno, 0);						// 输出一行数据
		y++;												       // 显示下一行
		dat += (hno>>3);									// 计算下一行的数据
		if( (hno&0x07)!=0 ) dat++;
	}
	GUI_ExchangeColor();
   
}

/****************************************************************************
* 名称：GUI_PutHZ()
* 功能：显示汉字。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的汉字点阵数据。
*           hno     要显示此行的点个数
*           lno     要显示此列的点个数
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_PutCharArr(uint8_t x, uint8_t y, uint8_t *dat, uint8_t hno, uint8_t lno, uint8_t IsInverse)
{  
	uint8_t  i;

	for(i=0; i<lno; i++)
	{  
		GUI_LoadLine(x, y, dat, hno, IsInverse);						// 输出一行数据
		y++;												                        // 显示下一行
		dat += (hno>>3);									                  // 计算下一行的数据
		if((hno&0x07)!=0) dat++;
	}
}
/*小写字母*/
void GUI_PutChar(uint8_t x, uint8_t y, char c, uint8_t hno, uint8_t lno, uint8_t IsInverse)
{
// (0) .(1) :(2) /(3) %(4) 
	switch(c)
	{
		case ' ':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[0]), hno, lno, IsInverse);
			break;
		case '.':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[1]), hno, lno, IsInverse);
			break;
		case ':':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[2]), hno, lno, IsInverse);
			break;
		case '/':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[3]), hno, lno, IsInverse);
			break;
		case '%':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[4]), hno, lno, IsInverse);
			break;
		case '-':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[67]), hno, lno, IsInverse);
			break;
		case '?':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[69]), hno, lno, IsInverse);
			break;
		case '+':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[68]), hno, lno, IsInverse);
			break;
		case '!':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[70]), hno, lno, IsInverse);
			break;
		
		case '@'://μ
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[73]), hno, lno, IsInverse);
			break;
		
		case '(':
			GUI_PutCharArr(x, y, (uint8_t *)(ZUOKUOHAO), hno, lno, IsInverse);
			break;
		case ')':
			GUI_PutCharArr(x, y, (uint8_t *)(YOUKUOHAO), hno, lno, IsInverse);
			break;
		default:
			if(c >= 'a' && c <= 'z')//小写字母最常用
			{
				GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[(c) - 97 + INDEX_a]), hno, lno, IsInverse); //因为a ascii 为97 数组中的a下标为40 所以为 data-97+40 就变成-57
			}
			if(c >= '0' && c <= '9')//数字
			{
				GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[(c) - 48 + INDEX_0]), hno, lno, IsInverse); //因为0 ascii 为48 数组中的0下标为3 所以为 data-48+3 就变成-45
			}
			if(c >= 'A' && c <= 'Z')//大写字母
			{
				GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[(c) - 65 +INDEX_A]), hno, lno, IsInverse); //因为A ascii 为65 数组中的A下标为14 所以为 data-65+14 就变成-51
			}
			if(c > 'z')
			{
				GUI_PutCharArr(x, y, (uint8_t *)(FONT_ENG_MENU[(c) - 97 +INDEX_a]), hno, lno, IsInverse); 
			}
			break;
	}
}

/*大写字母*/
void GUI_PutLargeChar(uint8_t x, uint8_t y, char c, uint8_t hno, uint8_t lno, uint8_t IsInverse)
{
	switch(c)
	{
		case '.':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_LARGE[0]), hno, lno, IsInverse);
			break;
		case ' ':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_LARGE[11]), hno, lno, IsInverse);
			break;
		case '-':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_LARGE[12]), hno, lno, IsInverse);
			break;
		default:
			if(c >= '0' && c <= '9')//数字
			{
				GUI_PutCharArr(x, y, (uint8_t *)(FONT_LARGE[(c) - 47]), hno, lno, IsInverse); //因为0 ascii 为48 数组中的0下标为3 所以为 data-48+3 就变成-45
			}
			break;
	}
}
void GUI_PutMediumChar(uint8_t x, uint8_t y, char c, uint8_t hno, uint8_t lno, uint8_t IsInverse)
{
	switch(c)
	{
		case '.':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_MEDIUM[0]), hno, lno, IsInverse);
			break;
		case ' ':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_MEDIUM[11]), hno, lno, IsInverse);
			break;
		case '-':
			GUI_PutCharArr(x, y, (uint8_t *)(FONT_MEDIUM[12]), hno, lno, IsInverse);
			break;
		default:
			if(c >= '0' && c <= '9')//数字
			{
				GUI_PutCharArr(x, y, (uint8_t *)(FONT_MEDIUM[(c) - 47]), hno, lno, IsInverse); //因为0 ascii 为48 数组中的0下标为3 所以为 data-48+3 就变成-45
			}
			break;
	}
}

void GUI_PutEngStr(uint8_t x, uint8_t y, uint8_t *dat, uint8_t hno, uint8_t lno, uint8_t IsInverse)
{
	uint8_t *temp = dat;
	if(temp == NULL)
	{
		return;
	}
	while(*temp != '\0')
	{
		GUI_PutChar(x, y, *temp, hno, lno, IsInverse);
		temp++;
		x += 8; //x坐标横移
	}
}
void GUI_PutLargeStr(uint8_t x, uint8_t y, uint8_t *dat, uint8_t hno, uint8_t lno, uint8_t IsInverse)
{
	uint8_t *temp = dat;
	if(temp == NULL)
	{
		return;
	}
	while(*temp != '\0')
	{
		GUI_PutLargeChar(x, y, *temp, hno, lno, IsInverse);
		temp++;
		x += MENU_FONT_ENG_LARGE_LSIZE;
	}
}
void GUI_PutMediumStr(uint8_t x, uint8_t y, uint8_t *dat, uint8_t hno, uint8_t lno, uint8_t IsInverse)
{
	uint8_t *temp = dat;
	if(temp == NULL)
	{
		return;
	}
	while(*temp != '\0')
	{
		GUI_PutMediumChar(x, y, *temp, hno, lno, IsInverse);
		temp++;
		x += MENU_FONT_ENG_MEDIUM_LSIZE;
	}
}


void GUI_PutChnStr(uint8_t x, uint8_t y, uint8_t *dat, uint8_t str_len, uint8_t hno, uint8_t lno, uint8_t IsInverse)
{
	if(dat == NULL)
	{
		return;
	}
	if(dat == qingjiezhouqishezhi_cn)
	{
		for(uint8_t i = 0; i < str_len; i++)
		{
			GUI_PutCharArr(x+i*16, y, (uint8_t *)(FONT_CHN_MENU_xin[dat[i]]), hno, lno, IsInverse);
		}
	}
	else
	{
		for(uint8_t i = 0; i < str_len; i++)
		{
			GUI_PutCharArr(x+i*16, y, (uint8_t *)(FONT_CHN_MENU[dat[i]]), hno, lno, IsInverse);
		}
	}
	
}


void GUI_PutChnStr_xin(uint8_t x, uint8_t y, uint8_t *dat, uint8_t str_len, uint8_t hno, uint8_t lno, uint8_t IsInverse)
{
	if(dat == NULL)
	{
		return;
	}
	for(uint8_t i = 0; i < str_len; i++)
	{
		GUI_PutCharArr(x+i*16, y, (uint8_t *)(FONT_CHN_MENU_xin[dat[i]]), hno, lno, IsInverse);
	}
	
}
/************************************************************
  FileName: lcd_drive.h
  Author:        Version :          Date:
  Description:     LCD驱动程序，包括底层驱动，刷新显示子程序。
  Version:         V1.0
  Function List:   
1. -------
2. -------
  History: 
      <author>  <time>   <version >   <desc>
      Vary    2018/11/21    V1.0     build this moudle  
***********************************************************/

#ifndef __LCDDRIVE_H
#define __LCDDRIVE_H

#include "gui.h"




#define UC1698U_Bus_Port   LCD_D0_GPIO_Port

#define UC1698U_RST_H()    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)
#define UC1698U_RST_L()    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET)

#define UC1698U_WR_H()     HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET)
#define UC1698U_WR_L()     HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_RESET)

#define UC1698U_RD_H()     HAL_GPIO_WritePin(LCD_RD_GPIO_Port, LCD_RD_Pin, GPIO_PIN_SET)
#define UC1698U_RD_L()     HAL_GPIO_WritePin(LCD_RD_GPIO_Port, LCD_RD_Pin, GPIO_PIN_RESET)

#define UC1698U_CS_H()     HAL_GPIO_WritePin(LCD_CS0_GPIO_Port, LCD_CS0_Pin, GPIO_PIN_SET)
#define UC1698U_CS_L()     HAL_GPIO_WritePin(LCD_CS0_GPIO_Port, LCD_CS0_Pin, GPIO_PIN_RESET)

#define UC1698U_CD_H()     HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, GPIO_PIN_SET)
#define UC1698U_CD_L()     HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, GPIO_PIN_RESET)

#define GPIO_WriteHigh(GPIOx,a) GPIOx->BSRR=(((uint8_t)(uint8_t)~(a))<<24)|(((uint32_t)(uint8_t)(a))<<8)   //写io口高8位状态



/****************************************************************************
* 名称：GUI_Initialize()
* 功能：初始化GUI，包括初始化显示缓冲区，初始化LCM并清屏。
* 入口参数：无
* 出口参数：无
* 说明：用户根据LCM的实际情况编写此函数。
****************************************************************************/
void  GUI_Initialize(void);


/****************************************************************************
* 名称：GUI_FillSCR()
* 功能：全屏填充。直接使用数据填充显示缓冲区。
* 入口参数：dat		填充的数据
* 出口参数：无
* 说明：用户根据LCM的实际情况编写此函数。
****************************************************************************/
void  GUI_FillSCR(TCOLOR dat);


/****************************************************************************
* 名称：GUI_ClearSCR()
* 功能：清屏。
* 入口参数：无
* 出口参数：无
* 说明：用户根据LCM的实际情况编写此函数。
****************************************************************************/
//void  GUI_ClearSCR(void);


/****************************************************************************
* 名称：GUI_Point()
* 功能：在指定位置上画点。
* 入口参数： x		指定点所在列的位置
*           y		指定点所在行的位置
*           color	显示颜色(对于黑白色LCM，为0时灭，为1时显示)
* 出口参数：返回值为1时表示操作成功，为0时表示操作失败。(操作失败原因是指定地址超出有
*          效范围)
* 说明：用户根据LCM的实际情况编写此函数。
****************************************************************************/
uint8_t  GUI_Point(uint32_t x, uint32_t y, TCOLOR color);


/****************************************************************************
* 名称：GUI_ReadPoint()
* 功能：读取指定点的颜色。
* 入口参数：x			指定点所在列的位置
*          y		指定点所在行的位置
*          ret		保存颜色值的指针
* 出口参数：返回0时表示指定地址超出有效范围。
* 说明：对于单色，设置ret的d0位为1或0，4级灰度则为d0、d1有效，8位RGB则d0--d7有效，
*      RGB结构则R、G、B变量有效。
****************************************************************************/
int  GUI_ReadPoint(uint32_t x, uint32_t y, TCOLOR *ret);



/****************************************************************************
* 名称：GUI_HLine()
* 功能：画水平线。
* 入口参数： x0		水平线起点所在列的位置
*           y0		水平线起点所在行的位置
*           x1      水平线终点所在列的位置
*           color	显示颜色(对于黑白色LCM，为0时灭，为1时显示)
* 出口参数：无
* 说明：对于单色、4级灰度的液晶，可通过修改此函数作图提高速度，如单色LCM，可以一次更
*      新8个点，而不需要一个点一个点的写到LCM中。
****************************************************************************/
void  GUI_HLine(uint32_t x0, uint32_t y0, uint32_t x1, TCOLOR color);



/****************************************************************************
* 名称：GUI_RLine()
* 功能：画垂直线。
* 入口参数： x0		垂直线起点所在列的位置
*           y0		垂直线起点所在行的位置
*           y1      垂直线终点所在行的位置
*           color	显示颜色
* 出口参数：无
* 说明：对于单色、4级灰度的液晶，可通过修改此函数作图提高速度，如单色LCM，可以一次更
*      新8个点，而不需要一个点一个点的写到LCM中。
****************************************************************************/
void  GUI_RLine(uint32_t x0, uint32_t y0, uint32_t y1, TCOLOR color);




/****************************************************************************
* 名称：GUI_CmpColor()
* 功能：判断颜色值是否一致。
* 入口参数：color1		颜色值1
*		   color2		颜色值2
* 出口参数：返回1表示相同，返回0表示不相同。
* 说明：由于颜色类型TCOLOR可以是结构类型，所以需要用户编写比较函数。
****************************************************************************/
//extern int  GUI_CmpColor(TCOLOR color1, TCOLOR color2);
#define  GUI_CmpColor(color1, color2)	(color1==color2)

/****************************************************************************
* 名称：GUI_CopyColor()
* 功能：颜色值复制。
* 入口参数：color1		目标颜色变量
*		   color2		源颜色变量
* 出口参数：无
* 说明：由于颜色类型TCOLOR可以是结构类型，所以需要用户编写复制函数。
****************************************************************************/
//extern void  GUI_CopyColor(TCOLOR *color1, TCOLOR color2);
#define  GUI_CopyColor(color1, color2) 	*color1 = color2

void GUI_UpdateDisplay(void);//更新显示

void LCD_GPIO_Config(void);
void LCD_Initialize(void);
void Write8DotsUC1698U(uint8_t Data);
void WriteDataUC1698U(uint8_t Data);
void WriteCommandUC1698U(uint8_t CmdData);
void WriteDataUC1698U(uint8_t Data);

void Delay_ms(unsigned short count);
void dis_dot(unsigned char dat1,unsigned char dat2);
void dis_gra(void);

unsigned char left(unsigned char m);
void push_gra(unsigned char gra);
void TEST(void);

//void  GUI_ClearSCR(void);
void  GUI_ClearSCR(uint8_t dat);

void set_StartPoint(uint8_t row);//设置从屏幕起始位置开始刷新

void gui_ClearLines(uint8_t yIndex_start, uint8_t yIndex_end, uint8_t isBlack);
void gui_DrawStatusBarLine(void);

void gui_DrawMessageBox(void);
void gui_ClearMessageBox(void);
void gui_ClearWarining(void);
void gui_ClearNoSignal(uint8_t is_cn, uint8_t start_y);

void gui_DrawWarining(uint8_t* arr);
void gui_DrawLock(uint8_t* arr);
void gui_ClearLock(void);

void gui_SetRefreshON(void);
void gui_SetRefreshOFF(void);
uint8_t gui_GetRefreshStatus(void);

void gui_ShowLogo(uint8_t *dat);

void gui_ClearChuchangshezhi(void);
#endif




/****************************************************************************************
* 文件名：LOADBIT.H
* 功能：显示单色图形及汉字显示。(头文件)
* 作者：黄绍斌
* 日期：2004.02.26
* 备注：
****************************************************************************************/
#ifndef  LOADBIT_H
#define  LOADBIT_H


#include "lcd_drive.h"
#include "GUI_StockC.h"
#include "font_lib.h"

#define LOADBIT_REVERSE  1
#define LOADBIT_NORMAL   0

/****************************************************************************
* 名称：GUI_LoadPic()
* 功能：输出单色图形数据，反白显示。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的数据
*           hno     要显示此行的点个数
*           lno     要显示此列的点个数
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_LoadPic(uint8_t x, uint8_t y, uint8_t *dat, uint8_t hno, uint8_t lno);


/****************************************************************************
* 名称：GUI_LoadPic1()
* 功能：输出单色图形数据，反相显示。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的数据
*           hno     要显示此行的点个数
*           lno     要显示此列的点个数
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_LoadPic1(uint8_t x, uint8_t y, uint8_t *dat, uint8_t hno, uint8_t lno);


/****************************************************************************
* 名称：GUI_PutHZ()
* 功能：显示汉字。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的汉字点阵数据
*           hno     要显示此行的点个数
*           lno     要显示此列的点个数
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_PutCharArr(uint8_t x, uint8_t y, uint8_t *dat, uint8_t hno, uint8_t lno, uint8_t IsInverse);

void GUI_PutChar(uint8_t x, uint8_t y, char c, uint8_t hno, uint8_t lno, uint8_t IsInverse);

void GUI_PutEngStr(uint8_t x, uint8_t y, uint8_t *dat, uint8_t hno, uint8_t lno, uint8_t IsInverse);
void GUI_PutChnStr(uint8_t x, uint8_t y, uint8_t *dat, uint8_t str_len, uint8_t hno, uint8_t lno, uint8_t IsInverse);

void GUI_PutLargeStr(uint8_t x, uint8_t y, uint8_t *dat, uint8_t hno, uint8_t lno, uint8_t IsInverse);
void GUI_PutMediumStr(uint8_t x, uint8_t y, uint8_t *dat, uint8_t hno, uint8_t lno, uint8_t IsInverse);

void test(uint8_t x, uint8_t y, uint8_t *dat);



#endif

#include "lcd_drive.h"
#include "string.h"


//要换成脏矩形算法

/*160行 每行20个8位数字*/	
TCOLOR  gui_disp_buf[GUI_LCM_YMAX][GUI_LCM_XMAX/8];				// 声明GUI缓冲buf

#define SETTING_GUANFANG 0

uint8_t *ptr;
uint8_t Display_On = 0 ;
uint8_t pic_dot;

void Delay_ms(unsigned short count)
{
  HAL_Delay(count);
}

//============================================================================= 
//函 数 名:  ReverseBin()
//功 能：	按位反转二进制   uint8_t的高四位和低四位中间线对称或
//入口参数： Data:需要转换的数值
//出口参数： 无
//返 回 值： 转换完成的数值
//============================================================================= 
uint8_t reverse_bin(uint8_t Data)
{
	uint16_t ret = Data;
	uint8_t count = 8;
	for(; Data != 0; Data >>= 1)
	{
		ret <<= 1;
		ret |= Data & 1;
		count--;
	}
	ret <<= count;
	return (uint8_t)ret;
}

//写gpio口高8位数据
void set_data(GPIO_TypeDef * GPIOx,uint8_t cmd)
{
	GPIO_WriteHigh(GPIOx, reverse_bin(cmd));
}

//============================================================================= 
//函 数 名:  WriteCommandUC1698U() 
//功 能：	写命令到UC1698U
//入口参数： CmdData:命令码
//出口参数： 无
//返 回 值： 无
//============================================================================= 
void WriteCommandUC1698U(uint8_t CmdData)
{
	UC1698U_CS_L();
	
	UC1698U_CD_L();
	
	UC1698U_RD_H();
	
	UC1698U_WR_L(); 

	set_data(UC1698U_Bus_Port,CmdData);

	UC1698U_RD_L();		

	UC1698U_CS_H();
}
//============================================================================= 
//函 数 名:  WriteDataUC1698U() 
//功 能：	写数据到UC1698U
//入口参数： CmdData:写入的数据
//出口参数： 无
//返 回 值： 无
//============================================================================= 
void WriteDataUC1698U(uint8_t Data)
{
	UC1698U_CS_L();
	
	UC1698U_CD_H();
	
	UC1698U_RD_H();	
	
	UC1698U_WR_L();
	
	set_data(UC1698U_Bus_Port,Data);
	  
	UC1698U_RD_L();	
	
	UC1698U_CS_H();

} 
/* 以下为LCM的用户接口层，主要负责解释用户命令，并发送到LCM，为用户编程提供接口 */


/***********************************************************************
* 名称：LCD_Initialize()
* 功能：LCM初始化，将LCM初始化为纯图形模式，显示起始地址为0x0000，。
* 入口参数：ContrastIsLight 是否用淡的对比度
* 出口参数：无
* 说明：函数会设置LCM数据总线为输出方式
***********************************************************************/
void LCD_Initialize(uint8_t ContrastIsIntense)
{
#define BR		  0X01	   	//Set LCD Bias Ratio:1/10 bias	  
#define PC1     0X03      //power control set as internal power

#define TC      0x00      //set temperate compensation as 0%
//#define PM      192       //Set Vbias Potentiometer  192    178
#define LC_210  0X05      //set LCD Control
#define LC_43   0x03
#define LC_5    0X01
#define LC_76   0x01
#define LC_8    0X00
#define NIV     0X10      //n-line inversion
#define CSF     0X02      //23:enable FRC,PWM,LRM sequence      
#define WPC0    0x25      //Starting Column Address
#define WPP0    0         //Starting Row Address
#define WPC1	  0x5A	  	//Ending Column Address
#define WPP1	  159		    //Ending Row Address
#define AC    	0x01		  //Address  Control	  
#define CEN 	  159 		  //COM scanning end (last COM with full line cycle,0 based index)

    
//	UC1698U_RST_H();	
//	Delay_ms(15);
//	UC1698U_RST_L();		 //硬件复位
//	Delay_ms(15);
//	UC1698U_RST_H(); 
//	Delay_ms(800);

	WriteCommandUC1698U(0xe2);           //system reset
	Delay_ms(100);

#if SETTING_GUANFANG
	
	
	WriteCommandUC1698U(0xe9);          //set bias   1/10  
	WriteCommandUC1698U(0x2b);          //set internal power control
	WriteCommandUC1698U(0x25);          //set TC=-0.01%
	WriteCommandUC1698U(0x81);         //set VLCD value              
	WriteCommandUC1698U(0x98);//vlcd=(VC0+Cpm*pm)*(1+(T-25)*CT%)16.5v 
	WriteCommandUC1698U(0xc5);         //set LCD mapping control
	//writei(0xa3);          //set line rate
	WriteCommandUC1698U(0xa0);          //set line rate
	WriteCommandUC1698U(0xd1);         //set color pattern=RBG
	WriteCommandUC1698U(0xd5);         //set color mode=4k-color d5   4k  
	WriteCommandUC1698U(0x84);         //
	WriteCommandUC1698U(0xc8);        //n line inversion    //  试着打开  试试其他值
	WriteCommandUC1698U(0x10); 
	WriteCommandUC1698U(0x90);   //set  fixed line
	WriteCommandUC1698U(0x00);      
	WriteCommandUC1698U(0xda);
	WriteCommandUC1698U(0xf8);       
	WriteCommandUC1698U(0x89);         //set ram address control      
	WriteCommandUC1698U(0xf1);       
	WriteCommandUC1698U(159);         //set ram address control   
	WriteCommandUC1698U(0xad);         //set on/off display enable (grade on) 

#else
	
	WriteCommandUC1698U(0x24|(TC&0x03));	      //温度控制 -0.25%/摄氏度
	
	WriteCommandUC1698U(0x28|(PC1&0X03));	      //功率拉满


	WriteCommandUC1698U(0x81);	                //对比电压 81H + 0-255   这是设置对比值就是暗淡
	WriteCommandUC1698U(ContrastIsIntense ? 192 : 178);
	
	WriteCommandUC1698U(0x84|(LC_8   & 0X01) );
	
	WriteCommandUC1698U(0x88| (0x07  & AC) );	    //0X89,设置RAM的地址控制
	
	WriteCommandUC1698U(0xa0|(LC_43  & 0X03) ); 	//帧频设置 3最快
	
	//WriteCommandUC1698U(0xa7); 反色显示
	
	WriteCommandUC1698U(0xc0|(LC_210 & 0X07) );   //LC_210  0X05 MY MX LC0 my=1:行地址0-159对应驱动行160-1   
	
	WriteCommandUC1698U(0xc8 );                 //设置N线倒置
	WriteCommandUC1698U( NIV );	
	
	WriteCommandUC1698U(0xd0|(LC_5   & 0X01) ); 	//设置ram中rgb数据对应列驱动seg端的输出关系 rgbrgbrgbrgb
	
	WriteCommandUC1698U(0xd4|(LC_76  & 0X03) );	  //D4-D7，D5设置颜色模式
	
	WriteCommandUC1698U(0xd8|(CSF    & 0X07) );      //设置扫描模式	 
	
	WriteCommandUC1698U(0xE8|(BR     & 0X03));	      //E9	偏离率之类的参数	
	
	WriteCommandUC1698U(0xf4 );                 //双字节命令				 	
	WriteCommandUC1698U(WPC0 );                 //0X25	
        
	WriteCommandUC1698U(0xf6 );				 		
	WriteCommandUC1698U(WPC1 );                 //5A
        
	WriteCommandUC1698U(0xf5 );               	//双字节命令			 	 	
	WriteCommandUC1698U(WPP0 );                 //0
        
	WriteCommandUC1698U(0xf7 );				 	 	
	WriteCommandUC1698U(WPP1 );                 //159
        
	WriteCommandUC1698U(0xf8|((AC>>3)& 0x01) );  //单字节
	
	
	
	WriteCommandUC1698U(0xf1 );	                //双字节命令		
	WriteCommandUC1698U(CEN );	
	
	WriteCommandUC1698U(0xad );		             	//10101abc   a:绿色增强模式1关0开   b：灰度显示0关1开   c：显示开关0关1开     这里的意思是打开单色显示
	
	
#endif
}

//============================================================================= 
//函 数 名: Write8DotsUC1698U() 
//功 能: 	UC1698U横向写8个点的子函数
//入口参数:  Data:要显示的数据
//出口参数:  无
//返 回 值:  无
//============================================================================= 
void Write8DotsUC1698U(uint8_t Data)
{
	uint8_t data;
	uint8_t TempData[4] = {0};
	
	
//	if(Data & 0x80)
//	{
//		TempData[0] = 0xf0;
//	}
//	if(Data & 0x40)
//	{
//		TempData[0] |= 0x0f;
//	}
//	
//	if(Data & 0x20)
//	{
//		TempData[1] = 0xf0;
//	}
//	if(Data & 0x10)
//	{
//		TempData[1] |= 0x0f;
//	}
//	

//	if(Data & 0x08)
//	{
//		TempData[2] = 0xf0;
//	}
//	if(Data & 0x04)
//	{
//		TempData[2] |= 0x0f;
//	}
//	
//	
//	if(Data & 0x02)
//	{
//		TempData[3] = 0xf0;
//	}
//	if(Data & 0x01)
//	{
//		TempData[3] |= 0x0f;
//	}

	data = Data >> 6;
	TempData[0]=(data&2)*240+(data&1)*15;
	
	data = Data >> 4;
	TempData[1]=(data&2)*240+(data&1)*15;
	
	data = Data >> 2;
	TempData[2]=(data&2)*240+(data&1)*15;
	
	TempData[3]=(Data&2)*240+(Data&1)*15;
	
	WriteDataUC1698U(TempData[0]);
	WriteDataUC1698U(TempData[1]);
	WriteDataUC1698U(TempData[2]);
	WriteDataUC1698U(TempData[3]);

}


//============================================================================= 
//函 数 名:  GUI_UpdateDisplay() 
//功 能：	更新数据到UC1698U，每次更新16*8点，在定时器中断中每1ms调用一次
//入口参数： 无
//出口参数： 无
//返 回 值： 无
//============================================================================= 
static uint16_t  DispIndex=0;
static uint8_t  *ptr_DispBuf;

void GUI_UpdateDisplay(void)
{
	uint16_t j;
	
	if(Display_On == 0)   //检测显示开关标志
	{
		return;
	}

	if(DispIndex == 0)
	{
		ptr_DispBuf =(uint8_t*)gui_disp_buf;
		
		WriteCommandUC1698U(0x60);						//row address LSB     
		WriteCommandUC1698U(0x70);						//row address MSB	
		
		WriteCommandUC1698U(0x05);						//Column address LSB 
		WriteCommandUC1698U(0x12);						//Column address MSB
	}
	
	for(j=0; j<20; j++)			   //每次刷新20*8点，可以任意设置，但要保证定时器时间大于刷屏时间
	{         
		Write8DotsUC1698U(*ptr_DispBuf++);
		DispIndex++;		
		if(DispIndex % 20 == 0)	
		{
			WriteDataUC1698U(0x00);
		}
		if(DispIndex == 3200)
		{
			DispIndex = 0; 
		}
	}
}

void set_StartPoint(uint8_t row)
{
	uint16_t index = row*20;
	
	
	WriteCommandUC1698U(0x60|(row&0x0F));						//设置行起始地址
	WriteCommandUC1698U(0x70|(row>>4));						//设置从0开始
	
	WriteCommandUC1698U(0x05);						//设置列起始地址
	WriteCommandUC1698U(0x12);						//101001 设置从37开始   一个对应3个像素点
	
	ptr_DispBuf =(uint8_t*)gui_disp_buf;
	ptr_DispBuf += index;
	
	DispIndex = index;
}




/****************************************************************************
*                    与LCM相关的GUI接口函数
****************************************************************************/


/****************************************************************************
* 名称：GUI_FillSCR()
* 功能：全屏填充。直接使用数据填充显示缓冲区。
* 入口参数：dat		填充的数据(对于黑白色LCM，为0的点灭，为1的点显示)
* 出口参数：无
****************************************************************************/
void  GUI_FillSCR(uint8_t dat)
{  
	uint32_t  i, j;

	for(i=0; i<GUI_LCM_YMAX; i++)		// 历遍所有行
	{  
		for(j=0; j<GUI_LCM_XMAX/8; j++)	// 历遍所有行
		{  
			gui_disp_buf[i][j] = dat;		// 填充数据
		}
	} 
}


/****************************************************************************
* 名称：GUI_Initialize()
* 功能：初始化GUI，包括初始化显示缓冲区，初始化LCM并清屏。
* 入口参数：无
* 出口参数：无
****************************************************************************/
void  GUI_Initialize(uint8_t ContrastIsIntense)
{  
	gui_SetRefreshOFF();
	LCD_Initialize(ContrastIsIntense);					// 初始化LCM模块工作模式，纯图形模式
	GUI_ClearSCR(0x00);       // 初始化缓冲区为0x00，并输出屏幕(清屏)
	gui_SetRefreshON();	      //开显示刷新
	for(uint8_t i=0; i<200;i++)GUI_UpdateDisplay();//刷新下屏幕
	//gui_DrawStatusBarLine();
} 



/****************************************************************************
* 名称：GUI_ClearSCR()
* 功能：清屏。
* 入口参数：无
* 出口参数：无
* 说明：用户根据LCM的实际情况编写此函数。
****************************************************************************/
void  GUI_ClearSCR(uint8_t dat)
{  
	//GUI_FillSCR(0x00);
	memset(gui_disp_buf, dat, sizeof(gui_disp_buf));
}

uint8_t const  DCB_HEX_TAB[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

/****************************************************************************
* 名称：GUI_Point()
* 功能：在指定位置上画点。
* 入口参数： x		指定点所在列的位置
*           y		指定点所在行的位置
*           color	显示颜色(对于黑白色LCM，为0时灭，为1时显示)
* 出口参数：返回值为1时表示操作成功，为0时表示操作失败。(操作失败原因是指定地址超出有
*          效范围)
* 说明：用户根据LCM的实际情况编写此函数。对于单色，只有一个位有效，则要使用左移的方法
*      实现point_dat = (point_dat&MASK_TAB [i]) | (color<<n)，其它位数的一样
*      处理。
****************************************************************************/
uint8_t  GUI_Point(uint32_t x, uint32_t y, TCOLOR color)
{  
	/* 参数过滤 */
	if(x>=GUI_LCM_XMAX)
	{
		return 0;
	}
	if(y>=GUI_LCM_YMAX)
	{
		return 0;
	}

	/* 设置缓冲区相应的点 */
	if( (color&0x01) != 0 )
	{
		gui_disp_buf[y][x>>3] |= DCB_HEX_TAB[x&0x07];
	}
	else
	{
		gui_disp_buf[y][x>>3] &= (~DCB_HEX_TAB[x&0x07]); 
	}
	//若不采取定时刷新，则此处增加绘点功能
	return 1;
}

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
int  GUI_ReadPoint(uint32_t x, uint32_t y, TCOLOR *ret)
{  
	TCOLOR  bak;

	/* 参数过滤 */
	if(x>=GUI_LCM_XMAX) 
	{
		return 0;
	}
	if(y>=GUI_LCM_YMAX)
	{
		return 0;
	}

	/* 取得该点颜色(用户自行更改) */ 
	bak = gui_disp_buf[y][x>>3];
	
	if( (bak & DCB_HEX_TAB[x&0x07]) != 0)
	{
		*ret = 1;
	}
	else
	{
		*ret = 0;
	}

	return 1;
}


/****************************************************************************
* 名称：GUI_HLine()
* 功能：画水平线。
* 入口参数： x0		水平线起点所在列的位置
*           y0		水平线起点所在行的位置
*           x1      水平线终点所在列的位置
*           color	显示颜色(对于黑白色LCM，为0时灭，为1时显示)
* 出口参数：无
* 说明：操作失败原因是指定地址超出缓冲区范围。
****************************************************************************/
void  GUI_HLine(uint32_t x0, uint32_t y0, uint32_t x1, uint8_t color) 
{ 
	uint32_t  bak;

	if(x0>x1) 						// 对x0、x1大小进行排列，以便画图
	{  
		bak = x1;
		x1 = x0;
		x0 = bak;
	}
	if(x0==x1)
	{  
		GUI_Point(x0, y0, color);
		return;
	}

	do
	{  
		/* 设置相应的点为1 */
		if(0!=color)
		{
			gui_disp_buf[y0][x0>>3] |= DCB_HEX_TAB[x0&0x07];
		}
		else
		{
			gui_disp_buf[y0][x0>>3] &= (~DCB_HEX_TAB[x0&0x07]);
		}

		x0++;
	}while(x1>x0);

	/* 对最后一点显示操作 */
	if(0!=color) 
	{
		gui_disp_buf[y0][x0>>3] |= DCB_HEX_TAB[x0&0x07];
	}
	else
	{
		gui_disp_buf[y0][x0>>3] &= (~DCB_HEX_TAB[x0&0x07]);
	}
}


/****************************************************************************
* 名称：GUI_RLine()
* 功能：画垂直线。
* 入口参数： x0		垂直线起点所在列的位置
*           y0		垂直线起点所在行的位置
*           y1      垂直线终点所在行的位置
*           color	显示颜色(对于黑白色LCM，为0时灭，为1时显示)
* 出口参数：无
* 说明：操作失败原因是指定地址超出缓冲区范围。
****************************************************************************/
void  GUI_RLine(uint32_t x0, uint32_t y0, uint32_t y1, uint8_t color) 
{ 
	uint32_t  bak;

	if(y0>y1) 						// 对y0、y1大小进行排列，以便画图
	{  
		bak = y1;
		y1 = y0;
		y0 = bak;
	}
	if(y0==y1)
	{  
		GUI_Point(x0, y0, color);
		return;
	}
	do
	{  
		GUI_Point(x0, y0, color);		// 逐点显示，描出垂直线
		y0++;
	}while(y1>y0);
	GUI_Point(x0, y0, color);	
}

void gui_ClearLines(uint8_t yIndex_start, uint8_t yIndex_end, uint8_t isBlack)
{
	for(uint8_t i = yIndex_start; i<yIndex_end; i++)
	{
		for(uint8_t j=0; j<20; j++)
		{
			gui_disp_buf[i][j] =  (isBlack ? 0xff : 0x00);
		}
	}
}

void gui_DrawStatusBarLine(void)
{
	for(uint8_t i=0; i<20; i++)
	{
		gui_disp_buf[24][i]=0xFF;
	}
}

void gui_DrawMessageBox(void)
{
	uint8_t arr_temp = 0x00;
	for(uint8_t i = 56; i < 120; i++)
	{
		for(uint8_t j = 2; j < 18; j++)
		{
			arr_temp = 0x00;
			if(j == 2)
			{
				arr_temp = 0x80;
			}
			if(j == 17)
			{
				arr_temp = 0x01;
			}
			if(i == 56)
			{
				arr_temp = 0xFF;
			}
			if(i == 119)
			{
				arr_temp = 0xFF;
			}
			
			gui_disp_buf[i][j] = arr_temp;
		}
	}
}
void gui_ClearMessageBox(void)
{
	for(uint8_t i = 56; i < 120; i++)
	{
		for(uint8_t j = 2; j < 18; j++)
		{
			gui_disp_buf[i][j] = 0x00;
		}
	}
}

void gui_SetRefreshON(void)
{
	Display_On = 1;
}
void gui_SetRefreshOFF(void)
{
	Display_On = 0;
}
uint8_t gui_GetRefreshStatus(void)
{
	return Display_On;
}

//void gui_ShowLogo(uint8_t *dat)
//{
//	uint16_t index = 0;
//  uint8_t  *ptr;
//	uint16_t j;
//	
//	for(uint8_t i=0; i<160;i++)
//	{
//		if(index == 0)
//		{
//			ptr = dat;
//			
//			WriteCommandUC1698U(0x60);						//row address LSB     
//			WriteCommandUC1698U(0x70);						//row address MSB	
//			
//			WriteCommandUC1698U(0x05);						//Column address LSB 
//			WriteCommandUC1698U(0x12);						//Column address MSB
//		}
//		
//		for(j=0; j<20; j++)			   //每次刷新20*8点，可以任意设置，但要保证定时器时间大于刷屏时间
//		{         
//			Write8DotsUC1698U(*ptr++);
//			index++;		
//			if(index % 20 == 0)	
//			{
//				WriteDataUC1698U(0x00);
//			}
//		}
//	}
//	
//	gui_SetRefreshOFF();//关闭刷新 在定时器里面开启刷新 然后有个while等待标志开启
//}

#define LOCK_SIZE 16
#define LOCK_Y 28

const uint8_t lock_width = LOCK_SIZE/8;
const uint8_t lock_height = LOCK_SIZE;

void gui_DrawLock(uint8_t* arr)
{
	uint8_t *p = arr;
	for(uint8_t i = LOCK_Y; i < LOCK_Y+lock_height; i++)//y轴
	{
		for(uint8_t j = 1; j < 3; j++)//x轴
		{
			gui_disp_buf[i][j] = *p++;
		}
	}
}

void gui_ClearLock(void)
{
	for(uint8_t i = LOCK_Y; i < LOCK_Y+lock_height; i++)//y轴
	{
		for(uint8_t j = 1; j < 3; j++)//x轴
		{
			gui_disp_buf[i][j] = 0x00;
		}
	}
}


void gui_ClearWarining(void)
{
	for(uint8_t i = 132; i < 158; i++)//y轴
	{
		for(uint8_t j = 0; j < 4; j++)//x轴
		{
			gui_disp_buf[i][j] = 0x00;
		}
	}
}

void gui_DrawWarining(uint8_t* arr)
{
	uint8_t *p = arr;
	for(uint8_t i = 132; i < 158; i++)//y轴
	{
		for(uint8_t j = 0; j < 4; j++)//x轴
		{
			gui_disp_buf[i][j] = *p++;
		}
	}
}

void gui_DrawMes(uint8_t* arr)
{
	uint8_t *p = arr;
	for(uint8_t i = 26; i < 42; i++)//y轴
	{
		for(uint8_t j = 16; j < 19; j++)//x轴
		{
			gui_disp_buf[i][j] = *p++;
		}
	}
}

void gui_ClearMes()
{
	for(uint8_t i = 26; i < 42; i++)//y轴
	{
		for(uint8_t j = 16; j < 19; j++)//x轴
		{
			gui_disp_buf[i][j] = 0x00;
		}
	}
}

void gui_DrawUpDownPage(uint8_t* arr)
{
	uint8_t *p = arr;
	for(uint8_t i = 142; i < 158; i++)//y轴
	{
		for(uint8_t j = 9; j < 11; j++)//x轴
		{
			gui_disp_buf[i][j] = *p++;
		}
	}	
}

void gui_ClearUpDownPage(void)
{
	for(uint8_t i = 142; i < 158; i++)//y轴
	{
		for(uint8_t j = 9; j < 11; j++)//x轴
		{
			gui_disp_buf[i][j] = 0x00;
		}
	}	
}


void gui_ClearNoSignal(uint8_t is_cn, uint8_t start_y)
{
	uint8_t start_x = 0, end_x = 0;
	if(is_cn)
	{
		start_x =  6;
		end_x = 14;
	}
	else
	{
		start_x = 5;
		end_x = 15;
	}
	for(uint8_t i = start_y; i < start_y+16; i++)//y轴
	{
		for(uint8_t j = start_x; j < end_x; j++)//x轴
		{
			gui_disp_buf[i][j] = 0x00;
		}
	}
}

void gui_ClearChuchangshezhi(void)
{
	for(uint8_t y = 16; y < 32; y++)
	{
		for(uint8_t x = 1; x < 9; x++)
		{
			gui_disp_buf[y][x] = 0x00;
		}
	}
}

void gui_DrawLogo(uint8_t row, uint8_t height, uint8_t *dat)
{
	uint16_t index = 0;
  uint8_t  *ptr;
	uint16_t j;
	
	for(uint8_t i=0; i<height;i++)
	{
		if(index == 0)
		{
			ptr = dat;
			
			WriteCommandUC1698U(0x60|(row&0x0F));						//设置行起始地址
			WriteCommandUC1698U(0x70|(row>>4));						//设置从0开始
		}
		
		for(j=0; j<20; j++)			   //每次刷新20*8点，可以任意设置，但要保证定时器时间大于刷屏时间
		{         
			Write8DotsUC1698U(*ptr++);
			index++;		
			if(index % 20 == 0)	
			{
				WriteDataUC1698U(0x00);
			}
		}
	}
}









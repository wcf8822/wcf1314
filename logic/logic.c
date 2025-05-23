#include "logic.h"

#include "lcd_drive.h"
#include "rs485.h"
#include "bmp280.h"
#include "soft_iic.h"
#include "rtc.h"
#include "heart.h"

#include "GUI_StockC.h"
#include "interfacial.h"
#include "setting.h"
#include "key.h"
#include "loadbit.h"
#include "gps.h"
#include "ch340e.h"
#include "battery.h"
#include "spi_flash.h"
#include "data_processing.h"
#include "dissolved_oxygen.h"
#include "log.h"
#include "bee.h"  //用来开机响下蜂鸣器

#define START_BEEON 0//开始是否让蜂鸣器响一下

#define BLINKLED() HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin)//反转led让led开闪

double ad_data = 0.0, ad_temp = 0.0;

uint8_t press_buf[6];

uint8_t flag_Bmp280On = 0;

uint16_t count_savedata = 0;   //数据间隔保存的计数值 单位：秒

void clear_SAVEDATACount(void)
{
	count_savedata = 0;
}


void logic_BatteryAD(void);

void update_HWVersion(void)
{
	uint8_t hdv = 0;
	if(HAL_GPIO_ReadPin(GPIO_Ver_0_GPIO_Port, GPIO_Ver_0_Pin))
	{
		hdv += 1;
	}
	if(HAL_GPIO_ReadPin(GPIO_Ver_1_GPIO_Port, GPIO_Ver_1_Pin))
	{
		hdv += 2;
	}
	switch(hdv)
	{
		case 0:
			interfacial_SetHWVersion(VER_A);//设置硬件版本
			break;
		case 1:
			interfacial_SetHWVersion(VER_B);//设置硬件版本
			break;
		case 2:
			interfacial_SetHWVersion(VER_C);//设置硬件版本  
			break;
		case 3:
			interfacial_SetHWVersion(VER_D);//设置硬件版本
			break;
	}
	
}


void lcdware_init(void)
{
	uint8_t IsFirst;
		
	W25QXX_Read(&IsFirst, SETTING_FIRSTRUN_ADDR, 1);
	if(IsFirst != SETTING_FIRSTRUN_JUDGE)//如果是第一次运行的话
	{
		first_write();
	}
	else
	{
		FlashToSetting(); //从flash中读取设置选项
		log_ReadLogCount(); //读取记录条数
	}
	
	if(setting_GetIsKeyGps())
	{
			HAL_GPIO_WritePin(GPS_EN_PORT, GPS_EN_PIN, GPIO_PIN_SET);
	}
	else
	{
			HAL_GPIO_WritePin(GPS_EN_PORT, GPS_EN_PIN, GPIO_PIN_RESET);			
	}
	
	GUI_Initialize(setting_GetContrast());   //屏幕初始化
	
	GUI_SetColor(1, 0);	//设置字体颜色和背景颜色

	set_StartPoint(0);//设置成从屏幕最开始一行刷

}
/*第一次上电整内存*/
void software_init(void)
{
	interfacial_InitMsg();
	
	interfacial_SetPage(PAGE_0_START, PAGE_NOT_BACK);

			
	RTC_UpdateShutDownTime(setting_GetAutoShut());//开机刷新下自动关机时间
	
	update_HWVersion();
	
	
	switch(setting_GetLogo())
	{
		case HENGLAN:
			HAL_GPIO_WritePin(LCD_BLC_GPIO_Port, LCD_BLC_Pin, GPIO_PIN_SET);//打开背光
			gui_DrawLogo(62, 36, (uint8_t *)logo_arr_hl);
			gui_SetRefreshOFF();
			while(!gui_GetRefreshStatus());//防止键盘误触什么的还能操作
			HAL_GPIO_WritePin(LCD_BLC_GPIO_Port, LCD_BLC_Pin, GPIO_PIN_RESET);//关闭背光
			break;
		
		case LUHENG:
			HAL_GPIO_WritePin(LCD_BLC_GPIO_Port, LCD_BLC_Pin, GPIO_PIN_SET);//打开背光
			gui_DrawLogo(37, 86, (uint8_t *)logo_arr_lh);
			gui_SetRefreshOFF();
			while(!gui_GetRefreshStatus());//防止键盘误触什么的还能操作
			HAL_GPIO_WritePin(LCD_BLC_GPIO_Port, LCD_BLC_Pin, GPIO_PIN_RESET);//关闭背光
			break;
		
		case LIHE://力和logo
			HAL_GPIO_WritePin(LCD_BLC_GPIO_Port, LCD_BLC_Pin, GPIO_PIN_SET);//打开背光
			gui_DrawLogo(62, 45, (uint8_t *)logo_arr_LIHE);
			gui_SetRefreshOFF();
			while(!gui_GetRefreshStatus());//防止键盘误触什么的还能操作
			HAL_GPIO_WritePin(LCD_BLC_GPIO_Port, LCD_BLC_Pin, GPIO_PIN_RESET);//关闭背光
			break;		

		default:
			
			break;
	}
	
	
	/*先画下状态栏防止开机那个是没东西的*/
	ad_data = get_bat_avg(25);     //最初给个值不至于直接0了
	HYM8563_UpdateTime();
	logic_BatteryAD();
	set_StartPoint(0);
	StatusBar_Update();
	
	
}

void hardware_init(void)
{
	__HAL_TIM_CLEAR_IT(&htim2,TIM_IT_UPDATE);      //使能系统主时钟TIM2
	HAL_TIM_Base_Start_IT(&htim2);
	
	__HAL_SPI_ENABLE(&hspi1);                      //使能spi1（flash bmp280）
	
	//这个不能根据cube默认生成的顺序,dma应初始化在串口前不然接收数据只能接收到最后一位
	MX_DMA_Init();
	MX_USART3_UART_Init();  
	MX_USART2_UART_Init();
	MX_USART1_UART_Init();
	

	
	rs485_usart.init(&huart3);                     //rs485串口空闲中断初始化
	gps_usart.init(&huart2);
	ch340e_usart.init(&huart1);

#if START_BEEON	
	setting_SetKeyPadTone(1);//在从flash中读取设置前先设置能响一下的
	set_BeeOn();             //开机响下蜂鸣器
#endif
	
	bmp280.init(&hspi1);                           //bmp280初始化在spi1口上
	
	if(bmp280_readId() == 0x58)                    //测试读取bmp280的id来反馈是否和bmp280通讯正常 
	{
		flag_Bmp280On =1;                            //不正常就不采集不然采集的话会卡在while里
	}	
	HYM8563_init();                                //RTC初始化
	lcdware_init();
}

void logic_bmp280(void)
{
	if(flag_Bmp280On)
	{
		bmp280_UpdateValue();//获取气压值
		set_PressArr(bmp280_GetPress());
	}
}

void logic_BatteryAD(void)
{
	ad_temp = get_bat();
	if(ad_temp != -1.0)
	{
		ad_data = ad_temp;
	}
	battery_draw(ad_data);
}

void logic_BatteryAD_Big(void)
{
	ad_temp = get_bat();
	if(ad_temp != -1.0)
	{
		ad_data = ad_temp;
	}
	battery_draw_big(ad_data);
}


void logic_DeviceDestory(void)/////////////////////////////////////////////////////////////////////////////////////////////////这里可能要改成自动选择第二个设备
{
	if(rs485_GetDeviceCount())
	{
		rs485_DeviceReduce();                                        //连接设备-1
	}
	
	if(interfacial_GetNeedWarning())                             //清报警标志
	{
		interfacial_ClearNeedWarning();
	}

  if(rs485_usart.tx_buf[0] ==get_CurDo()->modbus_id)	
	{
		switch(interfacial_GetCurPage())
		{
			case PAGE_0_START:
				interfacial_ClearLabel();                                  //清所有的数据标签
				gui_ClearLines(22, 160, 0);                                //清下界面
				rs485_SetIsChangeSenesor();                                //这个标志置一方便设备重连上来能够直接生成界面
				break;
			case PAGE_1_RESETCAL:
			case PAGE_4_SENSORINFO:
			case PAGE_5_COD_Cleanse:		
				generate_MessageBox(MESSAGE_SUCCESSFUL, 0);
				break;
			case PAGE_5_DO_ONE_First:
			case PAGE_5_DO_TWO_SECOND:
			case PAGE_5_MLSS_zero_signal:
			case PAGE_5_NH3N_ONE:
			case PAGE_5_NH3N_TWO:
			case PAGE_5_NH3N_pH_ONE:
			case PAGE_5_NH3N_pH_TWO:
			case PAGE_5_NH3N_pH_THREE:
					
			case PAGE_5_COD_shenghui_Tur_ONE:
			case PAGE_5_COD_shenghui_Tur_TWO:
			case PAGE_5_COD_shenghui_ONE:
			case PAGE_5_COD_shenghui_TWO:
			case PAGE_5_COD_shenghui_THREE:
			case PAGE_5_COD_shenghui_Zero:	 
			case PAGE_5_COD_DC17_IN_Tur_Zero:
			case PAGE_5_COD_DC18_Tur_Zero:
			case PAGE_5_COD_DC18_Tur_Slope:
			case PAGE_5_COD_DC18_Zero:
			case PAGE_5_COD_DC17_IN_Zero:
			case PAGE_5_COD_DC18_Slope:
			case PAGE_5_COD_DC18_Coefficient:
				
			case PAGE_5_shenghui_Tur_ONE:
			case PAGE_5_shenghui_Tur_TWO:
			case PAGE_5_shenghui_Tur_THREE:				
				
			case PAGE_5_shenghui_EC_ONE:
      		case PAGE_5_DE26_EC_Zero:
			case PAGE_5_DR31_ORP_ONE:
			case PAGE_5_shenghui_BGA_ONE:
			case PAGE_5_shenghui_BGA_TWO:
				
				if(rs485_GetSentType() == DO_SendType_SetKB)//如果正在校准的话
				{
					interfacial_GetCurrentInterfacial()->label_head->next_label->content_chn = (uint8_t *)jiaozhunshibai_cn;//校准失败
					interfacial_GetCurrentInterfacial()->label_head->next_label->content_eng = (uint8_t *)shibai_en;
					interfacial_GetCurrentInterfacial()->label_head->next_label->ChnContent_size = sizeof(jiaozhunshibai_cn);
					
					generate_MessageBox(MESSAGE_SUCCESSFUL, 0);
					interfacial_GetCurrentInterfacial()->page_father = ((interfacial_GetTempFatherPage() == PAGE_0_START) ? PAGE_0_START : PAGE_2_SENSORMANAGE);
				}
				break;
				
			default:
				break;
		}
  }

	 if(rs485_usart.tx_buf[0] == get_COMADo()->modbus_id){
	    if(get_COMBDo() == NULL){
				DO_ClearCOMADO();
				DO_ClearCurDO();	

			  rs485_SetSensorType(TYPE_NONE); //设置当前连接设备类型为无		
			}else{
				rs485_SetIsChangeSenesor();
				interfacial_ClearLabel();                                  //清所有的数据标签
				comA_DO.DO_list = comB_DO.DO_list;
				cur_DO.DO_list = comB_DO.DO_list;
				comA_DO.current_sensor_type = comB_DO.current_sensor_type;
				cur_DO.current_sensor_type = comB_DO.current_sensor_type;		
        DO_ClearCOMBDO();				
			}
	 }else{
		    rs485_SetIsChangeSenesor();		
				interfacial_ClearLabel();                                  //清所有的数据标签
				cur_DO.DO_list = comA_DO.DO_list; 
				cur_DO.current_sensor_type=comA_DO.current_sensor_type;
        DO_ClearCOMBDO();			
	 }			

}

static uint8_t warning_show = 1;
static uint8_t Mes_show = 1;
void warning(void)
{
	if(interfacial_GetNeedWarning())
	{
		if(interfacial_GetCurPage() == PAGE_0_START && !interfacial_GetMessageBoxFlag())//如果是主界面的话并且没有显示弹窗的话就开始反转报警图标
		{
			
			if(warning_show)
			{
				gui_DrawWarining((uint8_t *)warning_logo);
			}
			else
			{
				gui_ClearWarining();
			}
			warning_show = !warning_show;
			set_StartPoint(0);
		}
	}
	if (interfacial_GetCurPage() == PAGE_0_START && get_CurDo()->modbus_id == COD_DC17_ModbusID && get_CurDo()->DC17_Mes_Para.value_f == 2)
	{
		if(MesRun == 1 )
		{
			if(Mes_show)
			{
				gui_ClearMes();
			}
			else
			{
				gui_DrawMes((uint8_t *)celiangfuhao);
			}
			Mes_show = !Mes_show;
			set_StartPoint(0);
		}
		else
		{
			gui_DrawMes((uint8_t *)celiangfuhao);
		}
	}	
}


void Save_Data(void)
{
	if(cur_DO.DO_list == comA_DO.DO_list)
	{	
		log_SaveData(rs485_GetSensorType());	
		if(get_COMBDo() != NULL)
		{
			 cur_DO.DO_list = comB_DO.DO_list; 
			 cur_DO.current_sensor_type=comB_DO.current_sensor_type;
			 log_SaveData(rs485_GetSensorType());
			 cur_DO.DO_list = comA_DO.DO_list; 
			 cur_DO.current_sensor_type=comA_DO.current_sensor_type;
		}						
	}
	else
	{
		 log_SaveData(rs485_GetSensorType());	
		 cur_DO.DO_list = comA_DO.DO_list; 
		 cur_DO.current_sensor_type=comA_DO.current_sensor_type;
		 log_SaveData(rs485_GetSensorType());
		 cur_DO.DO_list = comB_DO.DO_list; 
		 cur_DO.current_sensor_type=comB_DO.current_sensor_type;					
	}
									
	generate_MessageBox(MESSAGE_SAVELOG, 1);	
}
void main_loop(void) //main函数调用的循环函数
{
	if(get_RtcFlag())
	{
		clear_RtcFlag();
		BLINKLED();
		HYM8563_UpdateTime();
	}
	if(get_BmpFlag())//2s
	{
		clear_BmpFlag();
		logic_bmp280();//更新气压值
		
		if(interfacial_GetCurPage() == PAGE_0_START &&  setting_GetAutoIntervalTime() != 0 && get_CurDo() != NULL ) // 间隔保存数据 
		{
			if(++count_savedata >= (uint16_t)(setting_GetAutoIntervalTime() / 2))
			{
				clear_SAVEDATACount();
				Save_Data();			
			}
		}
		else
		{
      		clear_SAVEDATACount();
		}
	}
	
	if(get_GPSFlag())
	{
		clear_GPSFlag();
		get_GpsData(); //从串口buff中解析gps数据
	}
	
	if(get_BatFlag())
	{
		clear_BatFlag();
		logic_BatteryAD(); //获取ad值
	}
	
	key_scan();        //先扫描按键再刷屏幕就有更快的响应
	
	if(get_TestFlag())
	{
		clear_TestFlag();
		if(interfacial_GetCurPage() == PAGE_0_START)
		{
			if(!rs485_GetDeviceCount())
			{
				no_signal();
			}
			else
			{
				if(interfacial_GetCurrentInterfacial()->label_head->content_chn == wuxinghao_cn)
				{
					interfacial_ClearLabel();//清  无信号
					clear_NoSignal();
				}
			}
		}
	}
	
	if(get_InterfacialFlag() )
	{
		clear_InterfacialFlag();
		interfacial_refresh();//刷新界面 //先刷新好再更新数据 防止destory之后直接空白界面
	}
	
	btn_func(); //按键轮询触发事件
	
	if(interfacial_GetNeedDestroyMSG())//销毁弹窗
	{
		interfacial_ClearNeedDestroyMSG();
		if(interfacial_GetCurMsgType() != MESSAGE_SETTING )
		{
			destory_MessageBox();
			
			if(interfacial_GetCurPage() == PAGE_0_START && interfacial_GetNeedWarning() && warning_show)
			{

				gui_DrawWarining((uint8_t *)warning_logo);
			}
		}
	}
	
	if(get_WarningFlag())
	{
		clear_WarningFlag();
		warning();
	}
	
	if(get_LcdFlag()) //将显存中的内容刷到屏幕上去
	{
		clear_LcdFlag();
		GUI_UpdateDisplay();
	}
	ch340_DataHandle();
	rs485_DataHandle();//先处理再发送 得实现一个消息队列（如果要设置两个东西怎么办不能单纯的通过它是否只是在循环发送来判断然后发送,否的话这个消息就发送不出去）
	
	if(rs485_GetIsDisconnect())//如果设备断开连接的话
	{
		rs485_ClearIsDisconnect();
		
		logic_DeviceDestory();
	}

	if(cur_DO.current_sensor_type == TYPE_DO)
	{
	    DO_UpdatePressSal(rs485_GetDoList()); //当仪表的气压和盐度发生变化的时候刷新数值到探头上去	
	}
	
	if(rs485_GetNeedSendStatus()) //485发送数据
	{
		rs485_ClearNeedSendStatus();

		rs485_SendBuf();
	}
	
	RTC_AutoShut();
}
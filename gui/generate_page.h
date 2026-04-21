#ifndef __GENERATE_PAGE
#define __GENERATE_PAGE

#include "gui.h"

#include "label.h"
#include "option.h"
#include "nano_option.h"
#include "interfacial.h"
#include "log.h"

#define ROWSPACING_ONE   16
#define ROWSPACING_TWO   8
#define ROWSPACING_THREE 4
#define ROWSPACING_FOUR  4
#define ROWSPACING_FIVE  4
#define ROWSPACING_MORE  2

log_union* generate_GetLogUnion(void);
float get_RowSpacing(void);                                                          //获取当前行间距值
void set_RowSpacing(float value);                                                    //设置选项行间距
uint8_t* generate_GetLogTimeArr(void);

void generate_StartPage(PtrToInterfacial interfacial);                               //生成初始界面

void generate_MenuPage(PtrToInterfacial interfacial);                                //生成菜单界面

void generate_SensorManage(PtrToInterfacial interfacial);                            //生成设备管理界面
void generate_SystemPage(PtrToInterfacial interfacial);                              //生成系统设置界面
void generate_Histor(PtrToInterfacial interfacial);                                  //生成历史数据界面
void generate_Histor_Option(PtrToInterfacial interfacial);                   //生产历史数据选择界面
void generate_Histor_ShowOption(PtrToInterfacial interfacial);
void generate_Histor_DeleteOption(PtrToInterfacial interfacial);
void generate_SetTimePage(PtrToInterfacial interfacial);                             //生成时间设置界面
void generate_AutoShut(PtrToInterfacial interfacial);                                //生成自动关机界面
void generate_AutoSave(PtrToInterfacial interfacial);
void generate_SetPressure(PtrToInterfacial interfacial);                             //生成气压设置界面
void generate_SetSalinity(PtrToInterfacial interfacial);                             //生成盐度设置界面
void generate_GpsInfo(PtrToInterfacial interfacial);                                 //生成gps查询界面
void generate_Language(PtrToInterfacial interfacial);                                //生成语言设置界面

void generate_AlarmType(PtrToInterfacial interfacial);
void generate_MeterInfo(PtrToInterfacial interfacial);
void generate_reset(PtrToInterfacial interfacial);                                   //生成恢复出厂设置界面




void generate_DataView(PtrToInterfacial interfacial, uint16_t data_index);           //生成日志查询界面

void generate_AlarmTone(PtrToInterfacial interfacial);                               //生成报警音设置界面
void generate_KeypadTone(PtrToInterfacial interfacial);                              //生成按键音设置界面
void generate_Set_Mes_mode(PtrToInterfacial interfacial);                            //生成测量模式设置界面
void generate_Set_Mes_Time(PtrToInterfacial interfacial);                            //生成单次测量模式测量间隔
void generate_BeepSeting(PtrToInterfacial interfacial);                              //生成提示音设置界面
void generate_AlarmSetting(PtrToInterfacial interfacial);                            //生成开关报警界面

void generate_SensorSearch(PtrToInterfacial interfacial);                            //生成查找设备界面
void generate_Sensors(PtrToInterfacial interfacial);                                 //生成具体的传感器界面

void generate_SensorInfo(PtrToInterfacial interfacial);                              //生成传感器信息界面
void generate_ResetCal(PtrToInterfacial interfacial);                                //生成重置校准系数界面
void generate_ParameterSetting(PtrToInterfacial interfacial);                        //生成传感器参数设置界面

void generate_AlarmValueSetting(PtrToInterfacial interfacial);                       //生成报警阈值设置界面

void generate_DataStore(PtrToInterfacial interfacial);                               //生成数据保存界面
void generate_DataDelete(PtrToInterfacial interfacial);                              //生成数据删除界面
void generate_DataLog(PtrToInterfacial interfacial);                                 //生成显示具体数据的界面
void generate_DataLogGPS(PtrToInterfacial interfacial);                              //生成显示具体数据gps的界面

void generate_Cal(PtrToInterfacial interfacial, PAGE_NUM father_page);               //生成校准界面
void generate_OnePoint(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);       //生成一点校准界面
void generate_TwoPointFirst(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);  //生成两点校准 第一点界面
void generate_TwoPointSecond(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type); //生成两点校准 第二点界面

void update_LogTitle(log_union* log , uint16_t index);

void generate_Cal_temp(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);

void generate_AutoLock_type(PtrToInterfacial interfacial);                           //生成自动锁定选择传感器类型界面
void generate_AutoLock(PtrToInterfacial interfacial);                                //生成自动锁定设置界面
void generate_AutoLock_value(PtrToInterfacial interfacial);                          //生成自动锁定等级设置界面

void generate_SlideAverage_type(PtrToInterfacial interfacial);                       //生成滑动平均选择传感器类型界面
void generate_SlideAverage_Switch(PtrToInterfacial interfacial);                     //生成滑动平均界面
void generate_SlideAverage_Value(PtrToInterfacial interfacial, uint8_t temp);        //生成滑动平均次数设置界面
void generate_SlideAverageA_Sensor_Value(PtrToInterfacial interfacial, uint8_t temp);
void generate_DZ09_Temp_xiaoshu(PtrToInterfacial interfacial);

void generate_IntervaSave(PtrToInterfacial interfacial,uint16_t num, uint8_t is_chn);
void generate_LockSave(PtrToInterfacial interfacial);
void generate_MLSS_ZERO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_DX01_Temp_xiaoshu(PtrToInterfacial interfacial);


void generate_NH3N_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_NH3N_TWO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_NH3N_pH_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_NH3N_pH_TWO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_NH3N_pH_THREE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);

void generate_COD_shenghui_Zero(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_COD_shenghui_Tur_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_COD_shenghui_Tur_TWO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_COD_shenghui_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_COD_shenghui_TWO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_COD_shenghui_THREE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_COD_Cleanse(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_COD_DC18_Tur_Zero(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type,PAGE_NUM page_num);
void generate_COD_DC18_Tur_Slope(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_COD_DC18_Zero(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type,PAGE_NUM page_num);
void generate_COD_DC18_Slope(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_COD_DC18_Coefficient(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);

void generate_shenghui_Tur_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_shenghui_Tur_TWO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_shenghui_Tur_THREE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);

void generate_shenghui_EC_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_shenghui_BGA_TWO(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);

void generate_pH_Standard_liquid_management(PtrToInterfacial interfacial);
void generate_Chl_Measure_Switch(PtrToInterfacial interfacial);
void generate_shenghui_COD_Current_absorbance(PtrToInterfacial interfacial);

void generate_DE26_EC_Zero(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);

void generate_Unit(PtrToInterfacial interfacial, uint8_t Unit);

void generate_FACTORY(PtrToInterfacial interfacial);//工厂模式
void generate_DC18_Set(PtrToInterfacial interfacial);//DC18
void generate_INCAL(PtrToInterfacial interfacial);//DC18内部校准
void generate_Model_selection(PtrToInterfacial interfacial);//
void generate_DE26_EC_tds_value(PtrToInterfacial interfacial);
void generate_DE26_EC_temp_data_value(PtrToInterfacial interfacial);
void generate_Sal_set_uint(PtrToInterfacial interfacial);
void generate_shenghui_BGA_ONE(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);
void generate_SetMes_Unit(PtrToInterfacial interfacial);
void generate_SetCal_Num(PtrToInterfacial interfacial);
void generate_SetTemp_para_Mode(PtrToInterfacial interfacial,PAGE_NUM cur_PageNum);
void generate_Temp_Clear_Time(PtrToInterfacial interfacial, PAGE_NUM cur_PageNum);
void generate_Sensor_Cal_Info(PtrToInterfacial interfacial);
void generate_OIW_K_CAL(PtrToInterfacial interfacial, SENSOR_TYPE sensor_type);

#endif









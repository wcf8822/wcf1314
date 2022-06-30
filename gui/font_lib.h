#ifndef __FONT_LIB_H
#define __FONT_LIB_H

/*字库大小*/
#define CHN_FONT_NUM 169
#define ENG_FONT_NUM 73

/*字库定义*/
#define FONT_ENG_MENU  eng_8x16
#define FONT_CHN_MENU  chn_16x16
#define FONT_LARGE     num_16x32
#define FONT_MEDIUM    num_10x20

/*中文标签横向像素点个数*/
#define MENU_FONT_CHN_LSIZE 16
#define MENU_FONT_CHN_RSIZE 16

/*英文标签纵向像素点个数*/
#define MENU_FONT_ENG_LSIZE 8
#define MENU_FONT_ENG_RSIZE 16

/*大型英文标签纵向像素点个数*/
#define MENU_FONT_ENG_LARGE_LSIZE 16
#define MENU_FONT_ENG_LARGE_RSIZE 32

/*中型英文标签纵向像素点个数*/
#define MENU_FONT_ENG_MEDIUM_LSIZE 11
#define MENU_FONT_ENG_MEDIUM_RSIZE 20


extern const unsigned char eng_8x16[ENG_FONT_NUM][16];
extern const unsigned char chn_16x16[CHN_FONT_NUM][32];
extern const unsigned char num_16x32[13][64];
extern const unsigned char num_10x20[13][40];




/*英文字库中起始字母对应的下标*/
#define INDEX_a 41
#define INDEX_A 15
#define INDEX_0 5

/*中文字库中特殊字符对应的下标*/
#define CHN_SPACE_INDEX  111
#define CHN_5_INDEX      112
#define CHN_EN_INDEX     115
#define CHN_CH_INDEX     119

/*英文符号对应的字库位置方便直接打印*/
#define MAOHAO      FONT_ENG_MENU[2]
#define XIEGANG     FONT_ENG_MENU[3]
#define BAIFENGHAO  FONT_ENG_MENU[4]
#define XIAOSHUDIAN FONT_ENG_MENU[1]
#define ZUOKUOHAO   FONT_ENG_MENU[71]
#define YOUKUOHAO   FONT_ENG_MENU[72]
#define SHESHIDU    chn_16x16[165]

extern const unsigned char logo_arr[200][16];

extern const unsigned char icon_lock[];


extern const unsigned char chuanganqiguanli_cn[5];
extern const unsigned char chuanganqiguanli_en[];

extern const unsigned char xitongshezhi_cn[4];
extern const unsigned char xitongshezhi_en[];

extern const unsigned char lishishuju_cn[4];
extern const unsigned char lishishuju_en[];

extern const unsigned char shijianshezhi_cn[4];
extern const unsigned char shijianshezhi_en[];

extern const unsigned char yuyanshezhi_cn[4];
extern const unsigned char yuyanshezhi_en[];

extern const unsigned char zidongguanji_cn[4];
extern const unsigned char zidongguanji_en[];

extern const unsigned char baojingshezhi_cn[4];
extern const unsigned char baojingshezhi_en[];

extern const unsigned char qiyashezhi_cn[4];
extern const unsigned char qiyashezhi_en[];

extern const unsigned char yandushezhi_cn[4];
extern const unsigned char yandushezhi_en[];

extern const unsigned char zuobiaochaxun_cn[4];
extern const unsigned char zuobiaochaxun_en[];

extern const unsigned char yibiaoxinxi_cn[4];
extern const unsigned char yibiaoxinxi_en[];

extern const unsigned char huifuchuchangshezhi_cn[6];
extern const unsigned char huifuchuchangshezhi_en[];

extern const unsigned char shujubaocun_cn[4];
extern const unsigned char shujubaocun_en[];

extern const unsigned char shujuxianshi_cn[4];
extern const unsigned char shujuxianshi_en[];

extern const unsigned char shujushanchu_cn[4];
extern const unsigned char shujushanchu_en[];

extern const unsigned char rizhi_cn[2];
extern const unsigned char rizhi_en[];

extern const unsigned char chuanganqixiaozhun_cn[5];
extern const unsigned char chuanganqixiaozhun_en[];

extern const unsigned char canshushezhi_cn[4];
extern const unsigned char chuanganqicanshushezhi_en[];

extern const unsigned char chuanganqixinxi_cn[5];
extern const unsigned char chuanganqixinxi_en[];

extern const unsigned char dimenxianyuzhi_cn[5];
extern const unsigned char dimenxianyuzhi_en[];

extern const unsigned char gaomenxianyuzhi_cn[5];
extern const unsigned char gaomenxianyuzhi_en[];

extern const unsigned char chuchangxulie_en[];

extern const unsigned char maomoxiaozhun_cn[4];
extern const unsigned char maomoxiaozhun_en[];

extern const unsigned char querenxiaozhun_cn[4];
extern const unsigned char querenxiaozhun_en[];

extern const unsigned char caidan_cn[2];
extern const unsigned char caidan_en[];

extern const unsigned char rongjieyang_cn[3];
extern const unsigned char rongjieyang_en[];
extern const unsigned char rongjieyang_en_quan[];

extern const unsigned char diandaolv_cn[3];
extern const unsigned char diandaolv_en[];

extern const unsigned char wendu_cn[2];
extern const unsigned char wendu_en[];

extern const unsigned char zhuodu_cn[2];
extern const unsigned char zhuodu_en[];

extern const unsigned char riqi_cn[3];
extern const unsigned char riqi_en[];

extern const unsigned char shijian_cn[3];
extern const unsigned char shijian_en[];

extern const unsigned char danwei_cn[4];
extern const unsigned char danwei_en[];

extern const unsigned char yingjianbanben_cn[5];
extern const unsigned char yingjianbanben_en[];

extern const unsigned char ruanjianbanben_cn[5];
extern const unsigned char ruanjianbanben_en[];

extern const unsigned char rizhiliebiao_cn[4];
extern const unsigned char rizhiliebiao_en[];

extern const unsigned char lingdian_cn[2];
extern const unsigned char lingdian_en[];

extern const unsigned char dandian_cn[2];
extern const unsigned char dandian_en[];

extern const unsigned char liangdian_cn[2];
extern const unsigned char liangdian_en[];

extern const unsigned char sandian_cn[2];
extern const unsigned char sandian_en[];

extern const unsigned char dangqianzhi_cn[4];
extern const unsigned char dangqianzhi_en[];

extern const unsigned char xiaozhunzhi_cn[4];
extern const unsigned char xiaozhunzhi_en[];

extern const unsigned char kaiqi_cn[2];
extern const unsigned char kaiqi_en[];

extern const unsigned char guanbi_cn[2];
extern const unsigned char guanbi_en[];

extern const unsigned char baocun_cn[2];
extern const unsigned char baocun_en[];

extern const unsigned char zidongsousuoshebei_cn[6];
extern const unsigned char zidongsousuoshebei_en[];

extern const unsigned char dizhi_cn[2];
extern const unsigned char dizhi_en[];

extern const unsigned char huifuxiaozhunxishu_cn[6];
extern const unsigned char huifuxiaozhunxishu_en[];


extern const unsigned char rongjieyangjiaozhun_cn[5];
extern const unsigned char rongjieyangjiaozhun_en[];
extern const unsigned char diandaolvjiaozhun_cn[5];
extern const unsigned char diandaolvjiaozhun_en[];
extern const unsigned char zhuodujiaozhun_cn[4];
extern const unsigned char zhuodujiaozhun_en[];
extern const unsigned char wendujiaozhun_cn[4];
extern const unsigned char wendujiaozhun_en[];
extern const unsigned char pHjiaozhun_cn[3];
extern const unsigned char pHjiaozhun_en[];

extern const unsigned char jinrujiaozhunmoshi_cn[6];
extern const unsigned char jinrujiaozhunmoshi_en[];

extern const unsigned char biaozhunzhi_cn[4];
extern const unsigned char biaozhunzhi_en[];


extern const unsigned char charubiaozhunrongye_cn[7];
extern const unsigned char charubiaozhunrongye_en[];

extern const unsigned char dengdaizhong_cn[5];
extern const unsigned char dengdaizhong_en[];

extern const unsigned char jiaozhunchenggong_cn[4];
//extern const unsigned char jiaozhunchenggong_en[];

extern const unsigned char chenggong_en[];
extern const unsigned char shibai_en[];

extern const unsigned char jiaozhunshibai_cn[4];


extern const unsigned char diyidian_cn[4];
extern const unsigned char diyidian_en[];

extern const unsigned char dierdian_cn[4];
extern const unsigned char dierdian_en[];

extern const unsigned char xiayidian_cn[3];
extern const unsigned char xiayidian_en[];

extern const unsigned char msg_successful_cn[4];
extern const unsigned char msg_fail_cn[4];
extern const unsigned char msg_SaveSuccessful_cn[6];
extern const unsigned char msg_SaveFail_cn[4];

extern const unsigned char zhengzaisousuo_cn[6];
extern const unsigned char zhengzaisousuo_en[];


extern const unsigned char datasaved[];
extern const unsigned char successfully[];

extern const unsigned char zuidakeyong_cn[5];
extern const unsigned char zuidakeyong_en[];

extern const unsigned char maxsize[6];

extern const unsigned char dangqianyiyong_cn[5];
extern const unsigned char dangqianyiyong_en[];

extern const unsigned char dangqianxuanze_cn[5];
extern const unsigned char dangqianxuanze_en[];

extern const unsigned char nodata_cn[3];
extern const unsigned char nodata_en[];

extern const unsigned char leixing_cn[3];
extern const unsigned char leixing_en[];
extern const unsigned char DT_str[];
extern const unsigned char SN_str[];

extern const unsigned char qiya_cn[2];
extern const unsigned char qiya_en[];

extern const unsigned char yandu_cn[2];
extern const unsigned char yandu_en[];

extern const unsigned char DOPercent_str[];
extern const unsigned char DOmgL_str[];


extern const unsigned char jingdu_cn[3];
extern const unsigned char jingdu_en[];
	
extern const unsigned char weidu_cn[3];
extern const unsigned char weidu_en[];

extern const unsigned char wuxinghao_cn[4];
extern const unsigned char wuxinghao_en[];

extern const unsigned char chuchangshezhi_cn[4];
extern const unsigned char chuchangshezhi_en[];

extern const unsigned char tishiyinshezhi_cn[5];
extern const unsigned char tishiyinshezhi_en[];

extern const unsigned char baojingyin_cn[3];
extern const unsigned char baojingyin_en[];

extern const unsigned char anjianyin_cn[3];
extern const unsigned char anjianyin_en[];

extern const unsigned char dushusuoding_cn[4];
extern const unsigned char dushusuoding_en[];

extern const unsigned char shoudong_cn[2];
extern const unsigned char shoudong_en[];

extern const unsigned char zidong_cn[2];
extern const unsigned char zidong_en[];










extern const unsigned char *DO_uint[3];
extern const unsigned char *Sal_uint[3];
extern const unsigned char *HW_version[4];

extern const unsigned char kPa[4];

extern const unsigned char msg_save_cn[7];
extern const unsigned char msg_save_en[];
extern const unsigned char msg_DelData_cn[7];
extern const unsigned char msg_DelData_en[];
extern const unsigned char msg_ConfirmCal_cn[7];
extern const unsigned char msg_ConfirmCal_en[];
extern const unsigned char msg_reset_cn[7];
extern const unsigned char msg_reset_en[];



extern const unsigned char shi_cn;
extern const unsigned char shi_en[];

extern const unsigned char fou_cn;
extern const unsigned char fou_en[];


extern const unsigned char minute5_cn[3];
extern const unsigned char minute10_cn[3];
extern const unsigned char minute20_cn[3];
	
extern const unsigned char minute5_en[];
extern const unsigned char minute10_en[];
extern const unsigned char minute20_en[];

//extern const unsigned char zhongwen[8];
//extern const unsigned char yingwen[6];
//中文
extern const unsigned char zhongwen_cn[8];
extern const unsigned char zhongwen_en[7];
//英文
extern const unsigned char yingwen_en[7];
extern const unsigned char yingwen_cn[8];
	


extern const unsigned char sensor_type_str[13][6];

extern const unsigned char battery_image[6][33];

extern const unsigned char warning_logo[104];
extern const unsigned char warning_logo_clear[104];


extern const unsigned char DO_str[];
extern const unsigned char pH_str[];
extern const unsigned char Tur_str[];
extern const unsigned char FCL_str[];
extern const unsigned char EC_str[];
extern const unsigned char ORP_str[];
extern const unsigned char NH4_str[];
extern const unsigned char F_str[];
extern const unsigned char CL_str[];
extern const unsigned char Chl_str[];
extern const unsigned char Vga_str[];
extern const unsigned char CODuv_str[];


#endif




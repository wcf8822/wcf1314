#ifndef __LABEL_H
#define __LABEL_H

#include "gui.h"

#define IS_PUNCTUATION 1
#define NOT_PUNCTUATION 0

#define HAVE_PARENTHESIS 1
#define DONT_HAVE_PARENTHESIS 0

typedef enum
{
	LABEL_NORMAL = 0,
	LABEL_MEDIUM,
	LABEL_LARGE
}LABEL_SIZE;

typedef enum{
	LABEL_PUNCTUATION, //标点符号
	LABEL_NUMBERORENG, //数字或者英文
	LABEL_STRING,      //字符串包括中英文
	LABEL_UINT,
	LABEL_xinziku,	//新字库
}LABEL_TYPE;

typedef enum{
	UINT_NONE,
	UINT_PERCENT,
	UINT_CELSIUS,
	UINT_FAHRENHEIT,
	UINT_MGL,
	UINT_USCM,
	UINT_NTU,
	UINT_MV,
	UINT_KPA,
	UINT_UGL,
	UINT_PPT,
	UINT_Cont,
	UINT_Sing,
	UINT_Cells_mL,//蓝绿藻
	UINT_Kcells_mL,//蓝绿藻
	UINT_mSCM,
	UINT_PPM,
	UINT_GL,
	UINT_PPB,
}LABEL_UINT_TYPE;

//标签节点
typedef struct struct_label{
	uint8_t have_parenthesis:1;        //是不是那种在后面有括号冒号的
	
	LABEL_TYPE label_type;             //标签的类型
	
	LABEL_UINT_TYPE uint_type;         //标签类型
	
	uint8_t x;                         //起始x
	uint8_t y;                         //起始y
	uint8_t ChnContent_size;           //中文内容的长度
	
	LABEL_SIZE label_size;             //数字显示大小类型
	
	uint8_t* content_chn;              //所要显示的中文内容
	uint8_t* content_eng;              //所要显示的英文内容
	
	struct struct_label* next_label;   //下一个节点的指针
}node_label;

typedef node_label* PtrToLabelNode;
typedef PtrToLabelNode list_label;



void LabelList_Destory(list_label *lbllist);

void LabelList_Add(uint8_t x, uint8_t y, uint8_t* ContentChn, uint8_t ChnLen, uint8_t* ContentEng, LABEL_SIZE lbl_size, LABEL_TYPE lbl_type, LABEL_UINT_TYPE uint_type, uint8_t have_parenthesis, list_label* lbllist);

void LabelList_Print(list_label lbllist , uint8_t IsChn);

uint8_t LabelList_count(list_label lbllist);










#endif


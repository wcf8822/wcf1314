
#ifndef __NANO_OPTION_H
#define __NANO_OPTION_H

#include "gui.h"






//双向链表


//x y 中文内容 中文字节长度 英文内容（如果是数字的话直接分配）  是不是数字（判断需不需要分配） 添加到哪个链表上
void NanoOptionList_Add(uint8_t x, uint8_t y, uint8_t* ContentChn, uint8_t ChnLen, uint8_t* ContentEng, 
												NANOOPTION_TYPE NanoOption_type, uint8_t value, uint8_t min_value, uint8_t max_value, uint8_t IsSingle, list_NanoOption* nanooptionlist);
//打印链表 是否打印中文
void NanoOptionList_Print  (list_NanoOption nanooptionlist, uint8_t IsChn);

void NanoOptionList_Destory(PtrToNanoOptionNode *nanooptionlist);

uint32_t NanoOptionList_GetValue(PtrToNanoOptionNode nanooptionlist, uint8_t coefficient);
uint32_t NanoOptionList_Get_np_Value(PtrToNanoOptionNode nanooptionlist, uint8_t coefficient);

extern int8_t orp_np;


#endif



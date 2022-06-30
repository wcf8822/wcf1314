#ifndef __OPTION_H
#define __OPTION_H

#include "gui.h"

//大小选项标志
#define OPTION_LARGE 1 //大选项
#define OPTION_SMALL 0 //小选项

#define CAN_BE_SELECTED    1
#define CANNOT_BE_SELECTED 0

#define SELECTED 1
#define DESELECTED 0

#define LANGUAGE_OPTION     1
#define NOT_LANGUAGE_OPTION 0

#define MAX_WINDOW 7

#define OPTION_STARTY 32


//双向链表


void OptionList_Add(uint8_t index, uint8_t* ContentChn, uint8_t ChnLen, uint8_t* ContentEng, PAGE_NUM NextPage, uint8_t IsLarge, uint8_t IsCanBeSelected, uint8_t IsLanguage, uint8_t IsEngOnly, list_NanoOption NanoOptionList, list_option* OptionList);
void OptionList_Print(list_option OptionList , uint8_t IsChn, float RowSpacing);
void OptionList_Destory(PtrToOptionNode *OptionList);

void set_OptionTop(PtrToOptionNode node);
void set_OptionTail(PtrToOptionNode node);

uint8_t get_OptionTopIndex(void);

void window_change(PtrToOptionNode option_selected, list_option head);
void refresh_window(PtrToOptionNode option_selected, list_option OptionList);

#endif



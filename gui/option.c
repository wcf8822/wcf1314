#include "option.h"
#include "loadbit.h"
#include "GUI_BASIC.h"

#include "nano_option.h"



/*
	uint8_t IsSelected : 1;                //是否被选中了
	uint8_t IsLarge : 1;                   //是否是那种大的选项要刷一条横的
	
	uint8_t ChnContent_size;               //中文标签的长度
	
	PAGE_NUM next_page;                    //如果被选中按确定后跳转的界面
	uint8_t* content_chn;                  //显示的中文内容
	uint8_t* content_eng;                  //显示的英文内容
	struct struct_option* next_option;     //为了方便下一个选项的指针
	struct struct_option* prev_option;     //为了方便上一个选项的指针
	struct struct_NanoOption* son_option;  //子标签的指针
*/

STATIC PtrToOptionNode pTop;
STATIC PtrToOptionNode pTail;


PtrToOptionNode Option_NodeGenerate(uint8_t index, uint8_t* ContentChn, uint8_t ChnLen, uint8_t* ContentEng, PAGE_NUM NextPage, uint8_t IsLarge, uint8_t IsCanBeSelected, uint8_t IsLanguage, uint8_t IsEngOnly, list_NanoOption NanoOptionList)
{
	PtrToOptionNode p = NULL;
	
	p = (PtrToOptionNode)malloc(sizeof(node_option)); //分配空间
	
	if(p == NULL)    //安全性判断
	{
		return NULL;
	}
	
	p->next_option = p;//直接创建一个循环节点
	p->prev_option = p;
	
	p->option_index = index;
	
	
	p->next_page = NextPage;                 //下一页
	p->IsSelected = 0;                        //是否被选中
	p->IsLarge = IsLarge;                     //是否是那种大标签
	p->IsCanBeSelected = IsCanBeSelected;
	p->IsLanguage = IsLanguage;
	p->IsEngOnly = IsEngOnly;
	
	p->content_chn = (uint8_t *)ContentChn;  //中文内容指针
	p->ChnContent_size = ChnLen;
	
	p->content_eng = (uint8_t *)ContentEng;  //英文内容指针
	
	p->son_option = NanoOptionList;

	return p;
}


//添加节点
void OptionList_Add(uint8_t index, uint8_t* ContentChn, uint8_t ChnLen, uint8_t* ContentEng, PAGE_NUM NextPage, uint8_t IsLarge, uint8_t IsCanBeSelected, uint8_t IsLanguage, uint8_t IsEngOnly, list_NanoOption NanoOptionList, list_option* OptionList) //头插
{
	PtrToOptionNode p = Option_NodeGenerate(index, ContentChn, ChnLen, ContentEng, NextPage, IsLarge, IsCanBeSelected, IsLanguage, IsEngOnly, NanoOptionList);//生成一个新的节点
	
	if((*OptionList) == NULL)//如果链表是空的话就直接把头指针指向第一个节点
	{
		*OptionList = p;
	}
	else//不是的话就尾插一个节点
	{
		p->prev_option = (*OptionList)->prev_option;
		p->next_option = (*OptionList);
		(*OptionList)->prev_option->next_option = p;
		(*OptionList)->prev_option = p;
	}
}


void OptionList_Destory(PtrToOptionNode *optionlist)
{
	PtrToOptionNode p = *optionlist, temp = NULL;
	pTail = pTop = NULL;
	
	if(*optionlist == NULL)
	{
		return;
	}
	do
	{
		temp = p->next_option;
		if(p->son_option != NULL)//如果选项有小选项的话删除小选项列表
		{
			NanoOptionList_Destory(&(p->son_option));
		}
		free(p);
		p = temp;
	}while(p != (*optionlist));
	
	
	
	*optionlist = NULL;	
}


//窗口设置中如果选项个数超过窗口值并且有空的项会溢出0 234这种会溢出因为这里是按个数算的不是按照选项下标算的
void OptionList_Print(list_option OptionList , uint8_t IsChn, float RowSpacing)    //打印选项列表
{
	PtrToOptionNode p = NULL;  //遍历链表的临时指针
	
	uint8_t index_temp = 0;    //记录窗口第一个选项的下表方便计算刷新的y值
	uint8_t option_y = 0;      //标签刷新所在的y值
	uint8_t index = 1;         //用于第一次找到头尾节点指针
	
	if(OptionList == NULL) //判断是否是空链表
	{
		return;
	}
	
	if(pTop == NULL || pTail == NULL)         //就是第一遍然后遍历自动截取窗口长度的表头表尾
	{
		pTop = OptionList;
		pTail = OptionList;

		do
		{
			pTail = pTail->next_option;
		} while (pTail->next_option != OptionList && ++index < MAX_WINDOW);
	}
	
	p = pTop;                     //从窗口头开始遍历
	index_temp = p->option_index; //窗口起始下标为表头的下标
	do
	{
		option_y = OPTION_STARTY + RowSpacing  + (p->option_index - index_temp)*(16+ RowSpacing);
		if(p->IsLarge)
		{
			gui_ClearLines(option_y-1, option_y+17,  p->IsSelected);
			set_StartPoint(16);//画框的时候刷新下开始刷新的点///////////////////////32
		}
		
		if(p->IsLanguage)
		{
			GUI_PutChnStr(0, option_y, p->content_chn, p->ChnContent_size, MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, p->IsSelected);
		}

		if(p->IsEngOnly == DisplayWord_xin)
		{
			if(IsChn)
			{
				GUI_PutChnStr_xin(0, option_y, p->content_chn, p->ChnContent_size, MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, p->IsSelected);
			}
			else
			{	
				if(p->content_eng == qianfenhao_en)
				{
					GUI_PutChnStr_xin(0, option_y, p->content_chn, p->ChnContent_size, MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, p->IsSelected);
				}
				else
				{
					GUI_PutEngStr(0, option_y, p->content_eng, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, p->IsSelected);

				}
			}
		}
		else if(p->IsEngOnly == IS_ENG_ONLY)
		{
			GUI_PutEngStr(0, option_y, p->content_eng, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, p->IsSelected);
		}
		else
		{
			if(IsChn)
			{
				GUI_PutChnStr(0, option_y, p->content_chn, p->ChnContent_size, MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, p->IsSelected);
			}
			else
			{
				GUI_PutEngStr(0, option_y, p->content_eng, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, p->IsSelected);
			}
		}
		
		if(p->son_option != NULL)
		{
			NanoOptionList_Print(p->son_option, IsChn);
		}
		p = p->next_option;
	}while(p != pTail->next_option); //遍历链表并打印
		
}

void set_OptionTop(PtrToOptionNode node)     //设置窗口顶指针
{
	pTop = node;
}
void set_OptionTail(PtrToOptionNode node)    //设置窗口尾指针
{
	pTail = node;
}

uint8_t get_OptionTopIndex(void)             //获取选项的下标
{
	return pTop->option_index;
}

void window_change(PtrToOptionNode option_selected, list_option head)  //滑动窗口
{
	uint8_t index = 1;               //头尾长度计数
	
	if(option_selected == pTail->next_option)
	{
		if( option_selected != head)
		{
			pTop = pTop->next_option;
			pTail = pTail->next_option;
		}
		else
		{
			
			pTop = head;
			pTail = head;
			do
			{
				pTail = pTail->next_option;
			} while (pTail->next_option != head && ++index < MAX_WINDOW);
		}
	}
	if(option_selected == pTop->prev_option)
	{
		if( option_selected != head->prev_option)
		{
			pTop = pTop->prev_option;
			pTail = pTail->prev_option;
		}
		else
		{
			pTail = head->prev_option;         //指向链表的最后一个节点
			pTop = head->prev_option;

			do
			{
				pTop = pTop->prev_option;
			} while (pTop->prev_option != head->prev_option && ++index < MAX_WINDOW);
		}
	}
}

void refresh_window(PtrToOptionNode option_selected, list_option OptionList)
{
	uint8_t index = 1;
	if(option_selected->option_index < MAX_WINDOW)
	{
		pTop = OptionList;
		pTail = OptionList;
		do
		{
			pTail = pTail->next_option;
		} while (pTail->next_option != OptionList && ++index < MAX_WINDOW);
	}
	else
	{
		pTail = option_selected;
		pTop = option_selected;
		do
		{
			pTop = pTop->prev_option;
		} while (pTop->prev_option != OptionList->prev_option && ++index < MAX_WINDOW);
	}
	
	
}









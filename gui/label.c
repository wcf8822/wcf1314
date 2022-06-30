//gui标签结构
#include "label.h"
#include "loadbit.h"

//标签是单链表


/***********************************************************************
* 名称：LabelList_NodeGenerate()
* 功能：创建新的节点并返回指向节点的指针
* 入口参数：x            标签的x值
*          y            标签的y值
*          content_chn  标签所要显示的中文内容
*          content_eng  标签所要现实的英文内容
* 出口参数：指向节点的指针
***********************************************************************/
PtrToLabelNode LabelList_NodeGenerate(uint8_t x, uint8_t y, uint8_t* ContentChn, uint8_t ChnLen, uint8_t* ContentEng, LABEL_SIZE lbl_size, LABEL_TYPE lbl_type, LABEL_UINT_TYPE uint_type, uint8_t have_parenthesis)
{
	PtrToLabelNode p = NULL;
	p = (PtrToLabelNode)malloc(sizeof(node_label));
	
	if(p == NULL)
	{
		return NULL;
	}
	
	p->x = x;
	p->y = y;
	p->content_chn = ContentChn;
	p->ChnContent_size = ChnLen;
	p->content_eng = ContentEng;
	p->label_size = lbl_size;
	p->label_type = lbl_type;
	p->uint_type = uint_type;
	p->have_parenthesis = have_parenthesis;
	
	p->next_label = NULL;
	return p;
}

/***********************************************************************
* 名称：LabelList_Delete()
* 功能：遍历并清空链表。
* 入口参数：lbllist   链表头指针的指针（因为要让指针指向NULL所以要用指针的指针）
* 出口参数：无
***********************************************************************/
void LabelList_Destory(list_label *lbllist)//清空链表 入口参数 头指针 这里不使用哨兵
{
	PtrToLabelNode p = NULL, temp = NULL;
	p = *lbllist;                          //指向头指针
	*lbllist = NULL;	
	while(p != NULL)
	{
		temp = p->next_label;
		free(p);
		p = temp;
	}
}

/***********************************************************************
* 名称：LabelList_Add()
* 功能：添加节点到标签链表中。
* 入口参数：x            标签的x值
*          y            标签的y值
*          content_chn  标签所要显示的中文内容
*          content_eng  标签所要现实的英文内容
*          lbllist      链表头指针的指针（因为要修改指针所以要调用指针的指针）
* 出口参数：无
***********************************************************************/
void LabelList_Add(uint8_t x, uint8_t y, uint8_t* ContentChn, uint8_t ChnLen, uint8_t* ContentEng, LABEL_SIZE lbl_size, LABEL_TYPE lbl_type, LABEL_UINT_TYPE uint_type, uint8_t have_parenthesis, list_label* lbllist)
{
	PtrToLabelNode p = NULL;
	
	p = LabelList_NodeGenerate(x, y, ContentChn, ChnLen, ContentEng, lbl_size, lbl_type, uint_type, have_parenthesis);
	
	p->next_label = (*lbllist);

	(*lbllist) = p;	
	
}


//获取链表长度
uint8_t LabelList_count(list_label lbllist)
{
	PtrToLabelNode p = lbllist;
	uint8_t count = 1;
	
	if(lbllist == NULL)
	{
		return 0;
	}
	
	while(p->next_label != NULL)
	{
		count++;
		p = p->next_label;
	}
	return count;
}


/***********************************************************************
* 名称：LabelList_Print()
* 功能：遍历标签列表然后将内容显示在屏幕上。
* 入口参数：lbllist  标签链表的头指针
*          IsChn    打印的东西 0为打印标签的英文 1为打印标签的中文
* 出口参数：无
***********************************************************************/
void LabelList_Print(list_label lbllist , uint8_t IsChn)
{
	PtrToLabelNode p = lbllist;
	uint8_t temp_x;
	
	if(p == NULL)//如果是空链表的话
	{
		return;
	}
	
	while(p != NULL)
	{
		
		switch(p->label_type)
		{
			case LABEL_PUNCTUATION:
				GUI_PutCharArr(p->x, p->y, p->content_eng, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
				break;
			
			case LABEL_NUMBERORENG:
				switch (p->label_size)
				{
					case LABEL_LARGE:
						GUI_PutLargeStr(p->x, p->y, p->content_eng, MENU_FONT_ENG_LARGE_LSIZE, MENU_FONT_ENG_LARGE_RSIZE, LOADBIT_NORMAL);
						break;

					case LABEL_MEDIUM:
						GUI_PutMediumStr(p->x, p->y, p->content_eng, MENU_FONT_ENG_MEDIUM_LSIZE, MENU_FONT_ENG_MEDIUM_RSIZE, LOADBIT_NORMAL);
						break;

					case LABEL_NORMAL:
						GUI_PutEngStr(p->x, p->y, p->content_eng, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
						break;
				}
				break;
			
			case LABEL_STRING:
				if(IsChn)
				{
					GUI_PutChnStr(p->x, p->y, p->content_chn, p->ChnContent_size, MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, LOADBIT_NORMAL);
				}
				else
				{
					GUI_PutEngStr(p->x, p->y, p->content_eng, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
				}
				break;
				
			case LABEL_UINT:
				temp_x = p->x;
				if(p->have_parenthesis)
				{
					GUI_PutChar(temp_x, p->y, '(', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
					temp_x+=8;
				}
				switch(p->uint_type)
				{
					case UINT_PERCENT:
						GUI_PutChar(temp_x, p->y, '%', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
						temp_x+=8;
						break;
					case UINT_CELSIUS:
						GUI_PutCharArr(temp_x, p->y, (uint8_t *)(SHESHIDU), MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_LSIZE, LOADBIT_NORMAL);
						temp_x+=16;
						break;
					case UINT_MGL:
						GUI_PutChar(temp_x, p->y, 'm', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
						temp_x+=8;
						GUI_PutChar(temp_x, p->y, 'g', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
						temp_x+=8;
						GUI_PutChar(temp_x, p->y, '/', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
						temp_x+=8;
						GUI_PutChar(temp_x, p->y, 'L', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
						temp_x+=8;
						break;
					case UINT_USCM:
						break;
					case UINT_NTU:
						break;
					case UINT_MV:
						break;
					case UINT_KPA:
						GUI_PutChar(temp_x, p->y, 'k', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
						temp_x+=8;
						GUI_PutChar(temp_x, p->y, 'P', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
						temp_x+=8;
						GUI_PutChar(temp_x, p->y, 'a', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
						temp_x+=8;
						break;
					case UINT_UGL:
						break;
					case UINT_PPT:
						GUI_PutChar(temp_x, p->y, 'p', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
						temp_x+=8;
						GUI_PutChar(temp_x, p->y, 'p', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
						temp_x+=8;
						GUI_PutChar(temp_x, p->y, 't', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
						temp_x+=8;
						break;
					
					default:
						break;
				}
				if(p->have_parenthesis)
				{
					GUI_PutChar(temp_x, p->y, ')', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
					temp_x+=8;
					GUI_PutChar(temp_x, p->y, ':', MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, LOADBIT_NORMAL);
					temp_x+=8;
				}
				
				break;
		}
		p = p->next_label;
	}
}







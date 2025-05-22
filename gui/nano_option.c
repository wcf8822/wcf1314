#include "nano_option.h"
#include "loadbit.h"
#include "GUI_BASIC.h"
#include "interfacial.h"


PtrToNanoOptionNode NanoOption_NodeGenerate(uint8_t x, uint8_t y, uint8_t* ContentChn, uint8_t ChnLen, uint8_t* ContentEng, 
																						NANOOPTION_TYPE NanoOption_type, uint8_t value, uint8_t min_value, uint8_t max_value, uint8_t IsSingle)
{
	PtrToNanoOptionNode p = NULL;
	p = (PtrToNanoOptionNode)malloc(sizeof(node_nano));
	
	if(p == NULL)
	{
		return NULL;
	}
	
	p->next_option = p;
	p->prev_option = p;
	
	p->x = x;                                 //起始x坐标
	p->y = y;                                 //起始y坐标
	p->content_chn = (uint8_t *)ContentChn;   //中文内容指针
	p->ChnContent_size = ChnLen;
	
	p->value = value;
	p->min_value = min_value;
	p->max_value = max_value;

	p->content_eng = (uint8_t *)ContentEng;  //英文内容指针
	
	p->IsSingle = IsSingle;
	p->NanoOption_type = NanoOption_type;
	
	return p;
}


void NanoOptionList_Add(uint8_t x, uint8_t y, uint8_t* ContentChn, uint8_t ChnLen, uint8_t* ContentEng, 
												NANOOPTION_TYPE NanoOption_type, uint8_t value, uint8_t min_value, uint8_t max_value, uint8_t IsSingle, list_NanoOption* nanooptionlist)
{
	PtrToNanoOptionNode p = NanoOption_NodeGenerate(x, y, ContentChn, ChnLen, ContentEng, NanoOption_type, value, min_value, max_value, IsSingle);
	
	if((*nanooptionlist) == NULL)
	{
		*nanooptionlist = p;
	}
	else
	{
		p->prev_option = (*nanooptionlist)->prev_option;
		p->next_option = (*nanooptionlist);
		(*nanooptionlist)->prev_option->next_option = p;
		(*nanooptionlist)->prev_option = p;
	}
}

static uint8_t temp_num_arr[3]={0, 0, '\0'};//数字标签显示缓存数组。
//小标签打印
void NanoOptionList_Print(list_NanoOption nanooptionlist, uint8_t IsChn)
{
	PtrToNanoOptionNode p = nanooptionlist;
	
	if(p == NULL) //判断是否是空链表
	{
		return;
	}
	do
	{
		switch(p->NanoOption_type)
		{
			case NANOOPTION_BUTTON:
				if(IsChn)
				{
					GUI_PutChnStr(p->x, p->y, p->content_chn, p->ChnContent_size, MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, p->IsSelected);
				}
				else
				{
					GUI_PutEngStr(p->x, p->y, p->content_eng, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, p->IsSelected);
				}
				break;
			
			case NANOOPTION_NUMBER:
				
				if(p->IsSingle)
				{
					if(p->value < 10)
					{
					 temp_num_arr[0]= p->value + '0';
					}
					else
					{
						temp_num_arr[0]= (p->value - 9) + 'A';
					}
					
					temp_num_arr[1] = '\0';
				}
				else//如果是单一一位数的话小于10要补0
				{
					
					temp_num_arr[0] = p->value / 10 + '0';
					temp_num_arr[1] = p->value % 10 + '0';
					
				}
				GUI_PutEngStr(p->x, p->y, temp_num_arr, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, p->IsSelected);
				break;
			
			case NANOOPTION_UINT:
				switch(interfacial_GetCurPage())
				{
					case PAGE_3_PRESSURE:
						GUI_PutEngStr(p->x, p->y, (uint8_t *)kPa, MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, p->IsSelected);
						break;
					case PAGE_3_SALT:
						GUI_PutEngStr(p->x, p->y, (uint8_t *)Sal_uint[p->value], MENU_FONT_ENG_LSIZE, MENU_FONT_ENG_RSIZE, p->IsSelected);
						break;
					
					
					default:
						break;
				}
				break;
				
			case NANOOPTION_STRING://中文类型
					GUI_PutChnStr(p->x, p->y, p->content_chn+p->value, p->ChnContent_size, MENU_FONT_CHN_LSIZE, MENU_FONT_CHN_RSIZE, p->IsSelected);
				break;
		}

		
		p = p->next_option;
	}while(p != nanooptionlist); //遍历链表并打印
}


void NanoOptionList_Destory(PtrToNanoOptionNode *nanooptionlist)
{
	PtrToNanoOptionNode p = *nanooptionlist, temp = NULL;
	if(*nanooptionlist == NULL)
	{
		return;
	}
	do
	{
		temp = p->next_option;
		free(p);
		p = temp;
	}while(p != (*nanooptionlist));
	
	*nanooptionlist = NULL;	
}


/*获取每个节点的值*/
/*coefficient 进制 100就两位数 10就单位数 */
uint32_t NanoOptionList_GetValue(PtrToNanoOptionNode nanooptionlist, uint8_t coefficient)
{
	uint32_t ret = 0;//临时变量
	PtrToNanoOptionNode p = nanooptionlist; //遍历链表的临时变量
	
	if(nanooptionlist == NULL) //安全性检测
	{
		return 0;
	}
	
	ret += p->value;
	do
	{
		p = p->next_option;
		ret = ret*coefficient + p->value;
	}while(p != nanooptionlist->prev_option);
	
	return ret;
}

int8_t orp_np = 0;
/*获取正负符号值*/
uint32_t NanoOptionList_Get_np_Value(PtrToNanoOptionNode nanooptionlist, uint8_t coefficient)
{
	uint32_t ret = 0;//临时变量
	PtrToNanoOptionNode p = nanooptionlist; //遍历链表的临时变量
	
	if(nanooptionlist == NULL) //安全性检测
	{
		return 0;
	}
	
	ret += p->value;
	orp_np = p->value;
	if (orp_np == 67 || orp_np == 68)
	{
		ret = 0;
	}
	
	do
	{
		p = p->next_option;
		ret = ret*coefficient + p->value;
	}while(p != nanooptionlist->prev_option);
	
	return ret;
}
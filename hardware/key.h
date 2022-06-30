#ifndef __KEY_H
#define __KEY_H


#include "main.h"


#define KEY_NORMAL   0
#define KEY_CLICKED  1

#define KEY_BURSTMODE GPIO_PIN_SET

#define KEY_UP_STATUS HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin)
#define KEY_DOWN_STATUS HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin)
#define KEY_OK_STATUS HAL_GPIO_ReadPin(KEY_OK_GPIO_Port, KEY_OK_Pin)
#define KEY_ESC_STATUS HAL_GPIO_ReadPin(KEY_ESC_GPIO_Port, KEY_ESC_Pin)
#define KEY_CAL_STATUS HAL_GPIO_ReadPin(KEY_CAL_GPIO_Port, KEY_CAL_Pin)
#define KEY_BLU_STATUS HAL_GPIO_ReadPin(KEY_BLU_GPIO_Port, KEY_BLU_Pin)
#define KEY_MENU_STATUS HAL_GPIO_ReadPin(KEY_MENU_GPIO_Port, KEY_MENU_Pin)


//所有按键状态的结构体声明
typedef struct{
	uint8_t flag_KeyUp     :1;
	uint8_t flag_KeyDown   :1;
	uint8_t flag_KeyOk     :1;
	uint8_t flag_KeyEsc    :1;
	uint8_t flag_KeyBlu    :1;
	uint8_t flag_KeyMenu   :1;
	uint8_t flag_KeyCal    :1;
	uint8_t flag_KeyClicked :1;
	
	uint8_t flag_KeyCalLong :1;
	uint8_t flag_KeyOkLong :1;
}KeyFlag_t;




//获取标志状态（既获取按键是否按下）
uint8_t get_KeyUpFlag(void);
uint8_t get_KeyDownFlag(void);
uint8_t get_KeyOkFlag(void);
uint8_t get_KeyOkLongFlag(void);
uint8_t get_KeyEscFlag(void);
uint8_t get_KeyMenuFlag(void);
uint8_t get_KeyCalFlag(void);
uint8_t get_KeyCalLongFlag(void);
uint8_t get_KeyBluFlag(void);
uint8_t get_KeyClickedFlag(void);

//清空标志
void clear_KeyUpFlag(void);
void clear_KeyDownFlag(void);
void clear_KeyOkFlag(void);
void clear_KeyEscFlag(void);
void clear_KeyMenuFlag(void);
void clear_KeyCalFlag(void);
void clear_KeyCalLongFlag(void);
void clear_KeyBluFlag(void);
void clear_KeyClickedFlag(void);

void clear_KeyAllFlag(void);

//设置标志
//void set_KeyUpFlag(void);
//void set_KeyDownFlag(void);
//void set_KeyOkFlag(void);
//void set_KeyEscFlag(void);
//void set_KeyMenuFlag(void);
//void set_KeyCalFlag(void);
void set_KeyCalLongFlag(void);
void set_KeyOkLongFlag(void);
//void set_KeyBluFlag(void);
//void set_KeyClickedFlag(void);


void key_scan(void);


#endif





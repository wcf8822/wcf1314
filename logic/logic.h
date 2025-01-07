#ifndef __JUDGE_H
#define __JUDGE_H


#include "main.h"

#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"







void hardware_init(void);

void main_loop(void);
void clear_SAVEDATACount(void);

void Save_Data(void);
void logic_BatteryAD_Big(void);
void software_init(void);



#endif





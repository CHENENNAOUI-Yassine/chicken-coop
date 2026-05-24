#ifndef UTILS_H
#define UTILS_H

#include "stm32f4xx.h"  


void ClearBuffer(char *buffer, int size);
void delay_ms(uint32_t ms);




void enter_standby_mode(void);
void RTC_Wakeup_Init(uint32_t seconds);

#endif // UTILS_H

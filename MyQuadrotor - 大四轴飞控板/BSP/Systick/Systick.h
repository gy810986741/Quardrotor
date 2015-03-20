#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "sys.h"

void SysTick_Init(void);
void Delay_us(__IO u32 nTime);
void Delay_ms(__IO u32 nTime);
void TimingDelay_Decrement(void);
#endif /* __SYSTICK_H */

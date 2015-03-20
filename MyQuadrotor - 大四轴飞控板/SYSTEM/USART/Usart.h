#ifndef _UART_H
#define _UART_H
#include "sys.h"
void USART1_Init(unsigned long bound);
void Uart1_Put_String(unsigned char *Str);
void Uart1_Put_Buf(unsigned char *DataToSend , u8 data_num);
//extern void USART1_IRQHandler(void);
#endif /* _Uart_H */

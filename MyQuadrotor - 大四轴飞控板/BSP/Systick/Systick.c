/******************** (C) COPYRIGHT 2014 Aking���������� ********************
 * �ļ���  ��Systick.c
 * ����    ��Systick ϵͳ�δ�ʱ��10us�жϺ�����,�ж�ʱ����������ã�
 *           ���õ��� 1us 10us 1ms �жϡ�         
 * ��汾  ��ST3.0.0
 * ����    ��aleanking  QQ: 313696178
**********************************************************************************/

#include "SysTick.h"

static __IO u32 TimingDelay;

/***************************************************
 * ��������SysTick_Init
 * ����  ������ϵͳ�δ�ʱ�� SysTick
 * ����  ����
 * ���  ����
 * ����  ���ⲿ���� 
 **************************************************/
void SysTick_Init(void)
{
	/* SystemCoreClock / 1000    1ms�ж�һ��
	 * SystemCoreClock / 100000	 10us�ж�һ��
	 * SystemCoreClock / 1000000 1us�ж�һ��
	 */
	if (SysTick_Config(SystemCoreClock/1000))    // 1ms�ж�һ��
	{ 
	/* Capture error */ 
		while (1);
	}
}


/***************************************************
 * ��������Delay_us
 * ����  ��us��ʱ����,1usΪһ����λ
 * ����  ��- nTime
 * ���  ����
 * ����  ��Delay_us( 1 ) ��ʵ�ֵ���ʱΪ  1us = 1us
 *       ���ⲿ���� 
 **************************************************/

void Delay_us(__IO u32 nTime)
{ 
	TimingDelay = nTime;

	while(TimingDelay != 0);
}

/***************************************************
 * ��������Delay_ms
 * ����  ��us��ʱ����,1msΪһ����λ
 * ����  ��- nTime
 * ���  ����
 * ����  ��Delay_us( 1 ) ��ʵ�ֵ���ʱΪ  1ms = 1ms
 *       ���ⲿ���� 
 **************************************************/

void Delay_ms(__IO u32 nTime)
{ 
	TimingDelay = nTime;

	while(TimingDelay != 0);
}

/***************************************************
 * ��������TimingDelay_Decrement
 * ����  ����ȡ���ĳ���
 * ����  ����
 * ���  ����
 * ����  ���� SysTick �жϺ��� SysTick_Handler()����
 **************************************************/
void TimingDelay_Decrement(void)
{
	if(TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}
/******************* (C) COPYRIGHT 2014 Aking���������� ******/

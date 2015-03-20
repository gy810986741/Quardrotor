#include "Timer.h"

u32 TIM3_IRQCNT = 0;
/**************************ʵ�ֺ���********************************************
*����ԭ��:		
*��������:1ms�ж�һ��,������Ϊ1000		
*******************************************************************************/
void Tim3_Init(u16 period_num)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	//�������ã�ʱ���ͱȽ�������ã���������ֻ�趨ʱ�����Բ���OC�Ƚ����
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

  //TIM3
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_TIM3_P;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_TIM3_S;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_DeInit(TIM3);

	TIM_TimeBaseStructure.TIM_Period=period_num;//װ��ֵ
	//prescaler is 1200,that is 72000000/72/500=2000Hz;
	TIM_TimeBaseStructure.TIM_Prescaler=72-1;//��Ƶϵ��
	//set clock division 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //or TIM_CKD_DIV2 or TIM_CKD_DIV4
	//count up
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	//clear the TIM2 overflow interrupt flag
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	//TIM2 overflow interrupt enable
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	//enable TIM2
	TIM_Cmd(TIM3,ENABLE);
}


//void Timer3_NVIC_Configuration(void) 
//{     
//	NVIC_InitTypeDef NVIC_InitStructure; 
//  //TIM3
//	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_TIM3_P;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_TIM3_S;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//} 




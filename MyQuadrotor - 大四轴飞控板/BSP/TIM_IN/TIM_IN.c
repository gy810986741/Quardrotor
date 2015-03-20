#include "TIM_IN.h"
u16 Rc_Pwm_In[4];
void Tim_Pwm_In_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
//TIM4 pwm in
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_TIM4_P;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_TIM4_S;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_DeInit(TIM4);

	TIM_TimeBaseStructure.TIM_Period = 20000-1;	//???
	TIM_TimeBaseStructure.TIM_Prescaler=71;//
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //or TIM_CKD_DIV2 or TIM_CKD_DIV4
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//TIM_ICPolarity_BothEdge;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;	
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;     
	TIM_ICInitStructure.TIM_ICFilter = 0x00;       
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//TIM_ICPolarity_BothEdge;;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;	
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;     
	TIM_ICInitStructure.TIM_ICFilter = 0x00;       
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//TIM_ICPolarity_BothEdge;;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;	
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;     
	TIM_ICInitStructure.TIM_ICFilter = 0x00;       
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//TIM_ICPolarity_BothEdge;;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;	
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;     
	TIM_ICInitStructure.TIM_ICFilter = 0x00;       
	TIM_ICInit(TIM4, &TIM_ICInitStructure);


	/* Enable the CC1 Interrupt Request */
	TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_CC4, ENABLE);
	/* TIM enable counter */
	TIM_Cmd(TIM4, ENABLE);

}

static T_RC_Data 	Rc_Data;//1000~2000

void Rc_DataAnl(void)
{
	Rc_Data.THROTTLE		=	Rc_Pwm_In[1];
	Rc_Data.YAW				=	Rc_Pwm_In[0];
	Rc_Data.ROLL			=	Rc_Pwm_In[3];
	Rc_Data.PITCH			=	Rc_Pwm_In[2];	
}
void Rc_GetValue(T_RC_Data *temp)
{
	temp->THROTTLE	= Rc_Data.THROTTLE;
	temp->YAW		= Rc_Data.YAW;
	temp->ROLL		= Rc_Data.ROLL;
	temp->PITCH		= Rc_Data.PITCH;	
}
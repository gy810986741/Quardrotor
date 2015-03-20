#include "stm32f10x_it.h" 
#include "SysTick.h"
#include "led.h"
#include "MPU6050.h"
#include "Data_Transfer.h"
#include "ADC.h"
#include "TIM_IN.h"
#include "MS5611.h"
#include "HMC5883L.h"

u8 TxBuffer[256];
u8 TxCounter=0;
u8 count=0;
u8 RxBuffer[50];
//static u8 RxState = 0;

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Printf
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

void SysTick_Handler(void)//500us
{
	static u16 STick_IRQCNT=0;
	static u16 BEEP_CNT=0;
	
	TimingDelay_Decrement();
	STick_IRQCNT++;
	BEEP_CNT++;
	if(STick_IRQCNT>500)
	{
		STick_IRQCNT=0;
		LED_Spark();
	}
	if((BEEP_CNT%=1000)<50)
		GPIO_ResetBits(LED234_GPIO,LED3_PIN);
	else
		GPIO_SetBits(LED234_GPIO,LED3_PIN);

}
void TIM3_IRQHandler(void)
{
	
	static u8 ms1_cnt=0;
	static u16 s1_cnt=0;
	
	if(TIM3->SR & TIM_IT_Update)
	{     
		TIM3->SR = ~TIM_FLAG_Update;//����жϱ�־
		ms1_cnt++;
		s1_cnt++;
		if(ms1_cnt==2)
		{
			ms1_cnt = 0;
			MPU6050_Read();
			FLAG_ATT = 1;
		}
		else 
		{
			MS5611_loop();
		}
//		if(s1_cnt==2000)
//		{
//			s1_cnt = 0;	
//			Bat_Votage=2*345*ADCConvertedValue/4096;
//		}
		Data_Exchange();
	}
}
//��ʱ��4ͨ�����벶������
u8  TIM4CH1_CAPTURE_STA=0;	//ͨ��1���벶��״̬		  �ø���λ�������־������λ���������  				
u16	TIM4CH1_CAPTURE_UPVAL;	//ͨ��1���벶��ֵ
u16	TIM4CH1_CAPTURE_DOWNVAL;//ͨ��1���벶��ֵ

u8  TIM4CH2_CAPTURE_STA=0;	//ͨ��2���벶��״̬		    				
u16	TIM4CH2_CAPTURE_UPVAL;	//ͨ��2���벶��ֵ
u16	TIM4CH2_CAPTURE_DOWNVAL;//ͨ��2���벶��ֵ

u8  TIM4CH3_CAPTURE_STA=0;	//ͨ��3���벶��״̬		    				
u16	TIM4CH3_CAPTURE_UPVAL;	//ͨ��3���벶��ֵ
u16	TIM4CH3_CAPTURE_DOWNVAL;//ͨ��3���벶��ֵ

u8  TIM4CH4_CAPTURE_STA=0;	//ͨ��4���벶��״̬		    				
u16	TIM4CH4_CAPTURE_UPVAL;	//ͨ��4���벶��ֵ
u16	TIM4CH4_CAPTURE_DOWNVAL;//ͨ��4���벶��ֵ

void TIM4_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM4, TIM_IT_CC1) == SET)//����1���������¼�
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);//�������1��־λ
		
		if(TIM4CH1_CAPTURE_STA == 0)//����������
		{
			TIM4CH1_CAPTURE_UPVAL = TIM_GetCapture1(TIM4);//��ȡ�����ص�����
			
			TIM4CH1_CAPTURE_STA = 1;		//����Բ�����������
			TIM_OC1PolarityConfig(TIM4, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
		}
		else                        //�����½��� (�Ѿ�����һ�������ĸߵ�ƽ���壡)
		{
			TIM4CH1_CAPTURE_DOWNVAL = TIM_GetCapture1(TIM4);//��ȡ�½��ص�����
			
			//�ж��Ƿ񳬳����������,����ߵ�ƽ����ʱ��us
			if(TIM4CH1_CAPTURE_DOWNVAL<TIM4CH1_CAPTURE_UPVAL)
			{
				Rc_Pwm_In[0] = 20000 - TIM4CH1_CAPTURE_UPVAL + TIM4CH1_CAPTURE_DOWNVAL;
			}
			else
			{
				Rc_Pwm_In[0] = TIM4CH1_CAPTURE_DOWNVAL - TIM4CH1_CAPTURE_UPVAL;
			}
			
			TIM4CH1_CAPTURE_STA = 0;
			TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Rising); //����Ϊ�����ز���
		}
	}	


	if(TIM_GetITStatus(TIM4, TIM_IT_CC2) == SET)//����2���������¼�
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);//�������2��־λ
		
		if(TIM4CH2_CAPTURE_STA == 0)//����������
		{
			TIM4CH2_CAPTURE_UPVAL = TIM_GetCapture2(TIM4);//��ȡ�����ص�����
			
			TIM4CH2_CAPTURE_STA = 1;		//����Բ�����������
			TIM_OC2PolarityConfig(TIM4, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
		}
		else                        //�����½��� (�Ѿ�����һ�������ĸߵ�ƽ���壡)
		{
			TIM4CH2_CAPTURE_DOWNVAL = TIM_GetCapture2(TIM4);//��ȡ�½��ص�����
			
			//�ж��Ƿ񳬳����������,����ߵ�ƽ����ʱ��us
			if(TIM4CH2_CAPTURE_DOWNVAL<TIM4CH2_CAPTURE_UPVAL)
			{
				Rc_Pwm_In[1] = 20000 - TIM4CH2_CAPTURE_UPVAL + TIM4CH2_CAPTURE_DOWNVAL;
			}
			else
			{
				Rc_Pwm_In[1] = TIM4CH2_CAPTURE_DOWNVAL - TIM4CH2_CAPTURE_UPVAL;
			}
			TIM4CH2_CAPTURE_STA = 0;
			TIM_OC2PolarityConfig(TIM4, TIM_ICPolarity_Rising); //����Ϊ�����ز���
		}
	}
	
	if(TIM_GetITStatus(TIM4, TIM_IT_CC3) == SET)//����3���������¼�
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);//�������3��־λ
		
		if(TIM4CH3_CAPTURE_STA == 0)//����������
		{
			TIM4CH3_CAPTURE_UPVAL = TIM_GetCapture3(TIM4);//��ȡ�����ص�����
			
			TIM4CH3_CAPTURE_STA = 1;		//����Բ�����������
			TIM_OC3PolarityConfig(TIM4, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
		}
		else                        //�����½��� (�Ѿ�����һ�������ĸߵ�ƽ���壡)
		{
			TIM4CH3_CAPTURE_DOWNVAL = TIM_GetCapture3(TIM4);//��ȡ�½��ص�����
			
			//�ж��Ƿ񳬳����������,����ߵ�ƽ����ʱ��us
			if(TIM4CH3_CAPTURE_DOWNVAL<TIM4CH3_CAPTURE_UPVAL)
			{
				Rc_Pwm_In[2] = 20000 - TIM4CH3_CAPTURE_UPVAL + TIM4CH3_CAPTURE_DOWNVAL;
			}
			else
			{
				Rc_Pwm_In[2] = TIM4CH3_CAPTURE_DOWNVAL - TIM4CH3_CAPTURE_UPVAL;
			}
			
			TIM4CH3_CAPTURE_STA = 0;
			TIM_OC3PolarityConfig(TIM4, TIM_ICPolarity_Rising); //����Ϊ�����ز���
			
		}
	}
	
	
	if(TIM_GetITStatus(TIM4, TIM_IT_CC4) == SET)//����4���������¼�
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);//�������4��־λ
		
		if(TIM4CH4_CAPTURE_STA == 0)//����������
		{
			TIM4CH4_CAPTURE_UPVAL = TIM_GetCapture4(TIM4);//��ȡ�����ص�����
			
			TIM4CH4_CAPTURE_STA = 1;		//����Բ�����������
			TIM_OC4PolarityConfig(TIM4, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
		}
		else                        //�����½��� (�Ѿ�����һ�������ĸߵ�ƽ���壡)
		{
			TIM4CH4_CAPTURE_DOWNVAL = TIM_GetCapture4(TIM4);//��ȡ�½��ص�����
			
			//�ж��Ƿ񳬳����������,����ߵ�ƽ����ʱ��us
			if(TIM4CH4_CAPTURE_DOWNVAL<TIM4CH4_CAPTURE_UPVAL)
			{
				Rc_Pwm_In[3] = 20000 - TIM4CH4_CAPTURE_UPVAL + TIM4CH4_CAPTURE_DOWNVAL;
			}
			else
			{
				Rc_Pwm_In[3] = TIM4CH4_CAPTURE_DOWNVAL - TIM4CH4_CAPTURE_UPVAL;
			}
			TIM4CH4_CAPTURE_STA = 0;
			TIM_OC4PolarityConfig(TIM4, TIM_ICPolarity_Rising); //����Ϊ�����ز���
		}
	}
	Rc_DataAnl();

}
void USART1_IRQHandler(void)
{
			unsigned char ResData;//���յ���һ���ֽ�		
	static 	unsigned char Data_Len = 0,//���ݳ���
						  Data_Count = 0;//���ݳ��ȼ���
	static 	unsigned char RxState = 0;//��������״̬����   �����л�״̬

	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)//
    {
        USART_ReceiveData(USART1);
    }		
	//�����ж�
	if((USART1->SR & (1 << 7))&&(USART1->CR1 & USART_CR1_TXEIE))
	{
		USART1->DR = TxBuffer[TxCounter++]; //дDR����жϱ�־          
		if(TxCounter == count)
		{
			USART1->CR1 &= ~USART_CR1_TXEIE;		//�ر�TXE�ж�
		}
	}
	//�����ж� (���ռĴ����ǿ�) 
	if(USART1->SR & (1 << 5))  
	{
		ResData = USART1->DR;
		switch (RxState)
		{
			case 0:
				if(ResData == 0xAA)
				{
					RxState = 1;
					RxBuffer[0] = ResData;
				}
			break;
			case 1:
				if(ResData == 0xAF)
				{
					RxState = 2;
					RxBuffer[1] = ResData;
				}
			break;
			case 2:
				if(ResData > 0 && ResData < 0XF1)
				{
					RxState = 3;
					RxBuffer[2] = ResData;
				}
			break;
			case 3:
				if(ResData < 50)
				{
					RxState = 4;
					RxBuffer[3] = ResData;
					Data_Len = ResData;
					Data_Count = 0;
				}
			break;
			case 4:
				if(Data_Len > 0)
				{
					Data_Len --;
					RxBuffer[4 + Data_Count ++] = ResData;
					if(Data_Len == 0)
						RxState = 5;
				}
			break;
			case 5:
				RxState = 0;
				RxBuffer[4 + Data_Count] = ResData;
				Data_Receive_Anl(RxBuffer, Data_Count + 5);
			break;
			default:
				RxState = 0;
			break;	
		}
	}
}







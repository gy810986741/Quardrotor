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
		TIM3->SR = ~TIM_FLAG_Update;//清除中断标志
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
//定时器4通道输入捕获配置
u8  TIM4CH1_CAPTURE_STA=0;	//通道1输入捕获状态		  用高两位做捕获标志，低六位做溢出计数  				
u16	TIM4CH1_CAPTURE_UPVAL;	//通道1输入捕获值
u16	TIM4CH1_CAPTURE_DOWNVAL;//通道1输入捕获值

u8  TIM4CH2_CAPTURE_STA=0;	//通道2输入捕获状态		    				
u16	TIM4CH2_CAPTURE_UPVAL;	//通道2输入捕获值
u16	TIM4CH2_CAPTURE_DOWNVAL;//通道2输入捕获值

u8  TIM4CH3_CAPTURE_STA=0;	//通道3输入捕获状态		    				
u16	TIM4CH3_CAPTURE_UPVAL;	//通道3输入捕获值
u16	TIM4CH3_CAPTURE_DOWNVAL;//通道3输入捕获值

u8  TIM4CH4_CAPTURE_STA=0;	//通道4输入捕获状态		    				
u16	TIM4CH4_CAPTURE_UPVAL;	//通道4输入捕获值
u16	TIM4CH4_CAPTURE_DOWNVAL;//通道4输入捕获值

void TIM4_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM4, TIM_IT_CC1) == SET)//捕获1发生捕获事件
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);//清除捕获1标志位
		
		if(TIM4CH1_CAPTURE_STA == 0)//捕获到上升沿
		{
			TIM4CH1_CAPTURE_UPVAL = TIM_GetCapture1(TIM4);//获取上升沿的数据
			
			TIM4CH1_CAPTURE_STA = 1;		//标记以捕获到了上升沿
			TIM_OC1PolarityConfig(TIM4, TIM_ICPolarity_Falling);//设置为下降沿捕获
		}
		else                        //捕获到下降沿 (已经捕获到一个完整的高电平脉冲！)
		{
			TIM4CH1_CAPTURE_DOWNVAL = TIM_GetCapture1(TIM4);//获取下降沿的数据
			
			//判读是否超出发生了溢出,计算高电平脉冲时间us
			if(TIM4CH1_CAPTURE_DOWNVAL<TIM4CH1_CAPTURE_UPVAL)
			{
				Rc_Pwm_In[0] = 20000 - TIM4CH1_CAPTURE_UPVAL + TIM4CH1_CAPTURE_DOWNVAL;
			}
			else
			{
				Rc_Pwm_In[0] = TIM4CH1_CAPTURE_DOWNVAL - TIM4CH1_CAPTURE_UPVAL;
			}
			
			TIM4CH1_CAPTURE_STA = 0;
			TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Rising); //设置为上升沿捕获
		}
	}	


	if(TIM_GetITStatus(TIM4, TIM_IT_CC2) == SET)//捕获2发生捕获事件
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);//清除捕获2标志位
		
		if(TIM4CH2_CAPTURE_STA == 0)//捕获到上升沿
		{
			TIM4CH2_CAPTURE_UPVAL = TIM_GetCapture2(TIM4);//获取上升沿的数据
			
			TIM4CH2_CAPTURE_STA = 1;		//标记以捕获到了上升沿
			TIM_OC2PolarityConfig(TIM4, TIM_ICPolarity_Falling);//设置为下降沿捕获
		}
		else                        //捕获到下降沿 (已经捕获到一个完整的高电平脉冲！)
		{
			TIM4CH2_CAPTURE_DOWNVAL = TIM_GetCapture2(TIM4);//获取下降沿的数据
			
			//判读是否超出发生了溢出,计算高电平脉冲时间us
			if(TIM4CH2_CAPTURE_DOWNVAL<TIM4CH2_CAPTURE_UPVAL)
			{
				Rc_Pwm_In[1] = 20000 - TIM4CH2_CAPTURE_UPVAL + TIM4CH2_CAPTURE_DOWNVAL;
			}
			else
			{
				Rc_Pwm_In[1] = TIM4CH2_CAPTURE_DOWNVAL - TIM4CH2_CAPTURE_UPVAL;
			}
			TIM4CH2_CAPTURE_STA = 0;
			TIM_OC2PolarityConfig(TIM4, TIM_ICPolarity_Rising); //设置为上升沿捕获
		}
	}
	
	if(TIM_GetITStatus(TIM4, TIM_IT_CC3) == SET)//捕获3发生捕获事件
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);//清除捕获3标志位
		
		if(TIM4CH3_CAPTURE_STA == 0)//捕获到上升沿
		{
			TIM4CH3_CAPTURE_UPVAL = TIM_GetCapture3(TIM4);//获取上升沿的数据
			
			TIM4CH3_CAPTURE_STA = 1;		//标记以捕获到了上升沿
			TIM_OC3PolarityConfig(TIM4, TIM_ICPolarity_Falling);//设置为下降沿捕获
		}
		else                        //捕获到下降沿 (已经捕获到一个完整的高电平脉冲！)
		{
			TIM4CH3_CAPTURE_DOWNVAL = TIM_GetCapture3(TIM4);//获取下降沿的数据
			
			//判读是否超出发生了溢出,计算高电平脉冲时间us
			if(TIM4CH3_CAPTURE_DOWNVAL<TIM4CH3_CAPTURE_UPVAL)
			{
				Rc_Pwm_In[2] = 20000 - TIM4CH3_CAPTURE_UPVAL + TIM4CH3_CAPTURE_DOWNVAL;
			}
			else
			{
				Rc_Pwm_In[2] = TIM4CH3_CAPTURE_DOWNVAL - TIM4CH3_CAPTURE_UPVAL;
			}
			
			TIM4CH3_CAPTURE_STA = 0;
			TIM_OC3PolarityConfig(TIM4, TIM_ICPolarity_Rising); //设置为上升沿捕获
			
		}
	}
	
	
	if(TIM_GetITStatus(TIM4, TIM_IT_CC4) == SET)//捕获4发生捕获事件
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);//清除捕获4标志位
		
		if(TIM4CH4_CAPTURE_STA == 0)//捕获到上升沿
		{
			TIM4CH4_CAPTURE_UPVAL = TIM_GetCapture4(TIM4);//获取上升沿的数据
			
			TIM4CH4_CAPTURE_STA = 1;		//标记以捕获到了上升沿
			TIM_OC4PolarityConfig(TIM4, TIM_ICPolarity_Falling);//设置为下降沿捕获
		}
		else                        //捕获到下降沿 (已经捕获到一个完整的高电平脉冲！)
		{
			TIM4CH4_CAPTURE_DOWNVAL = TIM_GetCapture4(TIM4);//获取下降沿的数据
			
			//判读是否超出发生了溢出,计算高电平脉冲时间us
			if(TIM4CH4_CAPTURE_DOWNVAL<TIM4CH4_CAPTURE_UPVAL)
			{
				Rc_Pwm_In[3] = 20000 - TIM4CH4_CAPTURE_UPVAL + TIM4CH4_CAPTURE_DOWNVAL;
			}
			else
			{
				Rc_Pwm_In[3] = TIM4CH4_CAPTURE_DOWNVAL - TIM4CH4_CAPTURE_UPVAL;
			}
			TIM4CH4_CAPTURE_STA = 0;
			TIM_OC4PolarityConfig(TIM4, TIM_ICPolarity_Rising); //设置为上升沿捕获
		}
	}
	Rc_DataAnl();

}
void USART1_IRQHandler(void)
{
			unsigned char ResData;//接收到的一个字节		
	static 	unsigned char Data_Len = 0,//数据长度
						  Data_Count = 0;//数据长度计数
	static 	unsigned char RxState = 0;//接收数据状态计数   用于切换状态

	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)//
    {
        USART_ReceiveData(USART1);
    }		
	//发送中断
	if((USART1->SR & (1 << 7))&&(USART1->CR1 & USART_CR1_TXEIE))
	{
		USART1->DR = TxBuffer[TxCounter++]; //写DR清除中断标志          
		if(TxCounter == count)
		{
			USART1->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE中断
		}
	}
	//接收中断 (接收寄存器非空) 
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







#include "Led.h"
/***************************************************
 * 函数名：LED_GPIO_Config
 * 描述  ：配置LED用到的I/O口
 * 输入  ：无
 * 输出  ：无
 **************************************************/
void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( LED1_GPIO_RCC|LED234_GPIO_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED2_PIN|LED3_PIN|LED4_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED234_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED1_PIN | BEEP_PIN;
	GPIO_Init(LED1_GPIO, &GPIO_InitStructure);
	
	GPIO_SetBits(LED234_GPIO,LED2_PIN|LED3_PIN|LED4_PIN);	 // turn off all led
	GPIO_SetBits(LED1_GPIO,LED1_PIN);
	GPIO_SetBits(BEEP_GPIO,BEEP_PIN);
}
void LED_Spark(void)
{
//	LED_1_Spark;
//	LED_2_Spark;
//	LED_3_Spark;
	LED_4_Spark;
}

void LED2_Spark(void)
{
	LED_2_Spark;
}

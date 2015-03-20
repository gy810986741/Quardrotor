#ifndef _LED_H
#define _LED_H
#include "sys.h"

#define LED234_GPIO_RCC			RCC_APB2Periph_GPIOA    /*LEDʹ�õ�GPIOʱ��*/
#define LED1_GPIO_RCC			RCC_APB2Periph_GPIOB    /*LEDʹ�õ�GPIOʱ��*/
#define LEDn					1                       /*LED����*/
#define LED234_GPIO				GPIOA                   /*LED��ʹ�õ�GPIO��*/
#define LED1_GPIO				GPIOB                   /*LED��ʹ�õ�GPIO��*/

#define LED1_PIN            	GPIO_Pin_2               /*LED1ʹ�õ�GPIO�ܽ�*/
#define LED2_PIN            	GPIO_Pin_7               /*LED1ʹ�õ�GPIO�ܽ�*/
#define LED3_PIN            	GPIO_Pin_6               /*LED1ʹ�õ�GPIO�ܽ�*/
#define LED4_PIN            	GPIO_Pin_5               /*LED1ʹ�õ�GPIO�ܽ�*/

#define LED_PIN_ALL   		  	LED1_PIN|LED2_PIN|LED3_PIN|LED4_PIN



#define BEEP_GPIO				GPIOB                   /*LED��ʹ�õ�GPIO��*/
#define BEEP_PIN            	GPIO_Pin_1              /*LED1ʹ�õ�GPIO�ܽ�*/



#define ON  								0
#define OFF 								1

#define LED_1(a)		       	if (a)	\
														   GPIO_SetBits(LED_GPIO,LED1_PIN);\
												   else		\
														   GPIO_ResetBits(LED_GPIO,LED1_PIN)
													 
#define LED_1_Spark          	LED1_GPIO->ODR ^=LED1_PIN
#define LED_2_Spark          	LED234_GPIO->ODR ^=LED2_PIN
#define LED_3_Spark          	LED234_GPIO->ODR ^=LED3_PIN
#define LED_4_Spark          	LED234_GPIO->ODR ^=LED4_PIN
												   
#define BEEP          			BEEP_GPIO->ODR ^=BEEP_PIN
//extern __IO uint16_t ADCConvertedValue; 
//void ADC_GPIO_Config(void);												   
void LED_GPIO_Config(void);
void LED_Spark(void);	
void LED2_Spark(void);		


#endif /* _LED_H */
													 
													 

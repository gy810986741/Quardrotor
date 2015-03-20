#include "ADC.h"

#define DR_ADDRESS                  ((uint32_t)0x4001244C)

__IO uint16_t ADCConvertedValue;     //ADCConvertedValue


void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;        //ADC
	DMA_InitTypeDef DMA_InitStructure;        //DMA

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		 //DMA

	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);		  //DMA1
	DMA_InitStructure.DMA_PeripheralBaseAddr = DR_ADDRESS;		  //DMA
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;   //
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//DMA
	DMA_InitStructure.DMA_BufferSize = 1;		   //DMA
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;	//
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //DMA
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   //
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//DMA
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		//H->M
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);          
	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);	  //??ADC?GPIOC??
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//???????
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;		  //??????
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;   //????????
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ADC????,????
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   //????,ADC?12??,?????
	ADC_InitStructure.ADC_NbrOfChannel = 1;	 //?????,1?
	ADC_Init(ADC1, &ADC_InitStructure);
	/* ADC1 regular channel13 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	//ADC???, ?13??? ????1,???? 
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);	  //ADC??,??
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);  //??ADC1
	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);	  //????
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));  //????????
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);		//????
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));	   //??????
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//??????,ADC??DMA???????RAM??
}


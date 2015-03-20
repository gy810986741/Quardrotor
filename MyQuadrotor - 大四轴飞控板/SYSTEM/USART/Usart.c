#include "Usart.h"
/*******************************************************************************
** 名    称 ： USART1_Init
** 功    能 ： 串口1初始化
** 入口参数 ： unsigned long bound	波特率
** 出口参数 ： 无
** 说    明 ： 
*******************************************************************************/
void USART1_Init(unsigned long bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//USART_ClockInitTypeDef  USART_ClockInitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能UART3，GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
 	USART_DeInit(USART1);  //复位串口
	 //USART1_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化
   
    //USART1_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化

   //USART1 NVIC 配置
	NVIC_Configuration();
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_UART_P;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_UART_S;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	//配置USART1
	//中断被屏蔽了
	USART_InitStructure.USART_BaudRate = bound;       //波特率可以通过地面站配置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据
	USART_InitStructure.USART_StopBits = USART_StopBits_1;   //在帧结尾传输1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;    //禁用奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //发送、接收使能
	//配置USART1时钟
	USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;  //时钟低电平活动
	USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;  //SLCK引脚上时钟输出的极性->低电平
	USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;  //时钟第二个边沿进行数据捕获
	USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable; //最后一位数据的时钟脉冲不从SCLK输出
//	
	USART_Init(USART1, &USART_InitStructure);
	USART_ClockInit(USART1, &USART_ClockInitStruct);

	//使能USART1接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//使能USART1
	USART_Cmd(USART1, ENABLE); 
}

void Uart1_Put_Buf(unsigned char *DataToSend , u8 data_num)
{
	u8 i=0;
	for(i=0;i<data_num;i++)
		TxBuffer[count++] = *(DataToSend+i);
	if(!(USART1->CR1 & USART_CR1_TXEIE))
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}
static void Uart1_Put_Char(unsigned char DataToSend)
{
	TxBuffer[count++] = DataToSend;  
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}
void Uart1_Put_String(unsigned char *Str)
{
	//判断Str指向的数据是否有效.
	while(*Str)
	{
		//是否是回车字符 如果是,则发送相应的回车 0x0d 0x0a
		if(*Str=='\r')Uart1_Put_Char(0x0d);
			else if(*Str=='\n')Uart1_Put_Char(0x0a);
				else Uart1_Put_Char(*Str);
		//指针++ 指向下一个字节.
		Str++;
	}
}


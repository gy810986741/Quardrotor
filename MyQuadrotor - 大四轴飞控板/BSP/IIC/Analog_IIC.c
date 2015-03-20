#include "Analog_IIC.h"

/*******************************************************************************
* Function Name  : I2C_GPIO_Config
* Description    : Configration Simulation IIC GPIO
* Input          : None 
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : I2C_delay
* Description    : Simulation IIC Timing series delay
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_delay(void)
{	
	u8 i = 0; //��������Ż��ٶȣ���������͵�5����д��
	while(i) 
	{ 
		i--; 
	}  
}

void delay5ms(void)
{	
	int i = 5000;  
	while(i) 
	{ 
		i--; 
	}  
}
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : Master Start Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : Wheather Start
****************************************************************************** */
static bool I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read) return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA_L;
	I2C_delay();
	if(SDA_read) return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SDA_L;
	I2C_delay();
	return TRUE;
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
static void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
static void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}   
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
static void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge Single
****************************************************************************** */
static bool I2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
		SCL_L;
		I2C_delay();
		return FALSE;
	}
	SCL_L;
	I2C_delay();
	return TRUE;
}
/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_SendByte(u8 SendByte) //���ݴӸ�λ����λ//
{
    u8 i = 8;
    while(i --)
    {
		SCL_L;
		I2C_delay();
		if(SendByte & 0x80)
			SDA_H;  
		else 
			SDA_L;   
		SendByte <<= 1;
		I2C_delay();
		SCL_H;
		I2C_delay();
    }
    SCL_L;
}  
/*******************************************************************************
* Function Name  : I2C_ReadByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave 
****************************************************************************** */
unsigned char I2C_ReadByte(void)  //���ݴӸ�λ����λ//
{ 
	u8 i = 8;
	u8 ReceiveByte = 0;
	SDA_H;				
	while(i --)
	{
		ReceiveByte <<= 1;      
		SCL_L;
		I2C_delay();
		SCL_H;
		I2C_delay();	
		if(SDA_read)
		{
			ReceiveByte |= 0x01;
		}
	}
	SCL_L;
	return ReceiveByte;
}       

bool Single_Write_Byte(unsigned char SlaveAddress, unsigned char REG_Address, unsigned char REG_data)
{
  	if(!I2C_Start())
		return FALSE;
    I2C_SendByte(SlaveAddress); //�����豸��ַ+д�ź�
    if(!I2C_WaitAck())
	{
		I2C_Stop();
		return FALSE;
	}
    I2C_SendByte(REG_Address); //���õ���ʼ��ַ      
    I2C_WaitAck();	
    if(!REG_data)//������Ϊ����ӦMS5611�ļĴ���д�뷽ʽ�صؼӵ�
	{
		I2C_Stop(); 
		return TRUE;
	}
	I2C_SendByte(REG_data);
    I2C_WaitAck();   
    I2C_Stop(); 
    return TRUE;
}
unsigned char Single_Read_Byte(unsigned char SlaveAddress, unsigned char REG_Address)
{   
	unsigned char REG_data;     	
	if(!I2C_Start())
		return FALSE;
	I2C_SendByte(SlaveAddress);  
	if(!I2C_WaitAck())
	{
		I2C_Stop();
		return FALSE;
	}
	I2C_SendByte((u8) REG_Address);//���õ���ʼ��ַ      
	I2C_WaitAck();
	I2C_Start();
	I2C_SendByte(SlaveAddress+1);
	I2C_WaitAck();
	REG_data= I2C_ReadByte();
	I2C_NoAck();
	I2C_Stop();
	return REG_data;
}				

bool Single_Write_Buffer(unsigned char SlaveAddress, unsigned char REG_Address, unsigned char *Buffer, unsigned char len)
{
	int i;
	if(!I2C_Start())
		return FALSE;
	I2C_SendByte(SlaveAddress); //�����豸��ַ+д�ź�
	if(!I2C_WaitAck())
	{
		I2C_Stop(); 
		return FALSE;
	}
	I2C_SendByte(REG_Address ); //���õ���ʼ��ַ      
	I2C_WaitAck();	
	for (i = 0; i < len; i++)
	{
		I2C_SendByte(Buffer[i]);
		if(!I2C_WaitAck())
		{
			I2C_Stop(); 
			return FALSE;
		}
	}
	I2C_Stop(); 
	return TRUE;
}


bool Single_Read_Buffer(unsigned char SlaveAddress, unsigned char REG_Address, unsigned char *Buffer, unsigned char len)
{   	

	if(!I2C_Start())
		return FALSE;
    I2C_SendByte(SlaveAddress);  
    if(!I2C_WaitAck())
	{
		I2C_Stop();
		return FALSE;
	}
    I2C_SendByte((u8) REG_Address);//���õ���ʼ��ַ      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(SlaveAddress+1);
    if(!I2C_WaitAck())
	{
		I2C_Stop();
		return FALSE;
	}
	while(len)
	{
		*Buffer = I2C_ReadByte();
		if (len == 1)
			 I2C_NoAck();
		else
			 I2C_Ack();
		len --;
		Buffer ++;
	}
    I2C_Stop();
	return TRUE;
}


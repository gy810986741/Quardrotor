#ifndef _ANALOG_IIC_H
#define _ANALOG_IIC_H 
#include "sys.h"


/*模拟IIC端口输出输入定义*/
#define SCL_H         GPIOB->BSRR = GPIO_Pin_10		
#define SCL_L         GPIOB->BRR  = GPIO_Pin_10 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_11
#define SDA_L         GPIOB->BRR  = GPIO_Pin_11

#define SCL_read      GPIOB->IDR  & GPIO_Pin_10
#define SDA_read      GPIOB->IDR  & GPIO_Pin_11

void I2C_GPIO_Config(void);
static bool I2C_Start(void);
static void I2C_Stop(void);
static void I2C_Ack(void);
static void I2C_NoAck(void);
static bool I2C_WaitAck(void); 	 //返回为:=1有ACK,=0无ACK
static unsigned char I2C_ReadByte(void);  //数据从高位到低位//
static void I2C_SendByte(u8 SendByte);    //数据从高位到低位//
unsigned char Single_Read_Byte(unsigned char SlaveAddress,unsigned char REG_Address);
bool Single_Write_Byte(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);
bool Single_Write_Buffer(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char *Buffer,unsigned char len);
bool Single_Read_Buffer(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char *Buffer,unsigned char len);


#endif /* _ANALOG_IIC_H */


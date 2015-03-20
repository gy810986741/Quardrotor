#ifndef __SYS_H
#define __SYS_H	
#include "stm32f10x.h"

#define SYSTEM_SUPPORT_UCOS		0		//定义系统文件夹是否支持UCOS
																	    
	 
//位带操作,实现51类似的GPIO控制功能
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入


#define NVIC_UART_P		2	//抢占优先级
#define NVIC_UART_S		1	//响应优先级
#define NVIC_TIM3_P		1	//抢占优先级
#define NVIC_TIM3_S		1	//响应优先级
#define NVIC_TIM4_P		0	//抢占优先级
#define NVIC_TIM4_S		1	//响应优先级

#define DATA_TRANSFER_USE_USART
//#define DATA_TRANSFER_USE_SPI_NRF

#define CONTROL_USE_RC
//#define CONTROL_USE_DATATRANSFER


#define RC_FUN_MIN	1200
#define RC_FUN_MAX	1800
#define RC_MIN	900
#define RC_MAX	2100


typedef struct{
				float rol;
				float pit;
				float yaw;}T_float_angle;
typedef struct{
				float X;
				float Y;
				float Z;}T_float_xyz;
typedef struct{
				int16_t X;
				int16_t Y;
				int16_t Z;}T_int16_xyz;
typedef struct int16_rcget{
				int16_t ROLL;
				int16_t PITCH;
				int16_t THROTTLE;
				int16_t YAW;
				int16_t AUX1;
				int16_t AUX2;
				int16_t AUX3;
				int16_t AUX4;
				int16_t AUX5;
				int16_t AUX6;}T_RC_Data;

typedef struct PID{
				float P,pout,I,iout,D,dout,IMAX,OUT;}PID;
typedef struct{
				u8	ARMED;}T_RC_Control;

				
				
extern PID PID_ROL,PID_PIT,PID_YAW,PID_ALT,PID_POS;
				extern PID PID_PID_1,PID_PID_2,PID_PID_3,PID_PID_4,PID_PID_5,PID_PID_6,PID_PID_7,PID_PID_8,PID_PID_9,PID_PID_10,PID_PID_11,PID_PID_12;
extern vs16 Moto_PWM_1,Moto_PWM_2,Moto_PWM_3,Moto_PWM_4,Moto_PWM_5,Moto_PWM_6,Moto_PWM_7,Moto_PWM_8;
				
				
				
extern unsigned int Bat_Votage;				

extern u16 Rc_Pwm_In[4];				
				
extern u8 FLAG_ATT;
extern T_int16_xyz 		Acc,Gyr;	//两次综合后的传感器数据
extern T_float_angle 	Att_Angle;	//ATT函数计算出的姿态角
extern T_int16_xyz		GYRO_OFFSET,ACC_OFFSET;			//零漂
extern vs32				Alt;
				
extern T_RC_Data 		Rc_D;		//遥控通道数据
extern T_RC_Control		Rc_C;		//遥控功能数据				

				
extern u8 TxBuffer[256];
extern u8 count;

void NVIC_Configuration(void);







#endif

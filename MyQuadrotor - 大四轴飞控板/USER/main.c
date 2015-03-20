#include "sys.h"
#include "led.h"
#include "Usart.h"
#include "Timer.h"
#include "motors.h"
#include "SysTick.h"
#include "Analog_IIC.h"
#include "MPU6050.h"
#include "AHRS_IMU.h"
#include "Data_Transfer.h"
#include "eeprom.h"
#include "control.h"
#include "Rc.h"
#include "ADC.h"
#include "TIM_IN.h"
#include "HMC5883L.h"
#include "nrf24l01.h"
#include "MS5611.h"

unsigned int Bat_Votage=0;

u8 FLAG_ATT=0;
T_int16_xyz 		Acc,Gyr;	//两次综合后的传感器数据
T_int16_xyz			Acc_AVG;
T_float_angle 		Att_Angle;	//ATT函数计算出的姿态角
vs32				Alt;

T_RC_Data 			Rc_D;		//遥控通道数据
T_RC_Control		Rc_C;		//遥控功能数据

void SYS_INIT(void)
{
	SystemInit();
	SysTick_Init();	
	LED_GPIO_Config();
	Delay_ms(50);
	GPIO_ResetBits(LED1_GPIO, LED1_PIN);
//	ADC_GPIO_Config();
	motorsInit();
	I2C_GPIO_Config();

	MPU6050_Init();
//	MS5611_Init();
//	FLASH_Unlock();
//	EE_INIT();
//	EE_READ_ACC_OFFSET();
//	EE_READ_GYRO_OFFSET();
//	EE_READ_PID();	
	Tim_Pwm_In_Init();
	
//	USART1_Init(115200);
//	Uart1_Put_String("CHECK NRF24L01!\r\n");
//	
//	Nrf24l01_Init(MODEL_TX2,40);
//	
//	if(Nrf24l01_Check())		
//		Uart1_Put_String("NRF24L01 IS OK !\r\n");
// 	else 									
//		Uart1_Put_String("NRF24L01 IS NOT OK !\r\n");	
	
	Tim3_Init(500);	 //定时器中断最后开

}

int main(void)
{
	static u8 att_cnt=0;
	static u8 rc_cnt=0;
	static T_int16_xyz mpu6050_dataacc1,mpu6050_dataacc2,mpu6050_datagyr1,mpu6050_datagyr2;
	static u8 senser_cnt=0,status_cnt=0,dt_rc_cnt=0,dt_moto_cnt=0,dt_vot_cnt=0;

	SYS_INIT();

	while (1)
	{
		if(FLAG_ATT)
		{
			FLAG_ATT = 0;                                                                                                                                      
			att_cnt++;
			rc_cnt++;

			if(rc_cnt==20)
			{
				rc_cnt = 0;
				#ifdef CONTROL_USE_RC
				Rc_GetValue(&Rc_D);
				#endif
				Rc_Fun(&Rc_D,&Rc_C);
			}
			if(att_cnt==1)
				MPU6050_Dataanl(&mpu6050_dataacc1,&mpu6050_datagyr1);
			else
			{
				att_cnt = 0;
				MPU6050_Dataanl(&mpu6050_dataacc2,&mpu6050_datagyr2);
				Acc.X = (mpu6050_dataacc1.X+mpu6050_dataacc2.X)/2;
				Acc.Y = (mpu6050_dataacc1.Y+mpu6050_dataacc2.Y)/2;
				Acc.Z = (mpu6050_dataacc1.Z+mpu6050_dataacc2.Z)/2;
				Gyr.X = (mpu6050_datagyr1.X+mpu6050_datagyr2.X)/2;
				Gyr.Y = (mpu6050_datagyr1.Y+mpu6050_datagyr2.Y)/2;
				Gyr.Z = (mpu6050_datagyr1.Z+mpu6050_datagyr2.Z)/2;
				Prepare_Data(&Acc,&Acc_AVG);
				IMUupdate(&Gyr,&Acc_AVG,&Att_Angle);
				Control(&Att_Angle,&Gyr,&Rc_D,Rc_C.ARMED);
//				if(Rc_C.ARMED)
//					LED1_ONOFF();
//				else
//					LED1_OFF;
				senser_cnt++;
				status_cnt++;
				dt_rc_cnt++;
				dt_moto_cnt++;
				if(senser_cnt == 5)
				{
					senser_cnt = 0;
					Send_Senser = 1;
				}
				if(status_cnt == 5)
				{
					status_cnt = 0;
					Send_Status = 1;
				}
				if(dt_rc_cnt == 10)
				{
					dt_rc_cnt = 0;
					Send_RCData = 1;
				} 
				if(dt_moto_cnt == 10)
				{
					dt_moto_cnt = 0;
					Send_MotoPwm = 1;
				}
				if(dt_vot_cnt==10)
				{
					dt_vot_cnt = 0;
					Send_Voltage = 1;
				}
				
			}
		}
				
	}
}

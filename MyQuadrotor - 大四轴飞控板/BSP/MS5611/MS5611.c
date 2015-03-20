#include "MS5611.h"
#include "Analog_IIC.h"
#include "SysTick.h"
#include <math.h>

uint32_t 	ms5611_ut;  // static result of temperature measurement
uint32_t 	ms5611_up;  // static result of pressure measurement
uint16_t 	ms5611_prom[7];  // on-chip ROM
int32_t 	Pressure,Temperature;
int32_t attitude;

uint8_t t_rxbuf[3],p_rxbuf[3];

void MS5611_loop(void)
{
	static u8 MS5611_sta = 1 , ms5611_iic_ok = 0 , MS5611_cnt = 1;
	
	if(MS5611_cnt)	MS5611_cnt--;
	
	if(MS5611_cnt==0)
	{
		switch(MS5611_sta)
		{
			case 0:
					ms5611_iic_ok = MS5611_RESET();
					if(ms5611_iic_ok)
					{
						MS5611_sta = 1;
						MS5611_cnt = 20;
					}
					break;
			case 1:
					ms5611_iic_ok = MS5611_START_T();
					if(ms5611_iic_ok)
					{
						MS5611_sta = 2;
						MS5611_cnt = 10;
					}
					break;
			case 2:
					ms5611_iic_ok = MS5611_READ_ADC_T();
					if(ms5611_iic_ok)
					{
						MS5611_sta = 3;
						MS5611_cnt = 1;
					}
					break;
			case 3:
					ms5611_iic_ok = MS5611_START_P();
					if(ms5611_iic_ok)
					{
						MS5611_sta = 4;
						MS5611_cnt = 10;
					}
					break;
			case 4:
					ms5611_iic_ok = MS5611_READ_ADC_P();
					if(ms5611_iic_ok)
					{
						MS5611_sta = 5;
						MS5611_cnt = 1;
					}
					break;
			case 5:
					ms5611_iic_ok = MS5611_Calculate();
					if(ms5611_iic_ok)
					{
						MS5611_sta = 1;
						MS5611_cnt = 1;
					}
					break;
		}
	}
}



u8 MS5611_RESET(void)//如数据读不出来，或程序跑不下去，请注释掉此处while(I2C_WaitAck());
{	
	return Single_Write_Byte(MS561101BA_SlaveAddress, MS561101BA_RST, 0);
}

//从PROM读取出厂校准数据
u8 MS5611_PROM_READ(void)
{
	u8 rxbuf[2] = { 0, 0 };
	u8 i;
	
	for(i = 0; i < 8; i ++)
	{
		Single_Read_Buffer(MS561101BA_SlaveAddress, MS561101BA_PROM_RD + i * 2, rxbuf, 2);
		ms5611_prom[i] = (u16)rxbuf[0] << 8 | rxbuf[1];
	}//打印PROM读取出厂校准数据，检测数据传输是否正常
	return 1;
}

u8 MS5611_READ_ADC_T(void)
{
    return Single_Read_Buffer(MS561101BA_SlaveAddress, MS561101BA_ADC_RD, t_rxbuf, 3); // read ADC
}
u8 MS5611_READ_ADC_P(void)
{
    return Single_Read_Buffer(MS561101BA_SlaveAddress, MS561101BA_ADC_RD, p_rxbuf, 3); // read ADC
}
u8 MS5611_START_T(void)
{
    return Single_Write_Byte(MS561101BA_SlaveAddress, MS561101BA_ADC_CONV + MS561101BA_ADC_D2 + MS561101BA_ADC_4096, 0); // D2 (temperature) conversion start!
}
u8 MS5611_START_P(void)
{
    return Single_Write_Byte(MS561101BA_SlaveAddress, MS561101BA_ADC_CONV + MS561101BA_ADC_D2 + MS561101BA_ADC_4096, 0); // D1 (pressure) conversion start!
}


u8 MS5611_Calculate(void)
{	
    int32_t T2,Aux,OFF2 = 0, SENS2 = 0;
//	int32_t Pressure;
//	int32_t Temperature,
		
	ms5611_ut = (t_rxbuf[0] << 16) | (t_rxbuf[1] << 8) | t_rxbuf[2];
	ms5611_up = (p_rxbuf[0] << 16) | (p_rxbuf[1] << 8) | p_rxbuf[2];
	//DT=D2-TREF(C5)=D2-PROM[5]*2^8	
	//实际温度和参考温度之间的差值
    int32_t dT = ms5611_ut - ((uint32_t)ms5611_prom[5] << 8);
	//OFF=PROM[2]*2^16+(PROM[4]*DT)/2^7
	//温漂
    int64_t OFF = ((uint32_t)ms5611_prom[2] << 16) + (((int64_t)dT * ms5611_prom[4]) >> 7);
	//SENS=PROM[1]*2^15+(PROM[3]*DT)/2^8
	//灵敏度
    int64_t SENS = ((uint32_t)ms5611_prom[1] << 15) + (((int64_t)dT * ms5611_prom[3]) >> 8);
	
    Temperature = 2000 + (((int64_t)dT * ms5611_prom[6]) >> 23);
	
	OFF=(u32)(ms5611_prom[2]<<16)+((u32)ms5611_prom[4]*dT)/128.0;
	SENS=(u32)(ms5611_prom[1]<<15)+((u32)ms5611_prom[3]*dT)/256.0;
	//温度补偿
	if(Temperature < 2000)// second order temperature compensation when under 20 degrees C
	{
		T2 = (dT*dT) / 0x80000000;
		Aux = (Temperature-2000)*(Temperature-2000);
		OFF2 = 2.5*Aux;
		SENS2 = 1.25*Aux;
		if(Temperature < -1500)
		{
			Aux = (Temperature+1500)*(Temperature+1500);
			OFF2 = OFF2 + 7*Aux;
			SENS2 = SENS + 5.5*Aux;
		}
	}
	else  //(Temperature > 2000)
	{
		T2 = 0;
		OFF2 = 0;
		SENS2 = 0;
	}
	
	Temperature = Temperature - T2;
	OFF -= OFF2;
	SENS -= SENS2;

	Pressure=(ms5611_up*SENS/2097152.0-OFF)/32768.0;
	attitude=(1.0f - pow(Pressure/101325.0f,0.190295f))*4433000.0f;
	return 1;
}

void MS5611_Init(void)
{
	MS5611_RESET();
	Delay_ms(10);
	MS5611_PROM_READ();
	Delay_ms(10);
} 

//***************数据转换******************* 
void Exchange_Number(void) 
{ 
		//串口读数转换值
//	u32 ex_Temperature; //串口读数转换值

//	MS561101BA_GetTemperature(MS561101BA_D2_OSR_4096);
//	MS561101BA_GetPressure(MS561101BA_D1_OSR_4096);
//	
//	ex_Pressure=8000*(1+Temperature/273.0)/Pressure; 
} 

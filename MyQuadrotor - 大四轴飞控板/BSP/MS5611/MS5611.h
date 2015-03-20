#include "sys.h"


#define	MS561101BA_SlaveAddress 0xee  //定义器件在IIC总线中的从地址
#define	MS561101BA_RST			0x1E 
#define	MS561101BA_ADC_READ		0x00 // ADC read command
#define MS561101BA_ADC_CONV		0x40 // ADC conversion command
#define MS561101BA_ADC_D1		0x00 // ADC D1 conversion
#define MS561101BA_ADC_D2		0x10 // ADC D2 conversion
#define MS561101BA_ADC_256		0x00 // ADC OSR=256
#define MS561101BA_ADC_512		0x02 // ADC OSR=512
#define MS561101BA_ADC_1024		0x04 // ADC OSR=1024
#define MS561101BA_ADC_2048		0x06 // ADC OSR=2048
#define MS561101BA_ADC_4096		0x08 // ADC OSR=4096

#define	MS561101BA_ADC_RD 		0x00 
#define	MS561101BA_PROM_RD 		0xA0 
//#define  MS561101BA_PROM_CRC 0xAE 

//////////////////////////////////////////////////////////////////////////////////// 

u8 MS5611_RESET(void);
u8 MS5611_PROM_READ(void);
u8 MS5611_READ_ADC_T(void);
u8 MS5611_READ_ADC_P(void);
u8 MS5611_START_T(void);
u8 MS5611_START_P(void);
u8 MS5611_Calculate(void);
void MS5611_Init(void);
void MS5611_loop(void);


//void MS561101BA_RESET(void); 
//void MS561101BA_PROM_READ(void); 
//u32 MS561101BA_DO_CONVERSION(u8 command); 
//void MS561101BA_GetTemperature(u8 OSR_Temp);
//void MS561101BA_GetPressure(u8 OSR_Pres);
//void MS561101BA_Init(void); 
//void SeriPushSend(u8 send_data); 

//void Exchange_Number(void); 


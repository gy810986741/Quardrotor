#include "HMC5883L.h"
#include "Analog_IIC.h"

float HMC5883_lastx,HMC5883_lasty,HMC5883_lastz;

int16_t  HMC5883_FIFO[3][11]; //磁力计滤波
void HMC58X3_getRaw(int16_t *x,int16_t *y,int16_t *z);

/******************************************************************************/
unsigned char HMC5883_IS_newdata(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==Bit_SET)
	{
		return 1;
	}
	else 
		return 0;
}
/****************************************************************************/
void HMC58X3_FIFO_init(void)
{
	int16_t temp[3];
	unsigned char i;
	for(i=0;i<50;i++)
	{
		HMC58X3_getRaw(&temp[0],&temp[1],&temp[2]);
	//  delay_us(200);  //延时再读取数据
	//  LED_Change(); //LED闪烁
	}
}

/*****************************************************************************/
void  HMC58X3_newValues(int16_t x,int16_t y,int16_t z)
{
	unsigned char i ;
	int32_t sum=0;

	for(i=1;i<10;i++){
		HMC5883_FIFO[0][i-1]=HMC5883_FIFO[0][i];
		HMC5883_FIFO[1][i-1]=HMC5883_FIFO[1][i];
		HMC5883_FIFO[2][i-1]=HMC5883_FIFO[2][i];
	}

	HMC5883_FIFO[0][9]=x;
	HMC5883_FIFO[1][9]=y;
	HMC5883_FIFO[2][9]=z;

	sum=0;
	for(i=0;i<10;i++)
	{	//取数组内的值进行求和再取平均
   		sum+=HMC5883_FIFO[0][i];
	}
	HMC5883_FIFO[0][10]=sum/10;	//将平均值更新

	sum=0;
	for(i=0;i<10;i++)
	{
   		sum+=HMC5883_FIFO[1][i];
	}
	HMC5883_FIFO[1][10]=sum/10;

	sum=0;
	for(i=0;i<10;i++)
	{
   		sum+=HMC5883_FIFO[2][i];
	}
	HMC5883_FIFO[2][10]=sum/10;
} //HMC58X3_newValues

/*****************************************************************************/
void HMC58X3_writeReg(unsigned char reg, unsigned char val) 
{
	Single_Write_Byte(HMC58X3_ADDR,reg,val);
}

/******************************************************************************/
void HMC58X3_getRaw(int16_t *x,int16_t *y,int16_t *z)
{
	unsigned char vbuff[6];
	vbuff[0]=vbuff[1]=vbuff[2]=vbuff[3]=vbuff[4]=vbuff[5]=0;
	Single_Read_Buffer(HMC58X3_ADDR,HMC58X3_RA_XM,vbuff,6);
	HMC58X3_newValues(((int16_t)vbuff[0] << 8) | vbuff[1],((int16_t)vbuff[4] << 8) | vbuff[5],((int16_t)vbuff[2] << 8) | vbuff[3]);
	*x = HMC5883_FIFO[0][10];
	*y = HMC5883_FIFO[1][10];
	*z = HMC5883_FIFO[2][10];
}

/*****************************************************************************/
void HMC58X3_getlastValues(int16_t *x,int16_t *y,int16_t *z) 
{
	*x = HMC5883_FIFO[0][10];
	*y = HMC5883_FIFO[1][10]; 
	*z = HMC5883_FIFO[2][10]; 
}

/******************************************************************************/
void HMC58X3_mgetValues(float *arry) 
{
	int16_t xr,yr,zr;
	HMC58X3_getRaw(&xr, &yr, &zr);
	arry[0]= HMC5883_lastx=(float)(xr);
	arry[1]= HMC5883_lasty=(float)(yr);
	arry[2]= HMC5883_lastz=(float)(zr);
}

/******************************************************************************/
void HMC58X3_setGain(unsigned char gain) 
{ 
	// 0-7, 1 default
	if (gain > 7) return;
	HMC58X3_writeReg(HMC58X3_RA_CONFB, gain << 5);
}

/*********************************************************************************/
void HMC58X3_setMode(unsigned char mode) 
{
	if (mode > 2)	return;
	HMC58X3_writeReg(HMC58X3_RA_MODE, mode);
//	delay_us(100);
}

/********************************************************************************/
void HMC58X3_init(u8 setmode)
{
	if (setmode) 	HMC58X3_setMode(0);
	HMC58X3_writeReg(HMC58X3_RA_CONFA, 0x70); // 8 samples averaged, 75Hz frequency, no artificial bias.
	HMC58X3_writeReg(HMC58X3_RA_CONFB, 0xA0);
	HMC58X3_writeReg(HMC58X3_RA_MODE, 0x00);
}

/*******************************************************************************/
void HMC58X3_setDOR(unsigned char DOR) 
{
	if (DOR>6) return;
	HMC58X3_writeReg(HMC58X3_RA_CONFA,DOR<<2);
}

/*******************************************************************************/
void HMC58X3_getID(char id[3]) 
{
	id[0]=Single_Read_Byte(HMC58X3_ADDR,HMC58X3_RA_IDA);  
	id[1]=Single_Read_Byte(HMC58X3_ADDR,HMC58X3_RA_IDB);
	id[2]=Single_Read_Byte(HMC58X3_ADDR,HMC58X3_RA_IDC);
}   // getID().

void HMC5883L_SetUp(void)
{ 
	HMC58X3_init(0); // Don't set mode yet, we'll do that later on.
	HMC58X3_setMode(0);
	HMC58X3_setDOR(6);  //75hz 更新率
	HMC58X3_FIFO_init();
}

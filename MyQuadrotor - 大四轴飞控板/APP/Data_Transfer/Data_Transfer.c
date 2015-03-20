#include "Data_Transfer.h"
#include "Usart.h"
#include "eeprom.h"
#include "MPU6050.h"
#include "nrf24l01.h"

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))



u8 Data_Check,Send_Status,Send_Senser,Send_RCData,Send_Offset,Send_PID1,Send_PID2,Send_PID3,Send_PID4,Send_PID5,Send_PID6,Send_MotoPwm,Send_Voltage;
u8 data_to_send[50];

void Nrf_Check_Event(void)
{
	u8 sta = NRF_Read_Reg(NRF_READ_REG + NRFRegSTATUS);
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	if(sta & (1<<RX_DR))
	{
		u8 rx_len = NRF_Read_Reg(R_RX_PL_WID);
		if(rx_len<33)
		{
			NRF_Read_Buf(RD_RX_PLOAD,NRF24L01_RXDATA,rx_len);// read receive payload from RX_FIFO buffer
			//LED1_ONOFF();
			Data_Receive_Anl(NRF24L01_RXDATA,rx_len);
		}
		else 
		{
			NRF_Write_Reg(FLUSH_RX,0xff);//��ջ�����
		}
	}
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	if(sta & (1<<TX_DS))
	{
		//LED1_ONOFF();
	}
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	if(sta & (1<<MAX_RT))
	{
		if(sta & 0x01)	//TX FIFO FULL
		{
			NRF_Write_Reg(FLUSH_TX,0xff);
		}
	}
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	NRF_Write_Reg(NRF_WRITE_REG + NRFRegSTATUS, sta);
}

u8 Nrf_Get_FIFOSta(void)
{
	return NRF_Read_Reg(NRF_READ_REG + FIFO_STATUS);
}

void Data_Receive_Anl(u8 *data_buf,u8 num)
{
	vs16 rc_value_temp;
	u8 sum = 0;
	u8 i=0;
	
	for(i=0;i<(num-1);i++)
		sum += *(data_buf+i);
	if(!(sum==*(data_buf+num-1)))		
	{	
		return;		//�ж�sum
	}
	if(!(*(data_buf)==0xAA && *(data_buf+1)==0xAF))	
	{	
		return;
	}		//�ж�֡ͷ
/////////////////////////////////////////////////////////////////////////////////////
	if(*(data_buf+2)==0X01)
	{
		if(*(data_buf+4)==0X01)
			MPU6050_CalOff_Acc();
		if(*(data_buf+4)==0X02)
			MPU6050_CalOff_Gyr();
		if(*(data_buf+4)==0X03)
		{
			MPU6050_CalOff_Acc();
			MPU6050_CalOff_Gyr();
		}
//		if(*(data_buf+4)==0X04)
//			Cal_Compass();
//		if(*(data_buf+4)==0X05)
//			MS5611_CalOffset();
	}
	if(*(data_buf+2)==0X02)
	{
		if(*(data_buf+4)==0X01)
		{
			Send_PID1 = 1;
			Send_PID2 = 1;
			Send_PID3 = 1;
			Send_PID4 = 1;
			Send_PID5 = 1;
			Send_PID6 = 1;
		}
		if(*(data_buf+4)==0X02)
			Send_Offset = 1;
	}
	
#ifndef CONTROL_USE_RC
	if(*(data_buf+2)==0X03)
	{
		Rc_D.THROTTLE = (vs16)(*(data_buf+4)<<8)|*(data_buf+5);
		Rc_D.YAW = (vs16)(*(data_buf+6)<<8)|*(data_buf+7);
		Rc_D.ROLL = (vs16)(*(data_buf+8)<<8)|*(data_buf+9);
		Rc_D.PITCH = (vs16)(*(data_buf+10)<<8)|*(data_buf+11);
		Rc_D.AUX1 = (vs16)(*(data_buf+12)<<8)|*(data_buf+13);
		Rc_D.AUX2 = (vs16)(*(data_buf+14)<<8)|*(data_buf+15);
		Rc_D.AUX3 = (vs16)(*(data_buf+16)<<8)|*(data_buf+17);
		Rc_D.AUX4 = (vs16)(*(data_buf+18)<<8)|*(data_buf+19);
		Rc_D.AUX5 = (vs16)(*(data_buf+20)<<8)|*(data_buf+21);
		Rc_D.AUX6 = (vs16)(*(data_buf+21)<<8)|*(data_buf+22);
//		Rc_Fun(&Rc_D,&Rc_C);
	}
#endif
	if(*(data_buf+2)==0X10)								//PID1
	{
		PID_ROL.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
		PID_ROL.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/1000;
		PID_ROL.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/1000;//֮ǰ��100��������С10��
		PID_PIT.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
		PID_PIT.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/1000;
		PID_PIT.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/1000;//֮ǰ��100��������С10��
		PID_YAW.P = (float)((vs16)(*(data_buf+16)<<8)|*(data_buf+17))/100;
		PID_YAW.I = (float)((vs16)(*(data_buf+18)<<8)|*(data_buf+19))/1000;
		PID_YAW.D = (float)((vs16)(*(data_buf+20)<<8)|*(data_buf+21))/1000;
		Data_Send_Check(sum);
		EE_SAVE_PID();
	}
	if(*(data_buf+2)==0X11)								//PID2
	{
		PID_ALT.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
		PID_ALT.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/100;
		PID_ALT.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
		PID_POS.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
		PID_POS.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/100;
		PID_POS.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/100;
		PID_PID_1.P = (float)((vs16)(*(data_buf+16)<<8)|*(data_buf+17))/100;
		PID_PID_1.I = (float)((vs16)(*(data_buf+18)<<8)|*(data_buf+19))/100;
		PID_PID_1.D = (float)((vs16)(*(data_buf+20)<<8)|*(data_buf+21))/100;
		Data_Send_Check(sum);
	}
	if(*(data_buf+2)==0X12)								//PID3
	{
		PID_PID_2.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
		PID_PID_2.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/100;
		PID_PID_2.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
		PID_PID_3.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
		PID_PID_3.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/100;
		PID_PID_3.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/100;
		PID_PID_4.P = (float)((vs16)(*(data_buf+16)<<8)|*(data_buf+17))/100;
		PID_PID_4.I = (float)((vs16)(*(data_buf+18)<<8)|*(data_buf+19))/100;
		PID_PID_4.D = (float)((vs16)(*(data_buf+20)<<8)|*(data_buf+21))/100;
		Data_Send_Check(sum);
	}
	if(*(data_buf+2)==0X13)								//PID4
	{
		PID_PID_5.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
		PID_PID_5.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/100;
		PID_PID_5.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
		PID_PID_6.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
		PID_PID_6.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/100;
		PID_PID_6.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/100;
		PID_PID_7.P = (float)((vs16)(*(data_buf+16)<<8)|*(data_buf+17))/100;
		PID_PID_7.I = (float)((vs16)(*(data_buf+18)<<8)|*(data_buf+19))/100;
		PID_PID_7.D = (float)((vs16)(*(data_buf+20)<<8)|*(data_buf+21))/100;
		Data_Send_Check(sum);
	}
	if(*(data_buf+2)==0X14)								//PID5
	{
		PID_PID_8.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
		PID_PID_8.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/100;
		PID_PID_8.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
		PID_PID_9.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
		PID_PID_9.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/100;
		PID_PID_9.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/100;
		PID_PID_10.P = (float)((vs16)(*(data_buf+16)<<8)|*(data_buf+17))/100;
		PID_PID_10.I = (float)((vs16)(*(data_buf+18)<<8)|*(data_buf+19))/100;
		PID_PID_10.D = (float)((vs16)(*(data_buf+20)<<8)|*(data_buf+21))/100;
		Data_Send_Check(sum);
	}
	if(*(data_buf+2)==0X15)								//PID6
	{
		PID_PID_11.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
		PID_PID_11.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/100;
		PID_PID_11.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
		PID_PID_12.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
		PID_PID_12.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/100;
		PID_PID_12.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/100;
		Data_Send_Check(sum);
		EE_SAVE_PID();
	}
	if(*(data_buf+2)==0X16)								//OFFSET
	{
//			AngleOffset_Rol = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/1000;
//			AngleOffset_Pit = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/1000;
	}
}



void Data_Send_Status(void)
{
	u8 _cnt=0;
	u8 sum = 0;
	u8 i=0;
	vs16 _temp;
	vs32 _temp2 = Alt;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x01;
	data_to_send[_cnt++]=0;
	
	_temp = (int)(Att_Angle.rol*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = (int)(Att_Angle.pit*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = (int)(Att_Angle.yaw*100);
	//_temp = (int)(Mag_Heading*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	
	data_to_send[_cnt++]=BYTE3(_temp2);
	data_to_send[_cnt++]=BYTE2(_temp2);
	data_to_send[_cnt++]=BYTE1(_temp2);
	data_to_send[_cnt++]=BYTE0(_temp2);

//	if(Rc_C.ARMED==0)			data_to_send[_cnt++]=0xA0;	//����
//	else if(Rc_C.ARMED==1)		data_to_send[_cnt++]=0xA1;
	
	data_to_send[3] = _cnt - 4;
	
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	data_to_send[_cnt++]=sum;
#ifdef DATA_TRANSFER_USE_USART
	Uart1_Put_Buf(data_to_send,_cnt);
#else
	NRF_TxPacket(data_to_send,_cnt);
#endif
}
void Data_Send_Senser(void)
{
	u8 _cnt=0;
	u8 sum = 0;
	u8 i=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x02;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=BYTE1(Acc.X);
	data_to_send[_cnt++]=BYTE0(Acc.X);
	data_to_send[_cnt++]=BYTE1(Acc.Y);
	data_to_send[_cnt++]=BYTE0(Acc.Y);
	data_to_send[_cnt++]=BYTE1(Acc.Z);
	data_to_send[_cnt++]=BYTE0(Acc.Z);
	data_to_send[_cnt++]=BYTE1(Gyr.X);
	data_to_send[_cnt++]=BYTE0(Gyr.X);
	data_to_send[_cnt++]=BYTE1(Gyr.Y);
	data_to_send[_cnt++]=BYTE0(Gyr.Y);
	data_to_send[_cnt++]=BYTE1(Gyr.Z);
	data_to_send[_cnt++]=BYTE0(Gyr.Z);
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	
	data_to_send[3] = _cnt-4;
	
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	data_to_send[_cnt++] = sum;
	
#ifdef DATA_TRANSFER_USE_USART
	Uart1_Put_Buf(data_to_send,_cnt);
#else
	NRF_TxPacket(data_to_send,_cnt);
#endif
}

void Data_Send_RCData(void)
{
	u8 _cnt=0;
	u8 sum = 0;
	u8 i=0;
	
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x03;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=BYTE1(Rc_D.THROTTLE);
	data_to_send[_cnt++]=BYTE0(Rc_D.THROTTLE);
	data_to_send[_cnt++]=BYTE1(Rc_D.YAW);
	data_to_send[_cnt++]=BYTE0(Rc_D.YAW);
	data_to_send[_cnt++]=BYTE1(Rc_D.ROLL);
	data_to_send[_cnt++]=BYTE0(Rc_D.ROLL);
	data_to_send[_cnt++]=BYTE1(Rc_D.PITCH);
	data_to_send[_cnt++]=BYTE0(Rc_D.PITCH);
	data_to_send[_cnt++]=BYTE1(Rc_D.AUX1);
	data_to_send[_cnt++]=BYTE0(Rc_D.AUX1);
	data_to_send[_cnt++]=BYTE1(Rc_D.AUX2);
	data_to_send[_cnt++]=BYTE0(Rc_D.AUX2);
	data_to_send[_cnt++]=BYTE1(Rc_D.AUX3);
	data_to_send[_cnt++]=BYTE0(Rc_D.AUX3);
	data_to_send[_cnt++]=BYTE1(Rc_D.AUX4);
	data_to_send[_cnt++]=BYTE0(Rc_D.AUX4);
	data_to_send[_cnt++]=BYTE1(Rc_D.AUX5);
	data_to_send[_cnt++]=BYTE0(Rc_D.AUX5);
	data_to_send[_cnt++]=BYTE1(Rc_D.AUX6);
	data_to_send[_cnt++]=BYTE0(Rc_D.AUX6);
	
	data_to_send[3] = _cnt-4;
	
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
#ifdef DATA_TRANSFER_USE_USART
	Uart1_Put_Buf(data_to_send,_cnt);
#else
	NRF_TxPacket(data_to_send,_cnt);
#endif
}
void Data_Send_MotoPWM(void)
{
	u8 _cnt=0;
	u8 sum = 0;
	u8 i=0;
	
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x06;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=BYTE1(Moto_PWM_1-1000);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_1-1000);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_2-1000);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_2-1000);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_3-1000);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_3-1000);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_4-1000);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_4-1000);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_5);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_5);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_6);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_6);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_7);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_7);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_8);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_8);
	
	data_to_send[3] = _cnt-4;
	
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
#ifdef DATA_TRANSFER_USE_USART
	Uart1_Put_Buf(data_to_send,_cnt);
#else
	NRF_TxPacket(data_to_send,_cnt);
#endif
}
void Data_Send_OFFSET(void)
{
//	u8 _cnt=0;
//	data_to_send[_cnt++]=0xAA;
//	data_to_send[_cnt++]=0xAA;
//	data_to_send[_cnt++]=0x16;
//	data_to_send[_cnt++]=0;
//	vs16 _temp = AngleOffset_Rol*1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = AngleOffset_Pit*1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	
//	data_to_send[3] = _cnt-4;
//	
//	u8 sum = 0;
//	for(u8 i=0;i<_cnt;i++)
//		sum += data_to_send[i];
//	
//	data_to_send[_cnt++]=sum;
//	
//#ifdef DATA_TRANSFER_USE_USART
//	Uart1_Put_Buf(data_to_send,_cnt);
//#else
//	NRF_TxPacket(data_to_send,_cnt);
//#endif
}
void Data_Send_PID1(void)
{
	u8 _cnt=0;
	vs16 _temp;
	u8 sum = 0;
	u8 i=0;
	
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x10;
	data_to_send[_cnt++]=0;
	
	
	_temp = PID_ROL.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_ROL.I * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_ROL.D * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PIT.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PIT.I * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PIT.D * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_YAW.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_YAW.I * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_YAW.D * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	
	data_to_send[3] = _cnt-4;
	
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
#ifdef DATA_TRANSFER_USE_USART
	Uart1_Put_Buf(data_to_send,_cnt);
#else
	NRF_TxPacket(data_to_send,_cnt);
#endif
}

void Data_Send_Voltage(void)
{
	u8 _cnt=0;
	u8 i=0;
	u8 sum = 0;
	TxBuffer[_cnt++]=0xAA;
	TxBuffer[_cnt++]=0xAA;
	TxBuffer[_cnt++]=0x05;
	TxBuffer[_cnt++]=0;
	
	TxBuffer[_cnt++]=BYTE1(Bat_Votage);
	TxBuffer[_cnt++]=BYTE0(Bat_Votage);

	TxBuffer[_cnt++]=BYTE1(Bat_Votage);
	TxBuffer[_cnt++]=BYTE0(Bat_Votage);
	
	TxBuffer[_cnt++]=BYTE1(Bat_Votage);
	TxBuffer[_cnt++]=BYTE0(Bat_Votage);	
	
	TxBuffer[3] = _cnt-4;
	
	
	for(i=0;i<_cnt;i++)
		sum += TxBuffer[i];
	
	TxBuffer[_cnt++]=sum;

	Uart1_Put_Buf(TxBuffer,_cnt);
}


void Data_Exchange(void)
{
#ifdef DATA_TRANSFER_USE_SPI_NRF
	Nrf_Check_Event();
	u8 sta = Nrf_Get_FIFOSta();
	if((sta & (1<<4))==0)
		return;
#endif
	if(Send_Status)
	{
		Send_Status = 0;
		Data_Send_Status();
	}
	else if(Send_Senser)
	{
		Send_Senser = 0;
		Data_Send_Senser();
	}
	else if(Send_PID1)
	{
		Send_PID1 = 0;
		Data_Send_PID1();
	}
//	else if(Send_PID2)
//	{
//		Send_PID2 = 0;
//		Data_Send_PID2();
//	}
//	else if(Send_PID3)
//	{
//		Send_PID3 = 0;
//		Data_Send_PID3();
//	}
//	else if(Send_PID4)
//	{
//		Send_PID4 = 0;
//		Data_Send_PID4();
//	}
//	else if(Send_PID5)
//	{
//		Send_PID5 = 0;
//		Data_Send_PID5();
//	}
//	else if(Send_PID6)
//	{
//		Send_PID6 = 0;
//		Data_Send_PID6();
//	}
	else if(Send_RCData)
	{
		Send_RCData = 0;
		Data_Send_RCData();
	}
	else if(Send_Offset)
	{
		Send_Offset = 0;
		Data_Send_OFFSET();
	}
	else if(Send_MotoPwm)
	{
		Send_MotoPwm = 0;
		Data_Send_MotoPWM();
	}
	else if(Send_Voltage)
	{
		Send_Voltage = 0;
		Data_Send_Voltage();
	}
}

void Data_Send_Check(u16 check)
{
	u8 sum = 0;
	u8 i=0;
	data_to_send[0]=0xAA;
	data_to_send[1]=0xAA;
	data_to_send[2]=0xF0;
	data_to_send[3]=3;
	data_to_send[4]=0xBA;
	
	data_to_send[5]=BYTE1(check);
	data_to_send[6]=BYTE0(check);
	
	
	for(i=0;i<7;i++)
		sum += data_to_send[i];
	
	data_to_send[7]=sum;
#ifdef DATA_TRANSFER_USE_USART
	Uart1_Put_Buf(data_to_send,8);
#else
	NRF_TxPacket(data_to_send,8);
#endif
}


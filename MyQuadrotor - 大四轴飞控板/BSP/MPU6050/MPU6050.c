#include "MPU6050.h"
#include "Analog_IIC.h"
#include "eeprom.h"

static u8   MPU6050_Buffer[14]={0};//接收数据缓存区
T_int16_xyz		GYRO_OFFSET,ACC_OFFSET;			//零漂
static u8				GYRO_OFFSET_OK = 1;
static u8				ACC_OFFSET_OK = 1;

void MPU6050_CalOff_Acc(void)
{
	ACC_OFFSET_OK = 0;
}
void MPU6050_CalOff_Gyr(void)
{
	GYRO_OFFSET_OK = 0;
}

//初始化MPU6050，根据需要请参考pdf进行修改************************
void MPU6050_Init(void)
{
	Single_Write_Byte(MPU6050_Addr,MPU6050_RA_PWR_MGMT_1,0x00);   //sleep=0
	Single_Write_Byte(MPU6050_Addr,MPU6050_RA_SMPLRT_DIV,0x07);   //陀螺仪1khz
	Single_Write_Byte(MPU6050_Addr,MPU6050_RA_CONFIG,0x03);       //低通滤波器 44hz滤波
	Single_Write_Byte(MPU6050_Addr,MPU6050_RA_GYRO_CONFIG,0x11);  //陀螺仪 gyro 2000/s
	Single_Write_Byte(MPU6050_Addr,MPU6050_RA_ACCEL_CONFIG,0x0B); //加速度计//acc +-4g
	Single_Write_Byte(MPU6050_Addr,MPU6050_RA_I2C_MST_CTRL,13);   //IIC时钟速率400khz
	Single_Write_Byte(MPU6050_Addr,MPU6050_RA_USER_CTRL,0x00);    //mst_en=0
}
void MPU6050_Read(void)
{
	Single_Read_Buffer(MPU6050_Addr,MPU6050_RA_ACCEL_XOUT_H,MPU6050_Buffer,14);
}

#define 	MPU6050_MAX		32767
#define		MPU6050_MIN		-32768

void MPU6050_Dataanl(T_int16_xyz *data_tempacc,T_int16_xyz *data_tempgyr)
{
	vs32 acc_x,acc_y,acc_z,gyr_x,gyr_y,gyr_z;
	
	acc_x=((((int16_t)MPU6050_Buffer[0]) << 8) | MPU6050_Buffer[1]) - ACC_OFFSET.X;
	acc_y=((((int16_t)MPU6050_Buffer[2]) << 8) | MPU6050_Buffer[3]) - ACC_OFFSET.Y;
	acc_z=((((int16_t)MPU6050_Buffer[4]) << 8) | MPU6050_Buffer[5]) - ACC_OFFSET.Z;
	//????ADC
	gyr_x=((((int16_t)MPU6050_Buffer[8]) << 8) | MPU6050_Buffer[9]) - GYRO_OFFSET.X;
	gyr_y=((((int16_t)MPU6050_Buffer[10]) << 8) | MPU6050_Buffer[11]) - GYRO_OFFSET.Y;
	gyr_z=((((int16_t)MPU6050_Buffer[12]) << 8) | MPU6050_Buffer[13]) - GYRO_OFFSET.Z;
	
	acc_x>MPU6050_MAX ? MPU6050_MAX:acc_x;
	acc_x<MPU6050_MIN ? MPU6050_MIN:acc_x;
	acc_y>MPU6050_MAX ? MPU6050_MAX:acc_y;
	acc_y<MPU6050_MIN ? MPU6050_MIN:acc_y;
	acc_z>MPU6050_MAX ? MPU6050_MAX:acc_z;
	acc_z<MPU6050_MIN ? MPU6050_MIN:acc_z;
	gyr_x>MPU6050_MAX ? MPU6050_MAX:gyr_x;
	gyr_x<MPU6050_MIN ? MPU6050_MIN:gyr_x;
	gyr_y>MPU6050_MAX ? MPU6050_MAX:gyr_y;
	gyr_y<MPU6050_MIN ? MPU6050_MIN:gyr_y;
	gyr_z>MPU6050_MAX ? MPU6050_MAX:gyr_z;
	gyr_z<MPU6050_MIN ? MPU6050_MIN:gyr_z;
	
	data_tempacc->X = acc_x;
	data_tempacc->Y = acc_y;
	data_tempacc->Z = acc_z;
	data_tempgyr->X = gyr_x;
	data_tempgyr->Y = gyr_y;
	data_tempgyr->Z = gyr_z;
	
	if(!GYRO_OFFSET_OK)
	{
		static int32_t	tempgx=0,tempgy=0,tempgz=0;
		static uint8_t cnt_g=0;

		if(cnt_g==0)
		{
			GYRO_OFFSET.X=0;
			GYRO_OFFSET.Y=0;
			GYRO_OFFSET.Z=0;
			tempgx = 0;
			tempgy = 0;
			tempgz = 0;
			cnt_g = 1;
			return;
		}
		tempgx+= data_tempgyr->X;
		tempgy+= data_tempgyr->Y;
		tempgz+= data_tempgyr->Z;
		if(cnt_g==200)
		{
			GYRO_OFFSET.X=tempgx/cnt_g;
			GYRO_OFFSET.Y=tempgy/cnt_g;
			GYRO_OFFSET.Z=tempgz/cnt_g;
			cnt_g = 0;
			GYRO_OFFSET_OK = 1;
			EE_SAVE_GYRO_OFFSET();//保存数据
			return;
		}
		cnt_g++;
	}
	if(!ACC_OFFSET_OK)
	{
		static int32_t	tempax=0,tempay=0,tempaz=0;
		static uint8_t cnt_a=0;

		if(cnt_a == 0)
		{
			ACC_OFFSET.X = 0;
			ACC_OFFSET.Y = 0;
			ACC_OFFSET.Z = 0;
			tempax = 0;
			tempay = 0;
			tempaz = 0;
			cnt_a = 1;
			return;
		}
		tempax+= data_tempacc->X;
		tempay+= data_tempacc->Y;
		//tempaz+= MPU6050_ACC_LAST.Z;
		if(cnt_a==200)
		{
			ACC_OFFSET.X=tempax/cnt_a;
			ACC_OFFSET.Y=tempay/cnt_a;
			ACC_OFFSET.Z=tempaz/cnt_a;
			cnt_a = 0;
			ACC_OFFSET_OK = 1;
			EE_SAVE_ACC_OFFSET();//保存数据
			return;
		}
		cnt_a++;		
	}
}



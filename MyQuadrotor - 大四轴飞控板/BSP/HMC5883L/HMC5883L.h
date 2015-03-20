#ifndef __HMC5883L_H
#define __HMC5883L_H
#include "sys.h"

#define HMC58X3_ADDR 0x3C 
//#define HMC_POS_BIAS 1
//#define HMC_NEG_BIAS 2


#define HMC58X3_RA_CONFA 	0	//配置寄存器A
#define HMC58X3_RA_CONFB 	1	//配置寄存器B
#define HMC58X3_RA_MODE 		2	//模式寄存器		[1:0] 模式选择位，用于设定操作模式。00-连续测量  01-单次测量

#define HMC58X3_RA_XM 		3	//数据输出 X MSB	当进行完读操作后寄存器将被清空
#define HMC58X3_RA_XL 		4	//数据输出 X LSB
#define HMC58X3_RA_YM 		7	//数据输出 Y MSB
#define HMC58X3_RA_YL 		8	//数据输出 Y LSB
#define HMC58X3_RA_ZM 		5	//数据输出 Z MSB
#define HMC58X3_RA_ZL 		6	//数据输出 Z LSB

#define HMC58X3_RA_STATUS 	9	//状态寄存器		[0]-DRDY，当全部六个数据寄存器写入完成后该位置位，发生读数据操作后自动清除
								//					[1]-LOCK，当六个数据寄存器未被全部读取时该位置位
#define HMC58X3_RA_IDA 		10	//识别寄存器A		'H'
#define HMC58X3_RA_IDB 		11	//识别寄存器B		'4'
#define HMC58X3_RA_IDC 		12	//识别寄存器C		'3'

void HMC5883L_SetUp(void);		//初始化
void HMC58X3_getID(char id[3]);	//读芯片ID
void HMC58X3_getRaw(int16_t *x,int16_t *y,int16_t *z);	//读ADC
void HMC58X3_mgetValues(float *arry); //IMU 专用的读取磁力计值
void HMC58X3_getlastValues(int16_t *x,int16_t *y,int16_t *z);

#endif


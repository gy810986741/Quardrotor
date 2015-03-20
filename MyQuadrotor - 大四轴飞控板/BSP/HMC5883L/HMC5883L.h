#ifndef __HMC5883L_H
#define __HMC5883L_H
#include "sys.h"

#define HMC58X3_ADDR 0x3C 
//#define HMC_POS_BIAS 1
//#define HMC_NEG_BIAS 2


#define HMC58X3_RA_CONFA 	0	//���üĴ���A
#define HMC58X3_RA_CONFB 	1	//���üĴ���B
#define HMC58X3_RA_MODE 		2	//ģʽ�Ĵ���		[1:0] ģʽѡ��λ�������趨����ģʽ��00-��������  01-���β���

#define HMC58X3_RA_XM 		3	//������� X MSB	���������������Ĵ����������
#define HMC58X3_RA_XL 		4	//������� X LSB
#define HMC58X3_RA_YM 		7	//������� Y MSB
#define HMC58X3_RA_YL 		8	//������� Y LSB
#define HMC58X3_RA_ZM 		5	//������� Z MSB
#define HMC58X3_RA_ZL 		6	//������� Z LSB

#define HMC58X3_RA_STATUS 	9	//״̬�Ĵ���		[0]-DRDY����ȫ���������ݼĴ���д����ɺ��λ��λ�����������ݲ������Զ����
								//					[1]-LOCK�����������ݼĴ���δ��ȫ����ȡʱ��λ��λ
#define HMC58X3_RA_IDA 		10	//ʶ��Ĵ���A		'H'
#define HMC58X3_RA_IDB 		11	//ʶ��Ĵ���B		'4'
#define HMC58X3_RA_IDC 		12	//ʶ��Ĵ���C		'3'

void HMC5883L_SetUp(void);		//��ʼ��
void HMC58X3_getID(char id[3]);	//��оƬID
void HMC58X3_getRaw(int16_t *x,int16_t *y,int16_t *z);	//��ADC
void HMC58X3_mgetValues(float *arry); //IMU ר�õĶ�ȡ������ֵ
void HMC58X3_getlastValues(int16_t *x,int16_t *y,int16_t *z);

#endif


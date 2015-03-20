#ifndef _NRF24L01_H_
#define _NRF24L01_H_
#include "sys.h"
//***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define NRF_READ_REG    0x00  	//�����üĴ���,��5λΪ�Ĵ�����ַ
#define NRF_WRITE_REG   0x20 	//д���üĴ���,��5λΪ�Ĵ�����ַ
#define R_RX_PL_WID   	0x60	
#define RD_RX_PLOAD     0x61  	//��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0  	//д��������ָ��
#define FLUSH_TX        0xE1 	//������� FIFOָ��
#define FLUSH_RX        0xE2  	//������� FIFOָ��
#define REUSE_TX_PL     0xE3  	//�����ظ�װ������ָ��
#define NOP             0xFF  	//����
//*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG          0x00  	//���üĴ�����ַ;
								//	bit0	1����ģʽ,0����ģʽ;
								//	bit1	��ѡ��;
								//	bit2	CRCģʽ;
								//	bit3	CRCʹ��;
								//	bit4	�ж�MAX_RT(�ﵽ����ط������ж�)ʹ��;
								//	bit5	�ж�TX_DSʹ��;
								//	bit6	�ж�RX_DRʹ��
#define EN_AA           0x01  	//�Զ�Ӧ��������  	bit0~5,��Ӧͨ��0~5
#define EN_RXADDR       0x02  	//�����ŵ����� 			bit0~5,��Ӧͨ��0~5
#define SETUP_AW        0x03  	//�շ���ַ�������
#define SETUP_RETR      0x04  	//�Զ��ط���������
#define RF_CH           0x05  	//����Ƶ������  		bit6:0,����ͨ��Ƶ��;
#define RF_SETUP        0x06  	//RF�Ĵ���;
								//	bit0	�������Ŵ�������
								//	bit2:1	���书��;
								//	bit3	��������(0:1Mbps,1:2Mbps);
#define NRFRegSTATUS    0x07  	//״̬�Ĵ���;
								//	bit0	TX FIFO����־;
								//	bit3:1	��������ͨ����(���:6);
								//	bit4	�ﵽ�����ط�
								//	bit5	���ݷ�������ж�;bit6:���������ж�;

#define OBSERVE_TX      0x08  	//���ͼ��Ĵ���,bit7:4,���ݰ���ʧ������;bit3:0,�ط�������
#define CD              0x09  	//�ز����Ĵ���,bit0,�ز����;
#define RX_ADDR_P0      0x0A  	//����ͨ��0���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P1      0x0B  	//����ͨ��1���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P2      0x0C  	//����ͨ��2���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P3      0x0D  	//����ͨ��3���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P4      0x0E  	//����ͨ��4���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P5      0x0F  	//����ͨ��5���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define TX_ADDR         0x10  	//���͵�ַ(���ֽ���ǰ),ShockBurstTMģʽ��,RX_ADDR_P0��˵�ַ���
#define RX_PW_P0        0x11  	//��������ͨ��0��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P1        0x12  	//��������ͨ��1��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P2        0x13  	//��������ͨ��2��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P3        0x14  	//��������ͨ��3��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P4        0x15  	//��������ͨ��4��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P5        0x16  	//��������ͨ��5��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define FIFO_STATUS     0x17  	//FIFO״̬�Ĵ���;
								//	bit0	RX FIFO�Ĵ����ձ�־;
								//	bit1	RX FIFO����־;
								//	bit2,3	����
								//	bit4	TX FIFO�ձ�־;
								//	bit5	TX FIFO����־;
								//	bit6	1,ѭ��������һ���ݰ�.0,��ѭ��;
//**************************************************************************************
//*********************************************NRF24L01*************************************
#define RX_DR			6		//�жϱ�־
#define TX_DS			5
#define MAX_RT			4

#define MODEL_RX		1		//��ͨ����
#define MODEL_TX		2		//��ͨ����
#define MODEL_RX2		3		//����ģʽ2,����˫����
#define MODEL_TX2		4		//����ģʽ2,����˫����

#define RX_PLOAD_WIDTH  32  	
#define TX_PLOAD_WIDTH  32  	
#define TX_ADR_WIDTH    5 	 	
#define RX_ADR_WIDTH    5   

#define SPI_CE_H()   GPIO_SetBits(GPIOA, GPIO_Pin_4) 
#define SPI_CE_L()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)

#define SPI_CSN_H()  GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define SPI_CSN_L()  GPIO_ResetBits(GPIOB, GPIO_Pin_12)

extern 	u8 	NRF24L01_RXDATA[RX_PLOAD_WIDTH];		//nrf24l01���յ�������
extern 	u8 	NRF24L01_TXDATA[RX_PLOAD_WIDTH];		//nrf24l01��Ҫ���͵�����

void Nrf24l01_Init(u8 model, u8 ch);				//��ʼ��,model=1/2/3/4,chΪʵ�õ�ͨ����
void NRF_TxPacket(uint8_t * tx_buf, uint8_t len);			//�������ݰ�,����model 2/4
void NRF_TxPacket_AP(uint8_t * tx_buf, uint8_t len);	//�������ݰ�,����model 3
uint8_t NRF_Read_Reg(uint8_t reg);
uint8_t NRF_Write_Reg(uint8_t reg, uint8_t value);
uint8_t NRF_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);
uint8_t Nrf24l01_Check(void);

#endif

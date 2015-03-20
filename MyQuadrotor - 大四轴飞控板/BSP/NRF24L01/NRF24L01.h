#ifndef _NRF24L01_H_
#define _NRF24L01_H_
#include "sys.h"
//***************************************NRF24L01寄存器指令*******************************************************
#define NRF_READ_REG    0x00  	//读配置寄存器,低5位为寄存器地址
#define NRF_WRITE_REG   0x20 	//写配置寄存器,低5位为寄存器地址
#define R_RX_PL_WID   	0x60	
#define RD_RX_PLOAD     0x61  	//读取接收数据指令
#define WR_TX_PLOAD     0xA0  	//写待发数据指令
#define FLUSH_TX        0xE1 	//清除发送 FIFO指令
#define FLUSH_RX        0xE2  	//清除接收 FIFO指令
#define REUSE_TX_PL     0xE3  	//定义重复装载数据指令
#define NOP             0xFF  	//保留
//*************************************SPI(nRF24L01)寄存器地址****************************************************
#define CONFIG          0x00  	//配置寄存器地址;
								//	bit0	1接收模式,0发射模式;
								//	bit1	电选择;
								//	bit2	CRC模式;
								//	bit3	CRC使能;
								//	bit4	中断MAX_RT(达到最大重发次数中断)使能;
								//	bit5	中断TX_DS使能;
								//	bit6	中断RX_DR使能
#define EN_AA           0x01  	//自动应答功能设置  	bit0~5,对应通道0~5
#define EN_RXADDR       0x02  	//可用信道设置 			bit0~5,对应通道0~5
#define SETUP_AW        0x03  	//收发地址宽度设置
#define SETUP_RETR      0x04  	//自动重发功能设置
#define RF_CH           0x05  	//工作频率设置  		bit6:0,工作通道频率;
#define RF_SETUP        0x06  	//RF寄存器;
								//	bit0	低噪声放大器增益
								//	bit2:1	发射功率;
								//	bit3	传输速率(0:1Mbps,1:2Mbps);
#define NRFRegSTATUS    0x07  	//状态寄存器;
								//	bit0	TX FIFO满标志;
								//	bit3:1	接收数据通道号(最大:6);
								//	bit4	达到最多次重发
								//	bit5	数据发送完成中断;bit6:接收数据中断;

#define OBSERVE_TX      0x08  	//发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define CD              0x09  	//载波检测寄存器,bit0,载波检测;
#define RX_ADDR_P0      0x0A  	//数据通道0接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P1      0x0B  	//数据通道1接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P2      0x0C  	//数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P3      0x0D  	//数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P4      0x0E  	//数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P5      0x0F  	//数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define TX_ADDR         0x10  	//发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define RX_PW_P0        0x11  	//接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P1        0x12  	//接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P2        0x13  	//接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P3        0x14  	//接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P4        0x15  	//接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P5        0x16  	//接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define FIFO_STATUS     0x17  	//FIFO状态寄存器;
								//	bit0	RX FIFO寄存器空标志;
								//	bit1	RX FIFO满标志;
								//	bit2,3	保留
								//	bit4	TX FIFO空标志;
								//	bit5	TX FIFO满标志;
								//	bit6	1,循环发送上一数据包.0,不循环;
//**************************************************************************************
//*********************************************NRF24L01*************************************
#define RX_DR			6		//中断标志
#define TX_DS			5
#define MAX_RT			4

#define MODEL_RX		1		//普通接收
#define MODEL_TX		2		//普通发送
#define MODEL_RX2		3		//接收模式2,用于双向传输
#define MODEL_TX2		4		//发送模式2,用于双向传输

#define RX_PLOAD_WIDTH  32  	
#define TX_PLOAD_WIDTH  32  	
#define TX_ADR_WIDTH    5 	 	
#define RX_ADR_WIDTH    5   

#define SPI_CE_H()   GPIO_SetBits(GPIOA, GPIO_Pin_4) 
#define SPI_CE_L()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)

#define SPI_CSN_H()  GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define SPI_CSN_L()  GPIO_ResetBits(GPIOB, GPIO_Pin_12)

extern 	u8 	NRF24L01_RXDATA[RX_PLOAD_WIDTH];		//nrf24l01接收到的数据
extern 	u8 	NRF24L01_TXDATA[RX_PLOAD_WIDTH];		//nrf24l01需要发送的数据

void Nrf24l01_Init(u8 model, u8 ch);				//初始化,model=1/2/3/4,ch为实用的通道号
void NRF_TxPacket(uint8_t * tx_buf, uint8_t len);			//发送数据包,用于model 2/4
void NRF_TxPacket_AP(uint8_t * tx_buf, uint8_t len);	//发送数据包,用于model 3
uint8_t NRF_Read_Reg(uint8_t reg);
uint8_t NRF_Write_Reg(uint8_t reg, uint8_t value);
uint8_t NRF_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);
uint8_t Nrf24l01_Check(void);

#endif

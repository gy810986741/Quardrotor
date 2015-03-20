#include "nrf24l01.h"

uint8_t NRF24L01_RXDATA[RX_PLOAD_WIDTH];//nrf24l01���յ�������
uint8_t NRF24L01_TXDATA[RX_PLOAD_WIDTH];//nrf24l01��Ҫ���͵�����


u8  TX_ADDRESS[TX_ADR_WIDTH]= {0xAA,0xBB,0xCC,0x00,0x01};	//���ص�ַ
u8  RX_ADDRESS[RX_ADR_WIDTH]= {0xAA,0xBB,0xCC,0x00,0x01};	//���յ�ַ

u8 Spi_RW(u8 dat) 
{ 
	/* �� SPI���ͻ������ǿ�ʱ�ȴ� */ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 
	/* ͨ�� SPI2����һ�ֽ����� */ 
	SPI_I2S_SendData(SPI2, dat); 
	/* ��SPI���ջ�����Ϊ��ʱ�ȴ� */ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Return the byte read from the SPI bus */ 
	return SPI_I2S_ReceiveData(SPI2); 
}

/*******************************************************************/
uint8_t NRF_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;
	SPI_CSN_L();					  /* ѡͨ���� */
	status = Spi_RW(reg);  /* д�Ĵ�����ַ */
	Spi_RW(value);		  /* д���� */
	SPI_CSN_H();					  /* ��ֹ������ */
  return 	status;
}
/******************************************************************/
uint8_t NRF_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;
	SPI_CSN_L();					  /* ѡͨ���� */
	Spi_RW(reg);			  /* д�Ĵ�����ַ */
	reg_val = Spi_RW(0);	  /* ��ȡ�üĴ����������� */
	SPI_CSN_H();					  /* ��ֹ������ */
    return 	reg_val;
}
/******************************************************************/
uint8_t NRF_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
	uint8_t i;
	uint8_t status;
	SPI_CSN_L();				        /* ѡͨ���� */
	status = Spi_RW(reg);	/* д�Ĵ�����ַ */
	for(i=0; i<uchars; i++)
	{
		Spi_RW(pBuf[i]);		/* д���� */
	}
	SPI_CSN_H();						/* ��ֹ������ */
    return 	status;	
}
/*******************************************************************/
uint8_t NRF_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
	uint8_t i;
	uint8_t status;
	SPI_CSN_L();						/* ѡͨ���� */
	status = Spi_RW(reg);	/* д�Ĵ�����ַ */
	for(i=0; i<uchars; i++)
	{
		pBuf[i] = Spi_RW(0); /* ��ȡ�������� */ 	
	}
	SPI_CSN_H();						/* ��ֹ������ */
    return 	status;
}
/******************************************************************/
void NRF_TxPacket(uint8_t * tx_buf, uint8_t len)
{	
	SPI_CE_L();		 //StandBy Iģʽ	
	NRF_Write_Buf(WR_TX_PLOAD, tx_buf, len); 			 // װ������	
	SPI_CE_H();		 //�ø�CE���������ݷ���
}
void NRF_TxPacket_AP(uint8_t * tx_buf, uint8_t len)
{	
	SPI_CE_L();		 //StandBy Iģʽ	
	NRF_Write_Buf(0xa8, tx_buf, len); 			 // װ������
	SPI_CE_H();		 //�ø�CE
}
u8 Nrf24l01_Check(void)
{ 
	u8 buf1[5]={0};
	u8 i; 
	/*д��5���ֽڵĵ�ַ. */ 
	NRF_Write_Buf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,5); 
	/*����д��ĵ�ַ */ 
	NRF_Read_Buf(TX_ADDR,buf1,5); 
	/*�Ƚ�*/ 
	for(i=0;i<5;i++) 
	{ 
		if(buf1[i]!=TX_ADDRESS[i]) 
			break; 
	} 
	if(i==5)
		return SUCCESS ; //MCU��NRF�ɹ����� 
	else
		return ERROR ; //MCU��NRF���������� 
}
void Nrf24l01_Init(u8 model, u8 ch)
{
	SPI_InitTypeDef SPI_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	 
	/*���� SPI_NRF_SPI�� SCK,MISO,MOSI����*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //���ù��� 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);  //PB13/14/15����
	
	/*����SPI_NRF_SPI��CE���ţ���SPI_NRF_SPI�� CSN ����:*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	SPI_CSN_H();
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //˫��ȫ˫�� 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; //��ģʽ 
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //���ݴ�С8λ 
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; //ʱ�Ӽ��ԣ�����ʱΪ�� 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; //��1��������Ч��������Ϊ����ʱ�� 
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; //NSS�ź���������� 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //8��Ƶ��9MHz 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //��λ��ǰ 
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(SPI2, &SPI_InitStructure); 
	/* Enable SPI1 */ 
	SPI_Cmd(SPI2, ENABLE);
	
	SPI_CE_L();
	
	NRF_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);	//дRX�ڵ��ַ 
	NRF_Write_Buf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH); 		//дTX�ڵ��ַ  
	NRF_Write_Reg(NRF_WRITE_REG+EN_AA,0x01); 							//ʹ��ͨ��0���Զ�Ӧ�� 
	NRF_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);						//ʹ��ͨ��0�Ľ��յ�ַ 
	NRF_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);						//�����Զ��ط����ʱ��:500us;����Զ��ط�����:10�� 
	NRF_Write_Reg(NRF_WRITE_REG+RF_CH,ch);								//����RFͨ��ΪCHANAL
	NRF_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f); 						//����TX�������,0db����,2Mbps,���������濪��
	//NRF_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x07); 						//����TX�������,0db����,1Mbps,���������濪��
/////////////////////////////////////////////////////////
	if(model==1)				//RX
	{
		NRF_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);								//ѡ��ͨ��0����Ч���ݿ�� 
		NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);   		 // IRQ�շ�����жϿ���,16λCRC,������
	}
	else if(model==2)		//TX
	{
		NRF_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);								//ѡ��ͨ��0����Ч���ݿ�� 
		NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����жϿ���,16λCRC,������
	}
	else if(model==3)		//RX2
	{
		NRF_Write_Reg(FLUSH_TX,0xff);
		NRF_Write_Reg(FLUSH_RX,0xff);
		NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);   		 // IRQ�շ�����жϿ���,16λCRC,������
		
		Spi_RW(0x50);
		Spi_RW(0x73);
		NRF_Write_Reg(NRF_WRITE_REG+0x1c,0x01);
		NRF_Write_Reg(NRF_WRITE_REG+0x1d,0x06);
	}
	else								//TX2
	{
		NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����жϿ���,16λCRC,������
		NRF_Write_Reg(FLUSH_TX,0xff);
		NRF_Write_Reg(FLUSH_RX,0xff);
		
		Spi_RW(0x50);
		Spi_RW(0x73);
		NRF_Write_Reg(NRF_WRITE_REG+0x1c,0x01);
		NRF_Write_Reg(NRF_WRITE_REG+0x1d,0x06);
	}
	SPI_CE_H();
}
////////////////////////////////////////////////////////////////////////////////

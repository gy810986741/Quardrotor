/**
  ******************************************************************************
  * @file    EEPROM_Emulation/src/eeprom.c 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    07/27/2009
  * @brief   This file provides all the EEPROM emulation firmware functions.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 
/** @addtogroup EEPROM_Emulation
  * @{
  */ 

/* Includes ------------------------------------------------------------------*/
#include "eeprom.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Global variable used to store variable value in read sequence */
uint16_t DataVar = 0;
uint32_t CurWrAddress;
/* Virtual address defined by the user: 0xFFFF value is prohibited */
extern uint16_t VirtAddVarTab[NumbOfVar];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static FLASH_Status EE_Format(void);
static uint16_t EE_FindValidPage(uint8_t Operation);
static uint16_t EE_VerifyPageFullWriteVariable(uint16_t VirtAddress, uint16_t Data);
static uint16_t EE_PageTransfer(uint16_t VirtAddress, uint16_t Data);


#define EE_6050_ACC_X_OFFSET_ADDR	0
#define EE_6050_ACC_Y_OFFSET_ADDR	1
#define EE_6050_ACC_Z_OFFSET_ADDR	2
#define EE_6050_GYRO_X_OFFSET_ADDR	3
#define EE_6050_GYRO_Y_OFFSET_ADDR	4
#define EE_6050_GYRO_Z_OFFSET_ADDR	5
#define EE_PID_ROL_P	6
#define EE_PID_ROL_I	7
#define EE_PID_ROL_D	8
#define EE_PID_PIT_P	9
#define EE_PID_PIT_I	10
#define EE_PID_PIT_D	11
#define EE_PID_YAW_P	12
#define EE_PID_YAW_I	13
#define EE_PID_YAW_D	14
#define EE_PID_ALT_P	15
#define EE_PID_ALT_I	16
#define EE_PID_ALT_D	17
#define EE_PID_POS_P	18
#define EE_PID_POS_I	19
#define EE_PID_POS_D	20

#define EE_PID_PID1_P	21
#define EE_PID_PID1_I	22
#define EE_PID_PID1_D	23
#define EE_PID_PID2_P	24
#define EE_PID_PID2_I	25
#define EE_PID_PID2_D	26
#define EE_PID_PID3_P	27
#define EE_PID_PID3_I	28
#define EE_PID_PID3_D	29
#define EE_PID_PID4_P	30
#define EE_PID_PID4_I	31
#define EE_PID_PID4_D	32
#define EE_PID_PID5_P	33
#define EE_PID_PID5_I	34
#define EE_PID_PID5_D	35
#define EE_PID_PID6_P	36
#define EE_PID_PID6_I	37
#define EE_PID_PID6_D	38
#define EE_PID_PID7_P	39
#define EE_PID_PID7_I	40
#define EE_PID_PID7_D	41
#define EE_PID_PID8_P	42
#define EE_PID_PID8_I	43
#define EE_PID_PID8_D	44
#define EE_PID_PID9_P	45
#define EE_PID_PID9_I	46
#define EE_PID_PID9_D	47
#define EE_PID_PID10_P	48
#define EE_PID_PID10_I	49
#define EE_PID_PID10_D	50
#define EE_PID_PID11_P	51
#define EE_PID_PID11_I	52
#define EE_PID_PID11_D	53
#define EE_PID_PID12_P	54
#define EE_PID_PID12_I	55
#define EE_PID_PID12_D	56
//ErrorStatus  HSEStartUpStatus;
//FLASH_Status FlashStatus;
uint16_t VirtAddVarTab[NumbOfVar] = {0xAA00, 0xAA01, 0xAA02, 0xAA03, 0xAA04, 0xAA05, 0xAA06, 0xAA07, 0xAA08, 0xAA09, 0xAA0A, 0xAA0B, 0xAA0C, 0xAA0D, 0xAA0E, 0xAA0F,
																		 0xAA10, 0xAA11, 0xAA12, 0xAA13, 0xAA14, 0xAA15, 0xAA16, 0xAA17, 0xAA18, 0xAA19, 0xAA1A, 0xAA1B, 0xAA1C, 0xAA1D, 0xAA1E, 0xAA1F,
																		 0xAA20, 0xAA21, 0xAA22, 0xAA23, 0xAA24, 0xAA25, 0xAA26, 0xAA27, 0xAA28, 0xAA29, 0xAA2A, 0xAA2B, 0xAA2C, 0xAA2D, 0xAA2E, 0xAA2F,
																		 0xAA30, 0xAA31, 0xAA32, 0xAA33, 0xAA34, 0xAA35, 0xAA36, 0xAA37, 0xAA38,};
uint16_t temp;
void EE_INIT(void)
{
	EE_Init();
}
void EE_SAVE_ACC_OFFSET(void)
{
	EE_WriteVariable(VirtAddVarTab[EE_6050_ACC_X_OFFSET_ADDR], ACC_OFFSET.X);
	EE_WriteVariable(VirtAddVarTab[EE_6050_ACC_Y_OFFSET_ADDR], ACC_OFFSET.Y);
	EE_WriteVariable(VirtAddVarTab[EE_6050_ACC_Z_OFFSET_ADDR], ACC_OFFSET.Z);
}
void EE_READ_ACC_OFFSET(void)
{
	EE_ReadVariable(VirtAddVarTab[EE_6050_ACC_X_OFFSET_ADDR], &ACC_OFFSET.X);
	EE_ReadVariable(VirtAddVarTab[EE_6050_ACC_Y_OFFSET_ADDR], &ACC_OFFSET.Y);
	EE_ReadVariable(VirtAddVarTab[EE_6050_ACC_Z_OFFSET_ADDR], &ACC_OFFSET.Z);
}
void EE_SAVE_GYRO_OFFSET(void)
{
	EE_WriteVariable(VirtAddVarTab[EE_6050_GYRO_X_OFFSET_ADDR], GYRO_OFFSET.X);
	EE_WriteVariable(VirtAddVarTab[EE_6050_GYRO_Y_OFFSET_ADDR], GYRO_OFFSET.Y);
	EE_WriteVariable(VirtAddVarTab[EE_6050_GYRO_Z_OFFSET_ADDR], GYRO_OFFSET.Z);
}
void EE_READ_GYRO_OFFSET(void)
{
	EE_ReadVariable(VirtAddVarTab[EE_6050_GYRO_X_OFFSET_ADDR], &GYRO_OFFSET.X);
	EE_ReadVariable(VirtAddVarTab[EE_6050_GYRO_Y_OFFSET_ADDR], &GYRO_OFFSET.Y);
	EE_ReadVariable(VirtAddVarTab[EE_6050_GYRO_Z_OFFSET_ADDR], &GYRO_OFFSET.Z);
}
void EE_SAVE_PID(void)
{
	u16 _temp;
	_temp = PID_ROL.P * 100;
	EE_WriteVariable(VirtAddVarTab[EE_PID_ROL_P],_temp);
	_temp = PID_ROL.I * 1000;
	EE_WriteVariable(VirtAddVarTab[EE_PID_ROL_I],_temp);
	_temp = PID_ROL.D * 1000;
	EE_WriteVariable(VirtAddVarTab[EE_PID_ROL_D],_temp);
	_temp = PID_PIT.P * 100;
	EE_WriteVariable(VirtAddVarTab[EE_PID_PIT_P],_temp);
	_temp = PID_PIT.I * 1000;
	EE_WriteVariable(VirtAddVarTab[EE_PID_PIT_I],_temp);
	_temp = PID_PIT.D * 1000;
	EE_WriteVariable(VirtAddVarTab[EE_PID_PIT_D],_temp);
	_temp = PID_YAW.P * 100;
	EE_WriteVariable(VirtAddVarTab[EE_PID_YAW_P],_temp);
	_temp = PID_YAW.I * 1000;
	EE_WriteVariable(VirtAddVarTab[EE_PID_YAW_I],_temp);
	_temp = PID_YAW.D * 1000;
	EE_WriteVariable(VirtAddVarTab[EE_PID_YAW_D],_temp);
	_temp = PID_ALT.P * 100;
	EE_WriteVariable(VirtAddVarTab[EE_PID_ALT_P],_temp);
	_temp = PID_ALT.I * 1000;
	EE_WriteVariable(VirtAddVarTab[EE_PID_ALT_I],_temp);
	_temp = PID_ALT.D * 100;
	EE_WriteVariable(VirtAddVarTab[EE_PID_ALT_D],_temp);
	_temp = PID_POS.P * 100;
	EE_WriteVariable(VirtAddVarTab[EE_PID_POS_P],_temp);
	_temp = PID_POS.I * 1000;
	EE_WriteVariable(VirtAddVarTab[EE_PID_POS_I],_temp);
	_temp = PID_POS.D * 100;
	EE_WriteVariable(VirtAddVarTab[EE_PID_POS_D],_temp);
//	_temp = PID_PID_1.P * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID1_P],_temp);
//	_temp = PID_PID_1.I * 1000;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID1_I],_temp);
//	_temp = PID_PID_1.D * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID1_D],_temp);
//	_temp = PID_PID_2.P * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID2_P],_temp);
//	_temp = PID_PID_2.I * 1000;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID2_I],_temp);
//	_temp = PID_PID_2.D * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID2_D],_temp);
//	_temp = PID_PID_3.P * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID3_P],_temp);
//	_temp = PID_PID_3.I * 1000;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID3_I],_temp);
//	_temp = PID_PID_3.D * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID3_D],_temp);
//	_temp = PID_PID_4.P * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID4_P],_temp);
//	_temp = PID_PID_4.I * 1000;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID4_I],_temp);
//	_temp = PID_PID_4.D * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID4_D],_temp);
//	_temp = PID_PID_5.P * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID5_P],_temp);
//	_temp = PID_PID_5.I * 1000;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID5_I],_temp);
//	_temp = PID_PID_5.D * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID5_D],_temp);
//	_temp = PID_PID_6.P * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID6_P],_temp);
//	_temp = PID_PID_6.I * 1000;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID6_I],_temp);
//	_temp = PID_PID_6.D * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID6_D],_temp);
//	_temp = PID_PID_7.P * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID7_P],_temp);
//	_temp = PID_PID_7.I * 1000;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID7_I],_temp);
//	_temp = PID_PID_7.D * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID7_D],_temp);
//	_temp = PID_PID_8.P * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID8_P],_temp);
//	_temp = PID_PID_8.I * 1000;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID8_I],_temp);
//	_temp = PID_PID_8.D * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID8_D],_temp);
//	_temp = PID_PID_9.P * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID9_P],_temp);
//	_temp = PID_PID_9.I * 1000;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID9_I],_temp);
//	_temp = PID_PID_9.D * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID9_D],_temp);
//	_temp = PID_PID_10.P * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID10_P],_temp);
//	_temp = PID_PID_10.I * 1000;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID10_I],_temp);
//	_temp = PID_PID_10.D * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID10_D],_temp);
//	_temp = PID_PID_11.P * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID11_P],_temp);
//	_temp = PID_PID_11.I * 1000;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID11_I],_temp);
//	_temp = PID_PID_11.D * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID11_D],_temp);
//	_temp = PID_PID_12.P * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID12_P],_temp);
//	_temp = PID_PID_12.I * 1000;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID12_I],_temp);
//	_temp = PID_PID_12.D * 100;
//	EE_WriteVariable(VirtAddVarTab[EE_PID_PID12_D],_temp);
}
void EE_READ_PID(void)
{
	u16 _temp;
	EE_ReadVariable(VirtAddVarTab[EE_PID_ROL_P],&_temp);
	PID_ROL.P = (float)_temp / 100;
	EE_ReadVariable(VirtAddVarTab[EE_PID_ROL_I],&_temp);
	PID_ROL.I = (float)_temp / 1000;
	EE_ReadVariable(VirtAddVarTab[EE_PID_ROL_D],&_temp);
	PID_ROL.D = (float)_temp / 1000;
	EE_ReadVariable(VirtAddVarTab[EE_PID_PIT_P],&_temp);
	PID_PIT.P = (float)_temp / 100;
	EE_ReadVariable(VirtAddVarTab[EE_PID_PIT_I],&_temp);
	PID_PIT.I = (float)_temp / 1000;
	EE_ReadVariable(VirtAddVarTab[EE_PID_PIT_D],&_temp);
	PID_PIT.D = (float)_temp / 1000;
	EE_ReadVariable(VirtAddVarTab[EE_PID_YAW_P],&_temp);
	PID_YAW.P = (float)_temp / 100;
	EE_ReadVariable(VirtAddVarTab[EE_PID_YAW_I],&_temp);
	PID_YAW.I = (float)_temp / 1000;
	EE_ReadVariable(VirtAddVarTab[EE_PID_YAW_D],&_temp);
	PID_YAW.D = (float)_temp / 1000;
	EE_ReadVariable(VirtAddVarTab[EE_PID_ALT_P],&_temp);
	PID_ALT.P = (float)_temp / 100;
	EE_ReadVariable(VirtAddVarTab[EE_PID_ALT_I],&_temp);
	PID_ALT.I = (float)_temp / 1000;
	EE_ReadVariable(VirtAddVarTab[EE_PID_ALT_D],&_temp);
	PID_ALT.D = (float)_temp / 100;
	EE_ReadVariable(VirtAddVarTab[EE_PID_POS_P],&_temp);
	PID_POS.P = (float)_temp / 100;
	EE_ReadVariable(VirtAddVarTab[EE_PID_POS_I],&_temp);
	PID_POS.I = (float)_temp / 1000;
	EE_ReadVariable(VirtAddVarTab[EE_PID_POS_D],&_temp);
	PID_POS.D = (float)_temp / 100;
	EE_ReadVariable(VirtAddVarTab[EE_PID_PID1_P],&_temp);
//	PID_PID_1.P = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID1_I],&_temp);
//	PID_PID_1.I = (float)_temp / 1000;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID1_D],&_temp);
//	PID_PID_1.D = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID2_P],&_temp);
//	PID_PID_2.P = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID2_I],&_temp);
//	PID_PID_2.I = (float)_temp / 1000;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID2_D],&_temp);
//	PID_PID_2.D = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID3_P],&_temp);
//	PID_PID_3.P = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID3_I],&_temp);
//	PID_PID_3.I = (float)_temp / 1000;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID3_D],&_temp);
//	PID_PID_3.D = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID4_P],&_temp);
//	PID_PID_4.P = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID4_I],&_temp);
//	PID_PID_4.I = (float)_temp / 1000;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID4_D],&_temp);
//	PID_PID_4.D = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID5_P],&_temp);
//	PID_PID_5.P = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID5_I],&_temp);
//	PID_PID_5.I = (float)_temp / 1000;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID5_D],&_temp);
//	PID_PID_5.D = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID6_P],&_temp);
//	PID_PID_6.P = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID6_I],&_temp);
//	PID_PID_6.I = (float)_temp / 1000;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID6_D],&_temp);
//	PID_PID_6.D = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID7_P],&_temp);
//	PID_PID_7.P = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID7_I],&_temp);
//	PID_PID_7.I = (float)_temp / 1000;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID7_D],&_temp);
//	PID_PID_7.D = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID8_P],&_temp);
//	PID_PID_8.P = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID8_I],&_temp);
//	PID_PID_8.I = (float)_temp / 1000;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID8_D],&_temp);
//	PID_PID_8.D = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID9_P],&_temp);
//	PID_PID_9.P = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID9_I],&_temp);
//	PID_PID_9.I = (float)_temp / 1000;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID9_D],&_temp);
//	PID_PID_9.D = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID10_P],&_temp);
//	PID_PID_10.P = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID10_I],&_temp);
//	PID_PID_10.I = (float)_temp / 1000;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID10_D],&_temp);
//	PID_PID_10.D = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID11_P],&_temp);
//	PID_PID_11.P = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID11_I],&_temp);
//	PID_PID_11.I = (float)_temp / 1000;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID11_D],&_temp);
//	PID_PID_11.D = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID12_P],&_temp);
//	PID_PID_12.P = (float)_temp / 100;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID12_I],&_temp);
//	PID_PID_12.I = (float)_temp / 1000;
//	EE_ReadVariable(VirtAddVarTab[EE_PID_PID12_D],&_temp);
//	PID_PID_12.D = (float)_temp / 100;
}



//初始化写地址，减少每次读写时查询时间
uint16_t InitCurrWrAddress(void)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;
  uint16_t ValidPage = PAGE0;
  //uint32_t Address;
  uint32_t PageEndAddress;
  
  /* Get valid Page for write operation */
  ValidPage = EE_FindValidPage(WRITE_IN_VALID_PAGE);

  /* Check if there is no valid page */
  if (ValidPage == NO_VALID_PAGE)
  {
    CurWrAddress = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * PAGE_SIZE));
    return  NO_VALID_PAGE;
  }
  
  /* Get the valid Page start Address */
  //Address = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * PAGE_SIZE));
  CurWrAddress = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * PAGE_SIZE));

  /* Get the valid Page end Address */
  PageEndAddress = (uint32_t)((EEPROM_START_ADDRESS - 2) + (uint32_t)((1 + ValidPage) * PAGE_SIZE));

  /* Check each active page address starting from begining */
  while (CurWrAddress < PageEndAddress)
  {
    /* Verify if Address and Address+2 contents are 0xFFFFFFFF */
    if ((*(__IO uint32_t*)CurWrAddress) == 0xFFFFFFFF)
    {
      
      /* Set variable virtual address */
      FlashStatus = FLASH_COMPLETE;
      /* Return program operation status */
      return FlashStatus;
    }
    else
    {
      /* Next address location */
      CurWrAddress = CurWrAddress + 4;
    }
  }

  /* Return PAGE_FULL in case the valid page is full */
  return PAGE_FULL;
}
/**
  * @brief  Restore the pages to a known good state in case of page's status
  *   corruption after a power loss.
  * @param  None.
  * @retval - Flash error code: on write Flash error
  *         - FLASH_COMPLETE: on success
  */
uint16_t __EE_Init(void)
{
  uint16_t PageStatus0 = 6, PageStatus1 = 6;
  uint16_t VarIdx = 0;
  uint16_t EepromStatus = 0, ReadStatus = 0;
  int16_t x = -1;
  uint16_t  FlashStatus;

  /* Get Page0 status */
  PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);
  /* Get Page1 status */
  PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

  /* Check for invalid header states and repair if necessary */
  switch (PageStatus0)
  {
    case ERASED:
      if (PageStatus1 == VALID_PAGE) /* Page0 erased, Page1 valid */
      {
        /* Erase Page0 */
        FlashStatus = FLASH_ErasePage(PAGE0_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
      }
      else if (PageStatus1 == RECEIVE_DATA) /* Page0 erased, Page1 receive */
      {
        /* Erase Page0 */
        FlashStatus = FLASH_ErasePage(PAGE0_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
        /* Mark Page1 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
      }
      else /* First EEPROM access (Page0&1 are erased) or invalid state -> format EEPROM */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        FlashStatus = EE_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
      }
      break;

    case RECEIVE_DATA:
      if (PageStatus1 == VALID_PAGE) /* Page0 receive, Page1 valid */
      {
        /* Transfer data from Page1 to Page0 */
        for (VarIdx = 0; VarIdx < NumbOfVar; VarIdx++)
        {
          if (( *(__IO uint16_t*)(PAGE0_BASE_ADDRESS + 6)) == VirtAddVarTab[VarIdx])
          {
            x = VarIdx;
          }
          if (VarIdx != x)
          {
            /* Read the last variables' updates */
            ReadStatus = EE_ReadVariable(VirtAddVarTab[VarIdx], &DataVar);
            /* In case variable corresponding to the virtual address was found */
            if (ReadStatus != 0x1)
            {
              /* Transfer the variable to the Page0 */
              EepromStatus = EE_VerifyPageFullWriteVariable(VirtAddVarTab[VarIdx], DataVar);
              /* If program operation was failed, a Flash error code is returned */
              if (EepromStatus != FLASH_COMPLETE)
              {
                return EepromStatus;
              }
            }
          }
        }
        /* Mark Page0 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
        /* Erase Page1 */
        FlashStatus = FLASH_ErasePage(PAGE1_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
      }
      else if (PageStatus1 == ERASED) /* Page0 receive, Page1 erased */
      {
        /* Erase Page1 */
        FlashStatus = FLASH_ErasePage(PAGE1_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
        /* Mark Page0 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
      }
      else /* Invalid state -> format eeprom */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        FlashStatus = EE_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
      }
      break;

    case VALID_PAGE:
      if (PageStatus1 == VALID_PAGE) /* Invalid state -> format eeprom */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        FlashStatus = EE_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
      }
      else if (PageStatus1 == ERASED) /* Page0 valid, Page1 erased */
      {
        /* Erase Page1 */
        FlashStatus = FLASH_ErasePage(PAGE1_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
      }
      else /* Page0 valid, Page1 receive */
      {
        /* Transfer data from Page0 to Page1 */
        for (VarIdx = 0; VarIdx < NumbOfVar; VarIdx++)
        {
          if ((*(__IO uint16_t*)(PAGE1_BASE_ADDRESS + 6)) == VirtAddVarTab[VarIdx])
          {
            x = VarIdx;
          }
          if (VarIdx != x)
          {
            /* Read the last variables' updates */
            ReadStatus = EE_ReadVariable(VirtAddVarTab[VarIdx], &DataVar);
            /* In case variable corresponding to the virtual address was found */
            if (ReadStatus != 0x1)
            {
              /* Transfer the variable to the Page1 */
              EepromStatus = EE_VerifyPageFullWriteVariable(VirtAddVarTab[VarIdx], DataVar);
              /* If program operation was failed, a Flash error code is returned */
              if (EepromStatus != FLASH_COMPLETE)
              {
                return EepromStatus;
              }
            }
          }
        }
        /* Mark Page1 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
        /* Erase Page0 */
        FlashStatus = FLASH_ErasePage(PAGE0_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          return FlashStatus;
        }
      }
      break;

    default:  /* Any other state -> format eeprom */
      /* Erase both Page0 and Page1 and set Page0 as valid page */
      FlashStatus = EE_Format();
      /* If erase/program operation was failed, a Flash error code is returned */
      if (FlashStatus != FLASH_COMPLETE)
      {
        return FlashStatus;
      }
      break;
  }

  return FLASH_COMPLETE;
}

uint16_t EE_Init(void)
{
uint16_t FlashStatus;
   
   FlashStatus=__EE_Init();
   
   InitCurrWrAddress();
   
   return(FlashStatus);
}
/**
  * @brief  Returns the last stored variable data, if found, which correspond to
  *   the passed virtual address
  * @param  VirtAddress: Variable virtual address
  * @param  Data: Global variable contains the read variable value
  * @retval Success or error status:
  *           - 0: if variable was found
  *           - 1: if the variable was not found
  *           - NO_VALID_PAGE: if no valid page was found.
  */
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data)
{
  uint16_t ValidPage = PAGE0;
  uint16_t AddressValue = 0x5555, ReadStatus = 1;
  uint32_t Address = 0x08010000, PageStartAddress = 0x08010000;

  /* Get active Page for read operation */
  ValidPage = EE_FindValidPage(READ_FROM_VALID_PAGE);

  /* Check if there is no valid page */
  if (ValidPage == NO_VALID_PAGE)
  {
    return  NO_VALID_PAGE;
  }

  /* Get the valid Page start Address */
  PageStartAddress = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * PAGE_SIZE));

  /* Get the valid Page end Address */
  //Address = (uint32_t)((EEPROM_START_ADDRESS - 2) + (uint32_t)((1 + ValidPage) * PAGE_SIZE));
  Address=CurWrAddress-2;

  /* Check each active page address starting from end */
  while (Address > (PageStartAddress + 2))
  {
    /* Get the current location content to be compared with virtual address */
    AddressValue = (*(__IO uint16_t*)Address);

    /* Compare the read address with the virtual address */
    if (AddressValue == VirtAddress)
    {
      /* Get content of Address-2 which is variable value */
      *Data = (*(__IO uint16_t*)(Address - 2));

      /* In case variable value is read, reset ReadStatus flag */
      ReadStatus = 0;

      break;
    }
    else
    {
      /* Next address location */
      Address = Address - 4;
    }
  }

  /* Return ReadStatus value: (0: variable exist, 1: variable doesn't exist) */
  return ReadStatus;
}

/**
  * @brief  Writes/upadtes variable data in EEPROM.
  * @param  VirtAddress: Variable virtual address
  * @param  Data: 16 bit data to be written
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data)
{
  uint16_t Status = 0;

  /* Write the variable virtual address and value in the EEPROM */
  Status = EE_VerifyPageFullWriteVariable(VirtAddress, Data);

  /* In case the EEPROM active page is full */
  if (Status == PAGE_FULL)
  {
    /* Perform Page transfer */
    Status = EE_PageTransfer(VirtAddress, Data);
  }

  /* Return last operation status */
  return Status;
}

/**
  * @brief  Erases PAGE0 and PAGE1 and writes VALID_PAGE header to PAGE0
  * @param  None
  * @retval Status of the last operation (Flash write or erase) done during
  *         EEPROM formating
  */
static FLASH_Status EE_Format(void)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;

  /* Erase Page0 */
  FlashStatus = FLASH_ErasePage(PAGE0_BASE_ADDRESS);

  /* If erase operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    return FlashStatus;
  }

  /* Set Page0 as valid page: Write VALID_PAGE at Page0 base address */
  FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);

  /* If program operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    return FlashStatus;
  }

  /* Erase Page1 */
  FlashStatus = FLASH_ErasePage(PAGE1_BASE_ADDRESS);

  /* Return Page1 erase operation status */
  return FlashStatus;
}

/**
  * @brief  Find valid Page for write or read operation
  * @param  Operation: operation to achieve on the valid page.
  *   This parameter can be one of the following values:
  *     @arg READ_FROM_VALID_PAGE: read operation from valid page
  *     @arg WRITE_IN_VALID_PAGE: write operation from valid page
  * @retval Valid page number (PAGE0 or PAGE1) or NO_VALID_PAGE in case
  *   of no valid page was found
  */
static uint16_t EE_FindValidPage(uint8_t Operation)
{
  uint16_t PageStatus0 = 6, PageStatus1 = 6;

  /* Get Page0 actual status */
  PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);

  /* Get Page1 actual status */
  PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

  /* Write or read operation */
  switch (Operation)
  {
    case WRITE_IN_VALID_PAGE:   /* ---- Write operation ---- */
      if (PageStatus1 == VALID_PAGE)
      {
        /* Page0 receiving data */
        if (PageStatus0 == RECEIVE_DATA)
        {
          return PAGE0;         /* Page0 valid */
        }
        else
        {
          return PAGE1;         /* Page1 valid */
        }
      }
      else if (PageStatus0 == VALID_PAGE)
      {
        /* Page1 receiving data */
        if (PageStatus1 == RECEIVE_DATA)
        {
          return PAGE1;         /* Page1 valid */
        }
        else
        {
          return PAGE0;         /* Page0 valid */
        }
      }
      else
      {
        return NO_VALID_PAGE;   /* No valid Page */
      }

    case READ_FROM_VALID_PAGE:  /* ---- Read operation ---- */
      if (PageStatus0 == VALID_PAGE)
      {
        return PAGE0;           /* Page0 valid */
      }
      else if (PageStatus1 == VALID_PAGE)
      {
        return PAGE1;           /* Page1 valid */
      }
      else
      {
        return NO_VALID_PAGE ;  /* No valid Page */
      }

    default:
      return PAGE0;             /* Page0 valid */
  }
}

/**
  * @brief  Verify if active page is full and Writes variable in EEPROM.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
static uint16_t EE_VerifyPageFullWriteVariable(uint16_t VirtAddress, uint16_t Data)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;
  uint16_t ValidPage = PAGE0;
  //uint32_t Address = 0x08010000, 
  uint32_t PageEndAddress = 0x080107FF;

  /* Get valid Page for write operation */
  ValidPage = EE_FindValidPage(WRITE_IN_VALID_PAGE);

  /* Check if there is no valid page */
  if (ValidPage == NO_VALID_PAGE)
  {
    return  NO_VALID_PAGE;
  }

  /* Get the valid Page start Address */
  //Address = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * PAGE_SIZE));
  //Address = CurWrAddress;//当前写地址

  /* Get the valid Page end Address */
  PageEndAddress = (uint32_t)((EEPROM_START_ADDRESS - 2) + (uint32_t)((1 + ValidPage) * PAGE_SIZE));

  /* Check each active page address starting from begining */
  while (CurWrAddress < PageEndAddress)
  {
    /* Verify if Address and Address+2 contents are 0xFFFFFFFF */
    if ((*(__IO uint32_t*)CurWrAddress) == 0xFFFFFFFF)
    {
      /* Set variable data */
      FlashStatus = FLASH_ProgramHalfWord(CurWrAddress, Data);
      /* If program operation was failed, a Flash error code is returned */
      if (FlashStatus != FLASH_COMPLETE)
      {
        return FlashStatus;
      }
      /* Set variable virtual address */
      FlashStatus = FLASH_ProgramHalfWord(CurWrAddress + 2, VirtAddress);
      
      CurWrAddress = CurWrAddress + 4;
      /* Return program operation status */
      return FlashStatus;
    }
    else
    {//修改后的算法是不会执行到这里的
      /* Next address location */
      CurWrAddress = CurWrAddress + 4;
    }
  }

  /* Return PAGE_FULL in case the valid page is full */
  return PAGE_FULL;
}

/**
  * @brief  Transfers last updated variables data from the full Page to
  *   an empty one.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
static uint16_t EE_PageTransfer(uint16_t VirtAddress, uint16_t Data)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;
  uint32_t NewPageAddress = 0x080103FF, OldPageAddress = 0x08010000;
  uint16_t ValidPage = PAGE0, VarIdx = 0;
  uint16_t EepromStatus = 0, ReadStatus = 0;

  /* Get active Page for read operation */
  ValidPage = EE_FindValidPage(READ_FROM_VALID_PAGE);

  if (ValidPage == PAGE1)       /* Page1 valid */
  {
    /* New page address where variable will be moved to */
    NewPageAddress = PAGE0_BASE_ADDRESS;

    /* Old page address where variable will be taken from */
    OldPageAddress = PAGE1_BASE_ADDRESS;
  }
  else if (ValidPage == PAGE0)  /* Page0 valid */
  {
    /* New page address where variable will be moved to */
    NewPageAddress = PAGE1_BASE_ADDRESS;

    /* Old page address where variable will be taken from */
    OldPageAddress = PAGE0_BASE_ADDRESS;
  }
  else
  {
    return NO_VALID_PAGE;       /* No valid Page */
  }

  /* Set the new Page status to RECEIVE_DATA status */
  FlashStatus = FLASH_ProgramHalfWord(NewPageAddress, RECEIVE_DATA);
  /* If program operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    return FlashStatus;
  }

  InitCurrWrAddress();//aft 重新初始化写地址
  /* Write the variable passed as parameter in the new active page */
  EepromStatus = EE_VerifyPageFullWriteVariable(VirtAddress, Data);
  /* If program operation was failed, a Flash error code is returned */
  if (EepromStatus != FLASH_COMPLETE)
  {
    return EepromStatus;
  }

  /* Transfer process: transfer variables from old to the new active page */
  for (VarIdx = 0; VarIdx < NumbOfVar; VarIdx++)
  {
    if (VirtAddVarTab[VarIdx] != VirtAddress)  /* Check each variable except the one passed as parameter */
    {
      /* Read the other last variable updates */
      ReadStatus = EE_ReadVariable(VirtAddVarTab[VarIdx], &DataVar);
      /* In case variable corresponding to the virtual address was found */
      if (ReadStatus != 0x1)
      {
        /* Transfer the variable to the new active page */
        EepromStatus = EE_VerifyPageFullWriteVariable(VirtAddVarTab[VarIdx], DataVar);
        /* If program operation was failed, a Flash error code is returned */
        if (EepromStatus != FLASH_COMPLETE)
        {
          return EepromStatus;
        }
      }
    }
  }

  /* Erase the old Page: Set old Page status to ERASED status */
  FlashStatus = FLASH_ErasePage(OldPageAddress);
  /* If erase operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    return FlashStatus;
  }

  /* Set new Page status to VALID_PAGE status */
  FlashStatus = FLASH_ProgramHalfWord(NewPageAddress, VALID_PAGE);
  /* If program operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    return FlashStatus;
  }

  /* Return last operation flash status */
  return FlashStatus;
}

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

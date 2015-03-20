#include "sys.h"



//#define MOTORS_GPIO_TIM_M1_2_PERIF  RCC_APB1Periph_TIM2
//#define MOTORS_GPIO_TIM_M1_2      	TIM2
//#define MOTORS_GPIO_TIM_M1_2_DBG  	DBGMCU_TIM2_STOP
////#define MOTORS_REMAP				GPIO_PartialRemap_TIM2

//#define MOTORS_GPIO_TIM_M3_4_PERIF  RCC_APB2Periph_TIM1
//#define MOTORS_GPIO_TIM_M3_4        TIM1
//#define MOTORS_GPIO_TIM_M3_4_DBG    DBGMCU_TIM1_STOP

//#define MOTORS_GPIO_PERIF         RCC_APB2Periph_GPIOA
//#define MOTORS_GPIO_PORT          GPIOA
//#define MOTORS_GPIO_M1            GPIO_Pin_0 // T2_CH1
//#define MOTORS_GPIO_M2            GPIO_Pin_1 // T2_CH2
//#define MOTORS_GPIO_M3            GPIO_Pin_8 // T1_CH1
//#define MOTORS_GPIO_M4            GPIO_Pin_9 // T1_CH4


//#define MOTORS_PWM_BITS     9
//#define MOTORS_PWM_PERIOD   ((1<<MOTORS_PWM_BITS) - 1)
//#define MOTORS_PWM_PRESCALE 0

//// Motors IDs define
//#define MOTOR_M1  0
//#define MOTOR_M2  1
//#define MOTOR_M3  2
//#define MOTOR_M4  3

//// Test defines
//#define MOTORS_TEST_RATIO         	(uint16_t)(0.5*(1<<16))
//#define MOTORS_TEST_RATIO1         	(uint16_t)(0.2*(1<<16))
//#define MOTORS_TEST_ON_TIME_MS    	10
//#define MOTORS_TEST_DELAY_TIME_MS 	50


///* Utils Conversion macro */
//#define C_BITS_TO_16(X) ((X)<<(16-MOTORS_PWM_BITS))
//#define C_16_TO_BITS(X) ((X)>>(16-MOTORS_PWM_BITS)&((1<<MOTORS_PWM_BITS)-1))

//extern const int MOTORS[4];


void motorsInit(void);
//void motorsSetRatio(int id, uint16_t ratio);
//void motorsTest(void);
//void motorsTesta(void);
//void motorsTestb(void);
void Moto_PwmRflash(int16_t MOTO1_PWM,int16_t MOTO2_PWM,int16_t MOTO3_PWM,int16_t MOTO4_PWM);
//void Moto_Init(void);

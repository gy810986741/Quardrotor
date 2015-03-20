#include "sys.h"

extern u8 Data_Check,Send_Status,Send_Senser,Send_RCData,Send_Offset,Send_PID1,Send_PID2,Send_PID3,Send_PID4,Send_PID5,Send_PID6,Send_MotoPwm,Send_Voltage;


void Data_Receive_Anl(u8 *data_buf,u8 num);
void Data_Exchange(void);
void Data_Send_Check(u16 check);




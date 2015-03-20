#ifndef _AHRS_IMU_H
#define _AHRS_IMU_H
#include "sys.h"

void Prepare_Data(T_int16_xyz *acc_in,T_int16_xyz *acc_out);

void IMUupdate(T_int16_xyz *gyr, T_int16_xyz *acc, T_float_angle *angle);


#endif /* _AHRS_IMU_H */


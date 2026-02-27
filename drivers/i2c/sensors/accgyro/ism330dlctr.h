/* 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * ism330dlctr.h
 *
 *  Created on: Feb 26, 2026
 *      Author: fil
 */

#ifndef DRIVERS_I2C_SENSORS_ACCGYRO_ISM330DLCTR_H_
#define DRIVERS_I2C_SENSORS_ACCGYRO_ISM330DLCTR_H_

#include <stdint.h>

#define ISM330DLCTR_I2C_ADDR        0xD4
#define ISM330DLCTR_WHO_AM_I        0x6B

// Register Map
#define ISM330DLCTR_WHO_AM_I_REG    0x0F
#define ISM330DLCTR_CTRL1_XL        0x10
#define ISM330DLCTR_CTRL2_G         0x11
#define ISM330DLCTR_CTRL3_C         0x12
#define ISM330DLCTR_OUTX_L_XL       0x22
#define ISM330DLCTR_OUTX_L_G        0x28

// Configuration Macros
#define ISM330DLCTR_ODR_104Hz       0x04
#define ISM330DLCTR_FS_XL_4G        0x08
#define ISM330DLCTR_FS_G_2000DPS    0x04
#define ISM330DLCTR_BDU_ENABLE      0x40  // Block Data Update (Bit 6 of CTRL3_C)

// Data Structure
typedef struct {

} IMU_Data_t;

typedef struct
{
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	I2C_HandleTypeDef 	*bus;
	uint16_t 			device_address;
	uint32_t 			wakeup_id;
	I2C_DriverStruct_t	*next_drv;
	/* driver proprietary data */
	uint8_t				who_am_i;
	uint32_t			sensor_id;
    float 				gyro_x,gyro_y,gyro_z;
    float 				acc_x,acc_y,acc_z;
}I2C_ism330dlctr_Drv_TypeDef;


extern uint32_t ism330dlctr_register(I2C_ism330dlctr_Drv_TypeDef *ism330dlctr_Drv);
extern uint8_t ism330dlctr_ReadAcc(I2C_ism330dlctr_Drv_TypeDef *ism330dlctr_Drv, IMU_Data_t *data);
extern uint8_t ism330dlctr_ReadGyro(I2C_ism330dlctr_Drv_TypeDef *ism330dlctr_Drv, IMU_Data_t *data);

#endif /* DRIVERS_I2C_SENSORS_ACCGYRO_ISM330DLCTR_H_ */

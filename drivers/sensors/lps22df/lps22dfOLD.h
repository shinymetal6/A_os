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
 * lps22df.h
 *
 *  Created on: Mar 13, 2024
 *      Author: fil
 */

#ifndef DRIVERS_SENSORS_LPS22DF_LPS22DF_H_
#define DRIVERS_SENSORS_LPS22DF_LPS22DF_H_

#include <stdint.h>
#include <stddef.h>
#include <math.h>

#define	LPS22DF_I2C_TIMEOUT				1000
#define	LPS22DF_ADDR					0xb8
#define	LPS22DF_P_LEN					3
#define	LPS22DF_T_LEN					2

#define LPS22DF_ID                      0xB4U
#define LPS22DF_INTERRUPT_CFG           0x0BU
#define LPS22DF_THS_P_L                 0x0CU
#define LPS22DF_THS_P_H                 0x0DU
#define LPS22DF_IF_CTRL                 0x0EU
#define LPS22DF_WHO_AM_I                0x0FU
#define LPS22DF_CTRL_REG1               0x10U
#define LPS22DF_CTRL_REG2               0x11U
#define LPS22DF_CTRL_REG3               0x12U
#define LPS22DF_CTRL_REG4               0x13U
#define LPS22DF_FIFO_CTRL               0x14U
#define LPS22DF_FIFO_WTM                0x15U
#define LPS22DF_REF_P_L                 0x16U
#define LPS22DF_REF_P_H                 0x17U
#define LPS22DF_I3C_IF_CTRL_ADD         0x19U
#define LPS22DF_RPDS_L                  0x1AU
#define LPS22DF_RPDS_H                  0x1BU
#define LPS22DF_INT_SOURCE              0x24U
#define LPS22DF_FIFO_STATUS1            0x25U
#define LPS22DF_FIFO_STATUS2            0x26U
#define LPS22DF_STATUS                  0x27U
#define LPS22DF_PRESS_OUT_XL            0x28U
#define LPS22DF_PRESS_OUT_L             0x29U
#define LPS22DF_PRESS_OUT_H             0x2AU
#define LPS22DF_TEMP_OUT_L              0x2BU
#define LPS22DF_TEMP_OUT_H              0x2CU
#define LPS22DF_FIFO_DATA_OUT_PRESS_XL  0x78U
#define LPS22DF_FIFO_DATA_OUT_PRESS_L   0x79U
#define LPS22DF_FIFO_DATA_OUT_PRESS_H   0x7AU

/* CTRL_REG1 definitions */
#define	LPS22DF_ONE_SHOT 				(0 << 3)
#define	LPS22DF_1Hz      				(1 << 3)
#define	LPS22DF_4Hz						(2 << 3)
#define	LPS22DF_10Hz     				(3 << 3)
#define	LPS22DF_25Hz     				(4 << 3)
#define	LPS22DF_50Hz     				(5 << 3)
#define	LPS22DF_75Hz     				(6 << 3)
#define	LPS22DF_100Hz    				(7 << 3)
#define	LPS22DF_200Hz    				(8 << 3)

#define	LPS22DF_4_AVG					1
#define	LPS22DF_8_AVG					2
#define	LPS22DF_16_AVG					3
#define	LPS22DF_32_AVG					4
#define	LPS22DF_64_AVG					5
#define	LPS22DF_128_AVG					6
#define	LPS22DF_256_AVG					7
#define	LPS22DF_512_AVG					8

#define	LPS22DF_ONE_SHOT_REG2			0x01
#define	LPS22DF_RESET_REG2				0x04
#define	LPS22DF_LPF_REG2				0x10
#define	LPS22DF_BOOT_REG2				0x80

#define	LPS22DF_IE_REG4					0x30

#define	LPS22DF_BOOT_ON_REG_INT_SOURCE	0x80

/* status definitions */
#define	LPS22DF_P_DA_STATUS				0x01
#define	LPS22DF_T_DA_STATUS				0x02

extern	uint32_t LPS22DF_Init(void);
extern	uint8_t LPS22DF_GetWhoAmI(void);
extern	uint8_t LPS22DF_Read_P_Data(uint8_t *pressure_p_ptr);
extern	uint8_t LPS22DF_Read_T_Data(uint8_t *pressure_t_ptr);
extern	int32_t LPS22DF_Start_Acquisition(void);
extern	uint8_t LPS22DF_GetStatus(void);


#endif /* DRIVERS_SENSORS_LPS22DF_LPS22DF_H_ */

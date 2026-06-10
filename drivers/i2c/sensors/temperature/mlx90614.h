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
 * mlx90614.h
 *
 *  Created on: Feb 9, 2026
 *      Author: fil
 */

#ifndef DRIVERS_I2C_SENSORS_TEMPERATURE_MLX90614_H_
#define DRIVERS_I2C_SENSORS_TEMPERATURE_MLX90614_H_

// MLX90614 I2C Address (7-bit)
#define MLX90614_I2C_ADDR       0x5A<<1
#define MLX90614_I2C_TIMEOUT	100

// Register Map (RAM = volatile, EEPROM = non-volatile)
#define MLX90614_RAM_ACCESS     0x00  // RAM address pointer
#define MLX90614_RAM_RAW_IR1    0x04  // Raw IR channel 1
#define MLX90614_RAM_RAW_IR2    0x05  // Raw IR channel 2 (dual-zone sensors)
#define MLX90614_RAM_TA         0x06  // Ambient temperature
#define MLX90614_RAM_TOBJ1      0x07  // Object temperature 1 (default output)
#define MLX90614_RAM_TOBJ2      0x08  // Object temperature 2 (dual-zone)
#define MLX90614_RAM_TOMAX      0x09  // Max object temperature (in spot)
#define MLX90614_RAM_TOMIN      0x0A  // Min object temperature (in spot)
#define MLX90614_RAM_TUR        0x0B  // Unfiltered object temperature
#define MLX90614_RAM_TUC        0x0C  // Unfiltered ambient temperature
#define MLX90614_RAM_RES0       0x0D  // Reserved
#define MLX90614_RAM_RES1       0x0E  // Reserved
#define MLX90614_RAM_RES2       0x0F  // Reserved

#define MLX90614_EEPROM_ACCESS  0x20  // EEPROM address pointer
#define MLX90614_EEPROM_TOMAX   0x20  // Max object temperature threshold
#define MLX90614_EEPROM_TOMIN   0x21  // Min object temperature threshold
#define MLX90614_EEPROM_PWMCTRL 0x22  // PWM control configuration
#define MLX90614_EEPROM_TARANGE 0x23  // Ambient temperature range
#define MLX90614_EEPROM_EMISS   0x24  // Emissivity coefficient (16-bit)
#define MLX90614_EEPROM_CONFIG  0x25  // Sensor configuration register
#define MLX90614_EEPROM_ADDR    0x2E  // I2C address register
#define MLX90614_EEPROM_ID1     0x3C  // Device ID low word
#define MLX90614_EEPROM_ID2     0x3D  // Device ID high word
#define MLX90614_EEPROM_ID3     0x3E  // Device ID version
#define MLX90614_EEPROM_ID4     0x3F  // Device ID revision

// Configuration Register Bits (EEPROM[0x25])
#define MLX90614_CFG_PWR_MODE   (1 << 0)   // 0 = continuous, 1 = sleep mode
#define MLX90614_CFG_I2C_STR    (1 << 5)   // I2C stretch enable
#define MLX90614_CFG_PEC_EN     (1 << 6)   // PEC enable (must be set!)
#define MLX90614_CFG_RESOL      (3 << 8)   // Resolution bits (not used in most models)

// Temperature conversion factor
#define MLX90614_TEMP_FACTOR    0.02f      // LSB = 0.02°C

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
	uint16_t			mlx90614_found;
	uint8_t				pec_enabled;
	uint16_t			device_id;
	uint16_t			device_version;
	uint8_t 			rx_data[3];
	uint8_t 			crc_buff[5];
	uint8_t				crc;
	float	 			t_ambient,t_object;
}I2C_Mlx90614_DriverStruct_t;


extern uint32_t mlx90614_register(I2C_Mlx90614_DriverStruct_t *mlx90614_Drv);
extern uint32_t mlx90614_read_ambient(I2C_Mlx90614_DriverStruct_t *mlx90614_Drv);
extern uint32_t mlx90614_read_object(I2C_Mlx90614_DriverStruct_t *mlx90614_Drv);

#endif /* DRIVERS_I2C_SENSORS_TEMPERATURE_MLX90614_H_ */

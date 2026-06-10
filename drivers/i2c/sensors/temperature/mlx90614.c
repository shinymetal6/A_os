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
 * mlx90614.c
 *
 *  Created on: Feb 9, 2026
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "../../i2c.h"
#include "mlx90614.h"

static const uint8_t mlx90614_crc_table[] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31,
    0x24, 0x23, 0x2a, 0x2d, 0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
    0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d, 0xe0, 0xe7, 0xee, 0xe9,
    0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1,
    0xb4, 0xb3, 0xba, 0xbd, 0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
    0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea, 0xb7, 0xb0, 0xb9, 0xbe,
    0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16,
    0x03, 0x04, 0x0d, 0x0a, 0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
    0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a, 0x89, 0x8e, 0x87, 0x80,
    0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8,
    0xdd, 0xda, 0xd3, 0xd4, 0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
    0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44, 0x19, 0x1e, 0x17, 0x10,
    0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f,
    0x6a, 0x6d, 0x64, 0x63, 0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
    0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13, 0xae, 0xa9, 0xa0, 0xa7,
    0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef,
    0xfa, 0xfd, 0xf4, 0xf3
};

uint8_t mlx90614_crc8 (uint8_t *p, uint8_t len)
{
uint16_t i;
uint16_t crc = 0x0;

	while (len--)
	{
		i = (crc ^ *p++) & 0xff;
		crc = (mlx90614_crc_table[i] ^ (crc << 8)) & 0xff;
	}
	return crc & 0xff;
}

ITCM_AREA_CODE uint32_t mlx90614_read_ram(I2C_Mlx90614_DriverStruct_t *mlx90614_Drv, uint8_t reg, uint16_t *value)
{
	*value = 0x0000;
    if (HAL_I2C_Mem_Read(mlx90614_Drv->bus, mlx90614_Drv->device_address, reg,1,mlx90614_Drv->rx_data,3,100))
        return 1;

    if (mlx90614_Drv->pec_enabled)
    {
    	mlx90614_Drv->crc_buff[0] = (mlx90614_Drv->device_address);
    	mlx90614_Drv->crc_buff[1] = reg;
    	mlx90614_Drv->crc_buff[2] = (mlx90614_Drv->device_address) + 1;
    	mlx90614_Drv->crc_buff[3] = mlx90614_Drv->rx_data[0];
    	mlx90614_Drv->crc_buff[4] = mlx90614_Drv->rx_data[1];
    	mlx90614_Drv->crc = mlx90614_crc8(mlx90614_Drv->crc_buff, 5);
    	if (mlx90614_Drv->crc != mlx90614_Drv->rx_data[2])
            return 1;
    }
    *value = (mlx90614_Drv->rx_data[1] << 8) | mlx90614_Drv->rx_data[0];
    return 0;
}

// Read EEPROM register (16-bit value + PEC)
ITCM_AREA_CODE uint32_t mlx90614_read_eeprom(I2C_Mlx90614_DriverStruct_t *mlx90614_Drv, uint8_t address, uint16_t *value)
{
    // EEPROM access requires special command format: reg | 0x20
    uint16_t cmd = address | MLX90614_EEPROM_ACCESS;
	*value = 0x0000;
    if (HAL_I2C_Mem_Read(mlx90614_Drv->bus, mlx90614_Drv->device_address, cmd,1,mlx90614_Drv->rx_data,3,100))
        return 1;

    if (mlx90614_Drv->pec_enabled)
    {
    	mlx90614_Drv->crc_buff[0] = (mlx90614_Drv->device_address);
    	mlx90614_Drv->crc_buff[1] = address;
    	mlx90614_Drv->crc_buff[2] = (mlx90614_Drv->device_address) + 1;
    	mlx90614_Drv->crc_buff[3] = mlx90614_Drv->rx_data[0];
    	mlx90614_Drv->crc_buff[4] = mlx90614_Drv->rx_data[1];
    	mlx90614_Drv->crc = mlx90614_crc8(mlx90614_Drv->crc_buff, 5);
    	if (mlx90614_Drv->crc != mlx90614_Drv->rx_data[2])
            return 1;
    }
    *value = (mlx90614_Drv->rx_data[1] << 8) | mlx90614_Drv->rx_data[0];
    return 0;
}

ITCM_AREA_CODE uint32_t mlx90614_read_temp_reg(I2C_Mlx90614_DriverStruct_t *mlx90614_Drv, uint8_t reg, float *temp)
{
    uint16_t raw;
    *temp = 0;
    if ( mlx90614_read_ram(mlx90614_Drv, reg, &raw) )
    	return 1;

    // Convert raw value to Celsius -> T = (raw * 0.02) - 273.15
    *temp = (raw * MLX90614_TEMP_FACTOR) - 273.15f;

    // Validate temperature range (sensor-specific)
    if (*temp < -70.0f || *temp > 380.0f)
        return 1;
    return 0;
}

// Read ambient temperature only
ITCM_AREA_CODE uint32_t mlx90614_read_ambient(I2C_Mlx90614_DriverStruct_t *mlx90614_Drv)
{
    return mlx90614_read_temp_reg(mlx90614_Drv, MLX90614_RAM_TA, &mlx90614_Drv->t_ambient);
}

// Read object temperature only
ITCM_AREA_CODE uint32_t mlx90614_read_object(I2C_Mlx90614_DriverStruct_t *mlx90614_Drv)
{
    return mlx90614_read_temp_reg(mlx90614_Drv, MLX90614_RAM_TOBJ1, &mlx90614_Drv->t_object);
}

ITCM_AREA_CODE uint32_t mlx90614_get_id(I2C_Mlx90614_DriverStruct_t *mlx90614_Drv)
{
	mlx90614_read_eeprom(mlx90614_Drv, MLX90614_EEPROM_ID1, &mlx90614_Drv->device_id);
	mlx90614_read_eeprom(mlx90614_Drv, MLX90614_EEPROM_ID3, &mlx90614_Drv->device_version);
    return 0;
}

ITCM_AREA_CODE uint32_t mlx90614_register(I2C_Mlx90614_DriverStruct_t *mlx90614_Drv)
{
I2C_DriverStruct_t *eptr, *pre_eptr;
	if ( mlx90614_Drv->bus == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( i2c_drv_ptr == NULL)
	{
		i2c_drv_ptr = (I2C_DriverStruct_t *)mlx90614_Drv;
		mlx90614_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = i2c_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (I2C_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)mlx90614_Drv;
		mlx90614_Drv->next_drv = NULL;
	}
	mlx90614_Drv->process = get_current_process();
    mlx90614_Drv->pec_enabled = 1;  // PEC required for reliable operation
	if ( mlx90614_Drv->device_address == 0 )
		mlx90614_Drv->device_address = MLX90614_I2C_ADDR;
	mlx90614_get_id(mlx90614_Drv);
	return 0;
}

#endif // #ifdef A_OS_I2C_ENABLED




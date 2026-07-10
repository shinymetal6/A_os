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
 * sht21s.c
 *
 *  Created on: Jul 9, 2026
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "../../i2c.h"
#include "sht21s.h"

ITCM_AREA_CODE uint32_t sht21s_i2cwrite(I2C_Sht21s_DriverStruct_t *sht21s_Drv,uint8_t cmd)
{
uint8_t	ret = HAL_BUSY;

	sht21s_Drv->flags  &= ~I2C_STATUS_WRITE_COMPLETE;
	sht21s_Drv->timeout = SHT21S_I2C_TIMEOUT;
	while(ret == HAL_BUSY )
	{
		ret = HAL_I2C_Master_Transmit_IT(sht21s_Drv->bus, sht21s_Drv->device_address, &cmd,1);
		if ( ret == HAL_BUSY)
		{
			task_delay(1);
			sht21s_Drv->timeout--;
			if ( sht21s_Drv->timeout == 0 )
				return 1;
		}
	}
	return 0;
}

ITCM_AREA_CODE uint32_t sht21s_i2cread(I2C_Sht21s_DriverStruct_t *sht21s_Drv)
{
uint8_t	ret = HAL_BUSY;

	sht21s_Drv->flags  &= ~I2C_STATUS_READ_COMPLETE;
	sht21s_Drv->timeout = SHT40_I2C_TIMEOUT;
	while(ret == HAL_BUSY )
	{
		ret = HAL_I2C_Master_Receive_IT(sht21s_Drv->bus,sht21s_Drv->device_address, sht21s_Drv->data, 3);
		if ( ret == HAL_BUSY)
		{
			task_delay(1);
			sht21s_Drv->timeout--;
			if ( sht21s_Drv->timeout == 0 )
				return 1;
		}
	}
	return 0;
}

ITCM_AREA_CODE uint8_t sht21s_checkCRC(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum)
{
uint8_t crc = 0;
    for (uint8_t i = 0; i < nbrOfBytes; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 8; j > 0; j--)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ 0x31;
            }
            else
            {
                crc = (crc << 1);
            }
        }
    }
    return (crc == checksum);
}

ITCM_AREA_CODE uint32_t sht21s_ReadTemperature(I2C_Sht21s_DriverStruct_t *sht21s_Drv)
{
	return sht21s_i2cwrite(sht21s_Drv,SHT21_CMD_TRIG_TEMP_NHM);
}

ITCM_AREA_CODE float sht21s_ConvertTemperature(I2C_Sht21s_DriverStruct_t *sht21s_Drv)
{
	// Convert raw data to Celsius
	// The lowest 2 bits of the LSB are status bits and must be cleared (masked with 0xFC)
	uint16_t raw = (sht21s_Drv->data[0] << 8) | (sht21s_Drv->data[1] & 0xFC);
	sht21s_Drv->temperature = -46.85f + 175.72f * ((float)raw / 65536.0f);
	return sht21s_Drv->temperature;
}

ITCM_AREA_CODE uint32_t sht21s_ReadHumidity(I2C_Sht21s_DriverStruct_t *sht21s_Drv)
{
	return	sht21s_i2cwrite(sht21s_Drv,SHT21_CMD_TRIG_HUM_NHM);
}

ITCM_AREA_CODE float sht21s_ConvertHumidity(I2C_Sht21s_DriverStruct_t *sht21s_Drv)
{
	// Convert raw data to %RH
	uint16_t raw = (sht21s_Drv->data[0] << 8) | (sht21s_Drv->data[1] & 0xFC);
	sht21s_Drv->humidity = -6.0f + 125.0f * ((float)raw / 65536.0f);
	// Clamp humidity to physical limits (0% to 100%)
	if (sht21s_Drv->humidity > 100.0f)
		sht21s_Drv->humidity = 100.0f;
	if (sht21s_Drv->humidity < 0.0f)
		sht21s_Drv->humidity = 0.0f;
	return sht21s_Drv->humidity;
}

ITCM_AREA_CODE uint32_t sht21s_register(I2C_Sht21s_DriverStruct_t *sht21s_Drv)
{
I2C_DriverStruct_t *eptr;
	if ( sht21s_Drv->bus == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( i2c_drv_ptr == NULL)
	{
		i2c_drv_ptr = (I2C_DriverStruct_t *)sht21s_Drv;
		sht21s_Drv->next_drv = NULL;
	}
	else
	{
		eptr = i2c_drv_ptr;
		while(eptr->next_drv != NULL)
			eptr = (I2C_DriverStruct_t *)eptr->next_drv;
		eptr->next_drv = (uint32_t *)sht21s_Drv;
		sht21s_Drv->next_drv = NULL;
	}
	sht21s_Drv->process = get_current_process();
	sht21s_Drv->device_address = SHT21S_I2C_ADDR;
	sht21s_i2cwrite(sht21s_Drv,SHT21_CMD_SOFT_RESET);
	return 0;
}

#endif // #ifdef A_OS_I2C_ENABLED


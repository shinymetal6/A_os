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
 * ism330dlctr.c
 *
 *  Created on: Feb 26, 2026
 *      Author: fil
 */
#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "../../i2c.h"
#include "ism330dlctr.h"

ITCM_AREA_CODE static uint8_t ism330dlctr_WriteReg(I2C_ism330dlctr_DriverStruct_t *ism330dlctr_Drv, uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    return HAL_I2C_Master_Transmit(ism330dlctr_Drv->bus, ISM330DLCTR_I2C_ADDR, buf, 2, 100);
}

ITCM_AREA_CODE static uint8_t ism330dlctr_ReadReg(I2C_ism330dlctr_DriverStruct_t *ism330dlctr_Drv, uint8_t reg, uint8_t *data, uint16_t len)
{
    return HAL_I2C_Mem_Read(ism330dlctr_Drv->bus, ISM330DLCTR_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, len, 100);
}

ITCM_AREA_CODE uint8_t ism330dlctr_ReadAcc(I2C_ism330dlctr_DriverStruct_t *ism330dlctr_Drv, IMU_Data_t *data) {
    uint8_t raw[6];
    if (ism330dlctr_ReadReg(ism330dlctr_Drv, ISM330DLCTR_OUTX_L_XL, raw, 6) != HAL_OK) return 0;

    // Convert Raw to Float (Little Endian)
    // Sensitivity for ±4g is 0.122 mg/LSB (0.000122 g/LSB)
    int16_t x_raw = (int16_t)((raw[1] << 8) | raw[0]);
    int16_t y_raw = (int16_t)((raw[3] << 8) | raw[2]);
    int16_t z_raw = (int16_t)((raw[5] << 8) | raw[4]);

    ism330dlctr_Drv->acc_x = x_raw * 0.000122f;
    ism330dlctr_Drv->acc_y = y_raw * 0.000122f;
    ism330dlctr_Drv->acc_z = z_raw * 0.000122f;

    return 1;
}

ITCM_AREA_CODE uint8_t ism330dlctr_ReadGyro(I2C_ism330dlctr_DriverStruct_t *ism330dlctr_Drv, IMU_Data_t *data) {
    uint8_t raw[6];
    if (ism330dlctr_ReadReg(ism330dlctr_Drv, ISM330DLCTR_OUTX_L_G, raw, 6) != HAL_OK) return 0;

    // Convert Raw to Float (Little Endian)
    // Sensitivity for ±2000dps is 70 mdps/LSB (0.070 dps/LSB)
    int16_t x_raw = (int16_t)((raw[1] << 8) | raw[0]);
    int16_t y_raw = (int16_t)((raw[3] << 8) | raw[2]);
    int16_t z_raw = (int16_t)((raw[5] << 8) | raw[4]);

    ism330dlctr_Drv->gyro_x = x_raw * 0.070f;
    ism330dlctr_Drv->gyro_y = y_raw * 0.070f;
    ism330dlctr_Drv->gyro_z = z_raw * 0.070f;

    return 1;
}

ITCM_AREA_CODE uint32_t ism330dlctr_register(I2C_ism330dlctr_DriverStruct_t *ism330dlctr_Drv)
{
I2C_DriverStruct_t *eptr, *pre_eptr;
	if ( ism330dlctr_Drv->bus == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( ism330dlctr_Drv->device_address == 0 )
		ism330dlctr_Drv->device_address = ISM330DLCTR_I2C_ADDR;
	if ( HAL_I2C_IsDeviceReady(ism330dlctr_Drv->bus,ism330dlctr_Drv->device_address,5,IMX335_TIMEOUT) )
		return DRIVER_REQUEST_FAILED;
    // 1. Check Who Am I
    if (ism330dlctr_ReadReg(ism330dlctr_Drv, ISM330DLCTR_WHO_AM_I_REG, &ism330dlctr_Drv->who_am_i, 1) != HAL_OK)
    	return 1;
    if (ism330dlctr_Drv->who_am_i != ISM330DLCTR_WHO_AM_I)
    	return 1;
    // 2. Allocate driver
	if ( i2c_drv_ptr == NULL)
	{
		i2c_drv_ptr = (I2C_DriverStruct_t *)ism330dlctr_Drv;
		ism330dlctr_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = i2c_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (I2C_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)ism330dlctr_Drv;
		ism330dlctr_Drv->next_drv = NULL;
	}
	ism330dlctr_Drv->process = get_current_process();

    // 3. Enable Block Data Update (Prevents reading half-updated data)
    ism330dlctr_WriteReg(ism330dlctr_Drv, ISM330DLCTR_CTRL3_C, ISM330DLCTR_BDU_ENABLE);

    // 4. Configure Accelerometer (ODR 104Hz, Full Scale ±4g)
    ism330dlctr_WriteReg(ism330dlctr_Drv, ISM330DLCTR_CTRL1_XL, ISM330DLCTR_ODR_104Hz | ISM330DLCTR_FS_XL_4G);

    // 5. Configure Gyroscope (ODR 104Hz, Full Scale ±2000dps)
    ism330dlctr_WriteReg(ism330dlctr_Drv, ISM330DLCTR_CTRL2_G, ISM330DLCTR_ODR_104Hz | ISM330DLCTR_FS_G_2000DPS);
	return 0;
}
#endif //#ifdef A_OS_I2C_ENABLED

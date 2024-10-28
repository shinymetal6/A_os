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
 * lis2dw12.c
 *
 *  Created on: Mar 13, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef SENSORS_LIS2DW12

#include "lis2dw12.h"
#include <string.h>

static uint8_t	read_lis_reg(uint8_t address)
{
uint8_t	rreg;
	hw_i2c_MemGet8(SENSORS_LIS2DW12_I2C_INDEX,LIS2DW12_ADDR,(uint16_t )address,&rreg, 1);
	return rreg;
}

static uint8_t	write_lis_reg(uint8_t address,uint8_t data)
{
	return hw_i2c_MemSend8(SENSORS_LIS2DW12_I2C_INDEX,LIS2DW12_ADDR, (uint16_t )address,&data, 1);
}

uint8_t LIS2DW12_GetWhoAmI(void)
{
	read_lis_reg(LIS2DW12_WHO_AM_I);
	return read_lis_reg(LIS2DW12_WHO_AM_I);
}

uint8_t LIS2DW12_ReadAccRegs(uint8_t *axis_ptr)
{
	return hw_i2c_MemGet8(SENSORS_LIS2DW12_I2C_INDEX,LIS2DW12_ADDR,(uint16_t )LIS2DW12_OUT_X_L,axis_ptr, 6);
}

uint8_t LIS2DW12_GetStatusReg(void)
{
	return read_lis_reg(LIS2DW12_STATUS);
}

uint8_t LIS2DW12_SetCTRL_2_Reg(void)
{
	write_lis_reg(LIS2DW12_CTRL2,0x40);
	HAL_Delay(1);
	write_lis_reg(LIS2DW12_CTRL2,0x80);
	HAL_Delay(1);
	return write_lis_reg(LIS2DW12_CTRL2,0x14);
}

uint8_t LIS2DW12_GetWakeSource(void)
{
	return read_lis_reg(LIS2DW12_WAKE_UP_SRC);
}

uint8_t LIS2DW12_GetFifoSamples(void)
{
	return read_lis_reg(LIS2DW12_FIFO_SAMPLES) & 0x3f;
}

uint8_t LIS2DW12_GetFifoSamplesReg(void)
{
	return read_lis_reg(LIS2DW12_FIFO_SAMPLES);
}

void LIS2DW12_ConfigureFIFO(uint8_t FIFOMode, uint8_t FIFOThreshold)
{
	write_lis_reg(LIS2DW12_FIFO_CTRL, FIFOMode << 5 | (FIFOThreshold-1) );
}

uint8_t LIS2DW12_Reset(void)
{
uint8_t temp;
	temp = read_lis_reg(LIS2DW12_CTRL2);
	write_lis_reg(LIS2DW12_CTRL2, temp | 0x40); // software reset the LIS2DW12
	return read_lis_reg(LIS2DW12_STATUS);
}

//https://github.com/kriswiner/LIS2DW12/blob/main/LIS2DW12_Tap_OrientationDetect_Ladybug/LIS2DW12.cpp#L36

uint8_t LIS2DW12_GetTempReg(void)
{
	return read_lis_reg(LIS2DW12_OUT_T);
}


uint8_t LIS2DW12_GetAccData(uint8_t data_index,uint8_t *axis_regs)
{
uint8_t	i,k;
	for(i=0,k=data_index*MAG_LEN;i<LIS2DW12_NUM_FIFO_LOCATIONS;i++,k+=MAG_LEN)
	{
		LIS2DW12_ReadAccRegs(&axis_regs[k]);
	}
	return 32;
}

uint8_t LIS2DW12_Initialize(uint8_t irq_mode,uint8_t fifo_locations,uint8_t *id,uint8_t *status,uint8_t *ie)
{
	write_lis_reg( LIS2DW12_CTRL1 , LIS2DW12_LP_MODE_1 | LIS2DW12_MODE_HIGH_PERF | LIS2DW12_ODR_100Hz );
	write_lis_reg( LIS2DW12_CTRL6, LIS2DW12_BW_FILT_ODR20 | LIS2DW12_FS_16G);
	write_lis_reg( LIS2DW12_CTRL2 , LIS2DW12_BOOT | LIS2DW12_BDU | LIS2DW12_IF_ADD_INC);
	write_lis_reg( LIS2DW12_CTRL3 , 0x0);	// pulsed irq
	if ( irq_mode == IRQ_MODE_ON_WAKEUP )
		write_lis_reg( LIS2DW12_CTRL4_INT1_PAD_CTRL , LIS2DW12_INT1_WU);
	else
		write_lis_reg( LIS2DW12_CTRL4_INT1_PAD_CTRL , LIS2DW12_INT1_FTH);

	write_lis_reg( LIS2DW12_CTRL5_INT2_PAD_CTRL , 0);
	write_lis_reg( LIS2DW12_WAKE_UP_THS ,  LIS2DW12_SINGLE_DOUBLE_TAP | 16);
	write_lis_reg( LIS2DW12_WAKE_UP_DUR ,  0x10);
	write_lis_reg( LIS2DW12_CTRL_REG7, LIS2DW12_DRDY_PULSED | LIS2DW12_INTERRUPTS_ENABLE | LIS2DW12_INT2_ON_INT1);

	LIS2DW12_ConfigureFIFO(LISDW_FIFOMODE_BYPASS, fifo_locations);		// clear fifo
	LIS2DW12_ConfigureFIFO(LISDW_FIFOMODE_CONTINUOUS, fifo_locations);// restart fifo

	*ie = read_lis_reg(LIS2DW12_CTRL4_INT1_PAD_CTRL);
	*status = LIS2DW12_GetStatusReg();
	return 0;
}

uint8_t	lis_who_am_i,lis_id,lis_status,lis_ie;

uint8_t LIS2DW12_Init(void)
{
	LIS2DW12_SetCTRL_2_Reg();
	LIS2DW12_GetStatusReg();
	lis_who_am_i = LIS2DW12_GetWhoAmI();
	return LIS2DW12_Initialize(IRQ_MODE_ON_FTH,LIS2DW12_NUM_FIFO_LOCATIONS,&lis_id,&lis_status,&lis_ie);
}

#endif // #ifdef SENSORS_LIS2DW12


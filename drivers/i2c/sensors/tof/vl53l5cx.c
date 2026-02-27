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
 * vl53l5cx.c
 *
 *  Created on: Feb 26, 2026
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "../../i2c.h"
#include "vl53l5cx.h"
#include "vl53l5cx_buffers.h"

ITCM_AREA_CODE uint32_t vl53l5cx_WrByte(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv,uint16_t RegisterAddress, uint8_t value)
{
	return HAL_I2C_Mem_Write(vl53l5cx_Drv->bus, vl53l5cx_Drv->device_address,RegisterAddress, I2C_MEMADD_SIZE_16BIT, &value, 1, 100);
}

ITCM_AREA_CODE uint32_t vl53l5cx_WrMulti(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv,uint16_t RegisterAddress, uint8_t *p_value, uint32_t size)
{
	return HAL_I2C_Mem_Write(vl53l5cx_Drv->bus, vl53l5cx_Drv->device_address,RegisterAddress, I2C_MEMADD_SIZE_16BIT, p_value, size, 50000);
}

ITCM_AREA_CODE uint32_t vl53l5cx_RdByte(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv,uint16_t RegisterAddress, uint8_t *p_value)
{
	return HAL_I2C_Mem_Read(vl53l5cx_Drv->bus, vl53l5cx_Drv->device_address,RegisterAddress, I2C_MEMADD_SIZE_16BIT, p_value, 1, 100);
}

ITCM_AREA_CODE uint32_t vl53l5cx_RdMulti(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv,uint16_t RegisterAddress, uint8_t *p_values, uint32_t size)
{
	return HAL_I2C_Mem_Read(vl53l5cx_Drv->bus, vl53l5cx_Drv->device_address,RegisterAddress, I2C_MEMADD_SIZE_16BIT, p_values, size, 50000);
}

ITCM_AREA_CODE void SwapBuffer(uint8_t *buffer,uint16_t  size)
{
uint32_t i, tmp;

  /* Example of possible implementation using <string.h> */
  for(i = 0; i < size; i = i + 4)
  {
    tmp = (
      buffer[i]<<24)
    |(buffer[i+1]<<16)
    |(buffer[i+2]<<8)
    |(buffer[i+3]);

    memcpy(&(buffer[i]), &tmp, 4);
  }
}

ITCM_AREA_CODE static uint8_t vl53l5cx_poll_for_mcu_boot(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv)
{
uint8_t go2_status0, go2_status1, status = 0;
uint16_t timeout = 0;

	do {
		status |= vl53l5cx_RdByte(vl53l5cx_Drv, 0x06, &go2_status0);
		if((go2_status0 & (uint8_t)0x80) != (uint8_t)0)
		{
			status |= vl53l5cx_RdByte(vl53l5cx_Drv, 0x07, &go2_status1);
			status |= go2_status1;
			break;
		}
		task_delay(1);;
		timeout++;

		if((go2_status0 & (uint8_t)0x1) != (uint8_t)0)
		{
			break;
		}
	}while (timeout < (uint16_t)500);

return status;
}

ITCM_AREA_CODE static uint8_t _vl53l5cx_poll_for_answer(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv,	uint8_t	size,uint8_t pos,uint16_t address,uint8_t mask,uint8_t expected_value)
{
	uint8_t timeout = 0;

	do {
		if ( vl53l5cx_RdMulti(vl53l5cx_Drv, address,	vl53l5cx_Drv->temp_buffer, size) )
			return 1;
		task_delay(20);

		/* 2s timeout */
		if((timeout >= (uint8_t)200) || ((size >= (uint8_t)4) && (vl53l5cx_Drv->temp_buffer[2] >= (uint8_t)0x7f)) )
			return 1;
		else
			timeout++;
	}while ((vl53l5cx_Drv->temp_buffer[pos] & mask) != expected_value);

	return 0;
}

ITCM_AREA_CODE static uint8_t _vl53l5cx_send_xtalk_data(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv,	uint8_t	resolution)
{
	uint8_t status = VL53L5CX_STATUS_OK;
	uint8_t res4x4[] = {0x0F, 0x04, 0x04, 0x17, 0x08, 0x10, 0x10, 0x07};
	uint8_t dss_4x4[] = {0x00, 0x78, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08};
	uint8_t profile_4x4[] = {0xA0, 0xFC, 0x01, 0x00};
	uint32_t signal_grid[64];
	int8_t i, j;

	(void)memcpy(vl53l5cx_Drv->temp_buffer, &(vl53l5cx_Drv->xtalk_data[0]),
		VL53L5CX_XTALK_BUFFER_SIZE);

	/* Data extrapolation is required for 4X4 Xtalk */
	if(resolution == (uint8_t)VL53L5CX_RESOLUTION_4X4)
	{
		(void)memcpy(&(vl53l5cx_Drv->temp_buffer[0x8]),
			res4x4, sizeof(res4x4));
		(void)memcpy(&(vl53l5cx_Drv->temp_buffer[0x020]),
			dss_4x4, sizeof(dss_4x4));

		SwapBuffer(vl53l5cx_Drv->temp_buffer, VL53L5CX_XTALK_BUFFER_SIZE);
		(void)memcpy(signal_grid, &(vl53l5cx_Drv->temp_buffer[0x34]),
			sizeof(signal_grid));

		for (j = 0; j < (int8_t)4; j++)
		{
			for (i = 0; i < (int8_t)4 ; i++)
			{
				signal_grid[i+(4*j)] =
				(signal_grid[(2*i)+(16*j)+0]
				+ signal_grid[(2*i)+(16*j)+1]
				+ signal_grid[(2*i)+(16*j)+8]
				+ signal_grid[(2*i)+(16*j)+9])/(uint32_t)4;
			}
		}
	    (void)memset(&signal_grid[0x10], 0, (uint32_t)192);
	    (void)memcpy(&(vl53l5cx_Drv->temp_buffer[0x34]),
                  signal_grid, sizeof(signal_grid));
	    SwapBuffer(vl53l5cx_Drv->temp_buffer, VL53L5CX_XTALK_BUFFER_SIZE);
	    (void)memcpy(&(vl53l5cx_Drv->temp_buffer[0x134]),
	    profile_4x4, sizeof(profile_4x4));
	    (void)memset(&(vl53l5cx_Drv->temp_buffer[0x078]),0 ,
                         (uint32_t)4*sizeof(uint8_t));
	}

	status |= vl53l5cx_WrMulti(vl53l5cx_Drv, 0x2cf8,	vl53l5cx_Drv->temp_buffer, VL53L5CX_XTALK_BUFFER_SIZE);
	status |=_vl53l5cx_poll_for_answer(vl53l5cx_Drv, 4, 1,VL53L5CX_UI_CMD_STATUS, 0xff, 0x03);

	return status;
}

ITCM_AREA_CODE static uint8_t _vl53l5cx_send_offset_data(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv,uint8_t	resolution)
{
	uint8_t status = VL53L5CX_STATUS_OK;
	uint32_t signal_grid[64];
	int16_t range_grid[64];
	uint8_t dss_4x4[] = {0x0F, 0x04, 0x04, 0x00, 0x08, 0x10, 0x10, 0x07};
	uint8_t footer[] = {0x00, 0x00, 0x00, 0x0F, 0x03, 0x01, 0x01, 0xE4};
	int8_t i, j;
	uint16_t k;

	(void)memcpy(vl53l5cx_Drv->temp_buffer,
               vl53l5cx_Drv->offset_data, VL53L5CX_OFFSET_BUFFER_SIZE);

	/* Data extrapolation is required for 4X4 offset */
	if(resolution == (uint8_t)VL53L5CX_RESOLUTION_4X4){
		(void)memcpy(&(vl53l5cx_Drv->temp_buffer[0x10]), dss_4x4, sizeof(dss_4x4));
		SwapBuffer(vl53l5cx_Drv->temp_buffer, VL53L5CX_OFFSET_BUFFER_SIZE);
		(void)memcpy(signal_grid,&(vl53l5cx_Drv->temp_buffer[0x3C]),
			sizeof(signal_grid));
		(void)memcpy(range_grid,&(vl53l5cx_Drv->temp_buffer[0x140]),
			sizeof(range_grid));

		for (j = 0; j < (int8_t)4; j++)
		{
			for (i = 0; i < (int8_t)4 ; i++)
			{
				signal_grid[i+(4*j)] =
				(signal_grid[(2*i)+(16*j)+ (int8_t)0]
				+ signal_grid[(2*i)+(16*j)+(int8_t)1]
				+ signal_grid[(2*i)+(16*j)+(int8_t)8]
				+ signal_grid[(2*i)+(16*j)+(int8_t)9])
                                  /(uint32_t)4;
				range_grid[i+(4*j)] =
				(range_grid[(2*i)+(16*j)]
				+ range_grid[(2*i)+(16*j)+1]
				+ range_grid[(2*i)+(16*j)+8]
				+ range_grid[(2*i)+(16*j)+9])
                                  /(int16_t)4;
			}
		}
	    (void)memset(&range_grid[0x10], 0, (uint16_t)96);
	    (void)memset(&signal_grid[0x10], 0, (uint16_t)192);
            (void)memcpy(&(vl53l5cx_Drv->temp_buffer[0x3C]),
		signal_grid, sizeof(signal_grid));
            (void)memcpy(&(vl53l5cx_Drv->temp_buffer[0x140]),
		range_grid, sizeof(range_grid));
            SwapBuffer(vl53l5cx_Drv->temp_buffer, VL53L5CX_OFFSET_BUFFER_SIZE);
	}

	for(k = 0; k < (VL53L5CX_OFFSET_BUFFER_SIZE - (uint16_t)4); k++)
	{
		vl53l5cx_Drv->temp_buffer[k] = vl53l5cx_Drv->temp_buffer[k + (uint16_t)8];
	}

	(void)memcpy(&(vl53l5cx_Drv->temp_buffer[0x1E0]), footer, 8);
	status |= vl53l5cx_WrMulti(vl53l5cx_Drv, 0x2e18, vl53l5cx_Drv->temp_buffer,	VL53L5CX_OFFSET_BUFFER_SIZE);
	status |=_vl53l5cx_poll_for_answer(vl53l5cx_Drv, 4, 1,VL53L5CX_UI_CMD_STATUS, 0xff, 0x03);

	return status;
}

ITCM_AREA_CODE uint8_t vl53l5cx_dci_write_data(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv,uint8_t *data,uint32_t index,uint16_t	data_size)
{
	uint8_t status = VL53L5CX_STATUS_OK;
	int16_t i;

	uint8_t headers[] = {0x00, 0x00, 0x00, 0x00};
	uint8_t footer[] = {0x00, 0x00, 0x00, 0x0f, 0x05, 0x01,
			(uint8_t)((data_size + (uint16_t)8) >> 8),
			(uint8_t)((data_size + (uint16_t)8) & (uint8_t)0xFF)};

	uint16_t address = (uint16_t)VL53L5CX_UI_CMD_END -
		(data_size + (uint16_t)12) + (uint16_t)1;

	/* Check if cmd buffer is large enough */
	if((data_size + (uint16_t)12) > (uint16_t)VL53L5CX_TEMPORARY_BUFFER_SIZE)
	{
		status |= VL53L5CX_STATUS_ERROR;
	}
	else
	{
		headers[0] = (uint8_t)(index >> 8);
		headers[1] = (uint8_t)(index & (uint32_t)0xff);
		headers[2] = (uint8_t)(((data_size & (uint16_t)0xff0) >> 4));
		headers[3] = (uint8_t)((data_size & (uint16_t)0xf) << 4);

	/* Copy data from structure to FW format (+4 bytes to add header) */
		SwapBuffer(data, data_size);
		for(i = (int16_t)data_size - (int16_t)1 ; i >= 0; i--)
		{
			vl53l5cx_Drv->temp_buffer[i + 4] = data[i];
		}

	/* Add headers and footer */
		(void)memcpy(&vl53l5cx_Drv->temp_buffer[0], headers, sizeof(headers));
		(void)memcpy(&vl53l5cx_Drv->temp_buffer[data_size + (uint16_t)4],
			footer, sizeof(footer));

	/* Send data to FW */
		status |= vl53l5cx_WrMulti(vl53l5cx_Drv,address,vl53l5cx_Drv->temp_buffer,(uint32_t)((uint32_t)data_size + (uint32_t)12));
		status |= _vl53l5cx_poll_for_answer(vl53l5cx_Drv, 4, 1,
			VL53L5CX_UI_CMD_STATUS, 0xff, 0x03);

		SwapBuffer(data, data_size);
	}

	return status;
}

ITCM_AREA_CODE uint8_t vl53l5cx_dci_read_data(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv,uint8_t *data,uint32_t index,uint16_t	data_size)
{
	int16_t i;
	uint8_t status = VL53L5CX_STATUS_OK;
        uint32_t rd_size = (uint32_t) data_size + (uint32_t)12;
	uint8_t cmd[] = {0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x0f,
			0x00, 0x02, 0x00, 0x08};

	/* Check if tmp buffer is large enough */
	if((data_size + (uint16_t)12)>(uint16_t)VL53L5CX_TEMPORARY_BUFFER_SIZE)
	{
		status |= VL53L5CX_STATUS_ERROR;
	}
	else
	{
		cmd[0] = (uint8_t)(index >> 8);
		cmd[1] = (uint8_t)(index & (uint32_t)0xff);
		cmd[2] = (uint8_t)((data_size & (uint16_t)0xff0) >> 4);
		cmd[3] = (uint8_t)((data_size & (uint16_t)0xf) << 4);

	/* Request data reading from FW */
		status |= vl53l5cx_WrMulti(vl53l5cx_Drv,(VL53L5CX_UI_CMD_END-(uint16_t)11),cmd, sizeof(cmd));
		status |= _vl53l5cx_poll_for_answer(vl53l5cx_Drv, 4, 1,	VL53L5CX_UI_CMD_STATUS,	0xff, 0x03);

	/* Read new data sent (4 bytes header + data_size + 8 bytes footer) */
		status |= vl53l5cx_RdMulti(vl53l5cx_Drv, VL53L5CX_UI_CMD_START,	vl53l5cx_Drv->temp_buffer, rd_size);
		SwapBuffer(vl53l5cx_Drv->temp_buffer, data_size + (uint16_t)12);

	/* Copy data from FW into input structure (-4 bytes to remove header) */
		for(i = 0 ; i < (int16_t)data_size;i++){
			data[i] = vl53l5cx_Drv->temp_buffer[i + 4];
		}
	}

	return status;
}

ITCM_AREA_CODE uint8_t vl53l5cx_get_resolution(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv,uint8_t *p_resolution)
{
	uint8_t status = VL53L5CX_STATUS_OK;

	status |= vl53l5cx_dci_read_data(vl53l5cx_Drv, vl53l5cx_Drv->temp_buffer,VL53L5CX_DCI_ZONE_CONFIG, 8);
	*p_resolution = vl53l5cx_Drv->temp_buffer[0x00]*vl53l5cx_Drv->temp_buffer[0x01];
	return status;
}

ITCM_AREA_CODE uint8_t vl53l5cx_dci_replace_data(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv,
		uint8_t				*data,
		uint32_t			index,
		uint16_t			data_size,
		uint8_t				*new_data,
		uint16_t			new_data_size,
		uint16_t			new_data_pos)
{
	uint8_t status = VL53L5CX_STATUS_OK;

	status |= vl53l5cx_dci_read_data(vl53l5cx_Drv, data, index, data_size);
	(void)memcpy(&(data[new_data_pos]), new_data, new_data_size);
	status |= vl53l5cx_dci_write_data(vl53l5cx_Drv, data, index, data_size);

	return status;
}

ITCM_AREA_CODE static uint8_t vl53l5cx_init(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv)
{
uint8_t tmp, status = 0;
uint8_t pipe_ctrl[] = {VL53L5CX_NB_TARGET_PER_ZONE, 0x00, 0x01, 0x00};
uint32_t single_range = 0x01;

	vl53l5cx_Drv->default_xtalk = (uint8_t*)VL53L5CX_DEFAULT_XTALK;
	vl53l5cx_Drv->default_configuration = (uint8_t*)VL53L5CX_DEFAULT_CONFIGURATION;
	vl53l5cx_Drv->is_auto_stop_enabled = (uint8_t)0x0;

	/* SW reboot sequence */
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x0009, 0x04);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x000F, 0x40);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x000A, 0x03);
	status |= vl53l5cx_RdByte(vl53l5cx_Drv, 0x7FFF, &tmp);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x000C, 0x01);

	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x0101, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x0102, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x010A, 0x01);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x4002, 0x01);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x4002, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x010A, 0x03);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x0103, 0x01);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x000C, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x000F, 0x43);
	task_delay(1);

	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x000F, 0x40);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x000A, 0x01);
	task_delay( 100);

	/* Wait for sensor booted (several ms required to get sensor ready ) */
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x00);
	status |= _vl53l5cx_poll_for_answer(vl53l5cx_Drv, 1, 0, 0x06, 0xff, 1);
	if(status != (uint8_t)0){
		return 1;
	}

	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x000E, 0x01);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x02);

	/* Enable FW access */
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x03, 0x0D);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x01);
	status |= _vl53l5cx_poll_for_answer(vl53l5cx_Drv, 1, 0, 0x21, 0x10, 0x10);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x00);

	/* Enable host access to GO1 */
	status |= vl53l5cx_RdByte(vl53l5cx_Drv, 0x7fff, &tmp);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x0C, 0x01);

	/* Power ON status */
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x101, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x102, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x010A, 0x01);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x4002, 0x01);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x4002, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x010A, 0x03);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x103, 0x01);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x400F, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x21A, 0x43);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x21A, 0x03);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x21A, 0x01);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x21A, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x219, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x21B, 0x00);

	/* Wake up MCU */
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x00);
	status |= vl53l5cx_RdByte(vl53l5cx_Drv, 0x7fff, &tmp);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x0C, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x01);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x20, 0x07);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x20, 0x06);

	/* Download FW into VL53L5 */
	if ( vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x09) )
		return 1;
	if (vl53l5cx_WrMulti(vl53l5cx_Drv,0,(uint8_t*)&VL53L5CX_FIRMWARE[0],0x8000) )
		return 1;
	if ( vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x0a) )
		return 1;
	if ( vl53l5cx_WrMulti(vl53l5cx_Drv,0,(uint8_t*)&VL53L5CX_FIRMWARE[0x8000],0x8000) )
		return 1;
	if ( vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x0b) )
		return 1;
	if ( vl53l5cx_WrMulti(vl53l5cx_Drv,0,(uint8_t*)&VL53L5CX_FIRMWARE[0x10000],0x5000) )
		return 1;
	if ( vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x01) )
		return 1;

	/* Check if FW correctly downloaded */
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x02);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x03, 0x0D);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x01);
	status |= _vl53l5cx_poll_for_answer(vl53l5cx_Drv, 1, 0, 0x21, 0x10, 0x10);
	if(status != (uint8_t)0){
		return 1;
	}

	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x00);
	status |= vl53l5cx_RdByte(vl53l5cx_Drv, 0x7fff, &tmp);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x0C, 0x01);

	/* Reset MCU and wait boot */
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7FFF, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x114, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x115, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x116, 0x42);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x117, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x0B, 0x00);
	status |= vl53l5cx_RdByte(vl53l5cx_Drv, 0x7fff, &tmp);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x0C, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x0B, 0x01);
	status |= vl53l5cx_poll_for_mcu_boot(vl53l5cx_Drv);
	if(status != (uint8_t)0){
		return 1;
	}


	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x02);

	/* Get offset NVM data and store them into the offset buffer */
	status |= vl53l5cx_WrMulti(vl53l5cx_Drv, 0x2fd8,
		(uint8_t*)VL53L5CX_GET_NVM_CMD, sizeof(VL53L5CX_GET_NVM_CMD));
	status |= _vl53l5cx_poll_for_answer(vl53l5cx_Drv, 4, 0,VL53L5CX_UI_CMD_STATUS, 0xff, 2);
	status |= vl53l5cx_RdMulti(vl53l5cx_Drv, VL53L5CX_UI_CMD_START,vl53l5cx_Drv->temp_buffer, VL53L5CX_NVM_DATA_SIZE);
	(void)memcpy(vl53l5cx_Drv->offset_data, vl53l5cx_Drv->temp_buffer,VL53L5CX_OFFSET_BUFFER_SIZE);
	status |= _vl53l5cx_send_offset_data(vl53l5cx_Drv, VL53L5CX_RESOLUTION_4X4);

	/* Set default Xtalk shape. Send Xtalk to sensor */
	(void)memcpy(vl53l5cx_Drv->xtalk_data, (uint8_t*)VL53L5CX_DEFAULT_XTALK,VL53L5CX_XTALK_BUFFER_SIZE);
	status |= _vl53l5cx_send_xtalk_data(vl53l5cx_Drv, VL53L5CX_RESOLUTION_4X4);

	/* Send default configuration to VL53L5CX firmware */
	status |= vl53l5cx_WrMulti(vl53l5cx_Drv, 0x2c34,
		vl53l5cx_Drv->default_configuration,
		sizeof(VL53L5CX_DEFAULT_CONFIGURATION));
	status |= _vl53l5cx_poll_for_answer(vl53l5cx_Drv, 4, 1,
		VL53L5CX_UI_CMD_STATUS, 0xff, 0x03);

	status |= vl53l5cx_dci_write_data(vl53l5cx_Drv, (uint8_t*)&pipe_ctrl,VL53L5CX_DCI_PIPE_CONTROL, (uint16_t)sizeof(pipe_ctrl));
#if VL53L5CX_NB_TARGET_PER_ZONE != 1
	tmp = VL53L5CX_NB_TARGET_PER_ZONE;
	status |= vl53l5cx_dci_replace_data(vl53l5cx_Drv, vl53l5cx_Drv->temp_buffer,
		VL53L5CX_DCI_FW_NB_TARGET, 16,
	(uint8_t*)&tmp, 1, 0x0C);
#endif

	status |= vl53l5cx_dci_write_data(vl53l5cx_Drv, (uint8_t*)&single_range,VL53L5CX_DCI_SINGLE_RANGE,(uint16_t)sizeof(single_range));

	tmp = (uint8_t)1;
	status |= vl53l5cx_dci_replace_data(vl53l5cx_Drv, vl53l5cx_Drv->temp_buffer,VL53L5CX_GLARE_FILTER, 40, (uint8_t*)&tmp, 1, 0x26);
	status |= vl53l5cx_dci_replace_data(vl53l5cx_Drv, vl53l5cx_Drv->temp_buffer,VL53L5CX_GLARE_FILTER, 40, (uint8_t*)&tmp, 1, 0x25);
	return status;
}


ITCM_AREA_CODE uint8_t vl53l5cx_start_ranging(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv)
{
	uint8_t resolution, status = VL53L5CX_STATUS_OK;
	uint16_t tmp;
	uint32_t i;
	uint32_t header_config[2] = {0, 0};

	union Block_header *bh_ptr;
	uint8_t cmd[] = {0x00, 0x03, 0x00, 0x00};

	status |= vl53l5cx_get_resolution(vl53l5cx_Drv, &resolution);
	vl53l5cx_Drv->data_read_size = 0;
	vl53l5cx_Drv->streamcount = 255;

	/* Enable mandatory output (meta and common data) */
	uint32_t output_bh_enable[] = {
		0x00000007U,
		0x00000000U,
		0x00000000U,
		0xC0000000U};

	/* Send addresses of possible output */
	uint32_t output[] ={VL53L5CX_START_BH,
		VL53L5CX_METADATA_BH,
		VL53L5CX_COMMONDATA_BH,
		VL53L5CX_AMBIENT_RATE_BH,
		VL53L5CX_SPAD_COUNT_BH,
		VL53L5CX_NB_TARGET_DETECTED_BH,
		VL53L5CX_SIGNAL_RATE_BH,
		VL53L5CX_RANGE_SIGMA_MM_BH,
		VL53L5CX_DISTANCE_BH,
		VL53L5CX_REFLECTANCE_BH,
		VL53L5CX_TARGET_STATUS_BH,
		VL53L5CX_MOTION_DETECT_BH};

	/* Enable selected outputs in the 'platform.h' file */
#ifndef VL53L5CX_DISABLE_AMBIENT_PER_SPAD
	output_bh_enable[0] += (uint32_t)8;
#endif
#ifndef VL53L5CX_DISABLE_NB_SPADS_ENABLED
	output_bh_enable[0] += (uint32_t)16;
#endif
#ifndef VL53L5CX_DISABLE_NB_TARGET_DETECTED
	output_bh_enable[0] += (uint32_t)32;
#endif
#ifndef VL53L5CX_DISABLE_SIGNAL_PER_SPAD
	output_bh_enable[0] += (uint32_t)64;
#endif
#ifndef VL53L5CX_DISABLE_RANGE_SIGMA_MM
	output_bh_enable[0] += (uint32_t)128;
#endif
#ifndef VL53L5CX_DISABLE_DISTANCE_MM
	output_bh_enable[0] += (uint32_t)256;
#endif
#ifndef VL53L5CX_DISABLE_REFLECTANCE_PERCENT
	output_bh_enable[0] += (uint32_t)512;
#endif
#ifndef VL53L5CX_DISABLE_TARGET_STATUS
	output_bh_enable[0] += (uint32_t)1024;
#endif
#ifndef VL53L5CX_DISABLE_MOTION_INDICATOR
	output_bh_enable[0] += (uint32_t)2048;
#endif

	/* Update data size */
	for (i = 0; i < (uint32_t)(sizeof(output)/sizeof(uint32_t)); i++)
	{
		if ((output[i] == (uint8_t)0)
                    || ((output_bh_enable[i/(uint32_t)32]
                         &((uint32_t)1 << (i%(uint32_t)32))) == (uint32_t)0))
		{
			continue;
		}

		bh_ptr = (union Block_header *)&(output[i]);
		if (((uint8_t)bh_ptr->type >= (uint8_t)0x1)
                    && ((uint8_t)bh_ptr->type < (uint8_t)0x0d))
		{
			if ((bh_ptr->idx >= (uint16_t)0x54d0)
                            && (bh_ptr->idx < (uint16_t)(0x54d0 + 960)))
			{
				bh_ptr->size = resolution;
			}
			else
			{
				bh_ptr->size = (uint16_t)((uint16_t)resolution
                                  * (uint16_t)VL53L5CX_NB_TARGET_PER_ZONE);
			}
			vl53l5cx_Drv->data_read_size += bh_ptr->type * bh_ptr->size;
		}
		else
		{
			vl53l5cx_Drv->data_read_size += bh_ptr->size;
		}
		vl53l5cx_Drv->data_read_size += (uint32_t)4;
	}
	vl53l5cx_Drv->data_read_size += (uint32_t)24;

	status |= vl53l5cx_dci_write_data(vl53l5cx_Drv,
			(uint8_t*)&(output), VL53L5CX_DCI_OUTPUT_LIST,
			(uint16_t)sizeof(output));

	header_config[0] = vl53l5cx_Drv->data_read_size;
	header_config[1] = i + (uint32_t)1;

	status |= vl53l5cx_dci_write_data(vl53l5cx_Drv,
			(uint8_t*)&(header_config), VL53L5CX_DCI_OUTPUT_CONFIG,
			(uint16_t)sizeof(header_config));

	status |= vl53l5cx_dci_write_data(vl53l5cx_Drv,
			(uint8_t*)&(output_bh_enable), VL53L5CX_DCI_OUTPUT_ENABLES,
			(uint16_t)sizeof(output_bh_enable));

	/* Start xshut bypass (interrupt mode) */
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x09, 0x05);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x02);

	/* Start ranging session */
	status |= vl53l5cx_WrMulti(vl53l5cx_Drv, VL53L5CX_UI_CMD_END -
			(uint16_t)(4 - 1), (uint8_t*)cmd, sizeof(cmd));
	status |= _vl53l5cx_poll_for_answer(vl53l5cx_Drv, 4, 1,
			VL53L5CX_UI_CMD_STATUS, 0xff, 0x03);

	/* Read ui range data content and compare if data size is the correct one */
	status |= vl53l5cx_dci_read_data(vl53l5cx_Drv,
			(uint8_t*)vl53l5cx_Drv->temp_buffer, 0x5440, 12);
	(void)memcpy(&tmp, &(vl53l5cx_Drv->temp_buffer[0x8]), sizeof(tmp));
	if(tmp != vl53l5cx_Drv->data_read_size)
	{
		status |= VL53L5CX_STATUS_ERROR;
	}

	return status;
}

ITCM_AREA_CODE uint8_t vl53l5cx_stop_ranging(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv)
{
	uint8_t tmp = 0, status = VL53L5CX_STATUS_OK;
	uint16_t timeout = 0;
	uint32_t auto_stop_flag = 0;

	status |= vl53l5cx_RdMulti(vl53l5cx_Drv,
                          0x2FFC, (uint8_t*)&auto_stop_flag, 4);
	if((auto_stop_flag != (uint32_t)0x4FF)
		&& (vl53l5cx_Drv->is_auto_stop_enabled == (uint8_t)1))
	{
		status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x00);

		/* Provoke MCU stop */
		status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x15, 0x16);
		status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x14, 0x01);

		/* Poll for G02 status 0 MCU stop */
		while(((tmp & (uint8_t)0x80) >> 7) == (uint8_t)0x00)
		{
			status |= vl53l5cx_RdByte(vl53l5cx_Drv, 0x6, &tmp);
			task_delay(10);
			timeout++;	/* Timeout reached after 5 seconds */

			if(timeout > (uint16_t)500)
			{
				status |= tmp;
				break;
			}
		}
	}

	/* Check GO2 status 1 if status is still OK */
	status |= vl53l5cx_RdByte(vl53l5cx_Drv, 0x6, &tmp);
	if((tmp & (uint8_t)0x80) != (uint8_t)0){
		status |= vl53l5cx_RdByte(vl53l5cx_Drv, 0x7, &tmp);
		if((tmp != (uint8_t)0x84) && (tmp != (uint8_t)0x85)){
		   status |= tmp;
		}
	}

	/* Undo MCU stop */
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x14, 0x00);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x15, 0x00);

	/* Stop xshut bypass */
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x09, 0x04);
	status |= vl53l5cx_WrByte(vl53l5cx_Drv, 0x7fff, 0x02);

	return status;
}

ITCM_AREA_CODE uint8_t vl53l5cx_check_data_ready(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv,uint8_t *p_isReady)
{
	uint8_t status = VL53L5CX_STATUS_OK;

	status |= vl53l5cx_RdMulti(vl53l5cx_Drv, 0x0, vl53l5cx_Drv->temp_buffer, 4);

	if((vl53l5cx_Drv->temp_buffer[0] != vl53l5cx_Drv->streamcount)
			&& (vl53l5cx_Drv->temp_buffer[0] != (uint8_t)255)
			&& (vl53l5cx_Drv->temp_buffer[1] == (uint8_t)0x5)
			&& ((vl53l5cx_Drv->temp_buffer[2] & (uint8_t)0x5) == (uint8_t)0x5)
			&& ((vl53l5cx_Drv->temp_buffer[3] & (uint8_t)0x10) ==(uint8_t)0x10)
			)
	{
		*p_isReady = (uint8_t)1;
		 vl53l5cx_Drv->streamcount = vl53l5cx_Drv->temp_buffer[0];
	}
	else
	{
        if ((vl53l5cx_Drv->temp_buffer[3] & (uint8_t)0x80) != (uint8_t)0)
        {
        	status |= vl53l5cx_Drv->temp_buffer[2];	/* Return GO2 error status */
        }

		*p_isReady = 0;
	}

	return status;
}

ITCM_AREA_CODE uint8_t vl53l5cx_get_ranging_data(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv )
{
	uint8_t status = VL53L5CX_STATUS_OK;
	union Block_header *bh_ptr;
	uint16_t header_id, footer_id;
	uint32_t i, j, msize;

	status |= vl53l5cx_RdMulti(vl53l5cx_Drv, 0x0,
			vl53l5cx_Drv->temp_buffer, vl53l5cx_Drv->data_read_size);
	vl53l5cx_Drv->streamcount = vl53l5cx_Drv->temp_buffer[0];
	SwapBuffer(vl53l5cx_Drv->temp_buffer, (uint16_t)vl53l5cx_Drv->data_read_size);

	/* Start conversion at position 16 to avoid headers */
	for (i = (uint32_t)16; i
             < (uint32_t)vl53l5cx_Drv->data_read_size; i+=(uint32_t)4)
	{
		bh_ptr = (union Block_header *)&(vl53l5cx_Drv->temp_buffer[i]);
		if ((bh_ptr->type > (uint32_t)0x1)
                    && (bh_ptr->type < (uint32_t)0xd))
		{
			msize = bh_ptr->type * bh_ptr->size;
		}
		else
		{
			msize = bh_ptr->size;
		}

		switch(bh_ptr->idx){
			case VL53L5CX_METADATA_IDX:
				vl53l5cx_Drv->VL53L5CX_ResultsData->silicon_temp_degc =
						(int8_t)vl53l5cx_Drv->temp_buffer[i + (uint32_t)12];
				break;

#ifndef VL53L5CX_DISABLE_AMBIENT_PER_SPAD
			case VL53L5CX_AMBIENT_RATE_IDX:
				(void)memcpy(vl53l5cx_Drv->VL53L5CX_ResultsData->ambient_per_spad,
				&(vl53l5cx_Drv->temp_buffer[i + (uint32_t)4]), msize);
				break;
#endif
#ifndef VL53L5CX_DISABLE_NB_SPADS_ENABLED
			case VL53L5CX_SPAD_COUNT_IDX:
				(void)memcpy(p_results->nb_spads_enabled,
				&(vl53l5cx_Drv->temp_buffer[i + (uint32_t)4]), msize);
				break;
#endif
#ifndef VL53L5CX_DISABLE_NB_TARGET_DETECTED
			case VL53L5CX_NB_TARGET_DETECTED_IDX:
				(void)memcpy(vl53l5cx_Drv->VL53L5CX_ResultsData->nb_target_detected,
				&(vl53l5cx_Drv->temp_buffer[i + (uint32_t)4]), msize);
				break;
#endif
#ifndef VL53L5CX_DISABLE_SIGNAL_PER_SPAD
			case VL53L5CX_SIGNAL_RATE_IDX:
				(void)memcpy(vl53l5cx_Drv->VL53L5CX_ResultsData->signal_per_spad,
				&(vl53l5cx_Drv->temp_buffer[i + (uint32_t)4]), msize);
				break;
#endif
#ifndef VL53L5CX_DISABLE_RANGE_SIGMA_MM
			case VL53L5CX_RANGE_SIGMA_MM_IDX:
				(void)memcpy(p_results->range_sigma_mm,
				&(vl53l5cx_Drv->temp_buffer[i + (uint32_t)4]), msize);
				break;
#endif
#ifndef VL53L5CX_DISABLE_DISTANCE_MM
			case VL53L5CX_DISTANCE_IDX:
				(void)memcpy(vl53l5cx_Drv->VL53L5CX_ResultsData->distance_mm,
				&(vl53l5cx_Drv->temp_buffer[i + (uint32_t)4]), msize);
				break;
#endif
#ifndef VL53L5CX_DISABLE_REFLECTANCE_PERCENT
			case VL53L5CX_REFLECTANCE_EST_PC_IDX:
				(void)memcpy(vl53l5cx_Drv->VL53L5CX_ResultsData->reflectance,
				&(vl53l5cx_Drv->temp_buffer[i + (uint32_t)4]), msize);
				break;
#endif
#ifndef VL53L5CX_DISABLE_TARGET_STATUS
			case VL53L5CX_TARGET_STATUS_IDX:
				(void)memcpy(vl53l5cx_Drv->VL53L5CX_ResultsData->target_status,
				&(vl53l5cx_Drv->temp_buffer[i + (uint32_t)4]), msize);
				break;
#endif
#ifndef VL53L5CX_DISABLE_MOTION_INDICATOR
			case VL53L5CX_MOTION_DETEC_IDX:
				(void)memcpy(&vl53l5cx_Drv->VL53L5CX_ResultsData->motion_indicator,
				&(vl53l5cx_Drv->temp_buffer[i + (uint32_t)4]), msize);
				break;
#endif
			default:
				break;
		}
		i += msize;
	}

#ifndef VL53L5CX_USE_RAW_FORMAT

	/* Convert data into their real format */
#ifndef VL53L5CX_DISABLE_AMBIENT_PER_SPAD
	for(i = 0; i < (uint32_t)VL53L5CX_RESOLUTION_8X8; i++)
	{
		vl53l5cx_Drv->VL53L5CX_ResultsData->ambient_per_spad[i] /= (uint32_t)2048;
	}
#endif

	for(i = 0; i < (uint32_t)(VL53L5CX_RESOLUTION_8X8
			*VL53L5CX_NB_TARGET_PER_ZONE); i++)
	{
#ifndef VL53L5CX_DISABLE_DISTANCE_MM
		vl53l5cx_Drv->VL53L5CX_ResultsData->distance_mm[i] /= 4;
		if(vl53l5cx_Drv->VL53L5CX_ResultsData->distance_mm[i] < 0)
		{
			vl53l5cx_Drv->VL53L5CX_ResultsData->distance_mm[i] = 0;
		}
#endif
#ifndef VL53L5CX_DISABLE_REFLECTANCE_PERCENT
		vl53l5cx_Drv->VL53L5CX_ResultsData->reflectance[i] /= (uint8_t)2;
#endif
#ifndef VL53L5CX_DISABLE_RANGE_SIGMA_MM
		p_results->range_sigma_mm[i] /= (uint16_t)128;
#endif
#ifndef VL53L5CX_DISABLE_SIGNAL_PER_SPAD
		vl53l5cx_Drv->VL53L5CX_ResultsData->signal_per_spad[i] /= (uint32_t)2048;
#endif
	}

	/* Set target status to 255 if no target is detected for this zone */
#ifndef VL53L5CX_DISABLE_NB_TARGET_DETECTED
	for(i = 0; i < (uint32_t)VL53L5CX_RESOLUTION_8X8; i++)
	{
		if(vl53l5cx_Drv->VL53L5CX_ResultsData->nb_target_detected[i] == (uint8_t)0){
			for(j = 0; j < (uint32_t)
				VL53L5CX_NB_TARGET_PER_ZONE; j++)
			{
#ifndef VL53L5CX_DISABLE_TARGET_STATUS
				vl53l5cx_Drv->VL53L5CX_ResultsData->target_status
				[((uint32_t)VL53L5CX_NB_TARGET_PER_ZONE
					*(uint32_t)i) + j]=(uint8_t)255;
#endif
			}
		}
	}
#endif

#ifndef VL53L5CX_DISABLE_MOTION_INDICATOR
	for(i = 0; i < (uint32_t)32; i++)
	{
		vl53l5cx_Drv->VL53L5CX_ResultsData->motion_indicator.motion[i] /= (uint32_t)65535;
	}
#endif

#endif

	/* Check if footer id and header id are matching. This allows to detect
	 * corrupted frames */
	header_id = ((uint16_t)(vl53l5cx_Drv->temp_buffer[0x8])<<8) & 0xFF00U;
	header_id |= ((uint16_t)(vl53l5cx_Drv->temp_buffer[0x9])) & 0x00FFU;

	footer_id = ((uint16_t)(vl53l5cx_Drv->temp_buffer[vl53l5cx_Drv->data_read_size
		- (uint32_t)4]) << 8) & 0xFF00U;
	footer_id |= ((uint16_t)(vl53l5cx_Drv->temp_buffer[vl53l5cx_Drv->data_read_size
		- (uint32_t)3])) & 0xFFU;

	if(header_id != footer_id)
	{
		status |= VL53L5CX_STATUS_CORRUPTED_FRAME;
	}

	return status;
}

ITCM_AREA_CODE uint32_t vl53l5cx_register(I2C_vl53l5cx_Drv_TypeDef *vl53l5cx_Drv)
{
I2C_DriverStruct_t *eptr, *pre_eptr;
	if ( vl53l5cx_Drv->bus == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( vl53l5cx_Drv->lpn_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( vl53l5cx_Drv->VL53L5CX_ResultsData == NULL)
		return DRIVER_REQUEST_FAILED;
	HAL_GPIO_WritePin(vl53l5cx_Drv->lpn_port, vl53l5cx_Drv->lpn_bit, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(vl53l5cx_Drv->lpn_port, vl53l5cx_Drv->lpn_bit, GPIO_PIN_SET);
	HAL_Delay(10);

	if ( vl53l5cx_Drv->device_address == 0 )
		vl53l5cx_Drv->device_address = VL53L5CX_DEFAULT_I2C_ADDRESS;
	if ( HAL_I2C_IsDeviceReady(vl53l5cx_Drv->bus,vl53l5cx_Drv->device_address,5,IMX335_TIMEOUT) )
		return DRIVER_REQUEST_FAILED;
    // 1. Allocate driver
	if ( i2c_drv_ptr == NULL)
	{
		i2c_drv_ptr = (I2C_DriverStruct_t *)vl53l5cx_Drv;
		vl53l5cx_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = i2c_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (I2C_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)vl53l5cx_Drv;
		vl53l5cx_Drv->next_drv = NULL;
	}
	vl53l5cx_Drv->process = get_current_process();
	return vl53l5cx_init(vl53l5cx_Drv);
}

#endif //#ifdef A_OS_I2C_ENABLED

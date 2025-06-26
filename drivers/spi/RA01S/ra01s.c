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
 * ra01s.c
 *
 *  Created on: Jun 26, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/kernel_opt.h"
#include "ra01s.h"
#include "sx127x.h"

extern	SPI_DriverStruct_t	SPI_DriverStruct[MAX_SPI_DEVICES];
extern	uint8_t				last_spi_used_handle,spi_driver_request;

sx127x_result_t static sx127x_read(sx127x_spi_hal_t* spi_struct, const uint8_t address, uint8_t* read_data, uint8_t read_size)
{
SPI_HandleTypeDef	*spi = spi_struct->spi_handler;
uint32_t timeout =  spi_struct->spi_timeout_ms;

	sx127x_result_t result = SX127X_STATUS_OK;
	HAL_GPIO_WritePin(spi_struct->CS_port,spi_struct->CS_bit, GPIO_PIN_RESET); 	// CS (NSS)
	result = (sx127x_result_t)HAL_SPI_Transmit(spi, &address, 1, timeout);
	if (result != SX127X_STATUS_OK)
	{
		return SX127X_STATUS_HAL_SPI_ERROR;
	}
	while (HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY);
	result = (sx127x_result_t)HAL_SPI_Receive(spi, read_data, read_size, timeout);
	if (result != SX127X_STATUS_OK)
	{
		return SX127X_STATUS_HAL_SPI_ERROR;
	}
	while (HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY);
	HAL_GPIO_WritePin(spi_struct->CS_port,spi_struct->CS_bit, GPIO_PIN_SET); 	// CS (NSS)
	return result;
}

sx127x_result_t static sx127x_write(sx127x_spi_hal_t* spi_struct, const uint8_t address, uint8_t* write_data, uint8_t write_size)
{
SPI_HandleTypeDef* spi = spi_struct->spi_handler;
uint32_t timeout =  spi_struct->spi_timeout_ms;
	sx127x_result_t result = SX127X_STATUS_OK;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 	// CS (NSS)
	result = (sx127x_result_t)HAL_SPI_Transmit(spi, &address, 1, timeout);
	if (result != SX127X_STATUS_OK)
	{
		return SX127X_STATUS_HAL_SPI_ERROR;
	}
	while (HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY);
	result = (sx127x_result_t)HAL_SPI_Transmit(spi, write_data, write_size, timeout);
	if (result != SX127X_STATUS_OK)
	{
		return SX127X_STATUS_HAL_SPI_ERROR;
	}
	while (HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	return result;
}

/*
 * unused functions
sx127x_result_t sx127x_read_registers(SPI_HandleTypeDef* spi, uint8_t address, int8_t* read_data, uint8_t read_size)
{
	return sx127x_read(spi, address & 0x7f, (uint8_t *)read_data, read_size, 2000);
}

sx127x_result_t sx127x_write_registers(SPI_HandleTypeDef* spi, const uint8_t address, uint8_t* write_data, uint8_t write_size)
{
	return sx127x_write(spi, address | 0x80, write_data, write_size, 2000);
}
*/

ITCM_AREA_CODE static sx127x_result_t sx127x_read_register(sx127x_spi_hal_t* spi, const uint8_t address, uint8_t* result)
{
	return sx127x_read(spi, address & 0x7f, result, 1);
}

ITCM_AREA_CODE static sx127x_result_t sx127x_write_register(sx127x_spi_hal_t* spi, const uint8_t address, uint8_t value)
{
	return sx127x_write(spi, address | 0x80, &value, 1);
}

ITCM_AREA_CODE static void sx127x_reset(void)
{

}

sx127x_spi_configuration_t sx127x_spi_conf;
sx127x_configuration_t sx127x_conf;

ITCM_AREA_CODE uint32_t	ra01s_register(RA01S_Drv_TypeDef *driver_private_data)
{
RA01S_Drv_TypeDef	*ra01s_Drv;
sx127x_spi_hal_t spi_hal;

	if ( SPI_DriverStruct[last_spi_used_handle].process != 0 )
		return DRIVER_REQUEST_FAILED;
	if ( driver_private_data->wakeup_id == 0)
		return DRIVER_REQUEST_FAILED;
	if ( driver_private_data->spi == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( driver_private_data->CS_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( driver_private_data->RESET_port == NULL)
		return DRIVER_REQUEST_FAILED;
	SPI_DriverStruct[last_spi_used_handle].driver_private_data = (uint32_t *)driver_private_data;
	ra01s_Drv = (RA01S_Drv_TypeDef *)SPI_DriverStruct[last_spi_used_handle].driver_private_data;
	ra01s_Drv->spi_timeout_ms = 100;
	SPI_DriverStruct[last_spi_used_handle].process = get_current_process();
	SPI_DriverStruct[last_spi_used_handle].driver_private_data = (uint32_t *)driver_private_data;

	/* remove reset */
	HAL_GPIO_WritePin(driver_private_data->RESET_port, driver_private_data->RESET_bit, GPIO_PIN_SET);

	ra01s_Drv->status |= RA01S_DRIVER_STATUS_IN_USE;

	spi_hal.spi_handler = ra01s_Drv->spi;
	spi_hal.spi_timeout_ms = ra01s_Drv->spi_timeout_ms;

	sx127x_spi_conf.spi_read_register_function = &sx127x_read_register;
	sx127x_spi_conf.spi_write_register_function = &sx127x_write_register;
	sx127x_spi_conf.delay_function = &HAL_Delay;
	sx127x_spi_conf.reset_function = &sx127x_reset;
	sx127x_spi_conf.spi_hal = &spi_hal;

	sx127x_conf.spi_conf = &sx127x_spi_conf;
	sx127x_conf.op_mode_range = SX127X_OP_MODE_RANGE_LORA;
	sx127x_conf.lna_boost_gain = SX127X_LNA_GAIN_12DB;
	sx127x_conf.is_rx_payload_crc_on = true;
	sx127x_conf.bandwidth_khz = SX127X_BANDWIDTH_KHZ_125;
	sx127x_conf.header_mode = SX127X_OP_MODE_EXPLICIT_HEADER;
	sx127x_conf.spreading_factor = SX127X_SPREADING_FACTOR_8;
	sx127x_conf.coding_rate = SX127X_CODING_RATE_4_6;
	sx127x_conf.frequency_hz = 434000000;
	sx127x_conf.frequency_mode = SX127X_LOW_FREQUENCY_MODE;
	sx127x_conf.is_auto_agc_on = true;
	sx127x_conf.tx_power_level_dbm = 17;
	sx127x_conf.pa_output_pin = SX127X_PA_SELECT_PA_BOOST_PIN;
	sx127x_lora_init(&sx127x_conf);
	return 0;
}

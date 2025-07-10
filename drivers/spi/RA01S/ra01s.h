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
 * ra01s.h
 *
 *  Created on: Jun 26, 2025
 *      Author: fil
 */

#ifndef MODULES_LORA_RA01S_RA01S_H_
#define MODULES_LORA_RA01S_RA01S_H_

#ifdef A_OS_SPI_ENABLED

#include "sx127x.h"

typedef struct
{
	sx127x_spi_configuration_t*		spi_conf;
	sx127x_op_mode_range_t 			op_mode_range;
	sx127x_lna_boost_gain_t 		lna_boost_gain;
	sx127x_spreading_factor_t 		spreading_factor;
	sx127x_bandwidth_khz_t 			bandwidth_khz;
	sx127x_frequency_mode_t 		frequency_mode;
	sx127x_coding_rate_t 			coding_rate;
	sx127x_header_mode_t 			header_mode;
	bool 							is_rx_payload_crc_on;
	uint32_t 						frequency_hz;
	bool 							is_auto_agc_on;
	uint8_t 						tx_power_level_dbm;
	sx127x_pa_select_t 				pa_output_pin;
} RA01S_UserConfig_TypeDef;

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	uint32_t 			wakeup_id;
	SPI_HandleTypeDef	*spi;
	uint32_t 			spi_timeout_ms;
	uint16_t			CS_bit;
	GPIO_TypeDef	 	*CS_port;
	uint16_t			RESET_bit;
	GPIO_TypeDef	 	*RESET_port;
	RA01S_UserConfig_TypeDef	*RA01S_UserConfig;
}RA01S_Drv_TypeDef;

#define	RA01S_DRIVER_STATUS_IN_USE		0x80
#define	RA01S_DRIVER_STATUS_INITIALIZED	0x40
#define	RA01S_DRIVER_STATUS_INITPEND	0x20
#define	RA01S_DRIVER_STATUS_REQUESTED	0x10
#define	RA01S_DRIVER_STATUS_FAILED		0x01
#define	RA01S_DRIVER_STATUS_UNUSED		0

extern uint32_t	ra01s_register(RA01S_Drv_TypeDef *driver_private_data);
#endif // #ifdef A_OS_SPI_ENABLED

#endif /* MODULES_LORA_RA01S_RA01S_H_ */

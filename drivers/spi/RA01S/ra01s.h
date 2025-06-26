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

#include "sx127x.h"

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
}RA01S_Drv_TypeDef;

#define	RA01S_DRIVER_STATUS_IN_USE		0x80
#define	RA01S_DRIVER_STATUS_INITIALIZED	0x40
#define	RA01S_DRIVER_STATUS_INITPEND	0x20
#define	RA01S_DRIVER_STATUS_REQUESTED	0x10
#define	RA01S_DRIVER_STATUS_FAILED		0x01
#define	RA01S_DRIVER_STATUS_UNUSED		0

extern uint32_t	ra01s_register(RA01S_Drv_TypeDef *driver_private_data);

#endif /* MODULES_LORA_RA01S_RA01S_H_ */

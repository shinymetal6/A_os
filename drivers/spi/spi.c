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
 * spi.c
 *
 *  Created on: Nov 23, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
//#include "../../../kernel/kernel_opt.h"

#include "spi.h"
#include <string.h>

SYSTEM_RAM	SPI_DriverStruct_t	SPI_DriverStruct[MAX_SPI_DEVICES];
SYSTEM_RAM	uint8_t				last_spi_used_handle,spi_driver_request;

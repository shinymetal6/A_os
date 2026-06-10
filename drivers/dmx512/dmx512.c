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
 * dmx512.c
 *
 *  Created on: Nov 18, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#ifdef A_OS_UART_ENABLED

#include "dmx512.h"
#include <string.h>

ITCM_AREA_CODE  uint32_t dmx512_set_break_length(DMX512_DriverStruct_t *dmx512_drv, uint32_t break_length)
{
	if ( dmx512_drv != NULL )
	{
		dmx512_drv->break_length = break_length;
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE void dmx512_send(DMX512_DriverStruct_t *dmx512_drv, uint8_t *buffer, uint16_t buffer_len)
{
	__disable_irq();
	set_gpio_mode(dmx512_drv->tx_port,dmx512_drv->tx_bit,MODE_OUTPUT,0);
	DWT_Delay_us(dmx512_drv->break_length);
	set_gpio_mode(dmx512_drv->tx_port,dmx512_drv->tx_bit,MODE_AF,0);
	uart_send(dmx512_drv->uart_drv,buffer,buffer_len);
	__enable_irq();
}


ITCM_AREA_CODE uint32_t	dmx512_register(DMX512_DriverStruct_t *dmx512_drv)
{
	if ( dmx512_drv->uart_drv == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( dmx512_drv->tx_port == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( dmx512_drv->wakeup_id == 0 )
		return DRIVER_REQUEST_FAILED;
	if ( dmx512_drv->break_length == 0 )
		return DRIVER_REQUEST_FAILED;
	return 0;
}

#endif // #ifdef A_OS_UART_ENABLED

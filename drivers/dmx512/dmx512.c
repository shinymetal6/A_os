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
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
//#include "../../kernel/kernel_opt.h"

#ifdef A_OS_UART_ENABLED

#include "dmx512.h"
#include <string.h>

extern	UARTS_DriverStruct_t	UARTS_DriverStruct[MAX_UARTS_DRIVERS];


ITCM_AREA_CODE uint32_t	dmx512_register(DMX512_Drv_TypeDef *driver_private_data)
{
	if ( driver_private_data->uart == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( driver_private_data->tx_port == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( driver_private_data->wakeup_id == 0 )
		return DRIVER_REQUEST_FAILED;
	if ( driver_private_data->break_length == 0 )
		return DRIVER_REQUEST_FAILED;
	return uart_register((UART_Drv_TypeDef *)driver_private_data);
}

ITCM_AREA_CODE  uint32_t dmx512_set_break_length(uint8_t handle, uint32_t break_length)
{
DMX512_Drv_TypeDef	*dmx512_Drv = (DMX512_Drv_TypeDef	*)UARTS_DriverStruct[handle].driver_private_data;

	if ( dmx512_Drv->uart != NULL )
	{
		dmx512_Drv->break_length = break_length;
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE void dmx512_send(uint8_t handle, uint8_t *buffer, uint16_t buffer_len)
{
DMX512_Drv_TypeDef	*dmx512_Drv = (DMX512_Drv_TypeDef	*)UARTS_DriverStruct[handle].driver_private_data;
	__disable_irq();
	set_gpio_mode(dmx512_Drv->tx_port,dmx512_Drv->tx_bit,MODE_OUTPUT,0);
	DWT_Delay_us(dmx512_Drv->break_length);
	set_gpio_mode(dmx512_Drv->tx_port,dmx512_Drv->tx_bit,MODE_AF,0);
	uart_send(handle,buffer,buffer_len);
	__enable_irq();
}
#endif // #ifdef A_OS_UART_ENABLED

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
 *  Created on: Mar 23, 2024
 *      Author: fil
 */


#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#ifdef DMX512_ENABLE

#include "../dmx512/dmx512.h"
extern	UART_HandleTypeDef huart3;
extern	void DWT_Delay_us(uint32_t au32_microseconds);

dmx_t	dmx;
#define set_pc10_gpio() GPIOC->MODER = 1 << 20
#define set_pc10_uart() GPIOC->MODER = 1 << 21

void dmx512_start(void)
{

	set_pc10_gpio();
	DWT_Delay_us(120);
	set_pc10_uart();

	hw_send_uart_dma(dmx.uart,dmx.DMXbuf,DMX_LEN);
}

void dmx512_init(uint32_t uart)
{
uint32_t	i;
uint8_t		k=255;
	for(i=0,k=255;i<DMX_LEN;i++,k--)
		dmx.DMXbuf[i] = k;
	dmx.DMXbuf[0] = 0;
	dmx.uart = uart;
}

#endif // #ifdef DMX512_ENABLE


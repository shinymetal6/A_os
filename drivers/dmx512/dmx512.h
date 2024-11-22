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
 * dmx512.h
 *
 *  Created on: Nov 18, 2024
 *      Author: fil
 */

#ifndef DRIVERS_DMX512_DMX512_H_
#define DRIVERS_DMX512_DMX512_H_

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				rx_char;
	uint16_t			rx_index;
	uint16_t			rx_max_len;
	uint16_t			rx_num_chars;
	uint16_t			timeout;
	uint16_t			timeout_reload_value;
	uint8_t				sentinel_start;
	uint8_t				sentinel_end;
	uint8_t				sentinel_flags;
	uint32_t			uart_error;
	UART_HandleTypeDef 	*uart;
	GPIO_TypeDef	 	*tx_port;
	uint16_t			tx_bit;
	GPIO_TypeDef	 	*rx_port;
	uint16_t			rx_bit;
	uint32_t			break_length;
	uint32_t 			wakeup_id;
}DMX512_Drv_TypeDef;

extern	uint32_t	dmx512_register(DMX512_Drv_TypeDef *driver_private_data);
extern	uint32_t 	dmx512_init(UART_HandleTypeDef	*huart,uint32_t wakeup_id,GPIO_TypeDef *tx_port,uint16_t tx_bit);
extern	void 		dmx512_send(uint8_t handle, uint8_t *buffer, uint16_t buffer_len);


#endif /* DRIVERS_DMX512_DMX512_H_ */

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
 * intdac_driver.h
 *
 *  Created on: Oct 23, 2024
 *      Author: fil
 */

#ifndef DRIVERS_ANALOG_INTDAC_DRIVER_INTDAC_DRIVER_H_
#define DRIVERS_ANALOG_INTDAC_DRIVER_INTDAC_DRIVER_H_


typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	DAC_HandleTypeDef 	*dac;
	TIM_HandleTypeDef 	*dac_timer;
	uint16_t 			*dac_buffer;
	uint16_t 			channel;
	uint16_t 			len;
	uint16_t 			alignment;
	char				driver_name[32];
}DAC_Drv_TypeDef;

/* status */
#define		DAC_STATUS_HALF			0x01
#define		DAC_STATUS_FULL			0x02
#define		DAC_STATUS_DATA_READY	0x40
#define		DAC_STATUS_RUNNING		0x80
/* flags */
#define		DAC_FLAGS_HALF_WAKEUP	0x20
#define		DAC_FLAGS_FULL_WAKEUP	0x40
#define		DAC_FLAGS_ALL_WAKEUP	0x80

extern uint32_t intdac_init(uint8_t handle);
extern uint32_t intdac_start(uint8_t handle);
extern uint32_t intdac_stop(uint8_t handle);
extern uint32_t intdac_get_status(uint8_t handle);

#endif /* DRIVERS_ANALOG_INTDAC_DRIVER_INTDAC_DRIVER_H_ */

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
 * dhtxx_am230x.h
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#ifndef DRIVERS_SENSORS_DHTXX_AM230X_DHTXX_AM230X_H_
#define DRIVERS_SENSORS_DHTXX_AM230X_DHTXX_AM230X_H_

#ifdef A_OS_TIMERS_ENABLED

#define	MAX_DHT11_DEVICES					8

#define	DHTXX_AM230X_SAMPLESLEN				96
#define	DHTXX_AM230X_BITBYTES_LEN			96
#define	DHTXX_AM230X_BITNUM					40
#define	DHTXX_AM230X_DATALEN				(DHTXX_AM230X_BITNUM/8)

#define	DHTXX_AM230X_0_VAL					30
#define	DHTXX_AM230X_1_VAL					70
#define	DHTXX_AM230X_PULSE_END				250

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	uint8_t				sensor_id;
	uint8_t				state_machine;
	uint32_t			ticks;
	uint16_t			dhtxx_am230x_samples[DHTXX_AM230X_SAMPLESLEN];
	uint16_t			dhtxx_am230x_length[DHTXX_AM230X_BITBYTES_LEN];
	uint8_t				dhtxx_data[DHTXX_AM230X_DATALEN];
	uint8_t				checksum;
	TIM_HandleTypeDef 	*dht_timer;
	uint16_t 			dht_timer_channel;
	GPIO_TypeDef	 	*one_wire_port;
	uint16_t			one_wire_bit;
}Dhtxx_am230x_Drv_TypeDef;
/* status */
#define	DHTXX_AM230X_RUNNING	0x40
#define	DHTXX_AM230X_ACQDONE	0x80

enum DHTXX_AM230X_STATE_MACHINE {
  DHTXX_AM230X_IDLE,
  DHTXX_AM230X_START,
  DHTXX_AM230X_START_BIT_SET,
  DHTXX_AM230X_WAIT_FOR_TIM_END,
  DHTXX_AM230X_END
};

#define	DHTXX_AM230X_START_TICKS	18
#define	DHTXX_AM230X_CYCLE_TICKS	6

extern	uint32_t	dhtxx_am230x_register(Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_driver_private_data);
extern	uint32_t	dhtxx_am230x_init(uint8_t handle_dht);
extern	uint32_t	dhtxx_am230x_start(uint8_t handle_dht);
extern	uint32_t	get_handle_from_dht_workers(uint32_t device_index);
extern	uint32_t	dhtxx_am230x_get_status(uint8_t handle_dht);

#endif // #ifdef A_OS_TIMERS_ENABLED

#endif /* DRIVERS_SENSORS_DHTXX_AM230X_DHTXX_AM230X_H_ */

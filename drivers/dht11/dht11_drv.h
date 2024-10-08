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
 * dht11_drv.h
 *
 *  Created on: Oct 8, 2024
 *      Author: fil
 */


#ifndef STM32H753_SAMPLES_DHT11_DRV_H_
#define STM32H753_SAMPLES_DHT11_DRV_H_

#define	DHT11_MAX_LEN				512

typedef struct
{
	uint8_t		status;
	uint8_t		state_machine;
	uint32_t	ticks;
	uint32_t	samples_number;
	uint16_t	dht11_samples[DHT11_MAX_LEN];
}Dht11_Drv_TypeDef;
/* status */
#define	DHT11_STARTBIT	0x01
#define	DHT11_ACQRUN	0x02
#define	DHT11_RUNNING	0x80

#define	DHT11_START_TICKS	18
extern	TIM_HandleTypeDef 			htim16;
#define DHT11_TIMER					htim16

#define	GPIOPORT_DHT11				GPIOB
#define	GPIOBIT_DHT11				8

enum DHT11_STATE_MACHINE {
  DHT11_IDLE,
  DHT11_START,
  DHT11_START_BIT_SET,
  DHT11_WAIT_FOR_TIM_END,
  DHT11_END
};

#endif /* STM32H753_SAMPLES_DHT11_DRV_H_ */

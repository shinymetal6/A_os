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

#define	DHT11_MAX_SAMPLES_LEN		128
#define	DHT11_MAX_BITBYTES_LEN		80
#define	DHT11_BYTES_NUM				5
#define	DHT11_START_MINIMUM			75
#define	DHT11_START_MAXIMUM			90

#define	DHT11_50uLOW_MIN			40
#define	DHT11_50uLOW_MAX			70
#define	DHT11_DECODED_0				30
#define	DHT11_DECODED_1MIN			50

typedef struct
{
	uint8_t		status;
	uint8_t		state_machine;
	uint32_t	ticks;
	uint32_t	samples_number;
	uint32_t	dht11_samples[DHT11_MAX_SAMPLES_LEN];
	uint32_t	dht11_bitbytes[DHT11_MAX_BITBYTES_LEN];
	uint32_t	dht11_decoded[DHT11_BYTES_NUM];
	uint8_t		checksum;
}Dht11_Drv_TypeDef;
/* status */
#define	DHT11_STARTBIT	0x01
#define	DHT11_ACQRUN	0x02
#define	DHT11_VALID		0x08
#define	DHT11_RUNNING	0x40
#define	DHT11_ACQDONE	0x80

#define	DHT11_START_TICKS	18
#define	DHT11_CYCLE_TICKS	6

enum DHT11_STATE_MACHINE {
  DHT11_IDLE,
  DHT11_START,
  DHT11_START_BIT_SET,
  DHT11_WAIT_FOR_TIM_END,
  DHT11_END
};

extern	void dht11_init(void);
extern	uint8_t dht11_start(void);
extern	void dht11_worker(void);
extern	uint8_t dht11_get_status(void);
extern	uint8_t dht11_get_values(uint8_t *values);

#endif /* STM32H753_SAMPLES_DHT11_DRV_H_ */

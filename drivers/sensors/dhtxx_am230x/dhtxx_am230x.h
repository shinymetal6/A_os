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
 *  Created on: Oct 9, 2024
 *      Author: fil
 */


#ifndef DRIVERS_SENSORS_DHTXX_AM230X_DHTXX_AM230X_H_
#define DRIVERS_SENSORS_DHTXX_AM230X_DHTXX_AM230X_H_

#define	DHTXX_AM230X_MAX_SAMPLES_LEN		128
#define	DHTXX_AM230X_MAX_BITBYTES_LEN		80
#define	DHTXX_AM230X_BYTES_NUM				5
#define	DHTXX_AM230X_START_MINIMUM			75
#define	DHTXX_AM230X_START_MAXIMUM			90

#define	DHTXX_AM230X_50uLOW_MIN			40
#define	DHTXX_AM230X_50uLOW_MAX			70
#define	DHTXX_AM230X_DECODED_0				30
#define	DHTXX_AM230X_DECODED_1MIN			50

typedef struct
{
	uint8_t		status;
	uint8_t		state_machine;
	uint32_t	ticks;
	uint32_t	samples_number;
	uint32_t	dhtxx_am230x_samples[DHTXX_AM230X_MAX_SAMPLES_LEN];
	uint32_t	dhtxx_am230x_bitbytes[DHTXX_AM230X_MAX_BITBYTES_LEN];
	uint32_t	dhtxx_am230x_decoded[DHTXX_AM230X_BYTES_NUM];
	uint8_t		checksum;
	uint32_t	errors;
}Dhtxx_am230x_Drv_TypeDef;
/* status */
#define	DHTXX_AM230X_STARTBIT	0x01
#define	DHTXX_AM230X_ACQRUN	0x02
#define	DHTXX_AM230X_VALID		0x08
#define	DHTXX_AM230X_RUNNING	0x40
#define	DHTXX_AM230X_ACQDONE	0x80

#define	DHTXX_AM230X_START_TICKS	18
#define	DHTXX_AM230X_CYCLE_TICKS	6

enum DHTXX_AM230X_STATE_MACHINE {
  DHTXX_AM230X_IDLE,
  DHTXX_AM230X_START,
  DHTXX_AM230X_START_BIT_SET,
  DHTXX_AM230X_WAIT_FOR_TIM_END,
  DHTXX_AM230X_END
};

extern	uint32_t dhtxx_am230x_init(void);
extern	uint32_t dhtxx_am230x_deinit(void);
extern	uint8_t dhtxx_am230x_start(void);
extern	uint8_t dhtxx_am230x_get_status(void);
extern	uint8_t dhtxx_am230x_get_values(uint8_t *values);


#endif /* DRIVERS_SENSORS_DHTXX_AM230X_DHTXX_AM230X_H_ */

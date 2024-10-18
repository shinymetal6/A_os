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
 * dcc.h
 *
 *  Created on: Oct 16, 2024
 *      Author: fil
 */

#ifndef DRIVERS_ACTUATORS_DCC_DCC_H_
#define DRIVERS_ACTUATORS_DCC_DCC_H_

typedef struct {
	uint16_t		preamble[16];
	uint16_t		packet_start_bit;
	uint16_t		address[8];
	uint16_t		data_byte_start0_bit;
	uint16_t		instruction[8];
	uint16_t		data_byte_start1_bit;
	uint16_t		detection_short[8];
	uint16_t		packet_end_bit;
	uint16_t		detection_long[8];
	uint16_t		long_packet_end_bit;
	uint16_t		endpacket_long;
}DCC_Drv_Pkt_TypeDef;

#define	DCC_PULSE58uS			58
#define	DCC_0					(DCC_PULSE58uS*2)
#define	DCC_1					DCC_PULSE58uS
#define	DCC_START_BIT			DCC_0
#define	DCC_END_BIT				DCC_1
#define	DCC_CUTOUT				(DCC_PULSE58uS*4)
#define	DCC_CTF_0				0
//#define	DCC_CTF_1				160
#define	DCC_CTF_1				460

typedef struct
{
	uint8_t				status;
	uint8_t				handle;
	uint8_t				repetition;
	uint8_t				repetition_counter;
	uint32_t			dma_dcc_value;
	uint32_t			dma_cutout_value;
	uint32_t			dma_dcc_index;
	uint32_t			dma_cutout_index;
	TIM_HandleTypeDef 	*dcc_timer;
	DMA_HandleTypeDef 	*dcc_hdma;
	GPIO_TypeDef	 	*enable_port;
	uint16_t			enable_bit;
}DCC_Control_Drv_TypeDef;
/* status */
#define	DCC_INITIALIZED			0x01
#define	DCC_RUNNING				0x02
#define	DCC_ON					0x04
#define	DCC_PACKET_INPROGRESS	0x20
#define	DCC_PACKET_EXTENDED		0x40
#define	DCC_PACKET_PENDING		0x80


#define	FIRST_HALF		0
#define	SECOND_HALF		1

#define	DCC_CMD		0
#define	DCC_TRACK	1
#define	DCC_ADDRESS	2
#define	DCC_DATAH	3
#define	DCC_DATAL	4
extern	uint32_t dcc_get_drv_struct(DriversDefs_t *new_struct,uint8_t peripheral_index);

#endif /* DRIVERS_ACTUATORS_DCC_DCC_H_ */

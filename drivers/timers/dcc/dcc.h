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
	/* timer header */
	uint8_t 				process;
	uint8_t					status;
	uint8_t					flags;
	uint32_t 				*next_dcc;
	TIM_HandleTypeDef 		*dcc_timer;
	uint8_t					timer_type;
	uint32_t 				wakeup_id;
	/* timer internals */
	uint8_t					command_repeat_number;
	uint8_t					command_repeat_counter;
	uint32_t				dma_dcc_value;
	uint32_t				dma_cutout_value;
	uint32_t				dma_dcc_index;
	uint32_t				dma_cutout_index;
	DMA_HandleTypeDef 		*hdma[2];
	uint32_t				timer_180phase_dcc_channel;
	uint32_t				timer_dcc_channel;
	uint32_t				timer_cutout_channel;
	GPIO_TypeDef	 		*enable_port;
	uint16_t				enable_bit;
	DCC_Drv_Pkt_TypeDef		DCC_Pkt[2];
	DCC_Drv_Pkt_TypeDef		DCC_Cutout_Pkt[2];
	DCC_Drv_Pkt_TypeDef		DCC_WorkPkt;
}DCC_DriverStruct_t;

/* status */
#define	DCC_INITIALIZED			0x01
#define	DCC_RUNNING				0x02
#define	DCC_ON					0x04
#define	DCC_RESET_SENT			0x08
#define	DCC_PACKET_INPROGRESS	0x20
#define	DCC_PACKET_EXTENDED		0x40
#define	DCC_PACKET_PENDING		0x80

/* flags */
#define	DCC_TIMER_DUAL_PHASE	0x01

#define	FIRST_HALF		0
#define	SECOND_HALF		1

#define	DCC_CMD		0
#define	DCC_TRACK	1
#define	DCC_ADDRESS	2
#define	DCC_DATAH	3
#define	DCC_DATAL	4

extern	uint32_t	dcc_register(DCC_DriverStruct_t *dcc_drv);
extern	uint32_t	dcc_start(DCC_DriverStruct_t *dcc_drv);
extern	uint32_t	dcc_stop(DCC_DriverStruct_t *dcc_drv);
extern	uint32_t	dcc_get_status(DCC_DriverStruct_t *dcc_drv);
extern	uint32_t	dcc_commands(DCC_DriverStruct_t *dcc_drv,uint8_t *values,uint16_t values_number);
extern 	void 		dcc_TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma);
extern 	void 		dcc_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma);

#endif /* DRIVERS_ACTUATORS_DCC_DCC_H_ */

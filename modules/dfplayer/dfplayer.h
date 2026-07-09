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
 * dfplayer.h
 *
 *  Created on: Jul 4, 2026
 *      Author: fil
 */

#ifndef MODULES_DFPLAYER_DFPLAYER_H_
#define MODULES_DFPLAYER_DFPLAYER_H_

#ifdef A_OS_UART_ENABLED

typedef struct
{
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	UART_DriverStruct_t	*uart_drv;
	GPIO_TypeDef	 	*busy_port;
	uint16_t			busy_bit;
	uint8_t 			DfPlayer_txbuffer[10];
	uint8_t				*DfPlayer_rxbuffer;
	uint8_t				DfPlayer_rxbuffer_len;
	uint8_t				DfPlayer_query_tracks_command;
	uint8_t				DfPlayer_sm;
	uint8_t				DfPlayer_state;
	uint8_t				DfPlayer_command;
	uint8_t				DfPlayer_last_command;
	uint16_t			DfPlayer_reply_to_last_command;
	uint8_t				DfPlayer_parameter;
}MODULES_DFPlayer_Struct_t;

/* DfPlayer_sm */
#define DFPLAYER_SM_INIT					0x00
#define DFPLAYER_SM_STOP					0x01
#define DFPLAYER_SM_SETSOURCE				0x02
#define DFPLAYER_SM_QUERYTRACKS				0x03
#define DFPLAYER_SM_SETVOLUME				0x04
#define DFPLAYER_SM_MAINLOOP				0x05

#define DFPLAYER_CMD_NEXT					0x01
#define DFPLAYER_CMD_PREV					0x02
#define DFPLAYER_CMD_PLAY_TRACK				0x03
#define DFPLAYER_CMD_INCREASE_VOLUME		0x04
#define DFPLAYER_CMD_DECREASE_VOLUME		0x05
#define DFPLAYER_CMD_SET_VOLUME				0x06
#define DFPLAYER_CMD_SET_EQ					0x07
#define DFPLAYER_CMD_SET_PLAY_MODE			0x08
#define DFPLAYER_CMD_SET_SOURCE				0x09
#define DFPLAYER_CMD_PLAY					0x0D
#define DFPLAYER_CMD_PAUSE					0x0E
#define DFPLAYER_CMD_STOP					0x16
/* Original modules use this */
//#define DFPLAYER_CMD_QUERY_TOTAL_TRACKS  	0x47
/* Cloned modules use this */
#define DFPLAYER_CMD_QUERY_TOTAL_TRACKS  	0x48

#define DFPLAYER_CMD_QUERY_TOTAL_TRACKS47  	0x47
#define DFPLAYER_CMD_QUERY_TOTAL_TRACKS48  	0x48

#define DFPLAYER_CMD_SET_PLAY_MODE_REPEAT	0x00
#define DFPLAYER_CMD_SET_PLAY_FOLDER_REPEAT	0x01
#define DFPLAYER_CMD_SET_PLAY_SINGLE_REPEAT	0x02
#define DFPLAYER_CMD_SET_PLAY_RANDOM		0x03

#define DFPLAYER_CMD_SET_EQ_NORMAL			0x00
#define DFPLAYER_CMD_SET_EQ_POP				0x01
#define DFPLAYER_CMD_SET_EQ_ROCK			0x02
#define DFPLAYER_CMD_SET_EQ_JAZZ			0x03
#define DFPLAYER_CMD_SET_EQ_CLASSIC			0x04
#define DFPLAYER_CMD_SET_EQ_BASE			0x05
/* sm only */
#define DFPLAYER_CMD_REINIT					0xa5

#define DFPLAYER_CMD_SOURCE_SD				0x02
#define DFPLAYER_CMD_DEFAULT_VOLUME			15

#define DFPLAYER_RXBUF_OPENINGFLAG			0x7e
#define DFPLAYER_RXBUF_FWVERSION			0xff
#define DFPLAYER_RXBUF_PKTLEN				0x06
#define DFPLAYER_RXBUF_CLOSINGFLAG			0xef

#define DFPLAYER_STD_REPLY_LEN				10

extern	uint32_t DfPlayer_register(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer);
extern	uint32_t DfPlayer_send_cmd(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint8_t cmd, uint16_t param, uint8_t request_feedback);
extern	uint32_t DfPlayer_is_busy(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer);

extern	uint32_t DfPlayer_play(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer);
extern	uint32_t DfPlayer_stop(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer);
extern	uint32_t DfPlayer_pause(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer);
extern	uint32_t DfPlayer_next(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer);
extern	uint32_t DfPlayer_prev(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer);
extern	uint32_t DfPlayer_set_volume(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint8_t volume);
extern	uint32_t DfPlayer_set_eq(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint16_t eq);
extern	uint32_t DfPlayer_set_play_mode(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint16_t play_mode);

// Updated to return success (1) or fail (0) status
extern	uint32_t DfPlayer_set_source(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint8_t source);
extern	uint32_t DfPlayer_play_track(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint16_t track);
extern	uint32_t DfPlayer_send_query_total_tracks(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer);
extern	uint32_t DFPlayer_ProcessResponse(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint32_t len);
extern	uint32_t DfPlayer_state_machine(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer);
extern	uint32_t DfPlayer_command(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer , uint8_t command);

#endif //#ifdef A_OS_UART_ENABLED

#endif /* MODULES_DFPLAYER_DFPLAYER_H_ */

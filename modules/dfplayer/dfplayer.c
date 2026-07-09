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
 * dfplayer.c
 *
 *  Created on: Jul 4, 2026
 *      Author: fil
 */

#include "main.h"

#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#ifdef A_OS_UART_ENABLED
#include "dfplayer.h"
#include <string.h>

uint32_t DfPlayer_is_busy(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer)
{
	/* Error case : busy forever */
    if ( MODULE_DFPlayer->busy_port == NULL )
    	return 1;
    return (HAL_GPIO_ReadPin(MODULE_DFPlayer->busy_port, MODULE_DFPlayer->busy_bit) );
}

uint32_t DfPlayer_send_cmd(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint8_t cmd, uint16_t param, uint8_t request_feedback)
{
	MODULE_DFPlayer->DfPlayer_last_command = cmd;
	MODULE_DFPlayer->DfPlayer_txbuffer[0] = 0x7E;
	MODULE_DFPlayer->DfPlayer_txbuffer[1] = 0xFF;
	MODULE_DFPlayer->DfPlayer_txbuffer[2] = 0x06;
	MODULE_DFPlayer->DfPlayer_txbuffer[3] = cmd;
	MODULE_DFPlayer->DfPlayer_txbuffer[4] = request_feedback ? 0x01 : 0x00; // <-- Feedback flag
	MODULE_DFPlayer->DfPlayer_txbuffer[5] = (param >> 8) & 0xFF;
	MODULE_DFPlayer->DfPlayer_txbuffer[6] = param & 0xFF;

    uint16_t checksum = 0;
    for (uint8_t i = 1; i < 7; i++)
    	checksum += MODULE_DFPlayer->DfPlayer_txbuffer[i];
    checksum = -checksum;

    MODULE_DFPlayer->DfPlayer_txbuffer[7] = (checksum >> 8) & 0xFF;
    MODULE_DFPlayer->DfPlayer_txbuffer[8] = checksum & 0xFF;
    MODULE_DFPlayer->DfPlayer_txbuffer[9] = 0xEF;

    uart_send(MODULE_DFPlayer->uart_drv, MODULE_DFPlayer->DfPlayer_txbuffer, 10);
    HAL_Delay(50); // Allow time for the module to process
    return 0;
}

/* Commands with reply */
uint32_t DfPlayer_set_source(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint8_t source)
{
	return DfPlayer_send_cmd(MODULE_DFPlayer,DFPLAYER_CMD_SET_SOURCE, source, 1);
}

/* Commands with no reply */

uint32_t DfPlayer_play_track(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint16_t track)
{
	return DfPlayer_send_cmd(MODULE_DFPlayer,DFPLAYER_CMD_PLAY_TRACK, track, 0);
}

uint32_t DfPlayer_play(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer)
{
	return DfPlayer_send_cmd(MODULE_DFPlayer,DFPLAYER_CMD_PLAY, 0x0000, 0);
}

uint32_t DfPlayer_stop(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer)
{
	return DfPlayer_send_cmd(MODULE_DFPlayer,DFPLAYER_CMD_STOP, 0x0000, 0);
}

uint32_t DfPlayer_pause(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer)
{
	return DfPlayer_send_cmd(MODULE_DFPlayer,DFPLAYER_CMD_PAUSE, 0x0000, 0);
}

uint32_t DfPlayer_next(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer)
{
	return DfPlayer_send_cmd(MODULE_DFPlayer,DFPLAYER_CMD_NEXT, 0x0000, 0);
}

uint32_t DfPlayer_increase_volume(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer)
{
	return DfPlayer_send_cmd(MODULE_DFPlayer,DFPLAYER_CMD_INCREASE_VOLUME, 0x0000, 0);
}

uint32_t DfPlayer_decrease_volume(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer)
{
	return DfPlayer_send_cmd(MODULE_DFPlayer,DFPLAYER_CMD_DECREASE_VOLUME, 0x0000, 0);
}

uint32_t DfPlayer_prev(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer)
{
	return DfPlayer_send_cmd(MODULE_DFPlayer,DFPLAYER_CMD_PREV, 0x0000, 0);
}

uint32_t DfPlayer_set_eq(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint16_t eq)
{
	if ( eq <= DFPLAYER_CMD_SET_EQ_BASE )
		return DfPlayer_send_cmd(MODULE_DFPlayer,DFPLAYER_CMD_PREV, eq, 0);
	return 1;
}

uint32_t DfPlayer_set_play_mode(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint16_t play_mode)
{
	if ( play_mode <= DFPLAYER_CMD_SET_PLAY_RANDOM )
		return DfPlayer_send_cmd(MODULE_DFPlayer,DFPLAYER_CMD_SET_PLAY_MODE, play_mode, 0);
	return 1;
}

uint32_t DfPlayer_set_volume(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint8_t volume)
{
    if (volume > 30)
    	volume = 30;
    return DfPlayer_send_cmd(MODULE_DFPlayer,DFPLAYER_CMD_SET_VOLUME, volume, 0);
}


uint32_t DfPlayer_send_query_total_tracks(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer)
{
	return DfPlayer_send_cmd(MODULE_DFPlayer,MODULE_DFPlayer->DfPlayer_query_tracks_command, 0x0000, 1);
}

uint32_t DFPlayer_ProcessResponse(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer,uint32_t len)
{
uint8_t		*data_ptr;

	/* skip std 0x41 reply for command accepted */
	if ( len > 10 )
		data_ptr = MODULE_DFPlayer->DfPlayer_rxbuffer + len-DFPLAYER_STD_REPLY_LEN;
	else
		return 0;
	if ((data_ptr[0]== DFPLAYER_RXBUF_OPENINGFLAG ) && (data_ptr[1]== DFPLAYER_RXBUF_FWVERSION ) && (data_ptr[2]== DFPLAYER_RXBUF_PKTLEN ) && (data_ptr[9]== DFPLAYER_RXBUF_CLOSINGFLAG ))
	{
		if ( data_ptr[3] == MODULE_DFPlayer->DfPlayer_last_command )
		{
			MODULE_DFPlayer->DfPlayer_reply_to_last_command = data_ptr[5]<<8 | data_ptr[6];
			if ( MODULE_DFPlayer->DfPlayer_last_command == MODULE_DFPlayer->DfPlayer_query_tracks_command )
				MODULE_DFPlayer->DfPlayer_number_of_tracks = MODULE_DFPlayer->DfPlayer_reply_to_last_command;
			return MODULE_DFPlayer->DfPlayer_reply_to_last_command;
		}
	}
	return 0;
}

uint32_t DfPlayer_register(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer)
{
    if ( MODULE_DFPlayer->busy_port == NULL )
    	return 1;
    if ( MODULE_DFPlayer->uart_drv == NULL )
    	return 1;
    if ( MODULE_DFPlayer->DfPlayer_rxbuffer_len == 0 )
    	return 1;
    if ( MODULE_DFPlayer->DfPlayer_query_tracks_command == 0 )
    	MODULE_DFPlayer->DfPlayer_query_tracks_command = DFPLAYER_CMD_QUERY_TOTAL_TRACKS48;
    MODULE_DFPlayer->DfPlayer_rxbuffer = MODULE_DFPlayer->uart_drv->data;
	MODULE_DFPlayer->DfPlayer_sm = DFPLAYER_SM_INIT;
    return 0;
}

uint32_t DfPlayer_state_machine(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer)
{
	switch(MODULE_DFPlayer->DfPlayer_sm)
	{
	case DFPLAYER_SM_INIT :
		DfPlayer_stop(MODULE_DFPlayer);
		MODULE_DFPlayer->DfPlayer_sm = DFPLAYER_SM_STOP;
		break;
	case DFPLAYER_SM_STOP :
		if ( DfPlayer_is_busy(MODULE_DFPlayer) )
		{
			DfPlayer_stop(MODULE_DFPlayer);
			MODULE_DFPlayer->DfPlayer_sm = DFPLAYER_SM_SETSOURCE;
		}
		break;
	case DFPLAYER_SM_SETSOURCE :
		if ( DfPlayer_is_busy(MODULE_DFPlayer) )
		{
			MODULE_DFPlayer->DfPlayer_sm = DFPLAYER_SM_QUERYTRACKS;
			DfPlayer_set_source(MODULE_DFPlayer,DFPLAYER_CMD_SOURCE_SD);
		}
		break;
	case DFPLAYER_SM_QUERYTRACKS :

		if ( DfPlayer_is_busy(MODULE_DFPlayer) )
		{
			MODULE_DFPlayer->DfPlayer_sm = DFPLAYER_SM_SETVOLUME;
			DfPlayer_send_query_total_tracks(MODULE_DFPlayer);
		}
		break;
	case DFPLAYER_SM_SETVOLUME :
		if ( DfPlayer_is_busy(MODULE_DFPlayer) )
		{
			MODULE_DFPlayer->DfPlayer_sm = DFPLAYER_SM_MAINLOOP;
			DfPlayer_set_volume(MODULE_DFPlayer,DFPLAYER_CMD_DEFAULT_VOLUME);
		}
		break;
	case DFPLAYER_SM_MAINLOOP :
		if ( MODULE_DFPlayer->DfPlayer_command )
		{
			if ( MODULE_DFPlayer->DfPlayer_command ==  DFPLAYER_CMD_STOP)
				DfPlayer_stop(MODULE_DFPlayer);
			else if ( MODULE_DFPlayer->DfPlayer_command ==  DFPLAYER_CMD_REINIT)
				MODULE_DFPlayer->DfPlayer_sm = DFPLAYER_SM_INIT;
			else
			{
				if ( DfPlayer_is_busy(MODULE_DFPlayer) )
				{
					switch ( MODULE_DFPlayer->DfPlayer_command )
					{
					case	DFPLAYER_CMD_NEXT:
						DfPlayer_next(MODULE_DFPlayer);
						break;
					case	DFPLAYER_CMD_PREV:
						DfPlayer_prev(MODULE_DFPlayer);
						break;
					case	DFPLAYER_CMD_INCREASE_VOLUME:
						DfPlayer_increase_volume(MODULE_DFPlayer);
						break;
					case	DFPLAYER_CMD_DECREASE_VOLUME:
						DfPlayer_decrease_volume(MODULE_DFPlayer);
						break;
					case	DFPLAYER_CMD_PLAY:
						DfPlayer_play(MODULE_DFPlayer);
						break;
					case	DFPLAYER_CMD_PAUSE:
						DfPlayer_pause(MODULE_DFPlayer);
						break;
					case	DFPLAYER_CMD_SET_VOLUME:
						DfPlayer_set_volume(MODULE_DFPlayer,MODULE_DFPlayer->DfPlayer_parameter);
						break;
					case	DFPLAYER_CMD_SET_EQ:
						DfPlayer_set_eq(MODULE_DFPlayer,MODULE_DFPlayer->DfPlayer_parameter);
						break;
					case	DFPLAYER_CMD_PLAY_TRACK:
						DfPlayer_play_track(MODULE_DFPlayer,MODULE_DFPlayer->DfPlayer_parameter);
						break;
					case	DFPLAYER_CMD_SET_PLAY_MODE:
						DfPlayer_set_play_mode(MODULE_DFPlayer,MODULE_DFPlayer->DfPlayer_parameter);
						break;
					}
				}
			}
			MODULE_DFPlayer->DfPlayer_command = 0;
		}
		break;
	}
    return MODULE_DFPlayer->DfPlayer_sm;
}

uint32_t DfPlayer_command(MODULES_DFPlayer_Struct_t *MODULE_DFPlayer , uint8_t command)
{
	MODULE_DFPlayer->DfPlayer_command = command;
	return 0;
}

#endif //#ifdef A_OS_UART_ENABLED

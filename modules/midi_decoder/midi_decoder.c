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
 * midi_decoder.c
 *
 *  Created on: Oct 2, 2025
 *      Author: fil
 */


#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#ifdef MIDI_ENABLED

#include "midi_decoder.h"

A_midi_decoder_t	*MidiDecoder;

ITCM_AREA_CODE uint16_t SYSEX_ConvertMidiBufOut(uint8_t *buf_out , uint16_t len)
{
uint32_t	r=0,k=0;

	if ( MidiDecoder->midi_transmit_sysex_buffer == NULL )
		return 0;
	MidiDecoder->midi_transmit_sysex_buffer[k] = SYSEX_STARTC;
	k++;
	MidiDecoder->midi_transmit_sysex_buffer[k] = SYSEX_START;
	k++;
	r = 0;
	while ( r < len )
	{
		MidiDecoder->midi_transmit_sysex_buffer[k] = buf_out[r];
		r++;
		k++;
		if ( (k & 0x03 ) == 0)
		{
			if (( len - r ) == 2)
				MidiDecoder->midi_transmit_sysex_buffer[k] = SYSEX_END_3;
			else if (( len - r ) == 1)
				MidiDecoder->midi_transmit_sysex_buffer[k] = SYSEX_END_2;
			else if (( len - r ) == 0)
				MidiDecoder->midi_transmit_sysex_buffer[k] = SYSEX_END_1;
			else
				MidiDecoder->midi_transmit_sysex_buffer[k] = SYSEX_STARTC;
			k++;
		}
	}
	MidiDecoder->midi_transmit_sysex_buffer[k] = SYSEX_END;
	while ((k & 0x03 ) != 0 )
		k++;
	return k;
}

ITCM_AREA_CODE static uint8_t MidiParseSysEx(uint8_t channel ,uint32_t len,uint8_t* buf)
{
uint8_t	i,j=1,k=0;

	if ( len > SYSEX_MAX_LEN )
		return 0;
	if ( MidiDecoder->midi_received_sysex_buffer == NULL )
		return 0;

	MidiDecoder->midi_received_sysex_len = 0;
	for(i=0;i<len;i+=4)
	{
		if ( buf[i] == SYSEX_STARTC )
		{
			MidiDecoder->midi_received_sysex_buffer[k] = buf[j];
			MidiDecoder->midi_received_sysex_buffer[k+1] = buf[j+1];
			MidiDecoder->midi_received_sysex_buffer[k+2] = buf[j+2];
			k +=3;
			j +=4;
			MidiDecoder->midi_received_sysex_len+=3;
		}
		if ( buf[i] == SYSEX_END_1 )
		{
			MidiDecoder->midi_received_sysex_buffer[k] = buf[j];
			MidiDecoder->midi_received_sysex_len+=0;
			i = len;
		}
		if ( buf[i] == SYSEX_END_2 )
		{
			MidiDecoder->midi_received_sysex_buffer[k] = buf[j];
			MidiDecoder->midi_received_sysex_buffer[k+1] = buf[j+1];
			MidiDecoder->midi_received_sysex_len+=1;
			i = len;
		}
		if ( buf[i] == SYSEX_END_3 )
		{
			MidiDecoder->midi_received_sysex_buffer[k] = buf[j];
			MidiDecoder->midi_received_sysex_buffer[k+1] = buf[j+1];
			MidiDecoder->midi_received_sysex_buffer[k+2] = buf[j+2];
			MidiDecoder->midi_received_sysex_len+=2;
			i = len;
		}
	}
	if ( MidiDecoder->midi_received_sysex_buffer[MidiDecoder->midi_received_sysex_len] == SYSEX_END )
	{
		if ( MidiDecoder->SysEx != NULL)
			MidiDecoder->SysEx();
	}
	return len;
}

ITCM_AREA_CODE static uint8_t	MidiParseControlChange(uint8_t cc_channel,uint8_t cc_index,uint8_t cc_value)
{
	if ( MidiDecoder->ControlChange != NULL)
		MidiDecoder->ControlChange(cc_channel,cc_index,cc_value);
	return 4;
}

ITCM_AREA_CODE static uint8_t	MidiParseProgramChange(uint8_t pc_index,uint8_t pc_value)
{
	if ( MidiDecoder->ProgramChange != NULL)
		MidiDecoder->ProgramChange(pc_index,pc_value);
	return 3;
}

ITCM_AREA_CODE static uint8_t	MidiParsePolyPressure(uint8_t midi_channel_status , uint8_t midi_note , uint8_t midi_velocity)
{
	if ( MidiDecoder->PolyPressure != NULL)
		MidiDecoder->PolyPressure(midi_channel_status , midi_note , midi_velocity);
	return 4;
}

ITCM_AREA_CODE static uint8_t	MidiParsePitchBend(uint8_t midi_channel_status , uint8_t midi_note , uint8_t midi_velocity)
{
	if ( MidiDecoder->PitchBend != NULL)
		MidiDecoder->PitchBend(midi_channel_status , midi_note , midi_velocity);
	return 4;
}

ITCM_AREA_CODE static uint8_t	MidiParseNote(uint8_t midi_channel_status , uint8_t midi_note , uint8_t midi_velocity)
{
	if ( MidiDecoder->Note != NULL)
		MidiDecoder->Note( midi_channel_status , midi_note , midi_velocity);
	return 4;
}

ITCM_AREA_CODE void MidiParser(uint8_t* buf, uint16_t len)
{
uint32_t	l_index=0;
	while ( l_index < len )
	{
		switch(buf[l_index+1] & CMD_MASK)
		{
			case NOTE_OFF			:
			case NOTE_ON			:
				l_index += MidiParseNote(buf[l_index+1] , buf[l_index+2] , buf[l_index+3]);
				break;
			case SYSEX_START			:
				l_index += MidiParseSysEx(buf[l_index+1],len,buf);
				break;
			case CONTROL_CHANGE			:
				l_index += MidiParseControlChange (buf[l_index+1],buf[l_index+2],buf[l_index+3]);
				break;
			case PROGRAM_CHANGE			:
				l_index += MidiParseProgramChange (buf[l_index+1],buf[l_index+2]);
				break;
			case POLY_PRESSURE			:
				l_index += MidiParsePolyPressure (buf[l_index+1] , buf[l_index+2] , buf[l_index+3]);
				break;
			case PITCH_BEND			:
				l_index += MidiParsePitchBend (buf[l_index+1] , buf[l_index+2] , buf[l_index+3]);
				break;
			default			:	l_index += 4; break;
		}
	}
}

ITCM_AREA_CODE uint32_t MidiInit(A_midi_decoder_t *user_midi)
{
	MidiDecoder = user_midi;
	return 0;
}

#endif // #ifdef MIDI_ENABLED

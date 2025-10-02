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
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#ifdef MIDI_ENABLED

#include "midi_decoder.h"

__attribute__ ((aligned (4)))	A_midi_decoder_t	A_midi_decoder;

uint16_t SYSEX_ConvertMidiBufOut(uint8_t *buf_out , uint16_t len)
{
uint32_t	r=0,k=0;

	if ( A_midi_decoder.midi_transmit_sysex_buffer == NULL )
		return 0;
	A_midi_decoder.midi_transmit_sysex_buffer[k] = SYSEX_STARTC;
	k++;
	A_midi_decoder.midi_transmit_sysex_buffer[k] = SYSEX_START;
	k++;
	r = 0;
	while ( r < len )
	{
		A_midi_decoder.midi_transmit_sysex_buffer[k] = buf_out[r];
		r++;
		k++;
		if ( (k & 0x03 ) == 0)
		{
			if (( len - r ) == 2)
				A_midi_decoder.midi_transmit_sysex_buffer[k] = SYSEX_END_3;
			else if (( len - r ) == 1)
				A_midi_decoder.midi_transmit_sysex_buffer[k] = SYSEX_END_2;
			else if (( len - r ) == 0)
				A_midi_decoder.midi_transmit_sysex_buffer[k] = SYSEX_END_1;
			else
				A_midi_decoder.midi_transmit_sysex_buffer[k] = SYSEX_STARTC;
			k++;
		}
	}
	A_midi_decoder.midi_transmit_sysex_buffer[k] = SYSEX_END;
	while ((k & 0x03 ) != 0 )
		k++;
	return k;
}

void SysExApplyValues(void)
{

}

uint8_t MidiParseSysEx(uint8_t channel , uint8_t sub_command,uint32_t len,uint8_t* buf)
{
uint8_t	i,j=1,k=0;

	if ( len > SYSEX_MAX_LEN )
		return 0;
	if ( A_midi_decoder.midi_received_sysex_buffer == NULL )
		return 0;

	A_midi_decoder.midi_received_sysex_len = 0;
	for(i=0;i<len;i+=4)
	{
		if ( buf[i] == SYSEX_STARTC )
		{
			A_midi_decoder.midi_received_sysex_buffer[k] = buf[j];
			A_midi_decoder.midi_received_sysex_buffer[k+1] = buf[j+1];
			A_midi_decoder.midi_received_sysex_buffer[k+2] = buf[j+2];
			k +=3;
			j +=4;
			A_midi_decoder.midi_received_sysex_len+=3;
		}
		if ( buf[i] == SYSEX_END_1 )
		{
			A_midi_decoder.midi_received_sysex_buffer[k] = buf[j];
			A_midi_decoder.midi_received_sysex_len+=0;
			i = len;
		}
		if ( buf[i] == SYSEX_END_2 )
		{
			A_midi_decoder.midi_received_sysex_buffer[k] = buf[j];
			A_midi_decoder.midi_received_sysex_buffer[k+1] = buf[j+1];
			A_midi_decoder.midi_received_sysex_len+=1;
			i = len;
		}
		if ( buf[i] == SYSEX_END_3 )
		{
			A_midi_decoder.midi_received_sysex_buffer[k] = buf[j];
			A_midi_decoder.midi_received_sysex_buffer[k+1] = buf[j+1];
			A_midi_decoder.midi_received_sysex_buffer[k+2] = buf[j+2];
			A_midi_decoder.midi_received_sysex_len+=2;
			i = len;
		}
	}
	if ( A_midi_decoder.midi_received_sysex_buffer[A_midi_decoder.midi_received_sysex_len] == SYSEX_END )
	{
		if ( A_midi_decoder.SysEx != NULL)
			A_midi_decoder.SysEx();
	}
	return len;
}

uint8_t	MidiParseControlChange(uint8_t cc_index,uint8_t cc_value)
{
	if ( A_midi_decoder.ControlChange != NULL)
		A_midi_decoder.ControlChange(cc_index,cc_value);
	return 4;
}

uint8_t	MidiParseProgramChange(uint8_t pc_index,uint8_t pc_value)
{
	if ( A_midi_decoder.ProgramChange != NULL)
		A_midi_decoder.ProgramChange(pc_index,pc_value);
	return 4;
}

uint8_t	MidiParseNote(uint8_t midi_channel , uint8_t midi_note , uint8_t midi_velocity, uint8_t cmdnumber)
{
	A_midi_decoder.midi_channel[A_midi_decoder.midi_commands_number] = midi_channel;
	A_midi_decoder.midi_note[A_midi_decoder.midi_commands_number] = midi_note;
	A_midi_decoder.midi_velocity[A_midi_decoder.midi_commands_number] = midi_velocity;
	if ( A_midi_decoder.Note != NULL)
		A_midi_decoder.Note(midi_channel , midi_note , midi_velocity);
	return 4;
}

void MidiParser(uint8_t* buf, uint16_t len)
{
uint8_t		cmd,midi_channel,midi_note,midi_velocity;
uint32_t	l_index=0;
	A_midi_decoder.midi_commands_number = 0;
	while ( l_index < len )
	{
		cmd = buf[l_index] & CIN_MASK;
		midi_channel = buf[l_index+1] & CHANNEL_MASK;
		midi_note = buf[l_index+2];
		midi_velocity = buf[l_index+3];
		switch(cmd)
		{
			case NOTE_OFF			:
			case NOTE_ON			:
				l_index += MidiParseNote(midi_channel , midi_note , midi_velocity, A_midi_decoder.midi_commands_number);
				A_midi_decoder.midi_commands_number++;
				break;
			case SYSEX_END_1			:
				break;
			case SYSEX_STARTC			:
			case SYSEX_END_2			:
			case SYSEX_END_3			:
				if ( buf[1] == SYSEX_START )
				{
					l_index += MidiParseSysEx(midi_channel,cmd,len,buf);
					A_midi_decoder.midi_commands_number++;
				}
				break;
			case CONTROL_CHANGE			:
				l_index += MidiParseControlChange (buf[l_index+2],buf[l_index+3]);
				A_midi_decoder.midi_commands_number++;
				break;
			case PROGRAM_CHANGE			:
				l_index += MidiParseProgramChange (buf[l_index+2],buf[l_index+3]);
				A_midi_decoder.midi_commands_number++;
				break;
				/*
			case POLY_PRESSURE			:
				l_index += UsbMidiParsePolyPressure (channel,midi_note,velocity);
				break;
			case PITCH_BEND			:
				l_index += UsbMidiParsePitchBend (channel,midi_note,velocity);
				break;
			case SINGLEBYTE			:
				l_index += UsbMidiParseSingleByte (channel,midi_note,velocity);
				break;
			case MISC			:
				l_index += UsbMidiParseMisc (channel,midi_note,velocity);
				break;
			case CABLE			:
				l_index += UsbMidiParseCable (channel,midi_note,velocity);
				break;
			case TWO_BYTES			:
				l_index += UsbMidiParseTwoBytes (channel,midi_note,velocity);
				break;
			case THREE_BYTES			:
				l_index += UsbMidiParseThreeBytes (channel,midi_note,velocity);
				break;
				*/
			default			:	l_index += 4; break;
		}
	}
}

uint32_t MidiInit(A_midi_t *MIDI)
{
	if ( MIDI->SysEx == NULL )
			return 1;
	if ( MIDI->Note == NULL )
		return 1;
	if ( MIDI->ControlChange == NULL )
		return 1;
	if ( MIDI->ProgramChange == NULL )
		return 1;
	A_midi_decoder.SysEx = MIDI->SysEx;
	A_midi_decoder.Note = MIDI->Note;
	A_midi_decoder.ControlChange = MIDI->ControlChange;
	A_midi_decoder.ProgramChange = MIDI->ProgramChange;
	A_midi_decoder.midi_received_sysex_buffer = MIDI->midi_received_sysex_buffer;
	return 0;
}

#endif // #ifdef MIDI_ENABLED

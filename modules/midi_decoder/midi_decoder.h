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
 * midi_decoder.h
 *
 *  Created on: Oct 2, 2025
 *      Author: fil
 */

#ifndef MODULES_MIDI_DECODER_MIDI_DECODER_H_
#define MODULES_MIDI_DECODER_MIDI_DECODER_H_

#ifdef MIDI_ENABLED

#define	MIDI_RXBUF_SIZE			64

#define	SYSEX_MAX_LEN			128
#define	NUM_PROGRAMS_MAX		8

#define	CMD_MASK				0xF0
#define	CHANNEL_MASK			0x0F

#define	CABLEN_MASK				0xF0
#define	CIN_MASK				0x0F

#define	MISC					0x00
#define	CABLE					0x01
#define	TWO_BYTES				0x02
#define	THREE_BYTES				0x03
#define	SYSEX_STARTC			0x04
#define	SYSEX_END_1				0x05
#define	SYSEX_END_2				0x06
#define	SYSEX_END_3				0x07

#define	NOTE_OFF				0x08
#define	NOTE_ON					0x09
#define	POLY_PRESSURE			0x0A
#define	CONTROL_CHANGE			0x0B
#define	PROGRAM_CHANGE			0x0C
#define	CHANNEL_PRESSURE		0x0D
#define	PITCH_BEND				0x0E
#define	SINGLEBYTE				0x0F

#define	SYSEX_START				0xF0
#define	SYSEX_END				0xF7

typedef struct {
	void		(*SysEx)(void);
	void		(*Note)(uint8_t midi_channel , uint8_t midi_note , uint8_t midi_velocity);
	void		(*ControlChange)(uint8_t cc_index,uint8_t cc_value);
	void		(*ProgramChange)(uint8_t pc_index,uint8_t pc_value);
	uint8_t		midi_commands_number;
	uint8_t		midi_commands[MIDI_RXBUF_SIZE/4];
	uint8_t		midi_note[MIDI_RXBUF_SIZE/4];
	uint8_t		midi_channel[MIDI_RXBUF_SIZE/4];
	uint8_t		midi_velocity[MIDI_RXBUF_SIZE/4];
	uint8_t		*midi_received_sysex_buffer;
	uint8_t		midi_received_sysex_len;
	uint8_t		*midi_transmit_sysex_buffer;
	uint8_t		midi_transmit_sysex_len;
} A_midi_decoder_t;

typedef struct {
	A_midi_decoder_t	*A_midi_decoder;
	void		(*SysEx)(void);
	void		(*Note)(uint8_t midi_channel , uint8_t midi_note , uint8_t midi_velocity);
	void		(*ControlChange)(uint8_t cc_index,uint8_t cc_value);
	void		(*ProgramChange)(uint8_t pc_index,uint8_t pc_value);
	uint8_t		*midi_received_sysex_buffer;
	uint8_t		*midi_transmit_sysex_buffer;
} A_midi_t;

extern	A_midi_decoder_t	A_midi_decoder;

extern	uint32_t MidiInit(A_midi_t *MIDI);
extern	void MidiParser(uint8_t* buf, uint16_t len);

#endif // #ifdef MIDI_ENABLED

#endif /* MODULES_MIDI_DECODER_MIDI_DECODER_H_ */

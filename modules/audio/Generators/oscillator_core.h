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
 * Project : bb1xx_743_00 
*/
/*
 * oscillator_core.h
 *
 *  Created on: Jan 8, 2024
 *      Author: fil
 */

#ifndef STM32H743_AUDIOPROCESSES_AUDIO_COMPONENTS_GENERATORS_OSCILLATOR_CORE_H_
#define STM32H743_AUDIOPROCESSES_AUDIO_COMPONENTS_GENERATORS_OSCILLATOR_CORE_H_

#ifdef SYNTH_ENGINE_ENABLE

#define	NUMOSCILLATORS	32
typedef struct _OscillatorsTypeDef
{
	uint32_t 				oscillator_age;
	uint8_t 				state;
	uint8_t 				midi_note;
	uint8_t 				waveform;
	uint8_t 				adsr_state;
	uint16_t 				adsr_prescaler;
	uint16_t 				adsr_counter;
	float					Alevel;
	float					Dlevel;
	float					Slevel;
	float					Rlevel;
	float					Avalue;
	float					Dvalue;
	float					Astep;
	float					Dstep;
	float					Rstep;
	uint16_t 				current_phase;
	uint16_t 				delta_phase;
	float 					detune;
	uint8_t 				duty;
	float 					volume;
} OscillatorsTypeDef;

#define	INT_PRECISION					256
#define	INT_PRECISION_SHIFT				(INT_PRECISION >> 5)
#define	MAX_DETUNE_PERCENT_F			2.0f
#define	SINE							0
#define	TRIANGLE						1
#define	SQUARE							2
#define	NOISE							3

#define	DAC_BIT							16
#define	DAC_RESOLUTION					(1<<DAC_BIT)
#define	HALF_DAC_RESOLUTION				(DAC_RESOLUTION/2)

#define	HALF_NUMBER_OF_AUDIO_SAMPLES	(AUDIO_BUF_SIZE/2)
#define	NUMBER_OF_AUDIO_SAMPLES			AUDIO_BUF_SIZE
#define	WAVETABLE_SIZE					AUDIO_BUF_SIZE
#define	VOICES							2
#define	SAMPLE_FREQUENCY				48000
#define	SAMPLE_PERIOD					(1.0F / SAMPLE_FREQUENCY)

#define	OSC_OFF							0
#define	OSC_GO_OFF						1
#define	OSC_ON							2

#define	DUTY_SCALE	(256.0F/100.0F)

#define	INVALID_MIDI_NOTE				0x80

enum ADSR_STATUS
{
	OSC_A_STATE,
	OSC_D_STATE,
	OSC_S_STATE,
	OSC_R_STATE,
	ADSR_CLOSE_STATE
};

typedef struct _SystemFlagsTypeDef
{
	/*
	uint16_t 	current_brightness;
	*/
	uint8_t 	audio_flags;
	uint8_t 	oscillator_flags;
	uint8_t		osc_waves[4];
	uint8_t 	osc_duty[4];
	uint8_t 	osc_detune[4];
	uint8_t 	fosc_detune[4];
	uint8_t 	osc_volume[4];	// range 0..10
	uint8_t 	osc_duty_percent[4];
	/*
	uint8_t 	Atime,Dtime,Sval,Rtime;
	uint8_t 	afx_flags;
	uint8_t 	afxtype_flags;
	uint8_t 	delay_flags;
	uint16_t 	delay_value_from_prog;
	uint16_t 	delay_value_from_midi;
	uint8_t 	control_flags;
	uint8_t 	rollback_flags;
	uint8_t 	program_number;
	uint8_t 	systick_counter;
	uint8_t 	tonormaldisplay_counter;
	uint8_t 	led_counter;
	uint8_t 	buttons_flag;
	uint8_t 	buttons_debounce;
	uint8_t 	menu_state;
	uint8_t 	menu_line_counter;
	uint8_t		midi_rx_length;
	uint8_t		midi_rx_buffer[USB_BUF_MAX_LEN];
	uint8_t		sysex_buffer[SYSEX_MAX_LEN];
	uint8_t		sysex_len;
	uint16_t 	control_adc1_buf[4];
	uint16_t 	control_adc2_buf[4];
	float 		delay_sample_multiplier,delay_feedback_multiplier;
	uint16_t 	last_tuner_val;
	*/
	float 		tuner_delta_multiplier;
	/*
	uint16_t 	last_fm_val;
	float 		fm_delta_multiplier;
	*/
	float 		oscillator_tuner_constant;
	/*
	uint16_t	delay_insertion_pointer;
	uint16_t	delay_extraction_pointer;
	float 		cv_voltage;
	float 		cv_voltage_div_10;
	*/
}SystemFlagsTypeDef;
/* audio_flags values */
#define	AUDIO_VCO_HALF_FLAG			0x01
#define	AUDIO_READY_FLAG			0x80
/* oscillator_flags */
#define	OSC_VOLUME_PENDING			0x01
#define	OSC_DETUNE_PENDING			0x02
#define	OSC_DUTY_PENDING			0x04
#define	OSC_WAVE_PENDING			0x08
#define	OSC_FM_PENDING				0x10
#define	OSC_TUNE_PENDING			0x20
//NOT USED							0x40
#define	OSC_SRCFLAG					0x80	/* 0 is from Cv , 1 is from MIDI */

extern	void SetADSR_oscParams(uint32_t osc_number, uint8_t velocity );
extern	void InitOscillatorsTables(void);
extern	void RunOscillator32(void);

#endif //#ifdef SYNTH_ENGINE_ENABLE
#endif /* STM32H743_AUDIOPROCESSES_AUDIO_COMPONENTS_GENERATORS_OSCILLATOR_CORE_H_ */

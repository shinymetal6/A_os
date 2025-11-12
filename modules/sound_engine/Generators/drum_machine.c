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
 * drum_machine.c
 *
 *  Created on: Nov 12, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"

#include "DrumSamples/drums.h"

ITCM_AREA_CODE static void play_drum(uint8_t drum_id)
{
AUDIO_Source_TypeDef *drum_machine = AudioSourceLeft;

	if (drum_id >= NUM_VOICES)
		return;
	while ( drum_machine != NULL )
	{
		if ( drum_machine->source_type == SOUND_SOURCE_IS_DRUM )
		{
			DrumMachine_TypeDef *drum_machine_voices = (DrumMachine_TypeDef *)drum_machine->ptr_gen_struct;
			drum_machine_voices->voices[drum_id].pos = 0;
			drum_machine_voices->voices[drum_id].playing = 1;
			return;
		}
		drum_machine = (AUDIO_Source_TypeDef *)drum_machine->next_source;
	}
}

// Call this periodically
ITCM_AREA_CODE void Drum_Machine_Update_sequencer(void)
{
AUDIO_Source_TypeDef *drum_machine = AudioSourceLeft;

	while ( drum_machine != NULL )
	{
		if ( drum_machine->source_type == SOUND_SOURCE_IS_DRUM )
		{
			DrumMachine_TypeDef *drum = (DrumMachine_TypeDef *)drum_machine->ptr_gen_struct;
			uint32_t i;
			drum->step_counter++;
		    if (drum->step_counter >= drum->step_interval)
		    {
		    	drum->step_counter = 0;
		        for (i = 0; i < NUM_VOICES; i++)
		            if (drum->pattern[i][drum->current_step])
		                play_drum(i);
		    }
		    drum->current_step = (drum->current_step + 1) % PATTERN_STEPS;
		}
		drum_machine = (AUDIO_Source_TypeDef *)drum_machine->next_source;
	}
}

ITCM_AREA_CODE void Drum_Machine_audio(void)
{
AUDIO_Source_TypeDef *drum_machine = AudioSourceLeft;
uint32_t i,v;
	while ( drum_machine != NULL )
	{
		if ( drum_machine->source_type == SOUND_SOURCE_IS_DRUM )
		{
		    for (i = 0; i < drum_machine->block_size; i++)
		    {
		    	drum_machine->out_buf[i] = 0;
		    }

			DrumMachine_TypeDef *drum = (DrumMachine_TypeDef *)drum_machine->ptr_gen_struct;

			for (v = 0; v < NUM_VOICES; v++)
			{
				if ( !drum->voices[v].playing)
					continue;

				const int16_t	*sample = drum->voices[v].sample;

				for (uint32_t i = 0; i < drum_machine->block_size; i++)
				{
					if (drum->voices[v].pos < drum->voices[v].length)
					{
						drum_machine->out_buf[i] += __FLOAT_2_Q15(((float )sample[drum->voices[v].pos] * drum->voices[v].volume));
						drum->voices[v].pos++;
					}
					else
					{
						drum->voices[v].playing = 0; // Done
						return;
					}
				}
			}
		}
		else
		{
			drum_machine = (AUDIO_Source_TypeDef *)drum_machine->next_source;
		}
	}
}

DrumVoice_TypeDef sample_voices[] =
{
    { TR808_kick,   		TR808_kick_length,   	0, 1.0f, 0 }, // Kick
    { TR808_snare,  		TR808_snare_length,  	0, 0.9f, 0 }, // Snare
    { TR808_hh,     		TR808_hh_length,     	0, 0.7f, 0 }, // Hi-hat
    { TR808_clap,   		TR808_clap_length,   	0, 0.8f, 0 },  // Clap
};

DrumVoice_TypeDef sample_boom_voices[] =
{
    { TR808_boom_kick,   	TR808_boom_kick_length,	0, 1.0f, 0 }, // Boom Kick
    { TR808_snare,  		TR808_snare_length,  	0, 0.9f, 0 }, // Snare
    { TR808_hh,     		TR808_hh_length,     	0, 0.7f, 0 }, // Hi-hat
    { TR808_clap,   		TR808_clap_length,   	0, 0.8f, 0 },  // Clap
};
// Simple 16-step pattern (1 = trigger)
uint8_t sample_pattern[NUM_VOICES][PATTERN_STEPS] = {
    {1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0}, // Kick: every beat
    {0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0}, // Snare: backbeat
    {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1}, // Hi-hat: 16ths
    {0,0,0,0, 0,0,1,0, 0,0,0,0, 0,0,1,0}  // Clap: off-beat
};

ITCM_AREA_CODE static void init_default_data(DrumMachine_TypeDef *drum)
{
	bcopy(drum->pattern , sample_pattern, sizeof(sample_pattern));
	bcopy(drum->voices , sample_voices, sizeof(sample_voices));
}

ITCM_AREA_CODE uint8_t Drum_Machine_Register(AUDIO_Source_TypeDef *drum_machine)
{
	if ( drum_machine->out_buf == NULL )
		return 1;
	if ( drum_machine->ptr_gen_struct == NULL )	// pointer to the user space voices drum machine
		return 1;

	if ( drum_machine->channel_in == AUDIO_SOURCE_LEFT)
	{
		if ( AudioSourceLeft == NULL )
		{
			AudioSourceLeft = drum_machine;
			drum_machine->next_source = NULL;
		}
		else
		{
			AUDIO_Source_TypeDef *source = AudioSourceLeft;
			while(source->next_source != NULL)
				source = (AUDIO_Source_TypeDef *)source->next_source;
			source->next_source = (uint32_t *)drum_machine;
			drum_machine->next_source = NULL;
		}
	}
	drum_machine->block_size = I2S_EFFECT_SIZE;
    // Initialize all voices
	DrumMachine_TypeDef *drum = (DrumMachine_TypeDef *)drum_machine->ptr_gen_struct;
    for (int i = 0; i < NUM_VOICES; i++)
    {
    	drum->voices[i].pos = 0;
    	drum->voices[i].playing = 0;
    }
    drum->step_interval = 10;
    drum->current_step = drum->step_counter = 0;
    init_default_data(drum);
	drum_machine->source_type = SOUND_SOURCE_IS_DRUM;
	return 0;
}

#endif // #ifdef SOUND_ENGINE_ENABLED


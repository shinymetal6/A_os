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


ITCM_AREA_CODE void Start_Drum_Sequence(uint8_t drum_id)
{
AUDIO_Source_TypeDef *drum_machine = AudioSourceLeft;

	if (drum_id >= NUM_VOICES)
		return;
	while ( drum_machine != NULL )
	{
		if ( drum_machine->source_type == SOUND_SOURCE_IS_DRUM )
		{
			DrumMachine_TypeDef *drum_machine_voices = (DrumMachine_TypeDef *)drum_machine->ptr_gen_struct;
			drum_machine_voices->drum_voices[drum_id].pos = 0;
			drum_machine_voices->drum_voices[drum_id].playing = 1;
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
		        {
		        	if ( drum->drum_voices[i].sample != NULL )
		        	{
			            if (drum->drum_pattern[i][drum->current_step])
			            	Start_Drum_Sequence(i);
		        	}
		        }
		    }
		    drum->current_step = (drum->current_step + 1) % drum->pattern_steps;
		}
		drum_machine = (AUDIO_Source_TypeDef *)drum_machine->next_source;
	}
}

ITCM_AREA_CODE q15_t *Drum_Machine_audio(void)
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
				if ( !drum->drum_voices[v].playing)
					continue;

				const int16_t	*sample = drum->drum_voices[v].sample;

				for (uint32_t i = 0; i < drum_machine->block_size; i++)
				{
					if (drum->drum_voices[v].pos < drum->drum_voices[v].length)
					{
						drum_machine->out_buf[i] += __FLOAT_2_Q15(((float )sample[drum->drum_voices[v].pos] * drum->drum_voices[v].volume));
						drum->drum_voices[v].pos++;
					}
					else
					{
						drum->drum_voices[v].playing = 0; // Done
						return drum_machine->out_buf;
					}
				}
				return NULL;
			}
			return NULL;
		}
		else
		{
			drum_machine = (AUDIO_Source_TypeDef *)drum_machine->next_source;
		}
	}
	return NULL;
}

ITCM_AREA_CODE static void init_default_drum_voices(DrumMachine_TypeDef *drum)
{
	bcopy(drum->pattern, drum->drum_pattern, drum->pattern_size);
	switch(drum->model)
	{
	case	TR_606	:
		bcopy(TR_606_voices,  drum->drum_voices , sizeof(DrumVoice_TypeDef)*TR_606_NUM_VOICES);
		break;
	case	TR_707	:
		bcopy(TR_707_voices,  drum->drum_voices , sizeof(DrumVoice_TypeDef)*TR_707_NUM_VOICES);
		break;
	case	TR_808	:
		bcopy(TR_808_voices,  drum->drum_voices , sizeof(DrumVoice_TypeDef)*TR_808_NUM_VOICES);
		break;
	case	TR_909	:
		bcopy(TR_909_voices,  drum->drum_voices , sizeof(DrumVoice_TypeDef)*TR_909_NUM_VOICES);
		break;
	case	HR_16B	:
		bcopy(HR_16B_voices,  drum->drum_voices , sizeof(DrumVoice_TypeDef)*HR_16B_NUM_VOICES);
		break;
	case	LM1	:
		bcopy(LM1_voices,  drum->drum_voices , sizeof(DrumVoice_TypeDef)*LM1_NUM_VOICES);
		break;
	default	:
		bcopy(TR_808_voices,  drum->drum_voices , sizeof(DrumVoice_TypeDef)*TR_808_NUM_VOICES);
		break;
	}
}

ITCM_AREA_CODE uint8_t Drum_Machine_Register(AUDIO_Source_TypeDef *drum_machine)
{
	if ( drum_machine->out_buf == NULL )
		return 1;
	if ( drum_machine->ptr_gen_struct == NULL )	// pointer to the user space drum_voices drum machine
		return 1;

	if ( drum_machine->source == AUDIO_SOURCE_LEFT)
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
	DrumMachine_TypeDef *drum = (DrumMachine_TypeDef *)drum_machine->ptr_gen_struct;
	if ( drum->step_interval == 0 )
		drum->step_interval = 10;
	if ( drum->pattern_steps == 0 )
		drum->pattern_steps = PATTERN_STEPS;

    drum->current_step = drum->step_counter = 0;
    init_default_drum_voices(drum);
	drum_machine->source_type = SOUND_SOURCE_IS_DRUM;
	return 0;
}

#endif // #ifdef SOUND_ENGINE_ENABLED


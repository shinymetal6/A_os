/*
 * noise.c
 *
 *  Created on: Aug 30, 2021
 *      Author: fil
 */

#include "main.h"
#include "../audio_component.h"

void NoiseFill(void)
{
uint32_t	i;
uint32_t random_number;
	for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i+=2)
	{
		HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
		noise_buffer_gen[i] =  (random_number>>16) & 0x0fff;
		noise_buffer_gen[i+1] =  random_number & 0xfff;
	}
}

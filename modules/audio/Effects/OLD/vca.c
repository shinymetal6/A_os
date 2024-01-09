/*
 * am_modulation.c
 *
 *  Created on: Jul 30, 2021
 *      Author: fil
 */

#include "main.h"
#include "../audio_component.h"

void Vca( uint16_t *buffer_out,uint16_t *buffer_in)
{
uint32_t	i;
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( SystemFlags.control_flags & CONTROL_VCA) == CONTROL_VCA)
			*buffer_out++ = (uint16_t )((float )*buffer_in++ * ((float )VCO_AM / 4096.0F));
		else
			*buffer_out++ = *buffer_in++;
	}
}

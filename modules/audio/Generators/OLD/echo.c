/*
 * echo.c
 *
 *  Created on: Jul 5, 2021
 *      Author: fil
 */


#include "main.h"
#include "../audio_component.h"

void echo( uint16_t *buffer_out,uint16_t *buffer_in)
{
uint32_t	i;
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		*buffer_out++ = *buffer_in++;

}

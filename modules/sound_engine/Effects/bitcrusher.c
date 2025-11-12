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
 * bitcrusher.c
 *
 *  Created on: Nov 11, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED

#include "../sound_engine.h"
#include "bitcrusher.h"

#include <stdint.h>
#include <math.h> // For pow() if needed

// Initialize the bitcrusher structure

// Set new bit depth
ITCM_AREA_CODE void BitCrusher_SetBitDepth(BITCRUSHER_Effect_TypeDef *bitcrusher, uint8_t i_bit_depth)
{
    if (i_bit_depth > 0 && i_bit_depth <= 32) {
        bitcrusher->i_bit_depth = i_bit_depth;
    }
}

// Set new downsample factor
ITCM_AREA_CODE void BitCrusher_SetDownsampleFactor(BITCRUSHER_Effect_TypeDef *bitcrusher, uint8_t i_downsample_factor)
{
    if (i_downsample_factor > 0) {
        bitcrusher->i_downsample_factor = i_downsample_factor;
    }
}

// Process a single audio sample
ITCM_AREA_CODE static float bitcrusher_effect(BITCRUSHER_Effect_TypeDef *bitcrusher, float input)
{
	float output_sample = input;

    // --- Downsample ---
    bitcrusher->sample_counter++;
    if (bitcrusher->sample_counter < bitcrusher->i_downsample_factor) {
        // Return the last processed sample (hold)
        return bitcrusher->last_sample;
    } else {
        // Process this sample and reset counter
        bitcrusher->sample_counter = 0;
    }

    // --- Bit Depth Reduction ---
    if (bitcrusher->i_bit_depth < 32) { // Only process if bit depth is less than full
        uint8_t reduction_bits = 32 - bitcrusher->i_bit_depth;

        // Shift right to remove lower bits
        q15_t ssample = __FLOAT_2_Q15(input);
        ssample >>= reduction_bits;

        float quantized_sample = __Q15_2_FLOAT(ssample);

        // Shift left to restore magnitude (fills lower bits with 0s)
        q15_t nsample = __FLOAT_2_Q15(quantized_sample);
        nsample >>= reduction_bits;

        quantized_sample = __Q15_2_FLOAT(nsample);

        output_sample = __Q15_2_FLOAT(quantized_sample);
    }

    // Store the processed sample for potential hold during downsampling
    bitcrusher->last_sample = output_sample;

    return output_sample;
}

ITCM_AREA_CODE void Effect_BITCRUSHER_Init(uint32_t *effect_s)
{
BITCRUSHER_Effect_TypeDef *bitcrusher = (BITCRUSHER_Effect_TypeDef *)effect_s;

    bitcrusher->i_bit_depth = (bitcrusher->bit_depth > 0 && bitcrusher->bit_depth <= 32) ? bitcrusher->i_bit_depth : 16; // Clamp to valid range
    bitcrusher->i_downsample_factor = (bitcrusher->downsample_factor > 0) ? bitcrusher->downsample_factor : 1;
    bitcrusher->sample_counter = 0;
    bitcrusher->last_sample = 0;
}

ITCM_AREA_CODE void Effect_BITCRUSHER(uint32_t *effect_s)
{
uint32_t	i;
BITCRUSHER_Effect_TypeDef *bitcrusher = (BITCRUSHER_Effect_TypeDef *)effect_s;

	if ((( bitcrusher->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( bitcrusher == NULL ))
		return;
	bitcrusher->time_start = DWT->CYCCNT;

	if (( bitcrusher->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
	{
		for ( i=0;i<bitcrusher->block_size;i++)
			bitcrusher->out_buf[i] = (q15_t ) bitcrusher_effect(bitcrusher,__Q15_2_FLOAT(bitcrusher->in_buf[i]));
	}
	else
	{
		for ( i=0;i<bitcrusher->block_size;i++)
			bitcrusher->out_buf[i]  = bitcrusher->in_buf[i];
	}

	bitcrusher->effect_time = (DWT->CYCCNT - bitcrusher->time_start) / (HSI_CLOCK / 1000000);
}
#endif // #ifdef SOUND_ENABLED


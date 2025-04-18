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
 * fm_oscillator.c
 *
 *  Created on: Apr 18, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
//#include "../../../kernel/kernel_opt.h"

#include "../audio.h"
#include "../effects.h"

#ifdef AUDIO_GENERATORS_ENABLED

#include "arm_math.h"
#include "fm_oscillator.h"

// Initialize oscillator
ITCM_AREA_CODE void osc_init_q15(FM_Oscillator_typedef *osc, float32_t frequency, WaveformType waveform) {
    osc->phase = 0; // Start with zero phase
    osc->phase_increment = (q31_t)((frequency / FM_OSC_SAMPLE_RATE) * (1LL << 31));
    osc->waveform = waveform;
}

// Update oscillator frequency
ITCM_AREA_CODE void osc_set_frequency_q15(FM_Oscillator_typedef *osc, float32_t frequency) {
    osc->phase_increment = (q31_t)((frequency / FM_OSC_SAMPLE_RATE) * (1LL << 31));
}

// Generate a block of waveform samples (Q15)
ITCM_AREA_CODE static void osc_generate_block_q15(FM_Oscillator_typedef *osc, q15_t *output, uint32_t block_size) {
    for (int i = 0; i < block_size; i++) {
        q15_t sample = 0;

        switch (osc->waveform) {
            case WAVEFORM_SINE: {
                // Sine wave generation
                q15_t phase_q15 = (q15_t)(osc->phase >> 16); // Scale phase to Q15 range
                sample = arm_sin_q15(phase_q15);
                break;
            }
            case WAVEFORM_SQUARE: {
                // Square wave generation
                if (osc->phase < (1LL << 30)) { // First half of the cycle
                    sample = 32767; // Maximum value in Q15
                } else { // Second half of the cycle
                    sample = -32768; // Minimum value in Q15
                }
                break;
            }
            case WAVEFORM_SAWTOOTH: {
                // Sawtooth wave generation
                q15_t phase_q15 = (q15_t)(osc->phase >> 16); // Scale phase to Q15 range
                sample = phase_q15; // Linear ramp from -32768 to 32767
                break;
            }
            default:
                sample = 0; // Default to silence
                break;
        }

        output[i] = sample;

        // Update phase accumulator
        osc->phase += osc->phase_increment;
        if (osc->phase >= (1LL << 31)) { // Wrap around if phase exceeds 2π
            osc->phase -= (1LL << 31);
        }
    }
}

// FM Synthesis: Generate a block of FM-modulated samples (Q15)
ITCM_AREA_CODE void fm_synthesis_osc(FM_Oscillator_typedef *carrier, FM_Oscillator_typedef *modulator, q15_t *output, uint32_t block_size, q15_t modulation_index) {
    for (int i = 0; i < block_size; i++)
    {
        // Generate modulator waveform sample
        q15_t mod_sample;
        osc_generate_block_q15(modulator, &mod_sample, 1);

        // Apply modulation to carrier frequency
        q31_t modulated_phase_increment = carrier->phase_increment + ((q31_t)mod_sample * modulation_index >> 15);

        // Generate carrier waveform sample
        q15_t car_sample;
        carrier->phase_increment = modulated_phase_increment; // Temporarily update phase increment
        osc_generate_block_q15(carrier, &car_sample, 1);

        output[i] = car_sample;

        // Restore original phase increment for carrier
        carrier->phase_increment = (q31_t)((carrier->phase_increment / (1LL << 31)) * FM_OSC_SAMPLE_RATE);
    }
}


#endif

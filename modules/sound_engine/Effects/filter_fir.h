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
 * filter_fir.h
 *
 *  Created on: Oct 30, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_EFFECTS_FILTER_FIR_H_
#define MODULES_SOUND_ENGINE_EFFECTS_FILTER_FIR_H_

#include <stdint.h>
/* fir coeffs generator */
#include <stdint.h>
#include <math.h>

// Maximum taps supported (adjust based on available RAM)
//#define FIR_GEN_MAX_TAPS 128

// Window types
typedef enum {
    FIR_WINDOW_HAMMING,
    FIR_WINDOW_HANN,
    FIR_WINDOW_BLACKMAN
} fir_window_type_t;

// Filter types
typedef enum {
    FIR_TYPE_LOWPASS,
    FIR_TYPE_HIGHPASS,
    FIR_TYPE_BANDPASS
} fir_filter_type_t;

/**
 * Generate FIR coefficients on-device
 * @param coeffs: Output buffer (must be >= taps * sizeof(float))
 * @param taps: Number of taps (must be even, 4-128)
 * @param type: Filter type
 * @param fs: Sample rate (e.g., 48000.0f)
 * @param f1: Cutoff frequency (lowpass/highpass) or low freq (bandpass)
 * @param f2: High frequency (bandpass only)
 * @param window: Window type
 * @return 0 on success, -1 on error
 */
int fir_generate_coeffs(float *coeffs, uint32_t taps,
                       fir_filter_type_t type,
                       float fs, float f1, float f2,
                       fir_window_type_t window);

/* end of fir coeffs generator */

#define FIR_TAPS  128  // Number of coefficients
#define FIR_BUFFER_SIZE 256  // Match your I2S half-buffer

typedef struct {
    float coeffs[FIR_TAPS];
    float delay_line[FIR_TAPS];
    uint16_t write_index;
} fir_float_t;

typedef struct {
	/* effect header */
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_effect;
	q15_t				*in_buf;
	q15_t				*out_buf;
	void 				(*effect)(uint32_t 	*effect_data);
	void 				(*effect_init)(uint32_t *effect_data);
	uint16_t			block_size;
	float				sample_rate;
	/* Here finishes the common area */
	uint32_t			time_start;
	uint32_t			effect_time;
	/* effect data */
	uint16_t			num_taps;
    float				coeffs[FIR_TAPS];
	fir_float_t			fir_coeffs;
	float 				cutoffFrequency;// = 1000.0f : cutoff frequency @1 kHz , center frequency for bw filters
	float 				bandwidth;// = 500.0f;        // Bandwidth for band-pass and notch filters
} FIR_Effect_TypeDef;
#define FIR_UPDATE_PARAMS 	0x01           		// Number of cascaded biquad stages (order = 2 * NUM_BIQUADS)


#endif /* MODULES_SOUND_ENGINE_EFFECTS_FILTER_FIR_H_ */

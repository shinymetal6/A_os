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
 * effects.h
 *
 *  Created on: Feb 22, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_H_
#define MODULES_AUDIO_EFFECTS_H_

#define	MAX_PARAMS					8
#define	MAX_SINGLESAMPLE_EFFECTS	8

typedef struct _BlockEffectsTypeDef
{
	uint8_t		*nxt_effect,*pre_effect;
	char 		effect_name[40];
	char 		effect_param[MAX_PARAMS][40];
	float 		parameter[MAX_PARAMS];
	uint8_t 	num_params;
	uint8_t 	effect_status;
	uint8_t 	current_order;
	void 		(*apply_effect)(int16_t* inputData, int16_t* outputData);
} BlockEffectsTypeDef;

typedef struct _SingleSampleEffectsTypeDef
{
	uint8_t		*nxt_effect,*pre_effect;
	char 		effect_name[40];
	char 		effect_param[MAX_PARAMS][40];
	float 		parameter[MAX_PARAMS];
	uint8_t 	num_params;
	uint8_t 	effect_status;
	uint8_t 	current_order;
	int16_t		(*apply_effect)(int16_t inputData);
} SingleSampleEffectsTypeDef;

/* effect_status */
#define	EFFECT_WAVE_ON 0x01
#define	EFFECT_ENABLED 0x80

#define	ADSR_EFFECT_ID			0
#define	DISTORSION_EFFECT_ID	1
#define	ECHO_EFFECT_ID			2
#define	FLANGER_EFFECT_ID		3
#define	FFT_ID					4
#define	IIR_EFFECT_ID			5
#define	MOOG1_F_EFFECT_ID		6
#define	MOOG2_F_EFFECT_ID		7
#define	NOISE_EFFECT_ID			8
#define	PASSTHROUGH_EFFECT_ID	9
#define	PHASER_EFFECT_ID		10
#define	PITCHSHIFT_EFFECT_ID	11
#define	REVERB_EFFECT_ID		12
#define	TREMOLO_EFFECT_ID		13
#define	VIBRATO_EFFECT_ID		14
#define	VCA_EFFECT_ID			15
#define	WAH_EFFECT_ID			16

#define	MAX_BLOCK_EFFECTS		WAH_EFFECT_ID+1

#define	BIQUAD_S_EFFECT_ID		0
#define	VCA_S_EFFECT_ID			1

typedef struct _EffectsOrderTypeDef
{
	uint8_t		effect_order[MAX_BLOCK_EFFECTS];
	uint8_t		effect_order_index;
} EffectsOrderTypeDef;
#define	EFFECT_ORDER_NOT_USED	0x80

#include 	<stdio.h>
#include 	<string.h>
#include 	"arm_math.h"

#include	"adsr.h"
#include	"biquad_s.h"
#include	"distortion.h"
#include	"echo.h"
#include	"flanger.h"
#include	"fft.h"
#include	"iir.h"
#include	"moog_filters.h"
#include	"noise.h"
#include	"passthrough.h"
#include	"phaser.h"
#include	"pitch_shift.h"
#include	"reverb.h"
#include	"tremolo.h"
#include	"vca.h"
#include	"vca_s.h"
#include	"vibrato.h"
#include	"wah.h"

extern	BlockEffectsTypeDef			BlockEffect[MAX_BLOCK_EFFECTS];
extern	SingleSampleEffectsTypeDef	SingleSampleEffect[MAX_SINGLESAMPLE_EFFECTS];

extern	void InitEffectsSequencer(void);
extern	uint16_t BlockEffectsSequencer(void);
extern	void SingleSampleEffectsSequencer(void);
extern	void ResetEffectsSequencer(void);
extern	void InsertBlockEffect(void (*do_effect),uint8_t position,uint8_t enabled);
extern	void RemoveBlockEffect(void (*do_effect),uint8_t position);

extern	void InsertSingleSampleEffect(void 	(*do_effect),uint8_t position);

#endif /* MODULES_AUDIO_EFFECTS_H_ */

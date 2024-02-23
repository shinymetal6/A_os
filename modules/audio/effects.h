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

#define	MAX_PARAMS		8
#define	MAX_EFFECTS		16

typedef struct _EffectsTypeDef
{
	char 					effect_name[40];
	char 					effect_param[MAX_PARAMS][40];
	float 					parameter[MAX_PARAMS];
	uint8_t 				num_params;
	uint8_t 				effect_status;
	void 					(*do_effect)(int16_t* inputData, int16_t* outputData);
} EffectsTypeDef;
/* effect_status */
#define	EFFECT_WAVE_ON 0x01
#define	EFFECT_ENABLED 0x80

#define	ADSR_EFFECT_ID			0
#define	DISTORSION_EFFECT_ID	1
#define	ECHO_EFFECT_ID			2
#define	FLANGER_EFFECT_ID		3
#define	IIR_EFFECT_ID			4
#define	MOOG1_F_EFFECT_ID		5
#define	MOOG2_F_EFFECT_ID		6
#define	NOISE_EFFECT_ID			7
#define	PHASER_EFFECT_ID		8
#define	PITCHSHIFT_EFFECT_ID	9
#define	REVERB_EFFECT_ID		10
#define	TREMOLO_EFFECT_ID		11
#define	VIBRATO_EFFECT_ID		12
#define	VCA_EFFECT_ID			13
#define	WAH_EFFECT_ID			14
#define	LAST_EFFECT				WAH_EFFECT_ID

typedef struct _EffectsPipeTypeDef
{
	uint8_t 				num_effects;
	void 					(*execute_effect)(int16_t* inputData, int16_t* outputData);
} EffectsPipeTypeDef;


#include 	<stdio.h>
#include 	<string.h>
#include 	"arm_math.h"

#include	"adsr.h"
#include	"distortion.h"
#include	"echo.h"
#include	"flanger.h"
#include	"iir.h"
#include	"moog_filters.h"
#include	"noise.h"
#include	"phaser.h"
#include	"pitch_shift.h"
#include	"reverb.h"
#include	"tremolo.h"
#include	"vca.h"
#include	"vibrato.h"
#include	"wah.h"

extern	EffectsTypeDef	Effect[MAX_EFFECTS];

extern	void EffectsSequencer(int16_t* inputData, int16_t* outputData);
extern	void ResetEffectsSequencer(void);
extern	void InsertEffect(void 	(*do_effect));

#endif /* MODULES_AUDIO_EFFECTS_H_ */

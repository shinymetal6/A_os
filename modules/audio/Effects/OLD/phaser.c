/*
 * phaser.c
 *
 *  Created on: Sep 1, 2021
 *      Author: fil
 */


#include "main.h"
#include "../audio_component.h"

/*---------------------------------------------------------------------*/
#define MAX_RATE		7.f // in Hz
#define MIN_RATE		0.02f // in Hz

/*This defines the phaser stages
that is the number of allpass filters
 */
#define PH_STAGES 6

/*---------------------------------------------------------------------*/

static float 	old[PH_STAGES];
static float 	f_min, f_max;
static float 	swrate;
static float 	wet ;
static float	dmin, dmax; //range
static float	fb; //feedback
static float 	lfoPhase;
static float 	lfoInc;
static float	a1;
static float	zm1;


void Phaser_Rate_set(uint8_t val)
{
	swrate = (MAX_RATE - MIN_RATE) / MIDI_MAX * val + MIN_RATE;
	lfoInc = _2PI * swrate / SAMPLE_FREQUENCY;
}

void Phaser_Feedback_set(uint8_t val)
{
	fb = 0.999f * (float )val / MIDI_MAX;
}

void Phaser_Wet_set(uint8_t val)
{
	wet = (float )val / MIDI_MAX;
}

void PhaserRate(float rate)
{
	swrate = rate;
	lfoInc = _2PI * swrate / SAMPLE_FREQUENCY;
}

void PhaserFeedback(float fdb)
{
	fb = fdb;
}

static float allpass(float yin, int ind)
{
	float yout;

	yout = - yin * a1 + old[ind];
	old[ind] = yout * a1 + yin;
	return yout;
}


float process_Phaser(float xin)
{
float yout;
int i;
float d;

	//calculate and update phaser sweep lfo...

	d  = dmin + (dmax - dmin) * ((effects_sinetable[lrintf(ALPHA * lfoPhase)] + 1.f)*0.5f);

	lfoPhase += lfoInc;
	if( lfoPhase >= _2PI ) 	lfoPhase -= _2PI;

	//update filter coeffs
	a1 = (1.f - d) / (1.f + d);

	//calculate output

	yout = allpass(xin + zm1 * fb, 0);

	for(i = 1; i < PH_STAGES; i++)
	{
		yout = allpass(yout, i);
	}
	zm1 = yout;

	yout = (1 - wet) * xin + wet * yout;

	/*
	yout = (yout > 1.0f) ? 1.0f : yout; //clip too loud samples
	yout = (yout < -1.0f) ? -1.0f : yout;
	*/
	return yout*0.8f;
}

void PhaserInit(void)
{
	f_min = 200.f;
	f_max = 1700.f;
	swrate = 0.1f;
	fb = 0.7f;
	wet = 0.3f;

	dmin = 2 * f_min / SAMPLE_FREQUENCY;
	dmax = 2 * f_max / SAMPLE_FREQUENCY;
	lfoInc = _2PI * swrate / SAMPLE_FREQUENCY;
}



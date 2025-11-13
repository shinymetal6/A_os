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
 * linndrum_LM1_samples.c
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED

// ===========================
// 1. LM-1 KICK DRUM ("Bass Drum")
// Deep, punchy, short decay — famously tight
// ===========================
const int16_t LM1_kick[] = {
     0,   400,   780,  1120,  1400,  1610,  1740,  1790,
  1750,  1640,  1460,  1230,   970,   690,   410,   150,
  -100,  -310,  -480,  -600,  -670,  -700,  -690,  -640,
  -560,  -460,  -350,  -240,  -140,   -50,    30,    90,
   130,   150,   150,   140,   120,   100,    80,    60,
    40,    20,    10,     0,     0,     0,     0
};

// ===========================
// 2. LM-1 SNARE DRUM ("Snare Drum")
// Bright, snappy, with natural room ambience (short reverb tail)
// ===========================
const int16_t LM1_snare[] = {
   900, -700,  800, -1000,  600, -400,  700, -500,
  -300,  800, -200,  100, -600,  400, -150,  500,
   100, -500,  300, -100,  400, -250,  100, -300,
    50,  200, -150,   80, -200,  120,  -60,  100,
   -50,  -30,   60,  -30,   15,  -40,   30,  -15,
    12,   -9,    6,   -4,    2,   -1,    1,    0,

    // Simulated room reverb tail (very short)
    8,   -6,    5,   -4,    3,   -2,    2,   -1,
    1,   -1,    1,   -1,    1,    0
};

// ===========================
// 3. LOW TOM ("Low Tom")
// Tuned tom with natural decay (~90 Hz)
// ===========================
const int16_t LM1_low_tom[] = {
    0,  190,  370,  530,  650,  720,  750,  730,
  670,  580,  460,  330,  200,   80,  -30, -120,
 -190, -230, -250, -240, -210, -180, -140, -100,
  -70,  -40,  -20,   -5,    5,    8,    6,    3,
    1,    0
};

// ===========================
// 4. MID TOM ("Mid Tom")
// ~130 Hz
// ===========================
const int16_t LM1_mid_tom[] = {
    0,  230,  440,  600,  710,  760,  750,  690,
  590,  460,  320,  180,   50,  -70, -160, -220,
 -250, -250, -220, -180, -140, -100,  -60,  -30,
  -10,    2,    6,    5,    3,    1,    0
};

// ===========================
// 5. HIGH TOM ("High Tom")
// ~190 Hz
// ===========================
const int16_t LM1_high_tom[] = {
    0,  270,  500,  670,  760,  780,  740,  650,
  520,  370,  220,   80,  -50, -140, -200, -230,
 -230, -200, -160, -120,  -80,  -50,  -30,  -15,
   -5,    2,    4,    3,    1,    0
};

// ===========================
// 6. CLOSED HI-HAT ("Closed Hi-Hat")
// Short, crisp, metallic
// ===========================
const int16_t LM1_closed_hh[] = {
   650, -450,  350, -280,  220, -180,  140, -110,
    90,  -70,   60,  -50,   40,  -30,   20,  -15,
    10,   -8,    6,   -5,    4,   -3,    2,   -1,
     1,    0
};

// ===========================
// 7. OPEN HI-HAT ("Open Hi-Hat")
// Longer decay, shimmering ring
// ===========================
const int16_t LM1_open_hh[] = {
   650, -450,  350, -280,  220, -180,  140, -110,
    90,  -70,   60,  -50,   40,  -30,   20,  -15,
    10,   -8,    6,   -5,    4,   -3,    2,   -1,
     1,   -1,    1,   -1,    1,   -1,    1,   -1,
     0
};

// ===========================
// 8. RIMSHOT ("Rim Shot")
// Wooden stick attack
// ===========================
const int16_t LM1_rimshot[] = {
   750, -550,  400, -300,  200, -150,  100,  -80,
    60,  -50,   40,  -30,   20,  -15,   10,   -8,
     6,   -5,    4,   -3,    2,   -1,    1,    0
};

// ===========================
// 9. CRASH CYMBAL ("Crash")
// Ringing decay with high-frequency content
// ===========================
const int16_t LM1_crash[] = {
   550, -400,  300, -200,  150, -100,   80,  -60,
    50,  -40,   30,  -20,   15,  -10,    8,   -6,
     5,   -4,    3,   -2,    2,   -1,    1,   -1,
     1,   -1,    1,   -1,    1,   -1,    1,   -1,
     1,    0
};

// ===========================
// 10. CLAP ("Hand Clap")
// Multi-hit burst
// ===========================
const int16_t LM1_clap[] = {
   450, -300,  500, -200,  100, -400,  300, -100,
   200, -300,  150, -100,  200, -150,   80,  -60,
    40,  -30,   20,  -15,   10,   -8,    6,   -5,
     4,   -3,    2,   -1,    1,    0
};

DrumVoice_TypeDef LM1_voices[LM1_NUM_VOICES] = {
    { LM1_kick,      LM1_kick_length,       0, 1.0f, 0 },
    { LM1_snare,     LM1_snare_length,      0, 0.9f, 0 },
    { LM1_low_tom,   LM1_low_tom_length,    0, 0.8f, 0 },
    { LM1_mid_tom,   LM1_mid_tom_length,    0, 0.8f, 0 },
    { LM1_high_tom,  LM1_high_tom_length,   0, 0.8f, 0 },
    { LM1_closed_hh, LM1_closed_hh_length,  0, 0.7f, 0 },
    { LM1_open_hh,   LM1_open_hh_length,    0, 0.7f, 0 },
    { LM1_rimshot,   LM1_rimshot_length,    0, 0.7f, 0 },
    { LM1_crash,     LM1_crash_length,      0, 0.8f, 0 },
    { LM1_clap,      LM1_clap_length,       0, 0.8f, 0 }
};


#endif // #ifdef SOUND_ENGINE_ENABLED

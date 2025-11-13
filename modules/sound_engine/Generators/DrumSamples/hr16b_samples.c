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
 * HR_16B_samples.c
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED

// ===========================
// 1. HR-16 KICK (Punchy, mid-focused, short decay)
// Simulates the "Standard Kick" or "Hard Kick"
// ===========================
const int16_t HR_16B_kick[] = {
     0,   500,   960,  1360,  1680,  1900,  2020,  2050,
  1990,  1850,  1640,  1380,  1080,   760,   440,   140,
  -140,  -380,  -570,  -710,  -800,  -840,  -830,  -780,
  -700,  -600,  -490,  -380,  -270,  -170,   -80,    10,
    70,   110,   130,   130,   120,   100,    80,    60,
    40,    20,    10,     0,     0,     0,     0,     0
};

// ===========================
// 2. HR-16 SNARE (Bright, snappy, with noise body)
// Classic "Snappy Snare"
// ===========================
const int16_t HR_16B_snare[] = {
   800, -700,  600, -900,  500, -400,  700, -500,
  -400,  800, -300,  200, -600,  400, -200,  500,
   100, -500,  300, -150,  400, -250,  100, -300,
    50,  200, -150,   80, -200,  120,  -60,  100,
   -50,  -30,   60,  -30,   15,  -40,   30,  -15,
    12,   -9,    6,   -4,    2,   -1,    1,    0
};

// ===========================
// 3. LOW TOM ("Deep Tom")
// Tuned ~80 Hz, smooth decay
// ===========================
const int16_t HR_16B_low_tom[] = {
    0,  180,  350,  500,  620,  700,  740,  730,
  680,  600,  490,  370,  250,  130,   30,  -60,
 -130, -180, -210, -220, -210, -190, -160, -120,
  -80,  -50,  -30,  -15,   -5,    2,    5,    4,
    2,    1,    0
};

// ===========================
// 4. MID TOM ("Mid Tom")
// ~120 Hz
// ===========================
const int16_t HR_16B_mid_tom[] = {
    0,  220,  420,  580,  690,  740,  730,  670,
  570,  440,  300,  160,   40,  -70, -150, -200,
 -220, -210, -180, -140, -100,  -60,  -30,  -10,
    2,    6,    5,    3,    1,    0
};

// ===========================
// 5. HIGH TOM ("High Tom")
// ~180 Hz
// ===========================
const int16_t HR_16B_high_tom[] = {
    0,  260,  480,  640,  730,  750,  710,  620,
  490,  340,  190,   60,  -60, -150, -210, -240,
 -240, -210, -170, -120,  -80,  -50,  -30,  -15,
   -5,    2,    4,    3,    1,    0
};

// ===========================
// 6. CLOSED HI-HAT ("Crash Hi-Hat" or "Short Hat")
// Sharp, bright, metallic
// ===========================
const int16_t HR_16B_closed_hh[] = {
   700, -500,  400, -300,  250, -200,  150, -120,
   100,  -80,   60,  -50,   40,  -30,   20,  -15,
    10,   -8,    6,   -5,    4,   -3,    2,   -1,
     1,    0
};

// ===========================
// 7. OPEN HI-HAT ("Open Hat Long")
// Longer decay, more shimmer
// ===========================
const int16_t HR_16B_open_hh[] = {
   700, -500,  400, -300,  250, -200,  150, -120,
   100,  -80,   60,  -50,   40,  -30,   20,  -15,
    10,   -8,    6,   -5,    4,   -3,    2,   -1,
     1,   -1,    1,   -1,    1,   -1,    1,   -1,
     0
};

// ===========================
// 8. CLAP ("Hand Clap")
// Multi-tap burst
// ===========================
const int16_t HR_16B_clap[] = {
   400, -300,  500, -200,  100, -400,  300, -100,
   200, -300,  150, -100,  200, -150,   80,  -60,
    40,  -30,   20,  -15,   10,   -8,    6,   -5,
     4,   -3,    2,   -1,    1,    0
};

// ===========================
// 9. RIMSHOT ("Rim Shot")
// Wooden, sharp transient
// ===========================
const int16_t HR_16B_rimshot[] = {
   800, -600,  400, -300,  200, -150,  100,  -80,
    60,  -50,   40,  -30,   20,  -15,   10,   -8,
     6,   -5,    4,   -3,    2,   -1,    1,    0
};

// ===========================
// 10. CRASH CYMBAL ("Crash")
// Ringing decay
// ===========================
const int16_t HR_16B_crash[] = {
   500, -400,  300, -200,  150, -100,   80,  -60,
    50,  -40,   30,  -20,   15,  -10,    8,   -6,
     5,   -4,    3,   -2,    2,   -1,    1,   -1,
     1,   -1,    1,   -1,    1,   -1,    1,   -1,
     1,    0
};

DrumVoice_TypeDef HR_16B_voices[HR_16B_NUM_VOICES] = {
    { HR_16B_kick,      HR_16B_kick_length,       0, 1.0f, 0 },
    { HR_16B_snare,     HR_16B_snare_length,      0, 0.9f, 0 },
    { HR_16B_low_tom,   HR_16B_low_tom_length,    0, 0.8f, 0 },
    { HR_16B_mid_tom,   HR_16B_mid_tom_length,    0, 0.8f, 0 },
    { HR_16B_high_tom,  HR_16B_high_tom_length,   0, 0.8f, 0 },
    { HR_16B_closed_hh, HR_16B_closed_hh_length,  0, 0.7f, 0 },
    { HR_16B_open_hh,   HR_16B_open_hh_length,    0, 0.7f, 0 },
    { HR_16B_clap,      HR_16B_clap_length,       0, 0.8f, 0 },
    { HR_16B_rimshot,   HR_16B_rimshot_length,    0, 0.7f, 0 },
    { HR_16B_crash,     HR_16B_crash_length,      0, 0.8f, 0 }
};

#endif // #ifdef SOUND_ENGINE_ENABLED

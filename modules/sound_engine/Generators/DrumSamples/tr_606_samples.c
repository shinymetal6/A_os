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
 * tr_606_samples.c
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED

#include "../drum_machine.h"
#include "tr_606_samples.h"

// ===========================
// 1. TR-606 KICK DRUM
// Deep sine sweep + attack click + decay
// ===========================
const int16_t TR_606_kick[] = {
     0,   480,   940,  1360,  1730,  2030,  2250,  2380,
  2420,  2370,  2240,  2040,  1780,  1480,  1150,   810,
   460,   120,  -190,  -470,  -710,  -910, -1060, -1160,
 -1210, -1220, -1180, -1110, -1010,  -890,  -750,  -600,
  -450,  -300,  -160,   -30,    80,   170,   240,   290,
   320,   330,   320,   300,   270,   230,   190,   150,
   110,    70,    30,    -5,   -35,   -60,   -80,   -95,
  -105,  -110,  -110,  -105,   -95,   -85,   -70,   -55,
   -40,   -25,   -10,     3,    15,    25,    33,    38,
    40,    40,    38,    35,    30,    25,    20,    15,
    10,     6,     3,     1,     0,     0,     0,     0
};

// ===========================
// 2. TR-606 SNARE
// White noise + resonant body tone (~200 Hz)
// Simulated with random-like pattern + sine component
// ===========================
const int16_t TR_606_snare[] = {
  1200, -1000,  800, -1400,  600, -400,  900, -700,
  -500,  1100, -300,  200, -800,  400, -200,  600,
   100, -900,  300, -150,  500, -350,  100, -400,
    50,  300, -200,  100, -250,  150,  -50,  100,
   -20,  -80,   60,  -30,   50,  -40,   20,  -30,
    15,   10,  -20,   10,  -10,   15,   -5,    8,
    -6,    4,   -3,    2,   -1,    1,    0,    0
};

// ===========================
// 3. LOW TOM
// ~80 Hz damped oscillator
// ===========================
const int16_t TR_606_low_tom[] = {
    0,  200,  390,  560,  700,  810,  880,  910,
  900,  850,  770,  660,  530,  400,  270,  150,
   50,  -30, -100, -150, -180, -190, -180, -160,
 -130, -100,  -70,  -40,  -20,   -5,    5,    8,
    6,    3,    0,    0
};

// ===========================
// 4. MID TOM
// ~120 Hz
// ===========================
const int16_t TR_606_mid_tom[] = {
    0,  250,  470,  650,  770,  830,  830,  780,
  690,  570,  430,  280,  140,   20,  -80, -160,
 -210, -240, -240, -220, -190, -150, -110,  -70,
  -40,  -20,   -5,    5,    8,    6,    3,    1,
    0,    0
};

// ===========================
// 5. HIGH TOM
// ~180 Hz
// ===========================
const int16_t TR_606_high_tom[] = {
    0,  300,  550,  720,  810,  820,  760,  650,
  500,  340,  180,   40, -100, -210, -280, -310,
 -300, -260, -200, -140, -90, -50, -20,  -5,
    5,    8,    6,    3,    1,    0
};

// ===========================
// 6. CLOSED HI-HAT
// Short metallic burst (~8 kHz content)
// ===========================
const int16_t TR_606_hh[] = {
   800, -600,  500, -400,  300, -250,  200, -150,
   100,  -80,   60,  -50,   40,  -30,   20,  -15,
    10,   -8,    6,   -5,    4,   -3,    2,   -1,
     1,    0
};

// ===========================
// 7. RIMSHOT
// Sharp wooden transient
// ===========================
const int16_t TR_606_rimshot[] = {
   900, -700,  400, -300,  200, -150,  100,  -80,
    60,  -50,   40,  -30,   20,  -15,   10,   -8,
     6,   -5,    4,   -3,    2,   -1,    1,    0
};

DrumVoice_TypeDef TR_606_voices[TR_606_NUM_VOICES] =
{
    { TR_606_kick,    TR_606_kick_length,     0, 1.0f, 0 },
    { TR_606_snare,   TR_606_snare_length,    0, 0.9f, 0 },
    { TR_606_low_tom, TR_606_low_tom_length,  0, 0.8f, 0 },
    { TR_606_mid_tom, TR_606_mid_tom_length,  0, 0.8f, 0 },
    { TR_606_high_tom,TR_606_high_tom_length, 0, 0.8f, 0 },
    { TR_606_hh,      TR_606_hh_length,       0, 0.7f, 0 },
    { TR_606_rimshot, TR_606_rimshot_length,  0, 0.7f, 0 }
};

#endif // #ifdef SOUND_ENGINE_ENABLED

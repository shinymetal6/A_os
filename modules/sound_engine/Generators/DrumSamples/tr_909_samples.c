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
 * tr_909_samples.c
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED

// ===========================
// 1. TR-909 KICK (Analog Sine Sweep)
// ===========================
const int16_t TR_909_kick[] = {
     0,   450,   880,  1270,  1600,  1860,  2040,  2130,
  2140,  2070,  1930,  1730,  1480,  1190,   880,   560,
   250,   -50,  -320,  -550,  -740,  -880,  -960,  -990,
  -970,  -910,  -820,  -710,  -590,  -460,  -330,  -210,
   -90,    20,    90,   130,   150,   150,   140,   120,
   100,    70,    50,    30,    15,     5,    -2,    -6,
    -8,    -8,    -7,    -5,    -3,    -1,     0,     0
};

// ===========================
// 2. TR-909 SNARE (Noise + Resonant Body ~200 Hz)
// ===========================
const int16_t TR_909_snare[] = {
  1100, -900,  700, -1000,  600, -400,  800, -600,
  -500,  900, -300,  200, -700,  400, -200,  500,
   100, -600,  300, -150,  400, -250,  100, -300,
    50,  200, -150,   80, -200,  120,  -60,  100,
   -50,  -30,   60,  -30,   15,  -40,   30,  -15,
    12,   -9,    6,   -4,    2,   -1,    1,    0
};

// ===========================
// 3. LOW TOM (~85 Hz)
// ===========================
const int16_t TR_909_low_tom[] = {
    0,  200,  390,  550,  670,  740,  760,  730,
  660,  560,  440,  310,  180,   60,  -50, -140,
 -200, -230, -230, -210, -180, -140, -100,  -60,
  -30,  -10,    5,    8,    6,    3,    1,    0
};

// ===========================
// 4. MID TOM (~125 Hz)
// ===========================
const int16_t TR_909_mid_tom[] = {
    0,  240,  450,  620,  730,  770,  740,  660,
  540,  400,  250,  110,  -20, -130, -210, -250,
 -260, -240, -200, -150, -100,  -60,  -30,  -10,
    2,    6,    5,    3,    1,    0
};

// ===========================
// 5. HIGH TOM (~180 Hz)
// ===========================
const int16_t TR_909_high_tom[] = {
    0,  280,  520,  690,  780,  800,  750,  650,
  510,  350,  190,   50, -100, -200, -260, -280,
 -270, -230, -180, -130,  -80,  -40,  -15,   -5,
    2,    4,    3,    1,    0
};

// ===========================
// 6. CLOSED HI-HAT (Digital, 12-bit style)
// ===========================
const int16_t TR_909_closed_hh[] = {
   600, -400,  300, -250,  200, -150,  120, -100,
    80,  -60,   50,  -40,   30,  -20,   15,  -10,
     7,   -5,    4,   -3,    2,   -1,    1,    0
};

// ===========================
// 7. OPEN HI-HAT (Longer decay, more shimmer)
// ===========================
const int16_t TR_909_open_hh[] = {
   600, -400,  300, -250,  200, -150,  120, -100,
    80,  -60,   50,  -40,   30,  -20,   15,  -10,
     7,   -5,    4,   -3,    2,   -1,    1,   -1,
     1,   -1,    1,   -1,    1,   -1,    1,   -1,
     0
};

// ===========================
// 8. RIMSHOT (Wooden click)
// ===========================
const int16_t TR_909_rimshot[] = {
   700, -500,  400, -300,  200, -150,  100,  -80,
    60,  -50,   40,  -30,   20,  -15,   10,   -8,
     6,   -5,    4,   -3,    2,   -1,    1,    0
};

// ===========================
// 9. CRASH CYMBAL (Metallic ring)
// ===========================
const int16_t TR_909_crash[] = {
   500, -400,  300, -200,  150, -100,   80,  -60,
    50,  -40,   30,  -20,   15,  -10,    8,   -6,
     5,   -4,    3,   -2,    2,   -1,    1,   -1,
     1,   -1,    1,   -1,    1,   -1,    1,   -1,
     1,    0
};

DrumVoice_TypeDef TR_909_voices[TR_909_NUM_VOICES] = {
    { TR_909_kick,      TR_909_kick_length,       0, 1.0f, 0 },
    { TR_909_snare,     TR_909_snare_length,      0, 0.9f, 0 },
    { TR_909_low_tom,   TR_909_low_tom_length,    0, 0.8f, 0 },
    { TR_909_mid_tom,   TR_909_mid_tom_length,    0, 0.8f, 0 },
    { TR_909_high_tom,  TR_909_high_tom_length,   0, 0.8f, 0 },
    { TR_909_closed_hh, TR_909_closed_hh_length,  0, 0.7f, 0 },
    { TR_909_open_hh,   TR_909_open_hh_length,    0, 0.7f, 0 },
    { TR_909_rimshot,   TR_909_rimshot_length,    0, 0.7f, 0 },
    { TR_909_crash,     TR_909_crash_length,      0, 0.8f, 0 }
};

#endif // #ifdef SOUND_ENGINE_ENABLED

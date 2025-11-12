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
 * tr_909_samples.h
 *
 *  Created on: Nov 12, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_909_SAMPLES_H_
#define MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_909_SAMPLES_H_

// ===========================
// 1. TR-909 KICK (Analog Sine Sweep)
// ===========================
const int16_t TR909_kick[] = {
     0,   450,   880,  1270,  1600,  1860,  2040,  2130,
  2140,  2070,  1930,  1730,  1480,  1190,   880,   560,
   250,   -50,  -320,  -550,  -740,  -880,  -960,  -990,
  -970,  -910,  -820,  -710,  -590,  -460,  -330,  -210,
   -90,    20,    90,   130,   150,   150,   140,   120,
   100,    70,    50,    30,    15,     5,    -2,    -6,
    -8,    -8,    -7,    -5,    -3,    -1,     0,     0
};
const uint32_t TR909_kick_len = 54;

// ===========================
// 2. TR-909 SNARE (Noise + Resonant Body ~200 Hz)
// ===========================
const int16_t TR909_snare[] = {
  1100, -900,  700, -1000,  600, -400,  800, -600,
  -500,  900, -300,  200, -700,  400, -200,  500,
   100, -600,  300, -150,  400, -250,  100, -300,
    50,  200, -150,   80, -200,  120,  -60,  100,
   -50,  -30,   60,  -30,   15,  -40,   30,  -15,
    12,   -9,    6,   -4,    2,   -1,    1,    0
};
const uint32_t TR909_snare_len = 50;

// ===========================
// 3. LOW TOM (~85 Hz)
// ===========================
const int16_t TR909_low_tom[] = {
    0,  200,  390,  550,  670,  740,  760,  730,
  660,  560,  440,  310,  180,   60,  -50, -140,
 -200, -230, -230, -210, -180, -140, -100,  -60,
  -30,  -10,    5,    8,    6,    3,    1,    0
};
const uint32_t TR909_low_tom_len = 30;

// ===========================
// 4. MID TOM (~125 Hz)
// ===========================
const int16_t TR909_mid_tom[] = {
    0,  240,  450,  620,  730,  770,  740,  660,
  540,  400,  250,  110,  -20, -130, -210, -250,
 -260, -240, -200, -150, -100,  -60,  -30,  -10,
    2,    6,    5,    3,    1,    0
};
const uint32_t TR909_mid_tom_len = 29;

// ===========================
// 5. HIGH TOM (~180 Hz)
// ===========================
const int16_t TR909_high_tom[] = {
    0,  280,  520,  690,  780,  800,  750,  650,
  510,  350,  190,   50, -100, -200, -260, -280,
 -270, -230, -180, -130,  -80,  -40,  -15,   -5,
    2,    4,    3,    1,    0
};
const uint32_t TR909_high_tom_len = 28;

// ===========================
// 6. CLOSED HI-HAT (Digital, 12-bit style)
// ===========================
const int16_t TR909_closed_hh[] = {
   600, -400,  300, -250,  200, -150,  120, -100,
    80,  -60,   50,  -40,   30,  -20,   15,  -10,
     7,   -5,    4,   -3,    2,   -1,    1,    0
};
const uint32_t TR909_closed_hh_len = 24;

// ===========================
// 7. OPEN HI-HAT (Longer decay, more shimmer)
// ===========================
const int16_t TR909_open_hh[] = {
   600, -400,  300, -250,  200, -150,  120, -100,
    80,  -60,   50,  -40,   30,  -20,   15,  -10,
     7,   -5,    4,   -3,    2,   -1,    1,   -1,
     1,   -1,    1,   -1,    1,   -1,    1,   -1,
     0
};
const uint32_t TR909_open_hh_len = 29;

// ===========================
// 8. RIMSHOT (Wooden click)
// ===========================
const int16_t TR909_rimshot[] = {
   700, -500,  400, -300,  200, -150,  100,  -80,
    60,  -50,   40,  -30,   20,  -15,   10,   -8,
     6,   -5,    4,   -3,    2,   -1,    1,    0
};
const uint32_t TR909_rimshot_len = 24;

// ===========================
// 9. CRASH CYMBAL (Metallic ring)
// ===========================
const int16_t TR909_crash[] = {
   500, -400,  300, -200,  150, -100,   80,  -60,
    50,  -40,   30,  -20,   15,  -10,    8,   -6,
     5,   -4,    3,   -2,    2,   -1,    1,   -1,
     1,   -1,    1,   -1,    1,   -1,    1,   -1,
     1,    0
};
const uint32_t TR909_crash_len = 32;

/*
DrumVoice voices[NUM_VOICES] = {
    { rs9_kick,      rs9_kick_len,       0, 1.0f, 0 },
    { rs9_snare,     rs9_snare_len,      0, 0.9f, 0 },
    { rs9_low_tom,   rs9_low_tom_len,    0, 0.8f, 0 },
    { rs9_mid_tom,   rs9_mid_tom_len,    0, 0.8f, 0 },
    { rs9_high_tom,  rs9_high_tom_len,   0, 0.8f, 0 },
    { rs9_closed_hh, rs9_closed_hh_len,  0, 0.7f, 0 },
    { rs9_open_hh,   rs9_open_hh_len,    0, 0.7f, 0 },
    { rs9_rimshot,   rs9_rimshot_len,    0, 0.7f, 0 },
    { rs9_crash,     rs9_crash_len,      0, 0.8f, 0 }
};
*/
#endif /* MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_909_SAMPLES_H_ */

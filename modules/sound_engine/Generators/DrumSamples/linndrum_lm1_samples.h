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
 * linndrum_lm1_samples.h
 *
 *  Created on: Nov 12, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_LINNDRUM_LM1_SAMPLES_H_
#define MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_LINNDRUM_LM1_SAMPLES_H_

// ===========================
// 1. LM-1 KICK DRUM ("Bass Drum")
// Deep, punchy, short decay — famously tight
// ===========================
const int16_t lm1_kick[] = {
     0,   400,   780,  1120,  1400,  1610,  1740,  1790,
  1750,  1640,  1460,  1230,   970,   690,   410,   150,
  -100,  -310,  -480,  -600,  -670,  -700,  -690,  -640,
  -560,  -460,  -350,  -240,  -140,   -50,    30,    90,
   130,   150,   150,   140,   120,   100,    80,    60,
    40,    20,    10,     0,     0,     0,     0
};
const uint32_t lm1_kick_len = 45;

// ===========================
// 2. LM-1 SNARE DRUM ("Snare Drum")
// Bright, snappy, with natural room ambience (short reverb tail)
// ===========================
const int16_t lm1_snare[] = {
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
const uint32_t lm1_snare_len = 68;  // Includes reverb

// ===========================
// 3. LOW TOM ("Low Tom")
// Tuned tom with natural decay (~90 Hz)
// ===========================
const int16_t lm1_low_tom[] = {
    0,  190,  370,  530,  650,  720,  750,  730,
  670,  580,  460,  330,  200,   80,  -30, -120,
 -190, -230, -250, -240, -210, -180, -140, -100,
  -70,  -40,  -20,   -5,    5,    8,    6,    3,
    1,    0
};
const uint32_t lm1_low_tom_len = 33;

// ===========================
// 4. MID TOM ("Mid Tom")
// ~130 Hz
// ===========================
const int16_t lm1_mid_tom[] = {
    0,  230,  440,  600,  710,  760,  750,  690,
  590,  460,  320,  180,   50,  -70, -160, -220,
 -250, -250, -220, -180, -140, -100,  -60,  -30,
  -10,    2,    6,    5,    3,    1,    0
};
const uint32_t lm1_mid_tom_len = 30;

// ===========================
// 5. HIGH TOM ("High Tom")
// ~190 Hz
// ===========================
const int16_t lm1_high_tom[] = {
    0,  270,  500,  670,  760,  780,  740,  650,
  520,  370,  220,   80,  -50, -140, -200, -230,
 -230, -200, -160, -120,  -80,  -50,  -30,  -15,
   -5,    2,    4,    3,    1,    0
};
const uint32_t lm1_high_tom_len = 29;

// ===========================
// 6. CLOSED HI-HAT ("Closed Hi-Hat")
// Short, crisp, metallic
// ===========================
const int16_t lm1_closed_hh[] = {
   650, -450,  350, -280,  220, -180,  140, -110,
    90,  -70,   60,  -50,   40,  -30,   20,  -15,
    10,   -8,    6,   -5,    4,   -3,    2,   -1,
     1,    0
};
const uint32_t lm1_closed_hh_len = 25;

// ===========================
// 7. OPEN HI-HAT ("Open Hi-Hat")
// Longer decay, shimmering ring
// ===========================
const int16_t lm1_open_hh[] = {
   650, -450,  350, -280,  220, -180,  140, -110,
    90,  -70,   60,  -50,   40,  -30,   20,  -15,
    10,   -8,    6,   -5,    4,   -3,    2,   -1,
     1,   -1,    1,   -1,    1,   -1,    1,   -1,
     0
};
const uint32_t lm1_open_hh_len = 29;

// ===========================
// 8. RIMSHOT ("Rim Shot")
// Wooden stick attack
// ===========================
const int16_t lm1_rimshot[] = {
   750, -550,  400, -300,  200, -150,  100,  -80,
    60,  -50,   40,  -30,   20,  -15,   10,   -8,
     6,   -5,    4,   -3,    2,   -1,    1,    0
};
const uint32_t lm1_rimshot_len = 24;

// ===========================
// 9. CRASH CYMBAL ("Crash")
// Ringing decay with high-frequency content
// ===========================
const int16_t lm1_crash[] = {
   550, -400,  300, -200,  150, -100,   80,  -60,
    50,  -40,   30,  -20,   15,  -10,    8,   -6,
     5,   -4,    3,   -2,    2,   -1,    1,   -1,
     1,   -1,    1,   -1,    1,   -1,    1,   -1,
     1,    0
};
const uint32_t lm1_crash_len = 32;

// ===========================
// 10. CLAP ("Hand Clap")
// Multi-hit burst
// ===========================
const int16_t lm1_clap[] = {
   450, -300,  500, -200,  100, -400,  300, -100,
   200, -300,  150, -100,  200, -150,   80,  -60,
    40,  -30,   20,  -15,   10,   -8,    6,   -5,
     4,   -3,    2,   -1,    1,    0
};
const uint32_t lm1_clap_len = 29;

/*
 * DrumVoice voices[NUM_VOICES] = {
    { lm1_kick,      lm1_kick_len,       0, 1.0f, 0 },
    { lm1_snare,     lm1_snare_len,      0, 0.9f, 0 },
    { lm1_low_tom,   lm1_low_tom_len,    0, 0.8f, 0 },
    { lm1_mid_tom,   lm1_mid_tom_len,    0, 0.8f, 0 },
    { lm1_high_tom,  lm1_high_tom_len,   0, 0.8f, 0 },
    { lm1_closed_hh, lm1_closed_hh_len,  0, 0.7f, 0 },
    { lm1_open_hh,   lm1_open_hh_len,    0, 0.7f, 0 },
    { lm1_rimshot,   lm1_rimshot_len,    0, 0.7f, 0 },
    { lm1_crash,     lm1_crash_len,      0, 0.8f, 0 },
    { lm1_clap,      lm1_clap_len,       0, 0.8f, 0 }
};
*/

#endif /* MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_LINNDRUM_LM1_SAMPLES_H_ */

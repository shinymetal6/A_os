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
 * tr_707_samples.h
 *
 *  Created on: Nov 12, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_707_SAMPLES_H_
#define MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_707_SAMPLES_H_

// ===========================
// 1. TR-707 KICK DRUM
// Short, punchy, with click transient and low sine body
// ===========================
const int16_t tr707_kick[] = {
     0,   520,  1000,  1420,  1760,  2000,  2140,  2180,
  2120,  1960,  1720,  1420,  1080,   740,   420,   140,
  -100,  -300,  -460,  -570,  -640,  -670,  -660,  -620,
  -550,  -460,  -360,  -260,  -160,   -70,    10,    70,
   110,   130,   140,   130,   110,    90,    60,    40,
    20,     5,    -5,   -12,   -16,   -18,   -17,   -15,
   -12,    -9,    -6,    -3,    -1,     0,     0,     0
};
const uint32_t tr707_kick_len = 54;

// ===========================
// 2. TR-707 SNARE
// Bright noise burst with metallic ring (~1.2 kHz tone)
// ===========================
const int16_t tr707_snare[] = {
  1000, -800,  900, -1100,  700, -500,  800, -600,
  -400,  700, -300,  200, -600,  400, -200,  500,
   100, -400,  200, -100,  300, -200,   80, -200,
    50,  150, -100,   60, -120,   80,  -40,   60,
   -30,  -20,   40,  -20,   10,  -25,   20,  -10,
     8,   -6,    4,   -2,    1,    0
};
const uint32_t tr707_snare_len = 52;

// ===========================
// 3. LOW TOM
// Tuned at ~90 Hz with decay
// ===========================
const int16_t tr707_low_tom[] = {
    0,  180,  350,  500,  620,  700,  740,  740,
  700,  630,  530,  420,  300,  180,   70,  -30,
 -110, -170, -200, -210, -190, -160, -120,  -80,
  -50,  -30,  -15,   -5,    2,    5,    4,    2,
    1,    0
};
const uint32_t tr707_low_tom_len = 32;

// ===========================
// 4. MID TOM
// ~130 Hz
// ===========================
const int16_t tr707_mid_tom[] = {
    0,  220,  420,  580,  690,  740,  730,  670,
  570,  440,  300,  160,   40,  -70, -150, -200,
 -220, -210, -180, -140, -100,  -60,  -30,  -10,
    2,    6,    5,    3,    1,    0
};
const uint32_t tr707_mid_tom_len = 29;

// ===========================
// 5. HIGH TOM
// ~190 Hz
// ===========================
const int16_t tr707_high_tom[] = {
    0,  260,  480,  640,  730,  750,  710,  620,
  490,  340,  190,   60,  -60, -150, -210, -240,
 -240, -210, -170, -120,  -80,  -50,  -30,  -15,
   -5,    2,    4,    3,    1,    0
};
const uint32_t tr707_high_tom_len = 29;

// ===========================
// 6. CLOSED HI-HAT
// Sharp, short, bright (originally 12 kHz sample)
// ===========================
const int16_t tr707_closed_hh[] = {
   700, -500,  400, -300,  250, -200,  150, -120,
   100,  -80,   60,  -50,   40,  -30,   20,  -15,
    10,   -8,    6,   -5,    4,   -3,    2,   -1,
     1,    0
};
const uint32_t tr707_closed_hh_len = 25;

// ===========================
// 7. OPEN HI-HAT
// Longer decay, more shimmer
// ===========================
const int16_t tr707_open_hh[] = {
   700, -500,  400, -300,  250, -200,  150, -120,
   100,  -80,   60,  -50,   40,  -30,   20,  -15,
    10,   -8,    6,   -5,    4,   -3,    2,   -1,
     1,   -1,    1,   -1,    1,   -1,    1,   -1,
     0
};
const uint32_t tr707_open_hh_len = 29;

// ===========================
// 8. RIMSHOT
// Wooden, sharp transient
// ===========================
const int16_t tr707_rimshot[] = {
   800, -600,  400, -300,  200, -150,  100,  -80,
    60,  -50,   40,  -30,   20,  -15,   10,   -8,
     6,   -5,    4,   -3,    2,   -1,    1,    0
};
const uint32_t tr707_rimshot_len = 24;

// ===========================
// 9. CLAP
// Multi-tap burst (like two hands clapping)
// ===========================
const int16_t tr707_clap[] = {
   400, -300,  500, -200,  100, -400,  300, -100,
   200, -300,  150, -100,  200, -150,   80,  -60,
    40,  -30,   20,  -15,   10,   -8,    6,   -5,
     4,   -3,    2,   -1,    1,    0
};
const uint32_t tr707_clap_len = 29;

// ===========================
// 10. COWBELL
// Metallic ring at ~800 Hz
// ===========================
const int16_t tr707_cowbell[] = {
     0,  300,  580,  800,  940,  990,  960,  860,
   700,  500,  280,   60, -150, -330, -460, -530,
  -540, -500, -420, -320, -210, -100,   10,   90,
   150,  180,  190,  180,  160,  130,  100,   70,
    40,   20,    5,   -5,   -8,   -6,   -3,   -1,
     0
};
const uint32_t tr707_cowbell_len = 39;

/*
 * DrumVoice voices[NUM_VOICES] = {
    { tr707_kick,      tr707_kick_len,       0, 1.0f, 0 },
    { tr707_snare,     tr707_snare_len,      0, 0.9f, 0 },
    { tr707_low_tom,   tr707_low_tom_len,    0, 0.8f, 0 },
    { tr707_mid_tom,   tr707_mid_tom_len,    0, 0.8f, 0 },
    { tr707_high_tom,  tr707_high_tom_len,   0, 0.8f, 0 },
    { tr707_closed_hh, tr707_closed_hh_len,  0, 0.7f, 0 },
    { tr707_open_hh,   tr707_open_hh_len,    0, 0.7f, 0 },
    { tr707_rimshot,   tr707_rimshot_len,    0, 0.7f, 0 },
    { tr707_clap,      tr707_clap_len,       0, 0.8f, 0 },
    { tr707_cowbell,   tr707_cowbell_len,    0, 0.8f, 0 }
};
*/
#endif /* MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_707_SAMPLES_H_ */

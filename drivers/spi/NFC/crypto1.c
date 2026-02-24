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
 * crypto1.c
 *
 *  Created on: Feb 13, 2026
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED
#ifdef NFC_ENABLED

#include "crypto1.h"

// Swap even/odd bits
static uint32_t swap_bits(uint32_t x) {
    x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
    x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
    x = ((x & 0x0F0F0F0F) << 4) | ((x >> 4) & 0x0F0F0F0F);
    x = ((x & 0x00FF00FF) << 8) | ((x >> 8) & 0x00FF00FF);
    return (x << 16) | (x >> 16);
}

// Feed bit into LFSR
static void lfsr_rollback_bit(crypto1_state *s, int32_t fb) {
    uint32_t odd = s->odd, even = s->even;
    s->odd = (odd << 1) | (even & 1);
    s->even = (even << 1) | fb;
}

// Clock LFSR forward
static int clock_cipher(crypto1_state *s, int32_t in) {
    int out = s->odd & 1;
    uint32_t feedback = (s->odd ^ (s->odd >> 1) ^ (s->odd >> 2) ^
                        (s->odd >> 3) ^ (s->odd >> 5) ^ (s->odd >> 7) ^
                        (s->odd >> 9) ^ (s->odd >> 11) ^ (s->odd >> 13) ^
                        (s->odd >> 15) ^ (s->odd >> 17) ^ (s->odd >> 19) ^
                        (s->odd >> 21) ^ (s->odd >> 23) ^ (s->odd >> 25) ^
                        (s->odd >> 27) ^ (s->odd >> 29) ^ (s->odd >> 31)) & 1;
    s->odd = (s->odd >> 1) | ((uint32_t)in << 31);
    s->even = (s->even >> 1) | ((uint32_t)feedback << 31);
    return out;
}

void crypto1_init(crypto1_state *s, uint64_t key) {
    s->odd = s->even = 0;
    for (uint32_t i = 47; i >= 0; i--) {
    	int32_t bit = (key >> i) & 1;
        clock_cipher(s, bit);
    }
}

void crypto1_prng_successor(crypto1_state *s, int32_t nbits) {
    for (uint32_t i = 0; i < nbits; i++) {
        clock_cipher(s, 0);
    }
}

uint8_t crypto1_byte(crypto1_state *s, uint8_t plain, uint8_t is_encrypted) {
    uint8_t encrypted = 0;
    for (uint32_t i = 0; i < 8; i++) {
    	int32_t keystream = clock_cipher(s, 0);
    	int32_t bit = (plain >> i) & 1;
        encrypted |= ((is_encrypted ? (bit ^ keystream) : bit) << i);
    }
    return encrypted;
}

uint64_t bytes_to_key(const uint8_t key[6]) {
    return ((uint64_t)key[0] << 40) |
           ((uint64_t)key[1] << 32) |
           ((uint64_t)key[2] << 24) |
           ((uint64_t)key[3] << 16) |
           ((uint64_t)key[4] << 8)  |
           ((uint64_t)key[5]);
}

#endif // #ifdef NFC_ENABLED
#endif // #ifdef A_OS_SPI_ENABLED


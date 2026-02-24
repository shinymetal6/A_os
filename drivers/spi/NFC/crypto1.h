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
 * crypto1.h
 *
 *  Created on: Feb 13, 2026
 *      Author: fil
 */

#ifndef DRIVERS_SPI_NFC_CRYPTO1_H_
#define DRIVERS_SPI_NFC_CRYPTO1_H_

#include <stdint.h>

typedef struct {
    uint64_t odd, even;
} crypto1_state;

// Initialize LFSR with 48-bit key
void crypto1_init(crypto1_state *s, uint64_t key);

// Generate next N bits (for nonce generation)
void crypto1_prng_successor(crypto1_state *s, int32_t nbits);

// Encrypt/decrypt a byte
uint8_t crypto1_byte(crypto1_state *s, uint8_t plain, uint8_t is_encrypted);

// Convert 6-byte array to uint64_t key
uint64_t bytes_to_key(const uint8_t key[6]);


#endif /* DRIVERS_SPI_NFC_CRYPTO1_H_ */

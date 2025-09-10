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
 * md5.h
 *
 *  Created on: Sep 3, 2025
 *      Author: fil
 */

#ifndef KERNEL_MD5_H_
#define KERNEL_MD5_H_

// md5.h
#ifndef MD5_H
#define MD5_H

#include <stdint.h>
#include <stddef.h>

/* Data structure for MD5 (Message-Digest) computation */
typedef struct {
	uint32_t i[2];               /* number of _bits_ handled mod 2^64 */
	uint32_t buf[4];             /* scratch buffer */
	unsigned char in[64];     /* input buffer */
	unsigned char digest[16]; /* actual digest after MD5Final call */
} MD5_CTX;

#define	HASH_STD_LEN	16
#define	HASH_ASCII_LEN	(HASH_STD_LEN*2)

extern	uint8_t md5_hash(uint8_t *buf,uint32_t len,uint8_t *hash);

#endif

#endif /* KERNEL_MD5_H_ */

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
 * aos_lora_systimer.h
 *
 *  Created on: Aug 6, 2025
 *      Author: fil
 */

#ifndef LIBRARIES_LORAWAN_AOS_TARGET_AOS_LORA_SYSTIMER_H_
#define LIBRARIES_LORAWAN_AOS_TARGET_AOS_LORA_SYSTIMER_H_

#define RTC_N_PREDIV_S 1
#define RTC_PREDIV_S ((1<<RTC_N_PREDIV_S)-1)

#define RTC_BKP_SECONDS    RTC_BKP_DR0
#define RTC_BKP_SUBSECONDS RTC_BKP_DR1
#define RTC_BKP_MSBTICKS   RTC_BKP_DR2

extern RTC_HandleTypeDef hrtc;

#endif /* LIBRARIES_LORAWAN_AOS_TARGET_AOS_LORA_SYSTIMER_H_ */

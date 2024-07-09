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
 * internal_rtc.h
 *
 *  Created on: Jul 5, 2024
 *      Author: fil
 */


#ifndef DRIVERS_INTERNAL_RTC_INTERNAL_RTC_H_
#define DRIVERS_INTERNAL_RTC_INTERNAL_RTC_H_

extern	void A_RTC_init(void);
extern	void A_RTC_set_time(uint8_t Hours,uint8_t Minutes,uint8_t Seconds);
extern	void A_RTC_set_date(uint8_t WeekDay,uint8_t Month,uint8_t Date,uint8_t Year);
extern	void A_RTC_set_alarm(uint8_t WeekDay, uint8_t Hours,uint8_t Minutes,uint8_t Seconds);
extern	uint8_t A_RTC_get_time(void);
extern	uint8_t A_RTC_get_date(void);


#endif /* DRIVERS_INTERNAL_RTC_INTERNAL_RTC_H_ */

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
 * internal_rtc.c
 *
 *  Created on: Jul 5, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/system_default.h"

#ifdef INTERNAL_RTC_ENABLED
#include "../../kernel/kernel_opt.h"
#include "../../kernel/scheduler.h"
#include "../../kernel/hwmanager.h"
#include "../../kernel/A_exported_functions.h"
#include "internal_rtc.h"

extern	HWMngr_t	HWMngr[PERIPHERAL_NUM];
extern	Asys_t		Asys;

SYSTEM_RAM	RTC_TimeTypeDef sTime = {0};
SYSTEM_RAM	RTC_DateTypeDef sDate = {0};
SYSTEM_RAM	RTC_AlarmTypeDef sAlarm = {0};


void A_RTC_set_time(uint8_t Hours,uint8_t Minutes,uint8_t Seconds)
{
	  sTime.Hours = Hours;
	  sTime.Minutes = Minutes;
	  sTime.Seconds = Seconds;
	  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
}

void A_RTC_set_date(uint8_t WeekDay,uint8_t Month,uint8_t Date,uint8_t Year)
{
	  sDate.WeekDay = WeekDay;
	  sDate.Month = Month;
	  sDate.Date = Date;
	  sDate.Year = Year;
	  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}

void A_RTC_init(void)
{
	  sTime.Hours = 0x0;
	  sTime.Minutes = 0x0;
	  sTime.Seconds = 0x0;
	  sTime.SubSeconds = 0x0;
	  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

	  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	  sDate.Month = RTC_MONTH_JANUARY;
	  sDate.Date = 0x1;
	  sDate.Year = 0x0;
	  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F2);
}


void A_RTC_set_alarm(uint8_t WeekDay, uint8_t Hours,uint8_t Minutes,uint8_t Seconds)
{
	  sAlarm.AlarmTime.Hours = Hours;
	  sAlarm.AlarmTime.Minutes = Minutes;
	  sAlarm.AlarmTime.Seconds = sTime.Seconds + Seconds;
	  sAlarm.AlarmTime.SubSeconds = 0;
	  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
	  sAlarm.AlarmDateWeekDay = WeekDay;
	  sAlarm.Alarm = RTC_ALARM_A;
	  HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);
	  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F2);
}

uint8_t A_RTC_get_time(void)
{
	  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	  return sTime.Seconds;
}

uint8_t A_RTC_get_date(void)
{
	  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	  return sDate.Date;
}


uint8_t rtcexpired = 0;
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	rtcexpired = 1;
}

#endif


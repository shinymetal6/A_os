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
 * aos_lora_systimer.c
 *
 *  Created on: Aug 6, 2025
 *      Author: fil
 */
#include "main.h"
#include "aos_lora_timer.h"
#include "aos_lora_systimer.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/kernel_opt.h"

static uint32_t TIMER_IF_BkUp_Read_MSBticks(void);
static inline uint32_t GetTimerTicks(void);
uint32_t TIMER_IF_Convert_Tick2ms(uint32_t tick);

uint32_t TIMER_IF_GetTime(uint16_t *mSeconds)
{
  uint32_t seconds = 0;
  /* USER CODE BEGIN TIMER_IF_GetTime */

  /* USER CODE END TIMER_IF_GetTime */
  uint64_t ticks;
  uint32_t timerValueLsb = GetTimerTicks();
  uint32_t timerValueMSB = TIMER_IF_BkUp_Read_MSBticks();

  ticks = (((uint64_t) timerValueMSB) << 32) + timerValueLsb;

  seconds = (uint32_t)(ticks >> RTC_N_PREDIV_S);

  ticks = (uint32_t) ticks & RTC_PREDIV_S;

  *mSeconds = TIMER_IF_Convert_Tick2ms(ticks);

  /* USER CODE BEGIN TIMER_IF_GetTime_Last */

  /* USER CODE END TIMER_IF_GetTime_Last */
  return seconds;
}

void TIMER_IF_BkUp_Write_Seconds(uint32_t Seconds)
{
  /* USER CODE BEGIN TIMER_IF_BkUp_Write_Seconds */

  /* USER CODE END TIMER_IF_BkUp_Write_Seconds */
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_SECONDS, Seconds);
  /* USER CODE BEGIN TIMER_IF_BkUp_Write_Seconds_Last */

  /* USER CODE END TIMER_IF_BkUp_Write_Seconds_Last */
}

void TIMER_IF_BkUp_Write_SubSeconds(uint32_t SubSeconds)
{
  /* USER CODE BEGIN TIMER_IF_BkUp_Write_SubSeconds */

  /* USER CODE END TIMER_IF_BkUp_Write_SubSeconds */
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_SUBSECONDS, SubSeconds);
  /* USER CODE BEGIN TIMER_IF_BkUp_Write_SubSeconds_Last */

  /* USER CODE END TIMER_IF_BkUp_Write_SubSeconds_Last */
}

uint32_t TIMER_IF_BkUp_Read_Seconds(void)
{
  uint32_t ret = 0;
  /* USER CODE BEGIN TIMER_IF_BkUp_Read_Seconds */

  /* USER CODE END TIMER_IF_BkUp_Read_Seconds */
  ret = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_SECONDS);
  /* USER CODE BEGIN TIMER_IF_BkUp_Read_Seconds_Last */

  /* USER CODE END TIMER_IF_BkUp_Read_Seconds_Last */
  return ret;
}

uint32_t TIMER_IF_BkUp_Read_SubSeconds(void)
{
  uint32_t ret = 0;
  /* USER CODE BEGIN TIMER_IF_BkUp_Read_SubSeconds */

  /* USER CODE END TIMER_IF_BkUp_Read_SubSeconds */
  ret = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_SUBSECONDS);
  /* USER CODE BEGIN TIMER_IF_BkUp_Read_SubSeconds_Last */

  /* USER CODE END TIMER_IF_BkUp_Read_SubSeconds_Last */
  return ret;
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void TIMER_IF_BkUp_Write_MSBticks(uint32_t MSBticks)
{
  /* USER CODE BEGIN TIMER_IF_BkUp_Write_MSBticks */

  /* USER CODE END TIMER_IF_BkUp_Write_MSBticks */
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_MSBTICKS, MSBticks);
  /* USER CODE BEGIN TIMER_IF_BkUp_Write_MSBticks_Last */

  /* USER CODE END TIMER_IF_BkUp_Write_MSBticks_Last */
}

static uint32_t TIMER_IF_BkUp_Read_MSBticks(void)
{
  /* USER CODE BEGIN TIMER_IF_BkUp_Read_MSBticks */

  /* USER CODE END TIMER_IF_BkUp_Read_MSBticks */
  uint32_t MSBticks;
  MSBticks = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_MSBTICKS);
  return MSBticks;
  /* USER CODE BEGIN TIMER_IF_BkUp_Read_MSBticks_Last */

  /* USER CODE END TIMER_IF_BkUp_Read_MSBticks_Last */
}


static inline uint32_t GetTimerTicks(void)
{
  /* USER CODE BEGIN GetTimerTicks */

  /* USER CODE END GetTimerTicks */
  uint32_t ssr = LL_RTC_TIME_GetSubSecond(RTC);
  /* read twice to make sure value it valid*/
  while (ssr != LL_RTC_TIME_GetSubSecond(RTC))
  {
    ssr = LL_RTC_TIME_GetSubSecond(RTC);
  }
  return UINT32_MAX - ssr;
  /* USER CODE BEGIN GetTimerTicks_Last */

  /* USER CODE END GetTimerTicks_Last */
}

SysTime_t SysTimeAdd( SysTime_t a, SysTime_t b )
{
  SysTime_t c =  { .Seconds = 0, .SubSeconds = 0 };

  c.Seconds = a.Seconds + b.Seconds;
  c.SubSeconds = a.SubSeconds + b.SubSeconds;
  if( c.SubSeconds >= 1000 )
  {
    c.Seconds++;
    c.SubSeconds -= 1000;
  }
  return c;
}

SysTime_t SysTimeSub( SysTime_t a, SysTime_t b )
{
  SysTime_t c = { .Seconds = 0, .SubSeconds = 0 };

  c.Seconds = a.Seconds - b.Seconds;
  c.SubSeconds = a.SubSeconds - b.SubSeconds;
  if( c.SubSeconds < 0 )
  {
    c.Seconds--;
    c.SubSeconds += 1000;
  }
  return c;
}
SysTime_t SysTimeGet( void )
{
	  SysTime_t calendarTime = { .Seconds = 0, .SubSeconds = 0 };
	  SysTime_t sysTime = { .Seconds = 0, .SubSeconds = 0 };
	  SysTime_t DeltaTime;

	  calendarTime.Seconds = UTIL_SYSTIMDriver.GetCalendarTime( ( uint16_t* )&calendarTime.SubSeconds );

	  DeltaTime.SubSeconds = (int16_t)UTIL_SYSTIMDriver.BKUPRead_SubSeconds();
	  DeltaTime.Seconds = UTIL_SYSTIMDriver.BKUPRead_Seconds();

	  sysTime = SysTimeAdd( DeltaTime, calendarTime );

	  return sysTime;
}
SysTime_t SysTimeGetMcuTime( void )
{
	  SysTime_t calendarTime = { .Seconds = 0, .SubSeconds = 0 };

	  calendarTime.Seconds = UTIL_SYSTIMDriver.GetCalendarTime( ( uint16_t* )&calendarTime.SubSeconds );

	  return calendarTime;
}


void SysTimeSet( SysTime_t sysTime )
{

}

uint32_t SysTimeToMs( SysTime_t sysTime )
{
  SysTime_t DeltaTime;
  DeltaTime.SubSeconds = (int16_t)UTIL_SYSTIMDriver.BKUPRead_SubSeconds();
  DeltaTime.Seconds = UTIL_SYSTIMDriver.BKUPRead_Seconds();

  SysTime_t calendarTime = SysTimeSub( sysTime, DeltaTime );
  return calendarTime.Seconds * 1000 + calendarTime.SubSeconds;
}

SysTime_t SysTimeFromMs( uint32_t timeMs )
{
  uint32_t seconds = timeMs / 1000;
  SysTime_t sysTime = { .Seconds = seconds, .SubSeconds =  timeMs - seconds * 1000 };
  SysTime_t DeltaTime = { 0 };

  DeltaTime.SubSeconds = (int16_t)UTIL_SYSTIMDriver.BKUPRead_SubSeconds();
  DeltaTime.Seconds = UTIL_SYSTIMDriver.BKUPRead_Seconds();
  return SysTimeAdd( sysTime, DeltaTime );
}


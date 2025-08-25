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
 * aos_lora_timer.c
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
static UTIL_TIMER_Object_t *TimerListHead = NULL;

extern	RTC_HandleTypeDef hrtc;
static uint32_t RtcTimerContext = 0;
static uint8_t 	RTC_Initialized = 0;


uint32_t get_subs(RTC_HandleTypeDef *rtc)
{
	return rtc->Instance->SSR;
}

void set_subs(RTC_HandleTypeDef *rtc,uint32_t subs)
{
	rtc->Instance->SSR = subs;
}

static inline uint32_t GetTimerTicks(void)
{
uint32_t ssr = get_subs(&hrtc);
	/* read twice to make sure value it valid*/
	while (ssr != get_subs(&hrtc))
		ssr = get_subs(&hrtc);
	return UINT32_MAX - ssr;
}

UTIL_TIMER_Status_t TIMER_IF_Init(void)
{
	RTC_Initialized = 1;
	return UTIL_TIMER_OK;
}

UTIL_TIMER_Status_t TIMER_IF_StopTimer(void)
{
UTIL_TIMER_Status_t ret = UTIL_TIMER_OK;
	/* Clear RTC Alarm Flag */
	__HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);
	/* Disable the Alarm A interrupt */
	HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
	return ret;
}

UTIL_TIMER_Status_t TIMER_IF_StartTimer(uint32_t timeout)
{
  UTIL_TIMER_Status_t ret = UTIL_TIMER_OK;
	  RTC_AlarmTypeDef sAlarm = {0};
	  /*Stop timer if one is already started*/
	  TIMER_IF_StopTimer();
	  timeout += RtcTimerContext;
	  /* starts timer*/
	  sAlarm.AlarmTime.SubSeconds = UINT32_MAX - timeout;
	  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	  sAlarm.AlarmSubSecondMask = 0;
	  sAlarm.Alarm = RTC_ALARM_A;
	  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  return ret;
}


uint32_t TIMER_IF_SetTimerContext(void)
{
	  RtcTimerContext = GetTimerTicks();
	  return RtcTimerContext;
}

uint32_t TIMER_IF_GetTimerContext(void)
{
	  return RtcTimerContext;
}

uint32_t TIMER_IF_GetTimerElapsedTime(void)
{
	return ((uint32_t)(GetTimerTicks() - RtcTimerContext));
}

uint32_t TIMER_IF_GetTimerValue(void)
{
	if (RTC_Initialized == 1)
		return GetTimerTicks();
	else
		return 0;
}

uint32_t TIMER_IF_GetMinimumTimeout(void)
{
	  return (MIN_ALARM_DELAY);
}


uint32_t TIMER_IF_Convert_ms2Tick(uint32_t timeMilliSec)
{
	return ((uint32_t)((((uint64_t) timeMilliSec) << RTC_N_PREDIV_S) / 1000));
}

uint32_t TIMER_IF_Convert_Tick2ms(uint32_t tick)
{
	return((uint32_t)((((uint64_t)(tick)) * 1000) >> RTC_N_PREDIV_S));
}

void TIMER_IF_DelayMs(uint32_t delay)
{
uint32_t delayTicks = TIMER_IF_Convert_ms2Tick(delay);
uint32_t timeout = GetTimerTicks();

	while (((GetTimerTicks() - timeout)) < delayTicks)
		__NOP();
}

const UTIL_TIMER_Driver_s UTIL_TimerDriver =
{
  .InitTimer = TIMER_IF_Init,
  .StartTimerEvt = TIMER_IF_StartTimer,
  .StopTimerEvt = TIMER_IF_StopTimer,
  .SetTimerContext = TIMER_IF_SetTimerContext,
  .GetTimerContext = TIMER_IF_GetTimerContext,
  .GetTimerElapsedTime = TIMER_IF_GetTimerElapsedTime,
  .GetTimerValue = TIMER_IF_GetTimerValue,
  .GetMinimumTimeout = TIMER_IF_GetMinimumTimeout,
  .ms2Tick = TIMER_IF_Convert_ms2Tick,
  .Tick2ms = TIMER_IF_Convert_Tick2ms,
};

void TimerSetTimeout( UTIL_TIMER_Object_t *TimerObject )
{
  uint32_t minTicks= UTIL_TimerDriver.GetMinimumTimeout( );
  TimerObject->IsPending = 1;

  /* In case deadline too soon */
  if(TimerObject->Timestamp  < (UTIL_TimerDriver.GetTimerElapsedTime(  ) + minTicks) )
  {
	  TimerObject->Timestamp = UTIL_TimerDriver.GetTimerElapsedTime(  ) + minTicks;
  }
  UTIL_TimerDriver.StartTimerEvt( TimerObject->Timestamp );
}
void TimerInsertTimer( UTIL_TIMER_Object_t *TimerObject)
{
  UTIL_TIMER_Object_t* cur = TimerListHead;
  UTIL_TIMER_Object_t* next = TimerListHead->Next;

  while (cur->Next != NULL )
  {
    if( TimerObject->Timestamp  > next->Timestamp )
    {
        cur = next;
        next = next->Next;
    }
    else
    {
        cur->Next = TimerObject;
        TimerObject->Next = next;
        return;

    }
  }
  cur->Next = TimerObject;
  TimerObject->Next = NULL;
}

void TimerInsertNewHeadTimer( UTIL_TIMER_Object_t *TimerObject )
{
  UTIL_TIMER_Object_t* cur = TimerListHead;

  if( cur != NULL )
  {
    cur->IsPending = 0;
  }

  TimerObject->Next = cur;
  TimerListHead = TimerObject;
  TimerSetTimeout( TimerListHead );
}

UTIL_TIMER_Status_t UTIL_TIMER_Init(void)
{
	  TimerListHead = NULL;
	  return UTIL_TimerDriver.InitTimer();
}

UTIL_TIMER_Status_t UTIL_TIMER_Create( UTIL_TIMER_Object_t *TimerObject, uint32_t PeriodValue, UTIL_TIMER_Mode_t Mode, void ( *Callback )( void *), void *Argument)
{
  if((TimerObject != NULL) && (Callback != NULL))
  {
    TimerObject->Timestamp = 0U;
    TimerObject->ReloadValue = UTIL_TimerDriver.ms2Tick(PeriodValue);
    TimerObject->IsPending = 0U;
    TimerObject->IsRunning = 0U;
    TimerObject->IsReloadStopped = 0U;
    TimerObject->Callback = Callback;
    TimerObject->argument = Argument;
    TimerObject->Mode = Mode;
    TimerObject->Next = NULL;
    return UTIL_TIMER_OK;
  }
  else
  {
    return UTIL_TIMER_INVALID_PARAM;
  }
}

bool TimerExists( UTIL_TIMER_Object_t *TimerObject )
{
  UTIL_TIMER_Object_t* cur = TimerListHead;

  while( cur != NULL )
  {
    if( cur == TimerObject )
    {
      return true;
    }
    cur = cur->Next;
  }
  return false;
}
UTIL_TIMER_Time_t UTIL_TIMER_GetCurrentTime(void)
{
	  uint32_t now = UTIL_TimerDriver.GetTimerValue( );
	  return  UTIL_TimerDriver.Tick2ms(now);
}
UTIL_TIMER_Status_t UTIL_TIMER_SetPeriod(UTIL_TIMER_Object_t *TimerObject, uint32_t NewPeriodValue)
{
	  UTIL_TIMER_Status_t  ret = UTIL_TIMER_OK;

	  if(NULL == TimerObject)
	  {
		  ret = UTIL_TIMER_INVALID_PARAM;
	  }
	  else
	  {
	    TimerObject->ReloadValue = UTIL_TimerDriver.ms2Tick(NewPeriodValue);
	    if(TimerExists(TimerObject))
	    {
	      (void)UTIL_TIMER_Stop(TimerObject);
	      ret = UTIL_TIMER_Start(TimerObject);
	    }
	  }
	  return ret;
}
UTIL_TIMER_Time_t UTIL_TIMER_GetElapsedTime(UTIL_TIMER_Time_t past )
{
	  uint32_t nowInTicks = UTIL_TimerDriver.GetTimerValue( );
	  uint32_t pastInTicks = UTIL_TimerDriver.ms2Tick( past );
	  /* intentional wrap around. Works Ok if tick duation below 1ms */
	  return UTIL_TimerDriver.Tick2ms( nowInTicks- pastInTicks );
}
UTIL_TIMER_Status_t UTIL_TIMER_Stop( UTIL_TIMER_Object_t *TimerObject )
{
	  UTIL_TIMER_Status_t  ret = UTIL_TIMER_OK;

	  if (NULL != TimerObject)
	  {
	    UTIL_TIMER_Object_t* prev = TimerListHead;
	    UTIL_TIMER_Object_t* cur = TimerListHead;
	    TimerObject->IsReloadStopped = 1U;

	    /* List is empty or the Obj to stop does not exist  */
	    if(NULL != TimerListHead)
	    {
	      TimerObject->IsRunning = 0U;

	      if( TimerListHead == TimerObject ) /* Stop the Head */
	      {
	          TimerListHead->IsPending = 0;
	          if( TimerListHead->Next != NULL )
	          {
	            TimerListHead = TimerListHead->Next;
	            TimerSetTimeout( TimerListHead );
	          }
	          else
	          {
	            UTIL_TimerDriver.StopTimerEvt( );
	            TimerListHead = NULL;
	          }
	      }
	      else /* Stop an object within the list */
	      {
	        while( cur != NULL )
	        {
	          if( cur == TimerObject )
	          {
	            if( cur->Next != NULL )
	            {
	              cur = cur->Next;
	              prev->Next = cur;
	            }
	            else
	            {
	              cur = NULL;
	              prev->Next = cur;
	            }
	            break;
	          }
	          else
	          {
	            prev = cur;
	            cur = cur->Next;
	          }
	        }
	      }
	      ret = UTIL_TIMER_OK;
	    }
	  }
	  else
	  {
	    ret = UTIL_TIMER_INVALID_PARAM;
	  }
	  return ret;
}
UTIL_TIMER_Status_t UTIL_TIMER_Start( UTIL_TIMER_Object_t *TimerObject)
{
	  UTIL_TIMER_Status_t  ret = UTIL_TIMER_OK;
	  uint32_t elapsedTime;
	  uint32_t minValue;
	  uint32_t ticks;

	  if(( TimerObject != NULL ) && ( TimerExists( TimerObject ) == false ) && (TimerObject->IsRunning == 0U))
	  {
	    ticks = TimerObject->ReloadValue;
	    minValue = UTIL_TimerDriver.GetMinimumTimeout( );

	    if( ticks < minValue )
	    {
	      ticks = minValue;
	    }

	    TimerObject->Timestamp = ticks;
	    TimerObject->IsPending = 0U;
	    TimerObject->IsRunning = 1U;
	    TimerObject->IsReloadStopped = 0U;
	    if( TimerListHead == NULL )
	    {
	      UTIL_TimerDriver.SetTimerContext();
	      TimerInsertNewHeadTimer( TimerObject ); /* insert a timeout at now+obj->Timestamp */
	    }
	    else
	    {
	      elapsedTime = UTIL_TimerDriver.GetTimerElapsedTime( );
	      TimerObject->Timestamp += elapsedTime;

	      if( TimerObject->Timestamp < TimerListHead->Timestamp )
	      {
	        TimerInsertNewHeadTimer( TimerObject);
	      }
	      else
	      {
	        TimerInsertTimer( TimerObject);
	      }
	    }
	  }
	  else
	  {
	    ret =  UTIL_TIMER_INVALID_PARAM;
	  }
	  return ret;
}


/*
const UTIL_SYSTIM_Driver_s UTIL_SYSTIMDriver =
{
  TIMER_IF_BkUp_Write_Seconds,
  TIMER_IF_BkUp_Read_Seconds,
  TIMER_IF_BkUp_Write_SubSeconds,
  TIMER_IF_BkUp_Read_SubSeconds,
  TIMER_IF_GetTime,
};
*/

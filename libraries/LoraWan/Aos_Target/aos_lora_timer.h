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
 * aos_lora_timer.h
 *
 *  Created on: Aug 6, 2025
 *      Author: fil
 */

#ifndef LIBRARIES_LORAWAN_AOS_TARGET_AOS_LORA_TIMER_H_
#define LIBRARIES_LORAWAN_AOS_TARGET_AOS_LORA_TIMER_H_

/** @defgroup SYSTIME timer server
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "time.h"

#define MIN_ALARM_DELAY    3

#define TM_DAYS_IN_LEAP_YEAR                        ( ( uint32_t )  366U )
#define TM_DAYS_IN_YEAR                             ( ( uint32_t )  365U )
#define TM_SECONDS_IN_1DAY                          ( ( uint32_t )86400U )
#define TM_SECONDS_IN_1HOUR                         ( ( uint32_t ) 3600U )
#define TM_SECONDS_IN_1MINUTE                       ( ( uint32_t )   60U )
#define TM_MINUTES_IN_1HOUR                         ( ( uint32_t )   60U )
#define TM_HOURS_IN_1DAY                            ( ( uint32_t )   24U )

#define TM_MONTH_JANUARY                            ( ( uint8_t ) 0U )
#define TM_MONTH_FEBRUARY                           ( ( uint8_t ) 1U )
#define TM_MONTH_MARCH                              ( ( uint8_t ) 2U )
#define TM_MONTH_APRIL                              ( ( uint8_t ) 3U )
#define TM_MONTH_MAY                                ( ( uint8_t ) 4U )
#define TM_MONTH_JUNE                               ( ( uint8_t ) 5U )
#define TM_MONTH_JULY                               ( ( uint8_t ) 6U )
#define TM_MONTH_AUGUST                             ( ( uint8_t ) 7U )
#define TM_MONTH_SEPTEMBER                          ( ( uint8_t ) 8U )
#define TM_MONTH_OCTOBER                            ( ( uint8_t ) 9U )
#define TM_MONTH_NOVEMBER                           ( ( uint8_t )10U )
#define TM_MONTH_DECEMBER                           ( ( uint8_t )11U )

#define TM_WEEKDAY_SUNDAY                           ( ( uint8_t )0U )
#define TM_WEEKDAY_MONDAY                           ( ( uint8_t )1U )
#define TM_WEEKDAY_TUESDAY                          ( ( uint8_t )2U )
#define TM_WEEKDAY_WEDNESDAY                        ( ( uint8_t )3U )
#define TM_WEEKDAY_THURSDAY                         ( ( uint8_t )4U )
#define TM_WEEKDAY_FRIDAY                           ( ( uint8_t )5U )
#define TM_WEEKDAY_SATURDAY                         ( ( uint8_t )6U )

#define UNIX_GPS_EPOCH_OFFSET                       315964800

typedef struct SysTime_s
{
uint32_t Seconds;
int16_t SubSeconds;
}SysTime_t;

typedef struct
{
  void     (*BKUPWrite_Seconds) ( uint32_t Seconds);  /*!< Set the timer differencebetween real time and rtc time */
  uint32_t (*BKUPRead_Seconds) ( void ); /*!< Get the timer differencebetween real time and rtc time */
  void     (*BKUPWrite_SubSeconds) ( uint32_t SubSeconds);  /*!< Set the timer differencebetween real time and rtc time */
  uint32_t (*BKUPRead_SubSeconds) ( void ); /*!< Get the timer differencebetween real time and rtc time */
  uint32_t (*GetCalendarTime)( uint16_t* SubSeconds );          /*!< Set the rtc time */
} UTIL_SYSTIM_Driver_s;

extern const UTIL_SYSTIM_Driver_s UTIL_SYSTIMDriver;

SysTime_t SysTimeAdd( SysTime_t a, SysTime_t b );

void SysTimeSet( SysTime_t sysTime );

SysTime_t SysTimeGetMcuTime( void );

uint32_t SysTimeToMs( SysTime_t sysTime );

SysTime_t SysTimeFromMs( uint32_t timeMs );

uint32_t SysTimeMkTime( const struct tm* localtime );

void SysTimeLocalTime( const uint32_t timestamp, struct tm *localtime );

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <cmsis_compiler.h>
#include "utilities_conf.h"

typedef enum {
  UTIL_TIMER_ONESHOT  = 0, /*!<One-shot timer. */
  UTIL_TIMER_PERIODIC = 1  /*!<Periodic timer. */
} UTIL_TIMER_Mode_t;

typedef enum {
  UTIL_TIMER_OK            = 0,  /*!<Operation terminated successfully.*/
  UTIL_TIMER_INVALID_PARAM = 1,  /*!<Invalid Parameter.                */
  UTIL_TIMER_HW_ERROR      = 2,  /*!<Hardware Error.                   */
  UTIL_TIMER_UNKNOWN_ERROR = 3   /*!<Unknown Error.                    */
} UTIL_TIMER_Status_t;

typedef struct TimerEvent_s
{
    uint32_t Timestamp;           /*!<Expiring timer value in ticks from TimerContext */
    uint32_t ReloadValue;         /*!<Reload Value when Timer is restarted            */
    uint8_t IsPending;            /*!<Is the timer waiting for an event               */
    uint8_t IsRunning;            /*!<Is the timer running                            */
    uint8_t IsReloadStopped;      /*!<Is the reload stopped                           */
    UTIL_TIMER_Mode_t Mode;       /*!<Timer type : one-shot/continuous                */
    void ( *Callback )( void *);  /*!<callback function                               */
    void *argument;               /*!<callback argument                               */
	struct TimerEvent_s *Next;    /*!<Pointer to the next Timer object.               */
} UTIL_TIMER_Object_t;

/**
  * @brief Timer driver definition
  */
typedef struct
{
    UTIL_TIMER_Status_t   (* InitTimer )( void );                  /*!< Initialisation of the low layer timer    */
    UTIL_TIMER_Status_t   (* DeInitTimer )( void );                /*!< Un-Initialisation of the low layer timer */

    UTIL_TIMER_Status_t   (* StartTimerEvt )( uint32_t timeout );  /*!< Start the low layer timer */
    UTIL_TIMER_Status_t   (* StopTimerEvt )( void);                /*!< Stop the low layer timer */

    uint32_t              (* SetTimerContext)( void );             /*!< Set the timer context */
    uint32_t              (* GetTimerContext)( void );             /*!< Get the timer context */

    uint32_t              (* GetTimerElapsedTime)( void );         /*!< Get elapsed time */
    uint32_t              (* GetTimerValue)( void );               /*!< Get timer value */
    uint32_t              (* GetMinimumTimeout)( void );           /*!< Get Minimum timeout */

    uint32_t              (* ms2Tick)( uint32_t timeMicroSec );    /*!< convert ms to tick */
    uint32_t              (* Tick2ms)( uint32_t tick );            /*!< convert tick into ms */
} UTIL_TIMER_Driver_s;

/**
  * @brief Timer value on 32 bits
  */
typedef uint32_t UTIL_TIMER_Time_t;
/**
  *  @}
  */

/* Exported variables ------------------------------------------------------------*/
/** @defgroup TIMER_SERVER_exported_Variable TIMER_SERVER exported Variable
  *  @{
  */
/**
 * @brief low layer interface to handle timing execution
 *
 * @remark This structure is defined and initialized in the specific platform
 *         timer implementation
 */
extern const UTIL_TIMER_Driver_s UTIL_TimerDriver;

/**
  *  @}
  */

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/** @defgroup TIMER_SERVER_exported_function TIMER_SERVER exported function
  *  @{
  */

/**
  * @brief Initialize the timer server
  *
  * @retval Status based on @ref UTIL_TIMER_Status_t
  */
UTIL_TIMER_Status_t UTIL_TIMER_Init(void);

/**
  * @brief Un-Initialize the timer server
  *
  * @retval Status based on @ref UTIL_TIMER_Status_t
  */
UTIL_TIMER_Status_t UTIL_TIMER_DeInit(void);

/**
  * @brief Create the timer object
  *
  * @remark TimerSetValue function must be called before starting the timer.
  *         this function initializes timestamp and reload value at 0.
  *
  * @param TimerObject Structure containing the timer object parameters
  * @param PeriodValue Period value of the timer in ms
  * @param Mode @ref UTIL_TIMER_Mode_t
  * @param Callback Function callback called at the end of the timeout
  * @param Argument argument for the callback function
  * @retval Status based on @ref UTIL_TIMER_Status_t
  */
UTIL_TIMER_Status_t UTIL_TIMER_Create( UTIL_TIMER_Object_t *TimerObject, uint32_t PeriodValue, UTIL_TIMER_Mode_t Mode, void ( *Callback )( void *) , void *Argument);

/**
  * @brief Start and adds the timer object to the list of timer events
  *
  * @param TimerObject Structure containing the timer object parameters
  * @retval Status based on @ref UTIL_TIMER_Status_t
  */
UTIL_TIMER_Status_t UTIL_TIMER_Start( UTIL_TIMER_Object_t *TimerObject );

/**
  * @brief Start and adds the timer object to the list of timer events
  *
  * @param TimerObject Structure containing the timer object parameters
  * @param PeriodValue period value of the timer
  * @retval Status based on @ref UTIL_TIMER_Status_t
  */
UTIL_TIMER_Status_t UTIL_TIMER_StartWithPeriod( UTIL_TIMER_Object_t *TimerObject, uint32_t PeriodValue);

/**
  * @brief Stop and removes the timer object from the list of timer events
  *
  * @param TimerObject Structure containing the timer object parameters
  * @retval Status based on @ref UTIL_TIMER_Status_t
  */
UTIL_TIMER_Status_t UTIL_TIMER_Stop( UTIL_TIMER_Object_t *TimerObject );


/**
  * @brief update the period and start the timer
  *
  * @param TimerObject Structure containing the timer object parameters
  * @param NewPeriodValue new period value of the timer
  * @retval Status based on @ref UTIL_TIMER_Status_t
  */
UTIL_TIMER_Status_t UTIL_TIMER_SetPeriod(UTIL_TIMER_Object_t *TimerObject, uint32_t NewPeriodValue);

/**
 * @brief update the period and start the timer
 *
 * @param TimerObject Structure containing the timer object parameters
 * @param ReloadMode new reload mode @ref UTIL_TIMER_Mode_t
 * @retval Status based on @ref UTIL_TIMER_Status_t
 */
UTIL_TIMER_Status_t UTIL_TIMER_SetReloadMode(UTIL_TIMER_Object_t *TimerObject, UTIL_TIMER_Mode_t ReloadMode);

/**
 * @brief get the remaining time before timer expiration
 *  *
 * @param TimerObject Structure containing the timer object parameters
 * @param Time time before expiration in ms
 * @retval Status based on @ref UTIL_TIMER_Status_t
 */
UTIL_TIMER_Status_t UTIL_TIMER_GetRemainingTime(UTIL_TIMER_Object_t *TimerObject, uint32_t *Time);

/**
 * @brief return timer state
 *
 * @param TimerObject Structure containing the timer object parameters
 * @retval boolean value is returned 0 = false and 1 = true
 */
uint32_t UTIL_TIMER_IsRunning( UTIL_TIMER_Object_t *TimerObject );


/**
  * @brief return the remaining time of the first timer in the chain list
  *
  * @retval return the time in ms, the value 0xFFFFFFFF means no timer running
  */
uint32_t UTIL_TIMER_GetFirstRemainingTime(void);

/**
  * @brief return the current time
  *
  * @retval time value
  */
UTIL_TIMER_Time_t UTIL_TIMER_GetCurrentTime(void);


/**
  * @brief return the elapsed time
  *
  * @param past a value returned by the function UTIL_TIMER_GetCurrentTime
  * @retval elasped time value
  */
UTIL_TIMER_Time_t UTIL_TIMER_GetElapsedTime(UTIL_TIMER_Time_t past );

/**
 * @brief Timer IRQ event handler
 *
 * @note Head Timer Object is automatically removed from the List
 *
 * @note e.g. it is not needed to stop it
 */
void UTIL_TIMER_IRQ_Handler( void );





#endif /* LIBRARIES_LORAWAN_AOS_TARGET_AOS_LORA_TIMER_H_ */

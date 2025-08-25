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
 * lorawan_app.c
 *
 *  Created on: Aug 4, 2025
 *      Author: fil
 */
#include "main.h"
#include "lorawan_app.h"
#include "../SubGHz_Phy/radio_def.h"
#include "../../../A_os/libraries/LoraWan/LmHandler/LmHandler.h"

extern	void LoraInfo_Init(void);

typedef enum TxEventType_e
{
  TX_ON_TIMER,
  TX_ON_EVENT
} TxEventType_t;

#define LED_PERIOD_TIME 500
#define JOIN_TIME 2000

#define LORAWAN_NVM_BASE_ADDRESS                    ((void *)0x0803F000UL)
/*
static void SendTxData(void)
{

}
static void OnTxTimerEvent(void *context)
{

}
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams)
{

}
*/
static void OnTxData(LmHandlerTxParams_t *params)
{

}
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{

}
static void OnBeaconStatusChange(LmHandlerBeaconParams_t *params)
{

}
static void OnSysTimeUpdate(void)
{

}
static void OnClassChange(DeviceClass_t deviceClass)
{

}
/*
static void StoreContext(void)
{

}
static void StopJoin(void)
{

}
static void OnStopJoinTimerEvent(void *context)
{

}
*/
static void OnNvmDataChange(LmHandlerNvmContextStates_t state)
{
}
static void OnStoreContextRequest(void *nvm, uint32_t nvm_size)
{
}
static void OnRestoreContextRequest(void *nvm, uint32_t nvm_size)
{
}
static void OnMacProcessNotify(void)
{

}
static void OnTxPeriodicityChanged(uint32_t periodicity)
{

}
static void OnTxFrameCtrlChanged(LmHandlerMsgTypes_t isTxConfirmed)
{

}
static void OnPingSlotPeriodicityChanged(uint8_t pingSlotPeriodicity)
{

}

static void OnSystemReset(void)
{

}

static uint8_t GetBatteryLevel(void)
{
	return 0;
}
static int16_t GetTemperatureLevel(void)
{
	return 0;
}
static void GetUniqueId(uint8_t *id)
{

}
static void GetDevAddr(uint32_t *devAddr)
{

}

/*
static ActivationType_t ActivationType = LORAWAN_DEFAULT_ACTIVATION_TYPE;
static bool ForceRejoin = LORAWAN_FORCE_REJOIN_AT_BOOT;
*/
static LmHandlerCallbacks_t LmHandlerCallbacks =
{
	.GetBatteryLevel =              GetBatteryLevel,
	.GetTemperature =               GetTemperatureLevel,
	.GetUniqueId =                  GetUniqueId,
	.GetDevAddr =                   GetDevAddr,
	.OnRestoreContextRequest =      OnRestoreContextRequest,
	.OnStoreContextRequest =        OnStoreContextRequest,
	.OnMacProcess =                 OnMacProcessNotify,
	.OnNvmDataChange =              OnNvmDataChange,
	.OnTxData =                     OnTxData,
	.OnRxData =                     OnRxData,
	.OnBeaconStatusChange =         OnBeaconStatusChange,
	.OnSysTimeUpdate =              OnSysTimeUpdate,
	.OnClassChange =                OnClassChange,
	.OnTxPeriodicityChanged =       OnTxPeriodicityChanged,
	.OnTxFrameCtrlChanged =         OnTxFrameCtrlChanged,
	.OnPingSlotPeriodicityChanged = OnPingSlotPeriodicityChanged,
	.OnSystemReset =                OnSystemReset,
};
static LmHandlerParams_t LmHandlerParams =
{
	.ActiveRegion =             ACTIVE_REGION,
	.DefaultClass =             LORAWAN_DEFAULT_CLASS,
	.AdrEnable =                LORAWAN_ADR_STATE,
	.IsTxConfirmed =            LORAWAN_DEFAULT_CONFIRMED_MSG_STATE,
	.TxDatarate =               LORAWAN_DEFAULT_DATA_RATE,
	.TxPower =                  LORAWAN_DEFAULT_TX_POWER,
	.PingSlotPeriodicity =      LORAWAN_DEFAULT_PING_SLOT_PERIODICITY,
	.RxBCTimeout =              LORAWAN_DEFAULT_CLASS_B_C_RESP_TIMEOUT
};

void LoRaWAN_Init(void)
{
	  LoraInfo_Init();
	  LmHandlerInit(&LmHandlerCallbacks, APP_VERSION);
	  LmHandlerConfigure(&LmHandlerParams);
}

void LoRaWAN_Process(void)
{

}


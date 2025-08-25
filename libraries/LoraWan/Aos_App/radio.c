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
 * radio.c
 *
 *  Created on: Aug 4, 2025
 *      Author: fil
 */
#include "main.h"
#include "lorawan_app.h"
#include "radio.h"
#include "../SubGHz_Phy/radio_def.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/kernel_opt.h"

RadioEvents_t *RadioEvents;


static void RadioInit( RadioEvents_t *events )
{
    RadioEvents = events;
    LoRa_Init();
}
static RadioState_t RadioGetStatus( void )
{

}
static void RadioSetModem( RadioModems_t modem )
{

}
static void RadioSetChannel( uint32_t freq )
{
}

static bool RadioIsChannelFree( uint32_t freq, uint32_t rxBandwidth, int16_t rssiThresh, uint32_t maxCarrierSenseTime )
{
}

static uint32_t RadioRandom( void )
{
    uint32_t rnd = 0;
    return rnd;
}

static void RadioSetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                              uint32_t datarate, uint8_t coderate,
                              uint32_t bandwidthAfc, uint16_t preambleLen,
                              uint16_t symbTimeout, bool fixLen,
                              uint8_t payloadLen,
                              bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                              bool iqInverted, bool rxContinuous )
{

}
static void RadioSetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev,
                              uint32_t bandwidth, uint32_t datarate,
                              uint8_t coderate, uint16_t preambleLen,
                              bool fixLen, bool crcOn, bool freqHopOn,
                              uint8_t hopPeriod, bool iqInverted, uint32_t timeout )
{

}

static bool RadioCheckRfFrequency( uint32_t frequency )
{
    return true;
}
static uint32_t RadioTimeOnAir( RadioModems_t modem, uint32_t bandwidth,
                                uint32_t datarate, uint8_t coderate,
                                uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                                bool crcOn )
{

}
static radio_status_t RadioSend( uint8_t *buffer, uint8_t size )
{
	LoRa_Tx(buffer,size);
	return RADIO_STATUS_OK;
}
static void RadioSleep( void )
{
	LoRa_SetModeStandby();
}

static void RadioStandby( void )
{
	LoRa_SetModeStandby();
}
static void RadioRx( uint32_t timeout )
{

}
static void RadioStartCad( void )
{

}
static void RadioSetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time )
{

}
static int16_t RadioRssi( RadioModems_t modem )
{

}
static void RadioWrite( uint16_t addr, uint8_t data )
{
	LoRa_WriteSingleRegister(addr,data);

}
static uint8_t RadioRead( uint16_t addr )
{
	LoRa_ReadSingleRegister(addr);

}
static void RadioWriteRegisters( uint16_t addr, uint8_t *buffer, uint8_t size )
{
	LoRa_WriteRegisters( addr, buffer, size);

}
static void RadioReadRegisters( uint16_t addr, uint8_t *buffer, uint8_t size )
{
	LoRa_ReadRegisters(addr, buffer, size);

}
static void RadioSetMaxPayloadLength( RadioModems_t modem, uint8_t max )
{

}
static void RadioSetPublicNetwork( bool enable )
{

}

static uint32_t RadioGetWakeupTime( void )
{

}

static void RadioIrqProcess( void )
{

}
static void RadioRxBoosted( uint32_t timeout )
{
}
static void RadioSetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime )
{

}
static void RadioTxPrbs( void )
{

}

static void RadioTxCw( int8_t power )
{

}
static int32_t RadioSetRxGenericConfig( GenericModems_t modem, RxConfigGeneric_t *config,
                                        uint32_t rxContinuous, uint32_t symbTimeout )
{

}
static int32_t RadioSetTxGenericConfig( GenericModems_t modem, TxConfigGeneric_t *config,
                                        int8_t power, uint32_t timeout )
{

}
static int32_t RFW_TransmitLongPacket( uint16_t payload_size, uint32_t timeout,
                                void ( *TxLongPacketGetNextChunkCb )( uint8_t **buffer, uint8_t buffer_size ) )
{

}
static int32_t RFW_ReceiveLongPacket( uint8_t boosted_mode, uint32_t timeout,
                               void ( *RxLongPacketStoreChunkCb )( uint8_t *buffer, uint8_t chunk_size ) )
{

}
static radio_status_t RadioLrFhssSetCfg( const radio_lr_fhss_cfg_params_t *cfg_params )
{

}
static radio_status_t RadioLrFhssGetTimeOnAirInMs( const radio_lr_fhss_time_on_air_params_t *params, uint32_t  *time_on_air_in_ms )
{

}

const struct Radio_s Radio =
{
    .Init = RadioInit,
    .GetStatus = RadioGetStatus,
    .SetModem = RadioSetModem,
    .SetChannel = RadioSetChannel,
    .IsChannelFree = RadioIsChannelFree,
    .Random = RadioRandom,
    .SetRxConfig = RadioSetRxConfig,
	.SetTxConfig = RadioSetTxConfig,
    .CheckRfFrequency = RadioCheckRfFrequency,
    .TimeOnAir = RadioTimeOnAir,
    .Send = RadioSend,
	.Sleep = RadioSleep,
	.Standby = RadioStandby,
    .Rx = RadioRx,
    .StartCad = RadioStartCad,
    .SetTxContinuousWave = RadioSetTxContinuousWave,
    .Rssi = RadioRssi,
    .Write = RadioWrite,
	.Read = RadioRead,
    .WriteRegisters = RadioWriteRegisters,
    .ReadRegisters = RadioReadRegisters,
    .SetMaxPayloadLength = RadioSetMaxPayloadLength,
    .SetPublicNetwork = RadioSetPublicNetwork,
    .GetWakeupTime = RadioGetWakeupTime,
    .IrqProcess = RadioIrqProcess,
    .RxBoosted = RadioRxBoosted,
    .SetRxDutyCycle = RadioSetRxDutyCycle,
    .TxPrbs = RadioTxPrbs,
    .TxCw = RadioTxCw,
	.RadioSetRxGenericConfig = RadioSetRxGenericConfig,
    .RadioSetTxGenericConfig = RadioSetTxGenericConfig,
	.RadioSetTxGenericConfig = RFW_TransmitLongPacket,
    .ReceiveLongPacket = RFW_ReceiveLongPacket,
    /* LrFhss extended radio functions */
    .LrFhssSetCfg = RadioLrFhssSetCfg,
    .LrFhssGetTimeOnAirInMs = RadioLrFhssGetTimeOnAirInMs
};

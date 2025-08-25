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
 * lorawan_app.h
 *
 *  Created on: Aug 4, 2025
 *      Author: fil
 */

#ifndef LIBRARIES_LORAWAN_AOS_APP_LORAWAN_APP_H_
#define LIBRARIES_LORAWAN_AOS_APP_LORAWAN_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* LoraWAN application configuration (Mw is configured by lorawan_conf.h) */
#define ACTIVE_REGION                               LORAMAC_REGION_EU868

/* USER CODE BEGIN EC_CAYENNE_LPP */
/*!
 * CAYENNE_LPP is myDevices Application server.
 */
/*#define CAYENNE_LPP*/
/* USER CODE END EC_CAYENNE_LPP */

/*!
 * Defines the application data transmission duty cycle. 10s, value in [ms].
 */
#define APP_TX_DUTYCYCLE                            10000

/*!
 * LoRaWAN User application port
 * @note do not use 224. It is reserved for certification
 */
#define LORAWAN_USER_APP_PORT                       2

/*!
 * LoRaWAN Switch class application port
 * @note do not use 224. It is reserved for certification
 */
#define LORAWAN_SWITCH_CLASS_PORT                   3

/*!
 * LoRaWAN default class
 */
#define LORAWAN_DEFAULT_CLASS                       CLASS_A

/*!
 * LoRaWAN default confirm state
 */
#define LORAWAN_DEFAULT_CONFIRMED_MSG_STATE         LORAMAC_HANDLER_UNCONFIRMED_MSG

/*!
 * LoRaWAN Adaptive Data Rate
 * @note Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_STATE                           LORAMAC_HANDLER_ADR_ON

/*!
 * LoRaWAN Default Data Rate
 * @note Please note that LORAWAN_DEFAULT_DATA_RATE is used only when LORAWAN_ADR_STATE is disabled
 */
#define LORAWAN_DEFAULT_DATA_RATE                   DR_0

/*!
 * LoRaWAN Default Tx output power
 * @note LORAWAN_DEFAULT_TX_POWER must be defined in the [XXXX_MIN_TX_POWER - XXXX_MAX_TX_POWER] range,
         else the end-device uses the XXXX_DEFAULT_TX_POWER value
 */
#define LORAWAN_DEFAULT_TX_POWER                    TX_POWER_0

/*!
 * LoRaWAN default activation type
 */
#define LORAWAN_DEFAULT_ACTIVATION_TYPE             ACTIVATION_TYPE_OTAA

/*!
 * LoRaWAN force rejoin even if the NVM context is restored
 * @note useful only when context management is enabled by CONTEXT_MANAGEMENT_ENABLED
 */
#define LORAWAN_FORCE_REJOIN_AT_BOOT                false

/*!
 * User application data buffer size
 */
#define LORAWAN_APP_DATA_BUFFER_MAX_SIZE            242

/*!
 * Default Unicast ping slots periodicity
 *
 * \remark periodicity is equal to 2^LORAWAN_DEFAULT_PING_SLOT_PERIODICITY seconds
 *         example: 2^4 = 16 seconds. The end-device will open an Rx slot every 16 seconds.
 */
#define LORAWAN_DEFAULT_PING_SLOT_PERIODICITY       4

/*!
 * Default response timeout for class b and class c confirmed
 * downlink frames in milli seconds.
 *
 * The value shall not be smaller than RETRANSMIT_TIMEOUT plus
 * the maximum time on air.
 */
#define LORAWAN_DEFAULT_CLASS_B_C_RESP_TIMEOUT      8000

#define APP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define APP_VERSION_SUB1   (0x03U) /*!< [23:16] sub1 version */
#define APP_VERSION_SUB2   (0x01U) /*!< [15:8]  sub2 version */
#define APP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */

#define APP_VERSION_MAIN_SHIFT 24  /*!< main byte shift */
#define APP_VERSION_SUB1_SHIFT 16  /*!< sub1 byte shift */
#define APP_VERSION_SUB2_SHIFT 8   /*!< sub2 byte shift */
#define APP_VERSION_RC_SHIFT   0   /*!< release candidate byte shift */

#define APP_VERSION         ((APP_VERSION_MAIN  << APP_VERSION_MAIN_SHIFT)\
                             |(APP_VERSION_SUB1 << APP_VERSION_SUB1_SHIFT)\
                             |(APP_VERSION_SUB2 << APP_VERSION_SUB2_SHIFT)\
                             |(APP_VERSION_RC   << APP_VERSION_RC_SHIFT))

void LoRaWAN_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* LIBRARIES_LORAWAN_AOS_APP_LORAWAN_APP_H_ */

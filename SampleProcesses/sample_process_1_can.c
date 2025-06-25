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
 * sample_process_1_can.c
 *
 *  Created on: Jun 24, 2025
 *      Author: fil
 */
#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_CAN
uint8_t		led_cntr=0;
#define	PRC1_TICK				10
extern	FDCAN_HandleTypeDef hfdcan1;

uint8_t can_txbuf[] = "CAN";
uint8_t can_rxbuf[32];

/* Bit time configuration:
  fdcan_ker_ck               = 40 MHz
  Time_quantum (tq)          = 25 ns
  Synchronization_segment    = 1 tq
  Propagation_segment        = 23 tq
  Phase_segment_1            = 8 tq
  Phase_segment_2            = 8 tq
  Synchronization_Jump_width = 8 tq
  Bit_length                 = 40 tq = 1 \B5s
  Bit_rate                   = 1 MBit/s
*/

FDCAN_FilterTypeDef	FDCAN_Filter =
{
		.IdType = FDCAN_STANDARD_ID,
		.FilterIndex = 0,
		.FilterType = FDCAN_FILTER_MASK,
		.FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
		.FilterID1 = 0x321,
		.FilterID2 = 0x7FF,
};
FDCAN_TxHeaderTypeDef FDCAN_TxHeader =
{
		.Identifier = 0x321,
		.IdType = FDCAN_STANDARD_ID,
		.TxFrameType = FDCAN_DATA_FRAME,
		.DataLength = FDCAN_DLC_BYTES_2,
		.ErrorStateIndicator = FDCAN_ESI_ACTIVE,
		.BitRateSwitch = FDCAN_BRS_OFF,
		.FDFormat = FDCAN_CLASSIC_CAN,
		.TxEventFifoControl = FDCAN_NO_TX_EVENTS,
		.MessageMarker = 0,
};

FDCAN_RxHeaderTypeDef	FDCAN_RxHeader;
CAN_Drv_TypeDef	CAN_Drv =
{
		.channel = 1,
		.hfdcan = &hfdcan1,
		.TxData = can_txbuf,
		.RxData = can_rxbuf,
		.TxHeader = &FDCAN_TxHeader,
		.RxHeader = &FDCAN_RxHeader,
		.FilterConfig = &FDCAN_Filter,
		.wakeup_id = WAKEUP_FROM_CAN_IRQ,
		.flags = FDCAN_WAKEUP_ON_RX0 | FDCAN_WAKEUP_ON_RX1 | FDCAN_WAKEUP_ON_TX,
};

void process_led(void)
{
	switch(led_cntr)
	{
	case 70 :
	case 90 :
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
		break;
	default :
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
		break;
	}
	led_cntr++;
	if ( led_cntr == 100 )
		led_cntr = 0;
}
uint8_t	can_pkt = 0;

void sample_process_1_can(uint32_t process_id)
{
uint32_t	wakeup,flags;
	can_register(&CAN_Drv);

	create_timer(TIMER_ID_0,PRC1_TICK,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	create_timer(TIMER_ID_1,PRC1_TICK*100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER| EVENT_CAN_IRQ);
		get_wakeup_flags(&wakeup,&flags);


		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if (( flags & TIMER_ID_0) == TIMER_ID_0)
				process_led();
			if (( flags & TIMER_ID_1) == TIMER_ID_1)
				can_send(&CAN_Drv);
		}
		if (( wakeup & WAKEUP_FROM_CAN_IRQ) == WAKEUP_FROM_CAN_IRQ)
		{
			can_pkt++;
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_CAN
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

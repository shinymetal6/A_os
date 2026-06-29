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
 * dcc.c
 *
 *  Created on: Oct 16, 2024
 *      Author: fil
 */

#include "main.h"

#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_TIMERS_ENABLED

#include "dcc.h"
#include <string.h>

DCC_DriverStruct_t *dcc_drv_ptr;

DCC_Drv_Pkt_TypeDef	DCC_StandardIdle_Pkt =
{
	.preamble = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.packet_start_bit = DCC_START_BIT,
	.address = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.data_byte_start0_bit = DCC_START_BIT,
	.instruction = {
			DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0
	},
	.data_byte_start1_bit = DCC_START_BIT,
	.detection_short = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.packet_end_bit = DCC_END_BIT,
	.detection_long = {
			DCC_CUTOUT,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.long_packet_end_bit = DCC_END_BIT,
	.endpacket_long = DCC_END_BIT
};

DCC_Drv_Pkt_TypeDef	DCC_ExtendedIdle_Pkt =
{
	.preamble = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.packet_start_bit = DCC_START_BIT,
	.address = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.data_byte_start0_bit = DCC_START_BIT,
	.instruction = {
			DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0
	},
	.data_byte_start1_bit = DCC_START_BIT,
	.detection_short = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.packet_end_bit = DCC_END_BIT,
	.detection_long = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.long_packet_end_bit = DCC_END_BIT,
	.endpacket_long = DCC_CUTOUT
};

DCC_Drv_Pkt_TypeDef	DCC_CutOutStandardPkt =
{
	.preamble = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.packet_start_bit = DCC_CTF_0,
	.address = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.data_byte_start0_bit = DCC_CTF_0,
	.instruction = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.data_byte_start1_bit = DCC_CTF_0,
	.detection_short = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.packet_end_bit = DCC_CTF_0,
	.detection_long = {
			DCC_CTF_1,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.long_packet_end_bit = DCC_CTF_0,
	.endpacket_long = DCC_CTF_0
};

DCC_Drv_Pkt_TypeDef	DCC_CutOutExtendedPkt =
{
	.preamble = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.packet_start_bit = DCC_CTF_0,
	.address = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.data_byte_start0_bit = DCC_CTF_0,
	.instruction = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.data_byte_start1_bit = DCC_CTF_0,
	.detection_short = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.packet_end_bit = DCC_CTF_0,
	.detection_long = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.long_packet_end_bit = DCC_CTF_0,
	.endpacket_long = DCC_CTF_1
};

extern void dcc_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma);
extern void dcc_TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma);

 static uint8_t dcc_TIM_PWM_Start_DMA(DCC_DriverStruct_t *dcc_drv)
{
	if ( HAL_TIM_PWM_Start(dcc_drv->dcc_timer, dcc_drv->timer_dcc_channel) )
		return 1;
	if ((dcc_drv->flags & DCC_TIMER_DUAL_PHASE ) == DCC_TIMER_DUAL_PHASE)
		if ( HAL_TIMEx_PWMN_Start(dcc_drv->dcc_timer, dcc_drv->timer_180phase_dcc_channel) )
			return 1;

	if ( HAL_TIM_PWM_Start(dcc_drv->dcc_timer, dcc_drv->timer_cutout_channel) )
		return 1;

	dcc_drv->dcc_timer->hdma[dcc_drv->dma_dcc_index]->XferCpltCallback 		= dcc_TIM_DMADelayPulseCplt;
	dcc_drv->dcc_timer->hdma[dcc_drv->dma_dcc_index]->XferHalfCpltCallback 	= dcc_TIM_DMADelayPulseHalfCplt;
    if (HAL_DMA_Start_IT(dcc_drv->dcc_timer->hdma[dcc_drv->dma_dcc_index]   , (uint32_t )&dcc_drv->DCC_Pkt[0],    (uint32_t)&dcc_drv->dcc_timer->Instance->PSC,sizeof(dcc_drv->DCC_Pkt)/2) != HAL_OK)
      return 1;

    if (HAL_DMA_Start_IT(dcc_drv->dcc_timer->hdma[dcc_drv->dma_cutout_index], (uint32_t )&dcc_drv->DCC_Cutout_Pkt[0], (uint32_t)&dcc_drv->dcc_timer->Instance->CCR4,sizeof(dcc_drv->DCC_Cutout_Pkt[0])/2) != HAL_OK)
      return 1;
    dcc_drv->dcc_timer->Instance->DIER = dcc_drv->dma_dcc_value | dcc_drv->dma_cutout_value;
	return 0;
}

 uint32_t dcc_start(DCC_DriverStruct_t *dcc_drv)
{
	dcc_TIM_PWM_Start_DMA(dcc_drv);
	dcc_drv->status |= DCC_ON;
	return 0;
}

 uint32_t dcc_stop(DCC_DriverStruct_t *dcc_drv)
{
	HAL_TIM_PWM_Stop_DMA(dcc_drv->dcc_timer, dcc_drv->timer_dcc_channel);
	if ((dcc_drv->flags & DCC_TIMER_DUAL_PHASE ) == DCC_TIMER_DUAL_PHASE)
		HAL_TIMEx_PWMN_Stop_DMA(dcc_drv->dcc_timer, dcc_drv->timer_180phase_dcc_channel);
	HAL_TIM_PWM_Stop_DMA(dcc_drv->dcc_timer, dcc_drv->timer_cutout_channel);
	dcc_drv->status &= ~DCC_ON;
	return 0;
}

 uint32_t dcc_get_status(DCC_DriverStruct_t *dcc_drv)
{
	return dcc_drv->status;
}

 uint32_t static dcc_init(DCC_DriverStruct_t *dcc_drv)
{
	memcpy((uint8_t *)&dcc_drv->DCC_Pkt[0],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
	memcpy((uint8_t *)&dcc_drv->DCC_Pkt[1],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
	memcpy((uint8_t *)&dcc_drv->DCC_Cutout_Pkt[0],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
	memcpy((uint8_t *)&dcc_drv->DCC_Cutout_Pkt[1],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
	dcc_drv->status = DCC_INITIALIZED;

	switch(dcc_drv->timer_dcc_channel)
	{
	case TIM_CHANNEL_1 : dcc_drv->dma_dcc_value = TIM_DMA_CC1; dcc_drv->dma_dcc_index = TIM_DMA_ID_CC1;break;
	case TIM_CHANNEL_2 : dcc_drv->dma_dcc_value = TIM_DMA_CC2; dcc_drv->dma_dcc_index = TIM_DMA_ID_CC2;break;
	case TIM_CHANNEL_3 : dcc_drv->dma_dcc_value = TIM_DMA_CC3; dcc_drv->dma_dcc_index = TIM_DMA_ID_CC3;break;
	case TIM_CHANNEL_4 : dcc_drv->dma_dcc_value = TIM_DMA_CC4; dcc_drv->dma_dcc_index = TIM_DMA_ID_CC4;break;
	default : return 1;
	}
	switch(dcc_drv->timer_cutout_channel)
	{
	case TIM_CHANNEL_1 : dcc_drv->dma_cutout_value = TIM_DMA_CC1; dcc_drv->dma_cutout_index = TIM_DMA_ID_CC1;break;
	case TIM_CHANNEL_2 : dcc_drv->dma_cutout_value = TIM_DMA_CC2; dcc_drv->dma_cutout_index = TIM_DMA_ID_CC2;break;
	case TIM_CHANNEL_3 : dcc_drv->dma_cutout_value = TIM_DMA_CC3; dcc_drv->dma_cutout_index = TIM_DMA_ID_CC3;break;
	case TIM_CHANNEL_4 : dcc_drv->dma_cutout_value = TIM_DMA_CC4; dcc_drv->dma_cutout_index = TIM_DMA_ID_CC4;break;
	default : return 1;
	}
	dcc_drv->hdma[0] = dcc_drv->dcc_timer->hdma[dcc_drv->dma_dcc_index];
	dcc_drv->hdma[1] = dcc_drv->dcc_timer->hdma[dcc_drv->dma_cutout_index];
	dcc_drv->command_repeat_counter = dcc_drv->command_repeat_number;
	return 0;
}

 uint32_t	dcc_register(DCC_DriverStruct_t *dcc_drv)
{
DCC_DriverStruct_t *eptr, *pre_eptr;
	if ( dcc_drv->wakeup_id == 0)
		return DRIVER_REQUEST_FAILED;
	if ( dcc_drv->dcc_timer == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( dcc_drv->enable_port == NULL)
		return DRIVER_REQUEST_FAILED;

	if ( dcc_drv_ptr == NULL)
	{
		dcc_drv_ptr = dcc_drv;
		dcc_drv->next_dcc = NULL;
	}
	else
	{
		eptr = pre_eptr = dcc_drv_ptr;
		while(eptr->next_dcc != NULL)
		{
			pre_eptr = eptr;
			eptr = (DCC_DriverStruct_t *)eptr->next_dcc;
		}
		pre_eptr->next_dcc = (uint32_t *)dcc_drv;
		dcc_drv->next_dcc = NULL;
	}

	dcc_drv->process = get_current_process();
	dcc_drv->timer_type = TIM_TYPE_DCC;
	if ((dcc_drv->flags & DCC_TIMER_DUAL_PHASE ) == DCC_TIMER_DUAL_PHASE)
		dcc_drv->timer_180phase_dcc_channel = dcc_drv->timer_dcc_channel;
	dcc_drv->status = DRIVER_STATUS_IN_USE;
	dcc_init(dcc_drv);
	return DRIVER_REQUEST_FAILED;
}

/* Interrupts */


 DCC_DriverStruct_t *driver_get_drv_from_dcc_dma_channel(DMA_HandleTypeDef *hdma)
{
DCC_DriverStruct_t *eptr, *pre_eptr;

	eptr = pre_eptr = dcc_drv_ptr;
	while(eptr != NULL)
	{
		if (( eptr->hdma[0] == hdma ) || ( eptr->hdma[1] == hdma ))
			return eptr;
		pre_eptr = eptr;
		if ( eptr->next_dcc == NULL )
			return NULL;
		eptr = (DCC_DriverStruct_t *)eptr->next_dcc;
	}
	return NULL;
}

 void dcc_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma)
{
DCC_DriverStruct_t *dcc_drv;

	if ( (dcc_drv = driver_get_drv_from_dcc_dma_channel(hdma)) == NULL )
		return;
	if ( dcc_drv->timer_type != TIM_TYPE_DCC )
			return;
	if ( hdma == dcc_drv->hdma[0] )
	{
		if (( dcc_drv->status & DCC_PACKET_PENDING) == DCC_PACKET_PENDING)
		{
			dcc_drv->enable_port->BSRR = dcc_drv->enable_bit;
			memcpy((uint8_t *)&dcc_drv->DCC_Pkt[SECOND_HALF],(uint8_t *)&dcc_drv->DCC_WorkPkt,sizeof(DCC_StandardIdle_Pkt));
			if (( dcc_drv->status & DCC_PACKET_EXTENDED ) == DCC_PACKET_EXTENDED)
				memcpy((uint8_t *)&dcc_drv->DCC_Cutout_Pkt[SECOND_HALF],(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
			else
				memcpy((uint8_t *)&dcc_drv->DCC_Cutout_Pkt[SECOND_HALF],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
			if ( dcc_drv->command_repeat_counter )
				dcc_drv->command_repeat_counter--;
			else
			{
				dcc_drv->status &= ~DCC_PACKET_PENDING;
				if (( dcc_drv->status & DCC_RESET_SENT ) == DCC_RESET_SENT)
					dcc_drv->status &= ~DCC_RESET_SENT;
			}
		}
		else
		{
			dcc_drv->enable_port->BSRR = (uint32_t)dcc_drv->enable_bit << 16;
			if (( dcc_drv->status & DCC_PACKET_INPROGRESS ) == DCC_PACKET_INPROGRESS)
			{
				if (( dcc_drv->status & DCC_PACKET_EXTENDED ) == DCC_PACKET_EXTENDED)
				{
					memcpy((uint8_t *)&dcc_drv->DCC_Cutout_Pkt[SECOND_HALF],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
					dcc_drv->status &= ~DCC_PACKET_EXTENDED;
				}
				memcpy((uint8_t *)&dcc_drv->DCC_Pkt[SECOND_HALF],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
				dcc_drv->status &= ~DCC_PACKET_INPROGRESS;
			}
		}
	}
}

 void dcc_TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma)
{
DCC_DriverStruct_t *dcc_drv;

	if ( (dcc_drv = driver_get_drv_from_dcc_dma_channel(hdma)) == NULL )
		return;
	if ( dcc_drv->timer_type != TIM_TYPE_DCC )
			return;
	if ( hdma == dcc_drv->hdma[1] )
	{
		if (( dcc_drv->status & DCC_PACKET_PENDING) == DCC_PACKET_PENDING)
		{
			dcc_drv->enable_port->BSRR = dcc_drv->enable_bit;
			memcpy((uint8_t *)&dcc_drv->DCC_Pkt[FIRST_HALF],(uint8_t *)&dcc_drv->DCC_WorkPkt,sizeof(DCC_StandardIdle_Pkt));
			if (( dcc_drv->status & DCC_PACKET_EXTENDED ) == DCC_PACKET_EXTENDED)
				memcpy((uint8_t *)&dcc_drv->DCC_Cutout_Pkt[FIRST_HALF],(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
			else
				memcpy((uint8_t *)&dcc_drv->DCC_Cutout_Pkt[FIRST_HALF],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
			if ( dcc_drv->command_repeat_counter )
				dcc_drv->command_repeat_counter--;
			else
			{
				dcc_drv->status &= ~DCC_PACKET_PENDING;
				if (( dcc_drv->status & DCC_RESET_SENT ) == DCC_RESET_SENT)
					dcc_drv->status &= ~DCC_RESET_SENT;
			}
		}
		else
		{
			dcc_drv->enable_port->BSRR = (uint32_t)dcc_drv->enable_bit << 16;
			if (( dcc_drv->status & DCC_PACKET_INPROGRESS ) == DCC_PACKET_INPROGRESS)
			{
				if (( dcc_drv->status & DCC_PACKET_EXTENDED ) == DCC_PACKET_EXTENDED)
				{
					memcpy((uint8_t *)&dcc_drv->DCC_Cutout_Pkt[FIRST_HALF],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
					dcc_drv->status &= ~DCC_PACKET_EXTENDED;
				}
				memcpy((uint8_t *)&dcc_drv->DCC_Pkt[FIRST_HALF],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
				dcc_drv->status &= ~DCC_PACKET_INPROGRESS;
			}
		}
	}
}

/* commands */

 void encode_byte(uint16_t *dest , uint8_t value)
{
uint8_t i,mask=0x80;
	for ( i=0;i<8;i++)
	{
		dest[i] = DCC_0;
		if (( value & mask) == mask)
			dest[i] = DCC_1;
		mask = mask >> 1;
	}
}

 void compile_reset_packet(DCC_DriverStruct_t *dcc_drv)
{
	memcpy((uint8_t *)&dcc_drv->DCC_WorkPkt,(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
	memcpy((uint8_t *)&dcc_drv->DCC_Cutout_Pkt,(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
	dcc_drv->status &= ~DCC_PACKET_EXTENDED;
	encode_byte((uint16_t *)&dcc_drv->DCC_WorkPkt.address,0);
	encode_byte((uint16_t *)&dcc_drv->DCC_WorkPkt.instruction,0);
	encode_byte((uint16_t *)&dcc_drv->DCC_WorkPkt.detection_short,0);
	dcc_drv->status |= DCC_RESET_SENT;
}


 uint8_t one_byte_commands(DCC_DriverStruct_t *dcc_drv,char cmd)
{
uint8_t	ret_val = 0;
	switch ( cmd)
	{
	case 'A' 	:
		dcc_start(dcc_drv);
		break;
	case 'a' 	:
		dcc_stop(dcc_drv);
		break;
	case 'R' 	:
		compile_reset_packet(dcc_drv);
		break;
	default:		ret_val = 1;
	}
	return ret_val;
}

 uint8_t four_bytes_commands(DCC_DriverStruct_t *dcc_drv,char cmd,uint16_t track,uint16_t address,uint16_t data)
{
uint8_t ecc;

	if ( cmd == 'T' )
	{
		ecc = address ^ data;
		memcpy((uint8_t *)&dcc_drv->DCC_WorkPkt,(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
		memcpy((uint8_t *)&dcc_drv->DCC_Cutout_Pkt,(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
		encode_byte((uint16_t *)&dcc_drv->DCC_WorkPkt.address,address);
		encode_byte((uint16_t *)&dcc_drv->DCC_WorkPkt.instruction,data);
		encode_byte((uint16_t *)&dcc_drv->DCC_WorkPkt.detection_short,ecc);
		dcc_drv->status &= ~DCC_PACKET_EXTENDED;
		return 0;
	}
	return 1;
}

 uint8_t five_bytes_commands(DCC_DriverStruct_t *dcc_drv,char cmd,uint16_t track,uint16_t address,uint16_t datal,uint16_t datah)
{
uint8_t ecc;
	if ( cmd == 'T' )
	{
		ecc = address ^ datal ^ datah;
		memcpy((uint8_t *)&dcc_drv->DCC_WorkPkt,(uint8_t *)&DCC_ExtendedIdle_Pkt,sizeof(DCC_ExtendedIdle_Pkt));
		memcpy((uint8_t *)&dcc_drv->DCC_Cutout_Pkt,(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
		encode_byte((uint16_t *)&dcc_drv->DCC_WorkPkt.address,address);
		encode_byte((uint16_t *)&dcc_drv->DCC_WorkPkt.instruction,datal);
		encode_byte((uint16_t *)&dcc_drv->DCC_WorkPkt.detection_short,datah);
		encode_byte((uint16_t *)&dcc_drv->DCC_WorkPkt.detection_long,ecc);
		dcc_drv->status |= DCC_PACKET_EXTENDED;
		return 0;
	}
	return 1;
}

 uint32_t dcc_commands(DCC_DriverStruct_t *dcc_drv,uint8_t *commands,uint16_t commands_number)
{
	switch(commands_number)
	{
	case 1:
		one_byte_commands(dcc_drv,commands[DCC_CMD]);
		break;
	case 4:
		four_bytes_commands(dcc_drv,commands[DCC_CMD],commands[DCC_TRACK],commands[DCC_ADDRESS],commands[DCC_DATAH]);
		break;
	case 5:
		five_bytes_commands(dcc_drv,commands[DCC_CMD],commands[DCC_TRACK],commands[DCC_ADDRESS],commands[DCC_DATAH],commands[DCC_DATAL]);
		break;
	default:
		return 1;
	}
	dcc_drv->command_repeat_counter = dcc_drv->command_repeat_number;
	dcc_drv->status |= (DCC_PACKET_PENDING | DCC_PACKET_INPROGRESS);
	return 0;
}
#endif // #ifdef A_OS_TIMERS_ENABLED

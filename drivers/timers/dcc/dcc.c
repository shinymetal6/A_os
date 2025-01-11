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

#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
//#include "../../../kernel/kernel_opt.h"
#ifdef A_OS_TIMERS_ENABLED

#include "dcc.h"
#include <string.h>

extern	TIM_DriverStruct_t	TIM_DriverStruct[MAX_TIM_DRIVERS];
extern	uint8_t				last_tim_used_handle,tim_driver_request;

extern void dcc_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma);
extern void dcc_TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma);

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
/*
DCC_Drv_Pkt_TypeDef		DCC_Pkt[2];
DCC_Drv_Pkt_TypeDef		DCC_Cutout_Pkt[2];
DCC_Drv_Pkt_TypeDef		DCC_WorkPkt;
*/
ITCM_AREA_CODE uint32_t driver_get_handle_from_dcc_dma_channel(uint32_t *handle_ch0 , uint32_t *handle_ch1)
{
uint32_t	i,drv_ret=0;
DCC_Control_Drv_TypeDef	*dcc_driver_data;
	for(i=0;i<MAX_TIM_DRIVERS;i++)
	{
		if ( TIM_DriverStruct[i].process != 0 )
		{
			dcc_driver_data = (DCC_Control_Drv_TypeDef *)TIM_DriverStruct[i].private_data;
			if ( dcc_driver_data->hdma[0] != NULL)
			{
				*handle_ch0 = i;
				drv_ret++;
			}
			if ( dcc_driver_data->hdma[1] != NULL)
			{
				*handle_ch1 = i;
				drv_ret++;
			}
		}
	}
	return drv_ret;
}

ITCM_AREA_CODE uint8_t dcc_TIM_PWM_Start_DMA(uint8_t handle)
{
DCC_Control_Drv_TypeDef	*dcc_driver_data = (DCC_Control_Drv_TypeDef *)TIM_DriverStruct[handle].private_data;

	if ( dcc_driver_data == NULL )
		return 1;

	if ( HAL_TIM_PWM_Start(dcc_driver_data->dcc_timer, dcc_driver_data[handle].timer_dcc_channel) )
		return 1;
	if ((dcc_driver_data->flags & DCC_TIMER_DUAL_PHASE ) == DCC_TIMER_DUAL_PHASE)
		if ( HAL_TIMEx_PWMN_Start(dcc_driver_data->dcc_timer, dcc_driver_data[handle].timer_180phase_dcc_channel) )
			return 1;

	if ( HAL_TIM_PWM_Start(dcc_driver_data->dcc_timer, dcc_driver_data[handle].timer_cutout_channel) )
		return 1;

	dcc_driver_data->dcc_timer->hdma[dcc_driver_data->dma_dcc_index]->XferCpltCallback 		= dcc_TIM_DMADelayPulseCplt;
	dcc_driver_data->dcc_timer->hdma[dcc_driver_data->dma_dcc_index]->XferHalfCpltCallback 	= dcc_TIM_DMADelayPulseHalfCplt;
    if (HAL_DMA_Start_IT(dcc_driver_data->dcc_timer->hdma[dcc_driver_data->dma_dcc_index]   , (uint32_t )&dcc_driver_data->DCC_Pkt[0],    (uint32_t)&dcc_driver_data->dcc_timer->Instance->PSC,sizeof(dcc_driver_data->DCC_Pkt)/2) != HAL_OK)
      return 1;

    if (HAL_DMA_Start_IT(dcc_driver_data->dcc_timer->hdma[dcc_driver_data->dma_cutout_index], (uint32_t )&dcc_driver_data->DCC_Cutout_Pkt[0], (uint32_t)&dcc_driver_data->dcc_timer->Instance->CCR4,sizeof(dcc_driver_data->DCC_Cutout_Pkt[0])/2) != HAL_OK)
      return 1;
    dcc_driver_data->dcc_timer->Instance->DIER = dcc_driver_data->dma_dcc_value | dcc_driver_data->dma_cutout_value;
	return 0;
}

ITCM_AREA_CODE void encode_byte(uint16_t *dest , uint8_t value)
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

ITCM_AREA_CODE void compile_reset_packet(uint8_t handle)
{
DCC_Control_Drv_TypeDef	*dcc_driver_data = (DCC_Control_Drv_TypeDef *)TIM_DriverStruct[handle].private_data;;

	memcpy((uint8_t *)&dcc_driver_data->DCC_WorkPkt,(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
	memcpy((uint8_t *)&dcc_driver_data->DCC_Cutout_Pkt,(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
	dcc_driver_data->status &= ~DCC_PACKET_EXTENDED;
	encode_byte((uint16_t *)&dcc_driver_data->DCC_WorkPkt.address,0);
	encode_byte((uint16_t *)&dcc_driver_data->DCC_WorkPkt.instruction,0);
	encode_byte((uint16_t *)&dcc_driver_data->DCC_WorkPkt.detection_short,0);
}

ITCM_AREA_CODE uint8_t one_byte_commands(uint8_t handle,char cmd)
{
uint8_t	ret_val = 0;
DCC_Control_Drv_TypeDef	*dcc_driver_data = (DCC_Control_Drv_TypeDef *)TIM_DriverStruct[handle].private_data;;

	switch ( cmd)
	{
	case 'A' 	:
		dcc_driver_data->status |= DCC_ON;
		break;
	case 'a' 	:
		dcc_driver_data->status &= ~DCC_ON;
		break;
	case 'R' 	:
		compile_reset_packet(handle);
		break;
	default:		ret_val = 1;
	}
	return ret_val;
}

ITCM_AREA_CODE uint8_t four_bytes_commands(uint8_t handle,char cmd,uint16_t track,uint16_t address,uint16_t data)
{
uint8_t ecc;
DCC_Control_Drv_TypeDef	*dcc_driver_data = (DCC_Control_Drv_TypeDef *)TIM_DriverStruct[handle].private_data;;

	if ( cmd == 'T' )
	{
		ecc = address ^ data;
		memcpy((uint8_t *)&dcc_driver_data->DCC_WorkPkt,(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
		memcpy((uint8_t *)&dcc_driver_data->DCC_Cutout_Pkt,(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
		encode_byte((uint16_t *)&dcc_driver_data->DCC_WorkPkt.address,address);
		encode_byte((uint16_t *)&dcc_driver_data->DCC_WorkPkt.instruction,data);
		encode_byte((uint16_t *)&dcc_driver_data->DCC_WorkPkt.detection_short,ecc);
		dcc_driver_data->status &= ~DCC_PACKET_EXTENDED;
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE uint8_t five_bytes_commands(uint8_t handle,char cmd,uint16_t track,uint16_t address,uint16_t datal,uint16_t datah)
{
uint8_t ecc;
DCC_Control_Drv_TypeDef	*dcc_driver_data = (DCC_Control_Drv_TypeDef *)TIM_DriverStruct[handle].private_data;;

	if ( cmd == 'T' )
	{
		ecc = address ^ datal ^ datah;
		memcpy((uint8_t *)&dcc_driver_data->DCC_WorkPkt,(uint8_t *)&DCC_ExtendedIdle_Pkt,sizeof(DCC_ExtendedIdle_Pkt));
		memcpy((uint8_t *)&dcc_driver_data->DCC_Cutout_Pkt,(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
		encode_byte((uint16_t *)&dcc_driver_data->DCC_WorkPkt.address,address);
		encode_byte((uint16_t *)&dcc_driver_data->DCC_WorkPkt.instruction,datal);
		encode_byte((uint16_t *)&dcc_driver_data->DCC_WorkPkt.detection_short,datah);
		encode_byte((uint16_t *)&dcc_driver_data->DCC_WorkPkt.detection_long,ecc);
		dcc_driver_data->status |= ~DCC_PACKET_EXTENDED;
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t dcc_start(uint8_t handle)
{
	dcc_TIM_PWM_Start_DMA(handle);
	return 0;
}

ITCM_AREA_CODE uint32_t dcc_stop(uint8_t handle)
{
DCC_Control_Drv_TypeDef	*dcc_driver_data = (DCC_Control_Drv_TypeDef *)TIM_DriverStruct[handle].private_data;;
	HAL_TIM_PWM_Stop_DMA(dcc_driver_data->dcc_timer, dcc_driver_data[handle].timer_dcc_channel);
	if ((dcc_driver_data->flags & DCC_TIMER_DUAL_PHASE ) == DCC_TIMER_DUAL_PHASE)
		HAL_TIMEx_PWMN_Stop_DMA(dcc_driver_data->dcc_timer, dcc_driver_data[handle].timer_180phase_dcc_channel);
	HAL_TIM_PWM_Stop_DMA(dcc_driver_data->dcc_timer, dcc_driver_data[handle].timer_cutout_channel);
	return 0;
}

ITCM_AREA_CODE uint32_t dcc_get_status(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE uint32_t dcc_get_values(uint8_t handle,uint8_t *values,uint16_t values_number)
{
	return 0;
}


ITCM_AREA_CODE uint32_t dcc_set_values(uint8_t handle,uint8_t *values,uint16_t values_number)
{
DCC_Control_Drv_TypeDef	*dcc_driver_data = (DCC_Control_Drv_TypeDef *)TIM_DriverStruct[handle].private_data;;
	switch(values_number)
	{
	case 1:
		one_byte_commands(handle,values[DCC_CMD]);
		break;
	case 4:
		four_bytes_commands(handle,values[DCC_CMD],values[DCC_TRACK],values[DCC_ADDRESS],values[DCC_DATAH]);
		break;
	case 5:
		five_bytes_commands(handle,values[DCC_CMD],values[DCC_TRACK],values[DCC_ADDRESS],values[DCC_DATAH],values[DCC_DATAL]);
		break;
	default:
		return 1;
	}
	dcc_driver_data->repetition_counter = dcc_driver_data->repetition;
	dcc_driver_data->status |= (DCC_PACKET_PENDING | DCC_PACKET_INPROGRESS);
	return 0;
}


ITCM_AREA_CODE uint32_t dcc_init(uint8_t handle)
{
DCC_Control_Drv_TypeDef	*dcc_driver_data = (DCC_Control_Drv_TypeDef *)TIM_DriverStruct[handle].private_data;;

	memcpy((uint8_t *)&dcc_driver_data->DCC_Pkt[0],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
	memcpy((uint8_t *)&dcc_driver_data->DCC_Pkt[1],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
	memcpy((uint8_t *)&dcc_driver_data->DCC_Cutout_Pkt[0],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
	memcpy((uint8_t *)&dcc_driver_data->DCC_Cutout_Pkt[1],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
	dcc_driver_data->status = DCC_INITIALIZED;
	dcc_driver_data->handle = handle;

	switch(dcc_driver_data->timer_dcc_channel)
	{
	case TIM_CHANNEL_1 : dcc_driver_data->dma_dcc_value = TIM_DMA_CC1; dcc_driver_data->dma_dcc_index = TIM_DMA_ID_CC1;break;
	case TIM_CHANNEL_2 : dcc_driver_data->dma_dcc_value = TIM_DMA_CC2; dcc_driver_data->dma_dcc_index = TIM_DMA_ID_CC2;break;
	case TIM_CHANNEL_3 : dcc_driver_data->dma_dcc_value = TIM_DMA_CC3; dcc_driver_data->dma_dcc_index = TIM_DMA_ID_CC3;break;
	case TIM_CHANNEL_4 : dcc_driver_data->dma_dcc_value = TIM_DMA_CC4; dcc_driver_data->dma_dcc_index = TIM_DMA_ID_CC4;break;
	default : return 1;
	}
	switch(dcc_driver_data->timer_cutout_channel)
	{
	case TIM_CHANNEL_1 : dcc_driver_data->dma_cutout_value = TIM_DMA_CC1; dcc_driver_data->dma_cutout_index = TIM_DMA_ID_CC1;break;
	case TIM_CHANNEL_2 : dcc_driver_data->dma_cutout_value = TIM_DMA_CC2; dcc_driver_data->dma_cutout_index = TIM_DMA_ID_CC2;break;
	case TIM_CHANNEL_3 : dcc_driver_data->dma_cutout_value = TIM_DMA_CC3; dcc_driver_data->dma_cutout_index = TIM_DMA_ID_CC3;break;
	case TIM_CHANNEL_4 : dcc_driver_data->dma_cutout_value = TIM_DMA_CC4; dcc_driver_data->dma_cutout_index = TIM_DMA_ID_CC4;break;
	default : return 1;
	}
	dcc_driver_data->hdma[0] = dcc_driver_data->dcc_timer->hdma[dcc_driver_data->dma_dcc_index];
	dcc_driver_data->hdma[1] = dcc_driver_data->dcc_timer->hdma[dcc_driver_data->dma_cutout_index];
	dcc_driver_data->repetition_counter = dcc_driver_data->repetition = 0;
	dcc_driver_data->repetition = 5;
	return 0;
}

ITCM_AREA_CODE uint32_t	dcc_register(DCC_Control_Drv_TypeDef *private_data)
{
DCC_Control_Drv_TypeDef	*dcc_driver_data;
	if ( TIM_DriverStruct[last_tim_used_handle].process == 0 )
	{
		TIM_DriverStruct[last_tim_used_handle].process = get_current_process();
		TIM_DriverStruct[last_tim_used_handle].flags = 0;
		TIM_DriverStruct[last_tim_used_handle].private_data = (uint32_t *)private_data;

		dcc_driver_data = (DCC_Control_Drv_TypeDef *)TIM_DriverStruct[last_tim_used_handle].private_data;
		dcc_driver_data->flags = 0;
		TIM_DriverStruct[last_tim_used_handle].status = DRIVER_STATUS_IN_USE;

		last_tim_used_handle++;
		tim_driver_request++;
		return last_tim_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

/* Interrupt */

ITCM_AREA_CODE void dcc_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma)
{
uint32_t handle_dcc , handle_cutout;
DCC_Control_Drv_TypeDef	*dcc_driver_data;
	if ( driver_get_handle_from_dcc_dma_channel(&handle_dcc,&handle_cutout) )
	{
		dcc_driver_data = (DCC_Control_Drv_TypeDef *)TIM_DriverStruct[handle_dcc].private_data;
		if ( hdma == dcc_driver_data->hdma[0] )
		{
			if (( dcc_driver_data->status & DCC_PACKET_PENDING) == DCC_PACKET_PENDING)
			{
				dcc_driver_data->enable_port->BSRR = dcc_driver_data->enable_bit;
				memcpy((uint8_t *)&dcc_driver_data->DCC_Pkt[SECOND_HALF],(uint8_t *)&dcc_driver_data->DCC_WorkPkt,sizeof(DCC_StandardIdle_Pkt));
				if (( dcc_driver_data->status & DCC_PACKET_EXTENDED ) == DCC_PACKET_EXTENDED)
					memcpy((uint8_t *)&dcc_driver_data->DCC_Cutout_Pkt[SECOND_HALF],(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
				else
					memcpy((uint8_t *)&dcc_driver_data->DCC_Cutout_Pkt[SECOND_HALF],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
				if ( dcc_driver_data->repetition_counter )
					dcc_driver_data->repetition_counter--;
				else
					dcc_driver_data->status &= ~DCC_PACKET_PENDING;
			}
			else
			{
				dcc_driver_data->enable_port->BSRR = (uint32_t)dcc_driver_data->enable_bit << 16;
				if (( dcc_driver_data->status & DCC_PACKET_INPROGRESS ) == DCC_PACKET_INPROGRESS)
				{
					if (( dcc_driver_data->status & DCC_PACKET_EXTENDED ) == DCC_PACKET_EXTENDED)
					{
						memcpy((uint8_t *)&dcc_driver_data->DCC_Cutout_Pkt[SECOND_HALF],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
						dcc_driver_data->status &= ~DCC_PACKET_EXTENDED;
					}
					memcpy((uint8_t *)&dcc_driver_data->DCC_Pkt[SECOND_HALF],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
					dcc_driver_data->status &= ~DCC_PACKET_INPROGRESS;
				}
			}
		}
	}
}

ITCM_AREA_CODE void dcc_TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma)
{
uint32_t handle_dcc , handle_cutout;
DCC_Control_Drv_TypeDef	*dcc_driver_data;
	if ( driver_get_handle_from_dcc_dma_channel(&handle_dcc,&handle_cutout) )
	{
		dcc_driver_data = (DCC_Control_Drv_TypeDef *)TIM_DriverStruct[handle_dcc].private_data;
		if ( hdma == dcc_driver_data->hdma[1] )
		{
			if (( dcc_driver_data->status & DCC_PACKET_PENDING) == DCC_PACKET_PENDING)
			{
				dcc_driver_data->enable_port->BSRR = dcc_driver_data->enable_bit;
				memcpy((uint8_t *)&dcc_driver_data->DCC_Pkt[FIRST_HALF],(uint8_t *)&dcc_driver_data->DCC_WorkPkt,sizeof(DCC_StandardIdle_Pkt));
				if (( dcc_driver_data->status & DCC_PACKET_EXTENDED ) == DCC_PACKET_EXTENDED)
					memcpy((uint8_t *)&dcc_driver_data->DCC_Cutout_Pkt[FIRST_HALF],(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
				else
					memcpy((uint8_t *)&dcc_driver_data->DCC_Cutout_Pkt[FIRST_HALF],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
				if ( dcc_driver_data->repetition_counter )
					dcc_driver_data->repetition_counter--;
				else
					dcc_driver_data->status &= ~DCC_PACKET_PENDING;
			}
			else
			{
				dcc_driver_data->enable_port->BSRR = (uint32_t)dcc_driver_data->enable_bit << 16;
				if (( dcc_driver_data->status & DCC_PACKET_INPROGRESS ) == DCC_PACKET_INPROGRESS)
				{
					if (( dcc_driver_data->status & DCC_PACKET_EXTENDED ) == DCC_PACKET_EXTENDED)
					{
						memcpy((uint8_t *)&dcc_driver_data->DCC_Cutout_Pkt[FIRST_HALF],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
						dcc_driver_data->status &= ~DCC_PACKET_EXTENDED;
					}
					memcpy((uint8_t *)&dcc_driver_data->DCC_Pkt[FIRST_HALF],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
					dcc_driver_data->status &= ~DCC_PACKET_INPROGRESS;
				}
			}
		}
	}
}

#endif // #ifdef A_OS_TIMERS_ENABLED

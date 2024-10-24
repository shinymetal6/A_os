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

#ifdef DCC_SYSTEM_ENABLE
#include "dcc.h"
#include <string.h>
extern	DriverStruct_t	*DriverStruct[MAX_DRIVERS];

static DCC_Drv_Pkt_TypeDef	DCC_StandardIdle_Pkt =
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

static DCC_Drv_Pkt_TypeDef	DCC_ExtendedIdle_Pkt =
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

static DCC_Drv_Pkt_TypeDef	DCC_CutOutStandardPkt =
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

static DCC_Drv_Pkt_TypeDef	DCC_CutOutExtendedPkt =
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

static DCC_Drv_Pkt_TypeDef		DCC_Pkt[2];
static DCC_Drv_Pkt_TypeDef		DCC_Cutout_Pkt[2];
static DCC_Drv_Pkt_TypeDef		DCC_WorkPkt;

static uint32_t driver_get_handle_from_dcc_dma_channel(uint32_t *handle_ch0 , uint32_t *handle_ch1)
{
uint32_t	i,drv_ret=0;
DCC_Control_Drv_TypeDef	*BasicDriverStruct;
	for(i=0;i<MAX_DRIVERS;i++)
	{
		if ( DriverStruct[i] != NULL )
		{
			if (( DriverStruct[i]->status & DRIVER_STATUS_IN_USE) ==  DRIVER_STATUS_IN_USE)
			{
				BasicDriverStruct = (DCC_Control_Drv_TypeDef *)DriverStruct[i]->driver_private_data;
				if ( BasicDriverStruct->hdma[0] != NULL)
				{
					*handle_ch0 = i;
					drv_ret++;
				}
				if ( BasicDriverStruct->hdma[1] != NULL)
				{
					*handle_ch1 = i;
					drv_ret++;
				}
			}
		}
	}
	return drv_ret;
}

static void dcc_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma)
{
uint32_t handle_dcc , handle_cutout;
DCC_Control_Drv_TypeDef	*DCC_Control_Drv;
	if ( driver_get_handle_from_dcc_dma_channel(&handle_dcc,&handle_cutout) )
	{
		DCC_Control_Drv = (DCC_Control_Drv_TypeDef	*)DriverStruct[handle_dcc]->driver_private_data;
		if ( hdma == DCC_Control_Drv->hdma[0] )
		{
			if (( DCC_Control_Drv->status & DCC_PACKET_PENDING) == DCC_PACKET_PENDING)
			{
				GPIO_SetGpioOUT(DCC_Control_Drv->enable_port, DCC_Control_Drv->enable_bit, GPIO_PIN_SET);
				memcpy((uint8_t *)&DCC_Pkt[SECOND_HALF],(uint8_t *)&DCC_WorkPkt,sizeof(DCC_StandardIdle_Pkt));
				if (( DCC_Control_Drv->status & DCC_PACKET_EXTENDED ) == DCC_PACKET_EXTENDED)
					memcpy((uint8_t *)&DCC_Cutout_Pkt[SECOND_HALF],(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
				else
					memcpy((uint8_t *)&DCC_Cutout_Pkt[SECOND_HALF],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
				if ( DCC_Control_Drv->repetition_counter )
					DCC_Control_Drv->repetition_counter--;
				else
					DCC_Control_Drv->status &= ~DCC_PACKET_PENDING;
			}
			else
			{
				GPIO_SetGpioOUT(DCC_Control_Drv->enable_port, DCC_Control_Drv->enable_bit, GPIO_PIN_RESET);
				if (( DCC_Control_Drv->status & DCC_PACKET_INPROGRESS ) == DCC_PACKET_INPROGRESS)
				{
					if (( DCC_Control_Drv->status & DCC_PACKET_EXTENDED ) == DCC_PACKET_EXTENDED)
					{
						memcpy((uint8_t *)&DCC_Cutout_Pkt[SECOND_HALF],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
						DCC_Control_Drv->status &= ~DCC_PACKET_EXTENDED;
					}
					memcpy((uint8_t *)&DCC_Pkt[SECOND_HALF],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
					DCC_Control_Drv->status &= ~DCC_PACKET_INPROGRESS;
				}
			}
		}
	}
}

static void dcc_TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma)
{
uint32_t handle_dcc , handle_cutout;
DCC_Control_Drv_TypeDef	*DCC_Control_Drv;
	if ( driver_get_handle_from_dcc_dma_channel(&handle_dcc,&handle_cutout) )
	{
		DCC_Control_Drv = (DCC_Control_Drv_TypeDef	*)DriverStruct[handle_dcc]->driver_private_data;
		if ( hdma == DCC_Control_Drv->hdma[1] )
		{
			if (( DCC_Control_Drv->status & DCC_PACKET_PENDING) == DCC_PACKET_PENDING)
			{
				GPIO_SetGpioOUT(DCC_Control_Drv->enable_port, DCC_Control_Drv->enable_bit, GPIO_PIN_SET);
				memcpy((uint8_t *)&DCC_Pkt[FIRST_HALF],(uint8_t *)&DCC_WorkPkt,sizeof(DCC_StandardIdle_Pkt));
				if (( DCC_Control_Drv->status & DCC_PACKET_EXTENDED ) == DCC_PACKET_EXTENDED)
					memcpy((uint8_t *)&DCC_Cutout_Pkt[FIRST_HALF],(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
				else
					memcpy((uint8_t *)&DCC_Cutout_Pkt[FIRST_HALF],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
				if ( DCC_Control_Drv->repetition_counter )
					DCC_Control_Drv->repetition_counter--;
				else
					DCC_Control_Drv->status &= ~DCC_PACKET_PENDING;
			}
			else
			{
				GPIO_SetGpioOUT(DCC_Control_Drv->enable_port, DCC_Control_Drv->enable_bit, GPIO_PIN_RESET);
				if (( DCC_Control_Drv->status & DCC_PACKET_INPROGRESS ) == DCC_PACKET_INPROGRESS)
				{
					if (( DCC_Control_Drv->status & DCC_PACKET_EXTENDED ) == DCC_PACKET_EXTENDED)
					{
						memcpy((uint8_t *)&DCC_Cutout_Pkt[FIRST_HALF],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
						DCC_Control_Drv->status &= ~DCC_PACKET_EXTENDED;
					}
					memcpy((uint8_t *)&DCC_Pkt[FIRST_HALF],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
					DCC_Control_Drv->status &= ~DCC_PACKET_INPROGRESS;
				}
			}
		}
	}
}

static uint8_t dcc_TIM_PWM_Start_DMA(uint8_t handle)
{
DCC_Control_Drv_TypeDef	*DCC_Control_Drv = (DCC_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;

	if ( DCC_Control_Drv == NULL )
		return 1;

	if ( HAL_TIM_PWM_Start(DCC_Control_Drv->dcc_timer, DCC_Control_Drv[handle].timer_dcc_channel) )
		return 1;
#ifdef DCC_TIMER_DUAL_PHASE
	if ( HAL_TIMEx_PWMN_Start(DCC_Control_Drv->dcc_timer, DCC_Control_Drv[handle].timer_180phase_dcc_channel) )
		return 1;
#endif

	if ( HAL_TIM_PWM_Start(DCC_Control_Drv->dcc_timer, DCC_Control_Drv[handle].timer_cutout_channel) )
		return 1;

	DCC_Control_Drv->dcc_timer->hdma[DCC_Control_Drv->dma_dcc_index]->XferCpltCallback 		= dcc_TIM_DMADelayPulseCplt;
	DCC_Control_Drv->dcc_timer->hdma[DCC_Control_Drv->dma_dcc_index]->XferHalfCpltCallback 	= dcc_TIM_DMADelayPulseHalfCplt;

    if (HAL_DMA_Start_IT(DCC_Control_Drv->dcc_timer->hdma[DCC_Control_Drv->dma_dcc_index]   , (uint32_t )&DCC_Pkt[0],    (uint32_t)&DCC_Control_Drv->dcc_timer->Instance->PSC,sizeof(DCC_Pkt)/2) != HAL_OK)
      return 1;

    if (HAL_DMA_Start_IT(DCC_Control_Drv->dcc_timer->hdma[DCC_Control_Drv->dma_cutout_index], (uint32_t )&DCC_Cutout_Pkt[0], (uint32_t)&DCC_Control_Drv->dcc_timer->Instance->CCR4,sizeof(DCC_Cutout_Pkt)/2) != HAL_OK)
      return 1;
    DCC_Control_Drv->dcc_timer->Instance->DIER = DCC_Control_Drv->dma_dcc_value | DCC_Control_Drv->dma_cutout_value;
	return 0;
}

static void encode_byte(uint16_t *dest , uint8_t value)
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

static void compile_reset_packet(uint8_t handle)
{
DCC_Control_Drv_TypeDef	*DCC_Control_Drv = (DCC_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;

	memcpy((uint8_t *)&DCC_WorkPkt,(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
	memcpy((uint8_t *)&DCC_Cutout_Pkt,(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
	DCC_Control_Drv->status &= ~DCC_PACKET_EXTENDED;
	encode_byte((uint16_t *)&DCC_WorkPkt.address,0);
	encode_byte((uint16_t *)&DCC_WorkPkt.instruction,0);
	encode_byte((uint16_t *)&DCC_WorkPkt.detection_short,0);
}

static uint8_t one_byte_commands(uint8_t handle,char cmd)
{
uint8_t	ret_val = 0;
DCC_Control_Drv_TypeDef	*DCC_Control_Drv = (DCC_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;

	switch ( cmd)
	{
	case 'A' 	:
		DCC_Control_Drv->status |= DCC_ON;
		break;
	case 'a' 	:
		DCC_Control_Drv->status &= ~DCC_ON;
		break;
	case 'R' 	:
		compile_reset_packet(handle);
		break;
	default:		ret_val = 1;
	}
	return ret_val;
}

uint8_t four_bytes_commands(uint8_t handle,char cmd,uint16_t track,uint16_t address,uint16_t data)
{
uint8_t ecc;
DCC_Control_Drv_TypeDef	*DCC_Control_Drv = (DCC_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;

	if ( cmd == 'T' )
	{
		ecc = address ^ data;
		memcpy((uint8_t *)&DCC_WorkPkt,(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
		memcpy((uint8_t *)&DCC_Cutout_Pkt,(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
		encode_byte((uint16_t *)&DCC_WorkPkt.address,address);
		encode_byte((uint16_t *)&DCC_WorkPkt.instruction,data);
		encode_byte((uint16_t *)&DCC_WorkPkt.detection_short,ecc);
		DCC_Control_Drv->status &= ~DCC_PACKET_EXTENDED;
		return 0;
	}
	return 1;
}

static uint8_t five_bytes_commands(uint8_t handle,char cmd,uint16_t track,uint16_t address,uint16_t datal,uint16_t datah)
{
uint8_t ecc;
DCC_Control_Drv_TypeDef	*DCC_Control_Drv = (DCC_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;

	if ( cmd == 'T' )
	{
		ecc = address ^ datal ^ datah;
		memcpy((uint8_t *)&DCC_WorkPkt,(uint8_t *)&DCC_ExtendedIdle_Pkt,sizeof(DCC_ExtendedIdle_Pkt));
		memcpy((uint8_t *)&DCC_Cutout_Pkt,(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
		encode_byte((uint16_t *)&DCC_WorkPkt.address,address);
		encode_byte((uint16_t *)&DCC_WorkPkt.instruction,datal);
		encode_byte((uint16_t *)&DCC_WorkPkt.detection_short,datah);
		encode_byte((uint16_t *)&DCC_WorkPkt.detection_long,ecc);
		DCC_Control_Drv->status |= ~DCC_PACKET_EXTENDED;
		return 0;
	}
	return 1;
}

static uint32_t dcc_start(uint8_t handle)
{
	dcc_TIM_PWM_Start_DMA(handle);
	return 0;
}

static uint32_t dcc_stop(uint8_t handle)
{
DCC_Control_Drv_TypeDef	*DCC_Control_Drv = (DCC_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	HAL_TIM_PWM_Stop_DMA(DCC_Control_Drv->dcc_timer, DCC_Control_Drv[handle].timer_dcc_channel);
#ifdef DCC_TIMER_DUAL_PHASE
	HAL_TIMEx_PWMN_Stop_DMA(DCC_Control_Drv->dcc_timer, DCC_Control_Drv[handle].timer_180phase_dcc_channel);
#endif
	HAL_TIM_PWM_Stop_DMA(DCC_Control_Drv->dcc_timer, DCC_Control_Drv[handle].timer_cutout_channel);
	return 0;
}

static uint32_t dcc_get_status(uint8_t handle)
{
	return 0;
}

static uint32_t dcc_get_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}


static uint32_t dcc_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
DCC_Control_Drv_TypeDef	*DCC_Control_Drv = (DCC_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
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
	DCC_Control_Drv->repetition_counter = DCC_Control_Drv->repetition;
	DCC_Control_Drv->status |= (DCC_PACKET_PENDING | DCC_PACKET_INPROGRESS);
	return 0;
}

static uint32_t dcc_extended_actions(uint32_t handle,uint32_t *action)
{
	return 0;
}

extern	DriverStruct_t	dcc_Drv;

uint32_t dcc_deinit(uint8_t handle)
{
DCC_Control_Drv_TypeDef	*DCC_Control_Drv = (DCC_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	DCC_Control_Drv->status = 0;
	return driver_unregister((DriverStruct_t *)&DriverStruct[handle]);
}

static uint32_t dcc_init(uint8_t handle)
{
DCC_Control_Drv_TypeDef	*DCC_Control_Drv = (DCC_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;

	memcpy((uint8_t *)&DCC_Pkt[0],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
	memcpy((uint8_t *)&DCC_Pkt[1],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
	memcpy((uint8_t *)&DCC_Cutout_Pkt[0],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
	memcpy((uint8_t *)&DCC_Cutout_Pkt[1],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
	DCC_Control_Drv->status = DCC_INITIALIZED;
	DCC_Control_Drv->handle = handle;

	switch(DCC_Control_Drv->timer_dcc_channel)
	{
	case TIM_CHANNEL_1 : DCC_Control_Drv->dma_dcc_value = TIM_DMA_CC1; DCC_Control_Drv->dma_dcc_index = TIM_DMA_ID_CC1;break;
	case TIM_CHANNEL_2 : DCC_Control_Drv->dma_dcc_value = TIM_DMA_CC2; DCC_Control_Drv->dma_dcc_index = TIM_DMA_ID_CC2;break;
	case TIM_CHANNEL_3 : DCC_Control_Drv->dma_dcc_value = TIM_DMA_CC3; DCC_Control_Drv->dma_dcc_index = TIM_DMA_ID_CC3;break;
	case TIM_CHANNEL_4 : DCC_Control_Drv->dma_dcc_value = TIM_DMA_CC4; DCC_Control_Drv->dma_dcc_index = TIM_DMA_ID_CC4;break;
	default : return 1;
	}
	switch(DCC_Control_Drv->timer_cutout_channel)
	{
	case TIM_CHANNEL_1 : DCC_Control_Drv->dma_cutout_value = TIM_DMA_CC1; DCC_Control_Drv->dma_cutout_index = TIM_DMA_ID_CC1;break;
	case TIM_CHANNEL_2 : DCC_Control_Drv->dma_cutout_value = TIM_DMA_CC2; DCC_Control_Drv->dma_cutout_index = TIM_DMA_ID_CC2;break;
	case TIM_CHANNEL_3 : DCC_Control_Drv->dma_cutout_value = TIM_DMA_CC3; DCC_Control_Drv->dma_cutout_index = TIM_DMA_ID_CC3;break;
	case TIM_CHANNEL_4 : DCC_Control_Drv->dma_cutout_value = TIM_DMA_CC4; DCC_Control_Drv->dma_cutout_index = TIM_DMA_ID_CC4;break;
	default : return 1;
	}
	DCC_Control_Drv->hdma[0] = DCC_Control_Drv->dcc_timer->hdma[DCC_Control_Drv->dma_dcc_index];
	DCC_Control_Drv->hdma[1] = DCC_Control_Drv->dcc_timer->hdma[DCC_Control_Drv->dma_cutout_index];
	DCC_Control_Drv->repetition_counter = DCC_Control_Drv->repetition = 0;
	DCC_Control_Drv->repetition = 5;
	return 0;
}

DriverStruct_t	DCC_Controller =
{
	.init = dcc_init,
	.deinit = dcc_deinit,
	.start = dcc_start,
	.stop = dcc_stop,
	.extended_action = dcc_extended_actions,
	.get_status = dcc_get_status,
	.get_values = dcc_get_values,
	.set_values = dcc_set_values,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.driver_name = "DCC",
};

uint32_t dcc_allocate_driver(DriverStruct_t *new_struct)
{
	memcpy(new_struct,&DCC_Controller,sizeof(DCC_Controller));
	return 0;
}
#endif // #ifdef DCC_SYSTEM_ENABLE


/*
 * dcc.h
 *
 *  Created on: Mar 5, 2024
 *      Author: fil
 */

#ifndef MODULES_DCC_DCC_H_
#define MODULES_DCC_DCC_H_

#include "../../kernel/system_default.h"

#ifdef DDC_SYSTEM_ENABLE

#define	DCC_PULSE58uS			58
#define	DCC_0					(DCC_PULSE58uS*2)
#define	DCC_1					DCC_PULSE58uS
#define	DCC_START_BIT			DCC_0
#define	DCC_END_BIT				DCC_1
#define	DCC_CUTOUT				(DCC_PULSE58uS*8)
#define	DCC_CTF_0				0
#define	DCC_CTF_1				160

#define	DCC_PKT_STD				0
#define	DCC_PKT_EXTENDED		1

typedef struct {
	uint16_t		preamble[16];
	uint16_t		packet_start_bit;
	uint16_t		address[8];
	uint16_t		data_byte_start0_bit;
	uint16_t		instruction[8];
	uint16_t		data_byte_start1_bit;
	uint16_t		detection_short[8];
	uint16_t		packet_end_bit;
	uint16_t		detection_long[8];
	uint16_t		long_packet_end_bit;
	uint16_t		endpacket_long;
}DCC_BasePkt_TypeDef;

typedef struct {
	TIM_HandleTypeDef 	*hwtimer_handle;
	DMA_HandleTypeDef 	*dma_dcc_handle;
	DMA_HandleTypeDef 	*dma_cutout_handle;
	uint32_t			hwtimer_id;
	uint32_t			dcc_channel;
	uint32_t			cutout_channel;
	uint8_t				owner;
	uint8_t				dcc_system;
	uint8_t				repetition_counter,wrk_repetition_counter;
	uint32_t			cmd_sent;
}DCC_Config_TypeDef;

/* dcc_system */
#define	SYSTEM_DCC_HALFDONE		0x80
#define	SYSTEM_DCC_DONE			0x40
#define	SYSTEM_DCC_EXT_CUTOUT	0x20
#define	SYSTEM_DCC_CLEAR		0x10
#define	SYSTEM_DCC_POWER		0x08
#define	SYSTEM_DCC_REP_ACTIVE	0x04
#define	SYSTEM_DCC_WORK_PH		0x02
#define	SYSTEM_DCC_WORK_PF		0x01

#define	DCC_TRACK_1				0
#define	DCC_TRACK_2				1

/* errors definition */
#define	PARAMETER_ERROR			"#0001"
#define	TRACK_NOT_POWERED_ERROR	"#0002"

extern	uint8_t dcc_init(uint32_t hwtimer_id , uint32_t track, uint32_t dcc_channel, uint32_t tx_number , uint32_t cutout_channel,DMA_HandleTypeDef *dma_dcc_handle,DMA_HandleTypeDef *dma_cutout_handle);
extern	uint8_t dcc_start(uint8_t track);
extern	uint8_t one_byte_commands(char cmd,uint8_t *reply_buf);
extern	uint8_t two_bytes_commands(char cmd,uint16_t track,uint16_t address,uint16_t data,uint8_t *reply_buf);
extern	uint8_t three_bytes_commands(char cmd,uint16_t track,uint16_t address,uint16_t datal,uint16_t datah,uint8_t *reply_buf);
extern	uint8_t dcc_enable_output(uint32_t track);
extern	uint8_t dcc_disable_output(uint32_t track);



#endif // #ifdef DDC_SYSTEM_ENABLE

#endif /* MODULES_DCC_DCC_H_ */

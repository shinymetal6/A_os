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
 * dcc_parse_command.c
 *
 *  Created on: Mar 1, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef DDC_SYSTEM

#include "dcc.h"
#include "dcc_parse_command.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

uint8_t compile_reset_packet(void)
{
	if ( ((DCC_System.dcc_flags & DCC_DCC1_POWER) == 0 ) && ((DCC_System.dcc_flags & DCC_DCC2_POWER) == 0 ))
			return 1;
	encode_byte((uint16_t *)&DCC_Work_Pkt.address,0);
	encode_byte((uint16_t *)&DCC_Work_Pkt.data,0);
	encode_byte((uint16_t *)&DCC_Work_Pkt.ecc,0);
	DCC_System.dcc_flags |= DCC_DCC1_PKTPEND;
	DCC_System.dcc_flags |= DCC_DCC2_PKTPEND;
	return 0;
}

uint8_t	temp_string[DCC_HOST_MAX_LEN];

uint8_t one_byte_commands(char cmd,uint8_t *reply_buf)
{
uint8_t	ret_val = 0;

	switch ( cmd)
	{
	case 'A' 	: 	sprintf((char *)reply_buf,"Track 0 Main On\n\r");
					DCC_System.dcc_flags |= DCC_DCC1_POWER;
					break;
	case 'a' 	: 	sprintf((char *)reply_buf,"Track 0 Main Off\n\r");
					DCC_System.dcc_flags &= ~DCC_DCC1_POWER;
					break;
	case 'B' 	: 	sprintf((char *)reply_buf,"Track 1 Main On\n\r");
					DCC_System.dcc_flags |= DCC_DCC2_POWER;
					break;
	case 'b' 	: 	sprintf((char *)reply_buf,"Track 1 Main Off\n\r");
					DCC_System.dcc_flags &= ~DCC_DCC2_POWER;
					break;
	case 'x' 	: 	sprintf((char *)reply_buf,"All Off\n\r");
					DCC_System.dcc_flags &= ~DCC_DCC1_POWER;
					DCC_System.dcc_flags &= ~DCC_DCC2_POWER;
					break;
	case 'X' 	: 	sprintf((char *)reply_buf,"All On\n\r");
					DCC_System.dcc_flags |= DCC_DCC1_POWER;
					DCC_System.dcc_flags |= DCC_DCC2_POWER;
					break;
	case 'R' 	: 	ret_val = compile_reset_packet();
					if ( ret_val )
						sprintf((char *)reply_buf,"RESET failed, no powered track found\n\r");
					else
					{
						sprintf((char *)reply_buf,"Sent RESET\n\r");
						DCC_System.dcc_flags |= DCC_DCC1_RESET;
						DCC_System.dcc_flags |= DCC_DCC2_RESET;
					}
					ret_val = 0;
					break;
	case 'S' 	: 	sprintf((char *)reply_buf,"Status\n\r");
					if ((DCC_System.dcc_flags & DCC_DCC1_POWER) == DCC_DCC1_POWER)
						strcat((char *)reply_buf,"Track0 ON\n\r");
					else
						strcat((char *)reply_buf,"Track0 OFF\n\r");
					if ((DCC_System.dcc_flags & DCC_DCC2_POWER) == DCC_DCC2_POWER)
						strcat((char *)reply_buf,"Track1 ON\n\r");
					else
						strcat((char *)reply_buf,"Track1 OFF\n\r");
					sprintf((char *)temp_string,"Repetition ch1 %d ch2 %d\n\r",DCC_System.dcc_ch1_repeat_number+1,DCC_System.dcc_ch2_repeat_number+1);
					strcat((char *)reply_buf,(char *)temp_string);
					break;
	default:		ret_val = 1;
	}
	return ret_val;
}

uint8_t two_bytes_commands(char cmd,int track,int address,int data,uint8_t *reply_buf)
{
uint8_t ecc;
	if ( cmd == 'T' )
	{
		ecc = address ^ data;
		encode_byte((uint16_t *)&DCC_Work_Pkt.address,address);
		encode_byte((uint16_t *)&DCC_Work_Pkt.data,data);
		encode_byte((uint16_t *)&DCC_Work_Pkt.ecc,ecc);
		encode_byte((uint16_t *)&DCC_Work_Pkt.fill,0xff);

		if ( track == 1 )
		{
			if ((DCC_System.dcc_flags & DCC_DCC1_POWER) == DCC_DCC1_POWER )
				DCC_System.dcc_flags |= DCC_DCC1_PKTPEND;
		}
		if ( track == 2 )
		{
			if ((DCC_System.dcc_flags & DCC_DCC1_POWER) == DCC_DCC1_POWER )
				DCC_System.dcc_flags |= DCC_DCC2_PKTPEND;
		}
		if ( ((DCC_System.dcc_flags & DCC_DCC1_PKTPEND) == DCC_DCC1_PKTPEND ) || ((DCC_System.dcc_flags & DCC_DCC2_PKTPEND) == DCC_DCC2_PKTPEND ) )
		{
			sprintf((char *)reply_buf,"track %d address %d data %d\n\r",track,address,data);
			return 0;
		}
		else
			sprintf((char *)reply_buf,"%s Command failed, track %d is off\n\r",TRACK_NOT_POWERED_ERROR,track);
	}
	return 1;
}

uint8_t three_bytes_commands(char cmd,int track,int address,int datal,int datah,uint8_t *reply_buf)
{
uint8_t ecc;
	if ( cmd == 'T' )
	{
		ecc = address ^ datal ^ datah;
		encode_byte((uint16_t *)&DCC_Work_Pkt.address,address);
		encode_byte((uint16_t *)&DCC_Work_Pkt.data,datal);
		encode_byte((uint16_t *)&DCC_Work_Pkt.ecc,datah);
		encode_byte((uint16_t *)&DCC_Work_Pkt.fill,ecc);
		if ( track == 1 )
		{
			if ((DCC_System.dcc_flags & DCC_DCC1_POWER) == DCC_DCC1_POWER )
				DCC_System.dcc_flags |= DCC_DCC1_PKTPEND;
		}
		if ( track == 2 )
		{
			if ((DCC_System.dcc_flags & DCC_DCC1_POWER) == DCC_DCC1_POWER )
				DCC_System.dcc_flags |= DCC_DCC2_PKTPEND;
		}
		if ( ((DCC_System.dcc_flags & DCC_DCC1_PKTPEND) == DCC_DCC1_PKTPEND ) || ((DCC_System.dcc_flags & DCC_DCC2_PKTPEND) == DCC_DCC2_PKTPEND ) )
			sprintf((char *)reply_buf,"track %d address %d datal %d datah %d\n\r",track,address,datal,datah);
		else
			sprintf((char *)reply_buf,"%s Command failed, track %d is off\n\r",TRACK_NOT_POWERED_ERROR,track);
		return 0;
	}
	return 1;
}

uint8_t dcc_parser(uint8_t *dcc_buffer,uint8_t *dcc_reply)
{
int	p0,p1,p2,p3,p4,pnum;
char cmd;
uint8_t	ret_val = 1;

	pnum = sscanf((char *)dcc_buffer,"%c %d %d %d %d %d",&cmd,&p0,&p1,&p2,&p3,&p4);
	switch (pnum)
	{
	case 1 :	ret_val = one_byte_commands(cmd,dcc_reply);
				break;
	case 4 :	ret_val = two_bytes_commands(cmd,p0,p1,p2,dcc_reply); // <T 1 12 33>
				break;
	case 5 :	ret_val = three_bytes_commands(cmd,p0,p1,p2,p3,dcc_reply); // <T 1 12 33 55>
				break;
	default:	break;// <D 1 12 33 55>
	}
	return ret_val;
}

#endif // #ifdef DDC_SYSTEM


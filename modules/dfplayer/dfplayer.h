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
 * dfplayer.h
 *
 *  Created on: Jul 4, 2026
 *      Author: fil
 */

#ifndef MODULES_DFPLAYER_DFPLAYER_H_
#define MODULES_DFPLAYER_DFPLAYER_H_

#include <stdbool.h>

extern UART_HandleTypeDef huart1; // Change to your actual UART handle

// --- TX Commands ---
#define DF_CMD_NEXT					0x01
#define DF_CMD_PREV					0x02
#define DF_CMD_PLAY_TRACK			0x03
#define DF_CMD_SET_VOL				0x06
#define DF_CMD_PLAY					0x0D
#define DF_CMD_PAUSE				0x0E
#define DF_CMD_PLAY_FOLDER			0x0F
#define DF_CMD_PLAY_ALARM_FOLDER	0x13
#define DF_CMD_PLAY_DNOTIFY_FOLDER	0x14
#define DF_CMD_STOP					0x16
#define DF_CMD_QUERY_STATUS			0x42

#define DF_CMD_QUERY_TRACK			0x4C
#define DF_CMD_QUERY_VOLUME			0x4D
#define DF_CMD_QUERY_EQ				0x4E
#define DF_CMD_QUERY_MODE			0x4F
#define DF_CMD_QUERY_DEVICE			0x3F
#define DF_CMD_QUERY_SD_TOTAL		0x52

// --- RX Response Codes ---
#define DF_HEADER				0x7e
#define DF_RESP_TRACK_FINISHED  0x3D
#define DF_RESP_INIT_FINISHED   0x3F
#define DF_RESP_ERROR           0x40
#define DF_RESP_SD_INSERTED     0x3C

// Struct to hold parsed response data
typedef struct {
    uint8_t command;
    uint16_t parameter;
    bool is_valid;
} DFPlayer_Response_t;

typedef struct {
    uint8_t header;
    uint8_t versionh;
    uint8_t versionl;
    uint8_t event;
    uint8_t feedback;
    uint16_t parameter;
    uint8_t checksumh;
    uint8_t checksuml;
    uint8_t footer;
} DFPlayer_Reply_t;

// Function Prototypes
void DFPlayer_Init(UART_DriverStruct_t *uart_drv);
void DFPlayer_SendCommand(UART_DriverStruct_t *uart_drv,uint8_t cmd, uint16_t param);

// High-Level API
void DFPlayer_PlayTrack(UART_DriverStruct_t *uart_drv,uint16_t track_num);
void DFPlayer_SetVolume(UART_DriverStruct_t *uart_drv,uint8_t volume);
void DFPlayer_Play(UART_DriverStruct_t *uart_drv);
void DFPlayer_PlayFolderFile(UART_DriverStruct_t *uart_drv,uint8_t folder_num, uint8_t file_num);

void DFPlayer_Pause(UART_DriverStruct_t *uart_drv);
void DFPlayer_QueryStatus(UART_DriverStruct_t *uart_drv);

void DFPlayer_ProcessResponse(UART_DriverStruct_t *uart_drv);

#endif /* MODULES_DFPLAYER_DFPLAYER_H_ */

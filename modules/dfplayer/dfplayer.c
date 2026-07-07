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
 * dfplayer.c
 *
 *  Created on: Jul 4, 2026
 *      Author: fil
 */

#include "main.h"

#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "dfplayer.h"
#include <string.h>

// Calculate checksum (2's complement of sum of bytes 1 to 6)
static uint16_t DFPlayer_CalcChecksum(uint8_t *data) {
    uint16_t sum = 0;
    for (int i = 1; i <= 6; i++) {
        sum += data[i];
    }
    return -sum;
}

// Send command (Updated to allow enabling feedback)
uint8_t DFPlayer_buf[10];
void DFPlayer_SendCommand(UART_DriverStruct_t *uart_drv,uint8_t cmd, uint16_t param)
{

	DFPlayer_buf[0] = 0x7E; // Start
	DFPlayer_buf[1] = 0xFF; // Version
	DFPlayer_buf[2] = 0x06; // Length
	DFPlayer_buf[3] = cmd;  // Command
	DFPlayer_buf[4] = 0x01; // Feedback enable
	DFPlayer_buf[5] = (param >> 8) & 0xFF; // Param MSB
	DFPlayer_buf[6] = param & 0xFF;        // Param LSB

    uint16_t checksum = DFPlayer_CalcChecksum(DFPlayer_buf);
    DFPlayer_buf[7] = (checksum >> 8) & 0xFF;
    DFPlayer_buf[8] = checksum & 0xFF;
    DFPlayer_buf[9] = 0xEF; // End

    uart_send(uart_drv, DFPlayer_buf, 10);
    //HAL_Delay(100); // Processing delay
}

void DFPlayer_Init(UART_DriverStruct_t *uart_drv)
{
	uart_start_receive(uart_drv);
    DFPlayer_SendCommand(uart_drv,0x3F, 0);
}

// --- High-Level API ---
void DFPlayer_PlayTrack(UART_DriverStruct_t *uart_drv,uint16_t track_num)
{
    DFPlayer_SendCommand(uart_drv,DF_CMD_PLAY_TRACK, track_num); // Enable feedback to know when it finishes
}

void DFPlayer_SetVolume(UART_DriverStruct_t *uart_drv,uint8_t volume)
{
    if (volume > 30) volume = 30;
    DFPlayer_SendCommand(uart_drv,DF_CMD_SET_VOL, volume); // No need for feedback on volume
}

void DFPlayer_Play(UART_DriverStruct_t *uart_drv)
{
    DFPlayer_SendCommand(uart_drv,DF_CMD_PLAY, 0);
}

void DFPlayer_Pause(UART_DriverStruct_t *uart_drv)
{
    DFPlayer_SendCommand(uart_drv,DF_CMD_PAUSE, 0);
}

void DFPlayer_PlayFolderFile(UART_DriverStruct_t *uart_drv,uint8_t folder_num, uint8_t file_num)
{
    uint16_t param = ((uint16_t)folder_num << 8) | file_num;
    DFPlayer_SendCommand(uart_drv,DF_CMD_PLAY_FOLDER, param);
}

void DFPlayer_QueryStatus(UART_DriverStruct_t *uart_drv)
{
    // Notice the 'true' at the end! This enables feedback so the DFPlayer replies.
    DFPlayer_SendCommand(uart_drv,DF_CMD_QUERY_STATUS, 0x00);
}

void DFPlayer_QueryCurrentTrack(UART_DriverStruct_t *uart_drv)
{
    DFPlayer_SendCommand(uart_drv,DF_CMD_QUERY_TRACK, 0x00);
}

void DFPlayer_QueryVolume(UART_DriverStruct_t *uart_drv)
{
    DFPlayer_SendCommand(uart_drv,DF_CMD_QUERY_VOLUME, 0x00);
}

void DFPlayer_QuerySDTotalTracks(UART_DriverStruct_t *uart_drv)
{
    DFPlayer_SendCommand(uart_drv,DF_CMD_QUERY_SD_TOTAL, 0x00);
}

void DFPlayer_ProcessResponse(UART_DriverStruct_t *uart_drv)
{
	DFPlayer_Reply_t	*reply = (DFPlayer_Reply_t *)uart_drv->data;
	if ( reply->header != DF_HEADER)
		return;
    uint16_t calc_checksum = DFPlayer_CalcChecksum(uart_drv->data);
    uint16_t recv_checksum = (uart_drv->data[7] << 8) | uart_drv->data[8];

    if (calc_checksum == recv_checksum)
    {
    	reply->event = uart_drv->data[3];
    	reply->parameter = (uart_drv->data[5] << 8) | uart_drv->data[6];
    }

    switch (reply->event) {
        case DF_RESP_INIT_FINISHED: // 0x3F
            // DFPlayer has finished initializing and detected the media.
            // Parameter: 1=USB, 2=SD, 3=Flash
            if (reply->parameter == 2) {
                // SD Card detected!
                DFPlayer_SetVolume(uart_drv,20);       // Set volume to 20 (out of 30)
                HAL_Delay(100);               // Small delay between commands
                DFPlayer_PlayTrack(uart_drv,1);        // Start playing track 0001.mp3
            }
            break;

        case DF_RESP_TRACK_FINISHED: // 0x3D
            // A track just finished playing.
            // Parameter: The track number that just finished.

            // Visual feedback: Toggle the LED when a track finishes

            // Optional: Automatically play the next track
            // DFPlayer_PlayTrack(resp->parameter + 1);
            break;

        case DF_RESP_ERROR: // 0x40
            // An error occurred (e.g., SD card removed, file not found)
            // Parameter: Error code (see DFPlayer datasheet for error codes)

            // Visual feedback: Rapidly blink LED to indicate error
            for (int i = 0; i < 5; i++) {
                HAL_Delay(100);
            }
            break;

        case DF_RESP_SD_INSERTED: // 0x3C
            // SD card was hot-swapped (inserted while powered on)
            DFPlayer_PlayTrack(uart_drv,1);
            break;

        default:
            // Other responses (like volume query replies, etc.)
            break;
    }
}

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
 * usbd_video_if.c
 *
 *  Created on: Mar 2, 2026
 *      Author: fil
 */
/*
 * usbd_video_if.c
 * UVC Interface for External Video Source on STM32H7
 * Resolution: 320x240 @ 30fps, YUY2 format
 *
 * ★★★ FIXED: UVC Header byte layout corrected ★★★
 */

#include "main.h"
#ifdef	STM32H743xx
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef	USB_DEVICE_ENABLED

#include "usbd_video_if.h"
#include <string.h>
#include <stdlib.h>

/* External USB Handle */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* ============================================================================
 * FRAME BUFFER CONFIGURATION
 * ============================================================================ */
#define FRAME_WIDTH         320U
#define FRAME_HEIGHT        240U
#define FRAME_SIZE_YUY2     (FRAME_WIDTH * FRAME_HEIGHT * 2U)  /* 153,600 bytes */
#define PAYLOAD_SIZE        (UVC_ISO_FS_MPS - 2U)              /* 510 bytes for FS */
#define PACKETS_PER_FRAME   ((FRAME_SIZE_YUY2 + PAYLOAD_SIZE - 1) / PAYLOAD_SIZE)

/* ============================================================================
 * PRIVATE VARIABLES
 * ============================================================================ */
static uint8_t *FrameBuffer = NULL;
static uint8_t  StreamingActive = 0U;
static uint32_t FrameCount = 0U;
static uint16_t PacketIndex = 0U;
static uint8_t  FrameID = 0U;

/* UVC Header buffer: 2 bytes header + max payload */
static uint8_t UVC_Header[1024] __attribute__((aligned(4)));

/* ============================================================================
 * PRIVATE FUNCTION PROTOTYPES
 * ============================================================================ */
static uint8_t VIDEO_Itf_Init(void);
static uint8_t VIDEO_Itf_DeInit(void);
static uint8_t VIDEO_Itf_Start(void);
static uint8_t VIDEO_Itf_Stop(void);
static uint8_t VIDEO_Itf_Control(USBD_VideoControlTypeDef *pctrl);
static uint8_t VIDEO_Itf_Data(uint8_t** pbuf, uint16_t* psize, uint16_t* pcktidx);

/* ============================================================================
 * INTERFACE STRUCTURE
 * ============================================================================ */
USBD_VIDEO_ItfTypeDef USBD_VIDEO_Interface_fops_FS =
{
    .Init = VIDEO_Itf_Init,
    .DeInit = VIDEO_Itf_DeInit,
    .Start = VIDEO_Itf_Start,
    .Stop = VIDEO_Itf_Stop,
    .Control = VIDEO_Itf_Control,
    .Data = VIDEO_Itf_Data,
};

/* ============================================================================
 * INTERFACE IMPLEMENTATION
 * ============================================================================ */

/**
 * @brief  Initialize the video interface
 */
static uint8_t VIDEO_Itf_Init(void)
{
    /* Reset state */
    StreamingActive = 0U;
    FrameCount = 0U;
    PacketIndex = 0U;
    FrameID = 0U;
    FrameBuffer = NULL;  /* Will be set by VIDEO_Itf_SetPtr() */

    /* Clear UVC header buffer */
    memset(UVC_Header, 0x00, sizeof(UVC_Header));

    return USBD_OK;
}

/**
 * @brief  DeInitialize the video interface
 */
static uint8_t VIDEO_Itf_DeInit(void)
{
    StreamingActive = 0U;
    FrameBuffer = NULL;  /* Don't free - owned by external source */
    return USBD_OK;
}

/**
 * @brief  Start streaming - CRITICAL: Reset all counters!
 */
static uint8_t VIDEO_Itf_Start(void)
{
    StreamingActive = 1U;
    FrameCount = 0U;
    PacketIndex = 0U;
    FrameID = 0U;

    /* Clear UVC header buffer */
    memset(UVC_Header, 0x00, sizeof(UVC_Header));

    return USBD_OK;
}

/**
 * @brief  Stop streaming
 */
static uint8_t VIDEO_Itf_Stop(void)
{
    StreamingActive = 0U;
    PacketIndex = 0U;
    return USBD_OK;
}

/**
 * @brief  Handle UVC control requests (Probe/Commit)
 */
static uint8_t VIDEO_Itf_Control(USBD_VideoControlTypeDef *pctrl)
{
    if (pctrl == NULL) {
        return USBD_FAIL;
    }

    /* Host committed a format - external source should adapt */
    /* For now, we use 320x240 YUY2 */

    return USBD_OK;
}

/**
 * @brief  Get next video packet - THIS IS THE CALLBACK
 *         ★★★ CRITICAL: Returns complete buffer with header + payload ★★★
 *
 * UVC Header Format (2 bytes):
 *   Byte 0: Bit 0 = FrameID, Bit 1 = EOF, Bit 2 = EOI, Bits 3-7 = Reserved
 *   Byte 1: Payload Header (0x00 for YUY2 uncompressed)
 */
static uint8_t VIDEO_Itf_Data(uint8_t** pbuf, uint16_t* psize, uint16_t* pcktidx)
{
    if (!StreamingActive || FrameBuffer == NULL) {
        *pbuf = NULL;
        *psize = 0;
        *pcktidx = 0;
        return USBD_OK;
    }

    /* ★★★ Toggle Frame ID at start of each new frame ★★★ */
    if (PacketIndex == 0) {
        FrameCount++;
        FrameID ^= 0x01U;  /* Toggle: 0→1→0→1... */
    }

    /* Calculate packet offset and size */
    uint32_t offset = (uint32_t)PacketIndex * PAYLOAD_SIZE;
    uint16_t remaining = FRAME_SIZE_YUY2 - offset;
    uint16_t payload_len = (remaining > PAYLOAD_SIZE) ? PAYLOAD_SIZE : remaining;

    /* ★★★ BUILD UVC HEADER CORRECTLY ★★★ */
    /* Byte 0: FrameID in bit 0, EOF/EOI will be OR'd for last packet */
    UVC_Header[0] = FrameID;
    /* Byte 1: Payload header (0x00 for YUY2 uncompressed format) */
    UVC_Header[1] = 0x00U;

    /* Set EOF/EOI on last packet ONLY */
    if (remaining <= PAYLOAD_SIZE) {
        UVC_Header[0] |= 0x02U;   /* EOF - Bit 1 */
        UVC_Header[0] |= 0x04U;   /* EOI - Bit 2 */
    }

    /* Copy frame data to transmit buffer (after UVC header) */
    memcpy(&UVC_Header[2], FrameBuffer + offset, payload_len);

    /* Return buffer and size */
    *pbuf = UVC_Header;
    *psize = payload_len + 2U;
    *pcktidx = PacketIndex + 1U;

    /* ★★★ Increment packet index, wrap at end of frame ★★★ */
    PacketIndex++;
    if (offset + payload_len >= FRAME_SIZE_YUY2) {
        PacketIndex = 0U;  /* Reset for next frame */
    }

    return USBD_OK;
}

/* ============================================================================
 * EXTERNAL API FUNCTIONS
 * ============================================================================ */

/**
 * @brief  Set the frame buffer pointer (called by external video source)
 * @param  data_ptr: Pointer to frame buffer (YUY2 format, 320x240)
 * @param  data_len: Length of frame data (should be 153,600 bytes)
 */
void VIDEO_Itf_SetPtr(uint8_t *data_ptr, uint32_t data_len)
{
    if (data_ptr != NULL && data_len >= FRAME_SIZE_YUY2) {
        FrameBuffer = data_ptr;
    }
}

/**
 * @brief  Start streaming (called by external video source)
 */
void VIDEO_Itf_StartStreaming(void)
{
    VIDEO_Itf_Start();
}

/**
 * @brief  Stop streaming (called by external video source)
 */
void VIDEO_Itf_StopStreaming(void)
{
    VIDEO_Itf_Stop();
}

/**
 * @brief  Check if streaming is active
 * @retval 1 if streaming, 0 if stopped
 */
uint8_t VIDEO_Itf_IsStreaming(void)
{
    return StreamingActive;
}

#endif /* USB_DEVICE_ENABLED */
#endif /* STM32H743xx */

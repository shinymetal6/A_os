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
 * FIXED: Test Pattern Generator for UVC on STM32H7
 * Resolution: 320x240 @ 30fps, YUY2 format
 *
 * ★★★ CRITICAL FIX: UVC_Header buffer size ★★★
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

/* ★★★ FIX: UVC_Header must be large enough for header + max payload ★★★ */
/* 2 bytes header + 1022 bytes max payload (HS) or 510 bytes (FS) */
static uint8_t UVC_Header[1024] __attribute__((aligned(4)));  /* ★★★ FIX: Was [2], now [1024] ★★★ */

/* Moving box animation */
static int16_t BoxX = 0, BoxY = 0, BoxDX = 2, BoxDY = 2;
static const uint16_t BoxSize = 40;

/* ============================================================================
 * PRIVATE FUNCTION PROTOTYPES
 * ============================================================================ */
static uint8_t VIDEO_Itf_Init(void);
static uint8_t VIDEO_Itf_DeInit(void);
static uint8_t VIDEO_Itf_Start(void);
static uint8_t VIDEO_Itf_Stop(void);
static uint8_t VIDEO_Itf_Control(USBD_VideoControlTypeDef *pctrl);
static uint8_t VIDEO_Itf_Data(uint8_t** pbuf, uint16_t* psize, uint16_t* pcktidx);

static void GenerateTestPattern(uint8_t *buffer, uint16_t width, uint16_t height);
static void UpdateBoxPosition(void);
static uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b);
static void ConvertRGB565ToYUY2(uint16_t *rgb, uint8_t *yuy2, uint16_t width, uint16_t height);

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
    /* Allocate frame buffer */
    FrameBuffer = (uint8_t *)malloc(FRAME_SIZE_YUY2);
    if (FrameBuffer == NULL) {
        return USBD_FAIL;
    }

    /* Initialize to gray (Y=128, U=128, V=128) */
    memset(FrameBuffer, 0x80, FRAME_SIZE_YUY2);

    /* Reset state */
    StreamingActive = 0U;
    FrameCount = 0U;
    PacketIndex = 0U;
    FrameID = 0U;
    BoxX = 0;
    BoxY = 0;

    /* Clear UVC header buffer */
    memset(UVC_Header, 0x00, sizeof(UVC_Header));

    /* Generate initial frame */
    GenerateTestPattern(FrameBuffer, FRAME_WIDTH, FRAME_HEIGHT);

    return USBD_OK;
}

/**
 * @brief  DeInitialize the video interface
 */
static uint8_t VIDEO_Itf_DeInit(void)
{
    StreamingActive = 0U;
    if (FrameBuffer != NULL) {
        free(FrameBuffer);
        FrameBuffer = NULL;
    }
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
    UVC_Header[0] = 0x02U;
    UVC_Header[1] = 0x00U;

    /* Generate first frame */
    GenerateTestPattern(FrameBuffer, FRAME_WIDTH, FRAME_HEIGHT);

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

    /* Host committed a format - we should adapt (optional for test pattern) */
    /* For now, we always use 320x240 YUY2 */

    return USBD_OK;
}

/**
 * @brief  Get next video packet - THIS IS THE CALLBACK
 *         ★★★ CRITICAL: Returns complete buffer with header + payload ★★★
 */
static uint8_t VIDEO_Itf_Data(uint8_t** pbuf, uint16_t* psize, uint16_t* pcktidx)
{
    if (!StreamingActive || FrameBuffer == NULL) {
        *pbuf = NULL;
        *psize = 0;
        *pcktidx = 0;
        return USBD_OK;
    }

    /* Generate new frame at start of each frame transmission */
    if (PacketIndex == 0) {
        FrameCount++;

        /* Update animation every 2 frames */
        if (FrameCount % 2 == 0) {
            UpdateBoxPosition();
            GenerateTestPattern(FrameBuffer, FRAME_WIDTH, FRAME_HEIGHT);
        }

        /* ★★★ FIX: Toggle Frame ID for each new frame ★★★ */
        //FrameID ^= 0x01U;
        FrameID ++;
        FrameID &= 1;

    }


    /* Calculate packet offset and size */
    uint32_t offset = (uint32_t)PacketIndex * PAYLOAD_SIZE;
    uint16_t remaining = FRAME_SIZE_YUY2 - offset;
    uint16_t payload_len = (remaining > PAYLOAD_SIZE) ? PAYLOAD_SIZE : remaining;

    /* ★★★ FIX: Build UVC header in buffer ★★★ */
    UVC_Header[0] = 0x02U;  /* Normal packet: Bit 1 set */
    UVC_Header[1] = FrameID; /* Frame ID from state (0 or 1) */

    /* Set EOF/EOI on last packet */
    if (remaining <= PAYLOAD_SIZE) {
        UVC_Header[0] |= 0x02U;  /* EOF */
        UVC_Header[0] |= 0x04U;  /* EOI */
    }

    /* ★★★ FIX: Copy frame data to transmit buffer (after UVC header) ★★★ */
    /* This is safe now because UVC_Header is 1024 bytes, not 2 bytes! */
    memcpy(&UVC_Header[2], FrameBuffer + offset, payload_len);

    /* Return buffer and size */
    *pbuf = UVC_Header;
    *psize = payload_len + 2U;  /* 2 bytes UVC header + payload */
    *pcktidx = PacketIndex + 1U;

    /* Increment packet index, wrap at end of frame */
    PacketIndex++;
    if (offset + payload_len >= FRAME_SIZE_YUY2) {
        PacketIndex = 0U;
    }

    return USBD_OK;
}

/* ============================================================================
 * TEST PATTERN GENERATION
 * ============================================================================ */

/**
 * @brief  Generate test pattern with moving box
 */
static void GenerateTestPattern(uint8_t *buffer, uint16_t width, uint16_t height)
{
    uint16_t *rgb_buf = (uint16_t *)malloc(width * height * 2U);
    if (rgb_buf == NULL) return;

    /* Background: blue gradient */
    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            rgb_buf[y * width + x] = RGB565(0, 0, 128);
        }
    }

    /* Draw moving red box */
    for (uint16_t y = BoxY; y < BoxY + BoxSize && y < height; y++) {
        for (uint16_t x = BoxX; x < BoxX + BoxSize && x < width; x++) {
            rgb_buf[y * width + x] = RGB565(255, 0, 0);
        }
    }

    /* Convert to YUY2 */
    ConvertRGB565ToYUY2(rgb_buf, buffer, width, height);
    free(rgb_buf);
}

/**
 * @brief  Update moving box position
 */
static void UpdateBoxPosition(void)
{
    BoxX += BoxDX;
    BoxY += BoxDY;

    if (BoxX <= 0 || BoxX >= (FRAME_WIDTH - BoxSize)) {
        BoxDX = -BoxDX;
    }
    if (BoxY <= 0 || BoxY >= (FRAME_HEIGHT - BoxSize)) {
        BoxDY = -BoxDY;
    }
}

/**
 * @brief  Create RGB565 color
 */
static uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

/**
 * @brief  Convert RGB565 to YUY2
 */
static void ConvertRGB565ToYUY2(uint16_t *rgb, uint8_t *yuy2, uint16_t width, uint16_t height)
{
    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x += 2) {
            uint16_t p0 = rgb[y * width + x];
            uint16_t p1 = (x + 1 < width) ? rgb[y * width + x + 1] : p0;

            uint8_t r0 = (p0 >> 11) & 0x1F; r0 = (r0 << 3) | (r0 >> 2);
            uint8_t g0 = (p0 >> 5) & 0x3F;  g0 = (g0 << 2) | (g0 >> 4);
            uint8_t b0 = p0 & 0x1F;         b0 = (b0 << 3) | (b0 >> 2);

            uint8_t r1 = (p1 >> 11) & 0x1F; r1 = (r1 << 3) | (r1 >> 2);
            uint8_t g1 = (p1 >> 5) & 0x3F;  g1 = (g1 << 2) | (g1 >> 4);
            uint8_t b1 = p1 & 0x1F;         b1 = (b1 << 3) | (b1 >> 2);

            int16_t y0 = (299 * r0 + 587 * g0 + 114 * b0) / 1000;
            int16_t y1 = (299 * r1 + 587 * g1 + 114 * b1) / 1000;
            int16_t u = (-169 * r0 - 331 * g0 + 500 * b0) / 1000 + 128;
            int16_t v = (500 * r0 - 419 * g0 - 81 * b0) / 1000 + 128;

            if (y0 < 0) y0 = 0; if (y0 > 255) y0 = 255;
            if (y1 < 0) y1 = 0; if (y1 > 255) y1 = 255;
            if (u < 0) u = 0;   if (u > 255) u = 255;
            if (v < 0) v = 0;   if (v > 255) v = 255;

            yuy2[(y * width + x) * 2 + 0] = (uint8_t)y0;
            yuy2[(y * width + x) * 2 + 1] = (uint8_t)u;
            yuy2[(y * width + x) * 2 + 2] = (uint8_t)y1;
            yuy2[(y * width + x) * 2 + 3] = (uint8_t)v;
        }
    }
}

void VIDEO_Itf_SetPtr(uint8_t *jpegdata_ptr, uint32_t jpeg_len)
{
    /* Not used for test pattern generator */
    (void)jpegdata_ptr;
    (void)jpeg_len;
}

void VIDEO_Itf_StartStreaming(void)
{
    VIDEO_Itf_Start();
}

#endif /* USB_DEVICE_ENABLED */
#endif /* STM32H743xx */

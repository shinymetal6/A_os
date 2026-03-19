/*
 * uvc_test_pattern_generator.c
 *
 *  Created on: Mar 18, 2026
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_UVCDEVICE

/* ============================================================================
 * TEST PATTERN GENERATION FUNCTIONS
 * ============================================================================ */
#define WIDTH_QVGA              320U
#define HEIGHT_QVGA             240U

static int16_t BoxX = 0;
static int16_t BoxY = 0;
static int16_t BoxDX = 2;
static int16_t BoxDY = 2;
static const uint16_t BoxSize = 40;

void ConvertRGB565ToYUY2(uint16_t *rgb565, uint8_t *yuy2, uint16_t width, uint16_t height)
{
    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x += 2) {
            /* Extract RGB from RGB565 */
            uint16_t pixel0 = rgb565[y * width + x];
            uint16_t pixel1 = (x + 1 < width) ? rgb565[y * width + x + 1] : pixel0;

            uint8_t r0 = (pixel0 >> 11) & 0x1F; r0 = (r0 << 3) | (r0 >> 2);
            uint8_t g0 = (pixel0 >> 5) & 0x3F;  g0 = (g0 << 2) | (g0 >> 4);
            uint8_t b0 = pixel0 & 0x1F;         b0 = (b0 << 3) | (b0 >> 2);

            uint8_t r1 = (pixel1 >> 11) & 0x1F; r1 = (r1 << 3) | (r1 >> 2);
            uint8_t g1 = (pixel1 >> 5) & 0x3F;  g1 = (g1 << 2) | (g1 >> 4);
            uint8_t b1 = pixel1 & 0x1F;         b1 = (b1 << 3) | (b1 >> 2);

            /* RGB to YUV conversion (BT.601) */
            int16_t y0 = (299 * r0 + 587 * g0 + 114 * b0) / 1000;
            int16_t y1 = (299 * r1 + 587 * g1 + 114 * b1) / 1000;
            int16_t u = (-169 * r0 - 331 * g0 + 500 * b0) / 1000 + 128;
            int16_t v = (500 * r0 - 419 * g0 - 81 * b0) / 1000 + 128;

            /* Clamp values */
            if (y0 < 0)
            	y0 = 0;
            if (y0 > 255)
            	y0 = 255;
            if (y1 < 0)
            	y1 = 0;
            if (y1 > 255)
            	y1 = 255;
            if (u < 0)
            	u = 0;
            if (u > 255)
            	u = 255;
            if (v < 0)
            	v = 0;
            if (v > 255)
            	v = 255;

            /* Pack as YUY2: Y0 U Y1 V */
            yuy2[(y * width + x) * 2 + 0] = (uint8_t)y0;
            yuy2[(y * width + x) * 2 + 1] = (uint8_t)u;
            yuy2[(y * width + x) * 2 + 2] = (uint8_t)y1;
            yuy2[(y * width + x) * 2 + 3] = (uint8_t)v;
        }
    }
}

uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

void GenerateColorBars(uint16_t *rgb565_buf,uint16_t *yuv_buf, uint16_t width, uint16_t height)
{
    if (rgb565_buf == NULL)
    	return;

    uint16_t bar_width = width / 8;

    /* 8 color bars: White, Yellow, Cyan, Green, Magenta, Red, Blue, Black */
    const uint16_t colors[8] = {
        RGB565(255, 255, 255),  /* White */
        RGB565(255, 255, 0),    /* Yellow */
        RGB565(0, 255, 255),    /* Cyan */
        RGB565(0, 255, 0),      /* Green */
        RGB565(255, 0, 255),    /* Magenta */
        RGB565(255, 0, 0),      /* Red */
        RGB565(0, 0, 255),      /* Blue */
        RGB565(0, 0, 0)         /* Black */
    };

    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            uint8_t bar_index = x / bar_width;
            if (bar_index > 7) bar_index = 7;
            rgb565_buf[y * width + x] = colors[bar_index];
        }
    }

    /* Convert to YUY2 */
    ConvertRGB565ToYUY2(rgb565_buf, (uint8_t * )yuv_buf, width, height);
}

/**
 * @brief  Generate horizontal gradient pattern
 */
void GenerateGradientH(uint16_t *rgb565_buf,uint16_t *yuv_buf, uint16_t width, uint16_t height)
{
    if (rgb565_buf == NULL) return;

    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            uint8_t intensity = (x * 255) / width;
            rgb565_buf[y * width + x] = RGB565(intensity, intensity, intensity);
        }
    }

    ConvertRGB565ToYUY2(rgb565_buf, (uint8_t * )yuv_buf, width, height);
}

/**
 * @brief  Generate vertical gradient pattern
 */
void GenerateGradientV(uint16_t *rgb565_buf,uint16_t *yuv_buf, uint16_t width, uint16_t height)
{
    if (rgb565_buf == NULL) return;

    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            uint8_t intensity = (y * 255) / height;
            rgb565_buf[y * width + x] = RGB565(intensity, intensity, intensity);
        }
    }
    ConvertRGB565ToYUY2(rgb565_buf, (uint8_t * )yuv_buf, width, height);
}

/**
 * @brief  Generate checkerboard pattern
 */
void GenerateCheckerboard(uint16_t *rgb565_buf,uint16_t *yuv_buf, uint16_t width, uint16_t height)
{
    if (rgb565_buf == NULL)
    	return;

    uint16_t square_size = 20;  /* 20x20 pixel squares */

    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            uint16_t check_x = x / square_size;
            uint16_t check_y = y / square_size;
            if ((check_x + check_y) % 2 == 0) {
                rgb565_buf[y * width + x] = RGB565(255, 255, 255);
            } else {
                rgb565_buf[y * width + x] = RGB565(0, 0, 0);
            }
        }
    }
    ConvertRGB565ToYUY2(rgb565_buf, (uint8_t * )yuv_buf, width, height);
}

/**
 * @brief  Generate solid color pattern (green screen)
 */
void GenerateSolidColor(uint16_t *rgb565_buf,uint16_t *yuv_buf, uint16_t width, uint16_t height)
{
    if (rgb565_buf == NULL) return;

    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            rgb565_buf[y * width + x] = RGB565(0, 255, 0);  /* Green */
        }
    }
    ConvertRGB565ToYUY2(rgb565_buf, (uint8_t * )yuv_buf, width, height);
}

/**
 * @brief  Generate moving box animation pattern
 */
void GenerateMovingBox(uint16_t *rgb565_buf,uint16_t *yuv_buf, uint16_t width, uint16_t height)
{
    if (rgb565_buf == NULL) return;

    /* Background: blue gradient */
    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            rgb565_buf[y * width + x] = RGB565(0, 0, 128);
        }
    }

    /* Draw moving red box */
    for (uint16_t y = BoxY; y < BoxY + BoxSize && y < height; y++) {
        for (uint16_t x = BoxX; x < BoxX + BoxSize && x < width; x++) {
            rgb565_buf[y * width + x] = RGB565(255, 0, 0);
        }
    }
    ConvertRGB565ToYUY2(rgb565_buf, (uint8_t * )yuv_buf, width, height);
}

/**
 * @brief  Update moving box position for animation
 */
void UpdateMovingBoxPosition(uint32_t FrameCount)
{
    /* Only update every 2 frames for smoother animation at 30fps */
    if (FrameCount % 2 != 0)
    	return;

    BoxX += BoxDX;
    BoxY += BoxDY;

    /* Bounce off edges */
    if (BoxX <= 0 || BoxX >= (WIDTH_QVGA - BoxSize)) {
        BoxDX = -BoxDX;
    }
    if (BoxY <= 0 || BoxY >= (HEIGHT_QVGA - BoxSize)) {
        BoxDY = -BoxDY;
    }
}



/**
 * @brief  Create RGB565 color from RGB components
 */


#endif // #ifdef SAMPLEPROCESS_1_UVCDEVICE
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

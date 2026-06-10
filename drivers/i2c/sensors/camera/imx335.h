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
 * imx335.h
 *
 *  Created on: Feb 26, 2026
 *      Author: fil
 */

#ifndef DRIVERS_I2C_SENSORS_CAMERA_IMX335_H_
#define DRIVERS_I2C_SENSORS_CAMERA_IMX335_H_

struct regval {
  uint16_t addr;
  uint8_t val;
};
#define IMX335_SENSOR_INFO_MAX_LENGTH      (32U)

typedef struct
{
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	I2C_HandleTypeDef 	*bus;
	uint16_t 			device_address;
	uint32_t 			wakeup_id;
	I2C_DriverStruct_t	*next_drv;
	/* driver proprietary data */
	uint8_t				device_flags;
	uint32_t 			device_config;
	uint32_t			sensor_id;
	GPIO_TypeDef	 	*i2c_scl_port;
	uint16_t			i2c_scl_bit;
	uint8_t 			device_address_size;
	uint32_t 			device_size;
	GPIO_TypeDef	 	*enable_port;
	uint16_t			enable_bit;
	GPIO_TypeDef	 	*reset_port;
	uint16_t			reset_bit;
	uint32_t 			timeout;
	uint32_t			who_am_i;
	uint32_t 			Resolution;
	uint32_t 			PixelFormat;
	uint32_t			gain;
	uint32_t			exposure;
	/* Sensor config */
	uint32_t 			Config_Resolution;
	uint32_t 			Config_LightMode;
	uint32_t 			Config_SpecialEffect;
	uint32_t 			Config_Brightness;
	uint32_t 			Config_Saturation;
	uint32_t 			Config_Contrast;
	uint32_t 			Config_HueDegree;
	uint32_t 			Config_Gain;
	uint32_t 			Config_Exposure;
	uint32_t 			Config_MirrorFlip;
	uint32_t 			Config_Zoom;
	uint32_t 			Config_NightMode;
	uint32_t 			Config_ExposureMode;
	uint32_t 			Config_SensorInfo;
	uint32_t 			Config_TestPattern;
	uint32_t 			Config_ColorEffect;
	/* Sensor info */
	char 				name[IMX335_SENSOR_INFO_MAX_LENGTH];
	uint8_t 			bayer_pattern;
	uint8_t 			color_depth;
	uint32_t 			width;
	uint32_t 			height;
	uint32_t 			gain_min;
	uint32_t 			gain_max;
	uint32_t 			exposure_min;
	uint32_t 			exposure_max;
}I2C_imx335_DriverStruct_t;

#define IMX335_REG_MODE_SELECT    0x3000
#define IMX335_MODE_STREAMING       0x00
#define IMX335_MODE_STANDBY         0x01

#define IMX335_REG_HOLD           0x3001
#define IMX335_REG_VMAX           0x3030
#define IMX335_REG_SHUTTER        0x3058
#define IMX335_REG_GAIN           0x30e8
#define IMX335_REG_TPG            0x329e

#define IMX335_REG_ID             0x3912
#define IMX335_CHIP_ID            0x00

#define IMX335_SHUTTER_MIN        9

#define IMX335_EXPOSURE_DEFAULT   23814

#define IMX335_NAME               "IMX335"
#define IMX335_BAYER_PATTERN      0 /* From ISP definition RGGB / TODO comnon enumeration in camera */
#define IMX335_COLOR_DEPTH        10 /* in bits */
#define IMX335_GAIN_MIN           (0 * 1000)
#define IMX335_GAIN_MAX           (72 * 1000)
#define IMX335_GAIN_DEFAULT       (20 * 1000)
#define IMX335_GAIN_UNIT_MDB      300
#define IMX335_EXPOSURE_MIN       0           /* in us */
#define IMX335_EXPOSURE_MAX       33266       /* in us, for sensor @30fps */



#define IMX335_REG_HREVERSE       0x304EU
#define IMX335_REG_VREVERSE       0x304FU
#define AREA3_ST_ADR_1_LSB        0x3074U
#define AREA3_ST_ADR_1_MSB        0x3075U

/* For 2592x1944 */
#define IMX335_WIDTH              2592
#define IMX335_HEIGHT             1944
#define IMX335_PCLK               396000000
#define IMX335_TIMEOUT            100

#define IMX335_1H_PERIOD_USEC (1000000.0F / 4500 / 30)

/* Camera resolutions */
#define IMX335_R2592_1944                6U	/* 2592x1944 Resolution       */

/* Camera Pixel Format */
#define IMX335_RAW_RGGB10               10U    /* Pixel Format RAW_RGGB10    */

/* Camera Frequency */
#define  IMX335_INCK_6MHZ     0U
#define  IMX335_INCK_18MHZ    1U
#define  IMX335_INCK_24MHZ    2U
#define  IMX335_INCK_27MHZ    3U
#define  IMX335_INCK_74MHZ    4U

/* Mirror/Flip */
#define IMX335_MIRROR_FLIP_NONE         0x00U   /* Set camera normal mode     */
#define IMX335_FLIP                     0x01U   /* Set camera flip config     */
#define IMX335_MIRROR                   0x02U   /* Set camera mirror config   */
#define IMX335_MIRROR_FLIP              0x03U   /* Set camera mirror and flip */

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))


#define  IMX335_I2C_ADDRESS		0x34 // 00110100 -> 000110100
#define  IMX335_1_I2C_ADDRESS		0xd4 // 00110100 -> 000110100
#define  IMX335_2_I2C_ADDRESS		0x52 // 00110100 -> 000110100
#define  IMX335_3_I2C_ADDRESS		0x1a

extern uint32_t imx335_register(I2C_imx335_DriverStruct_t *imx335_Drv);

#endif /* DRIVERS_I2C_SENSORS_CAMERA_IMX335_H_ */

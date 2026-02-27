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
 * imx335.c
 *
 *  Created on: Feb 26, 2026
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "../../i2c.h"
#include "imx335.h"

static const struct regval res_2592_1944_regs[] = {
  {0x3000, 0x01},
  {0x3002, 0x00},
  {0x300c, 0x3b},
  {0x300d, 0x2a},
  {0x3018, 0x04},
  {0x302c, 0x3c},
  {0x302e, 0x20},
  {0x3056, 0x98},
  {0x3074, 0xc8},
  {0x3076, 0x30},
  {0x304c, 0x00},
  {0x314c, 0xc6},
  {0x315a, 0x02},
  {0x3168, 0xa0},
  {0x316a, 0x7e},
  {0x31a1, 0x00},
  {0x3288, 0x21},
  {0x328a, 0x02},
  {0x3414, 0x05},
  {0x3416, 0x18},
  {0x3648, 0x01},
  {0x364a, 0x04},
  {0x364c, 0x04},
  {0x3678, 0x01},
  {0x367c, 0x31},
  {0x367e, 0x31},
  {0x3706, 0x10},
  {0x3708, 0x03},
  {0x3714, 0x02},
  {0x3715, 0x02},
  {0x3716, 0x01},
  {0x3717, 0x03},
  {0x371c, 0x3d},
  {0x371d, 0x3f},
  {0x372c, 0x00},
  {0x372d, 0x00},
  {0x372e, 0x46},
  {0x372f, 0x00},
  {0x3730, 0x89},
  {0x3731, 0x00},
  {0x3732, 0x08},
  {0x3733, 0x01},
  {0x3734, 0xfe},
  {0x3735, 0x05},
  {0x3740, 0x02},
  {0x375d, 0x00},
  {0x375e, 0x00},
  {0x375f, 0x11},
  {0x3760, 0x01},
  {0x3768, 0x1b},
  {0x3769, 0x1b},
  {0x376a, 0x1b},
  {0x376b, 0x1b},
  {0x376c, 0x1a},
  {0x376d, 0x17},
  {0x376e, 0x0f},
  {0x3776, 0x00},
  {0x3777, 0x00},
  {0x3778, 0x46},
  {0x3779, 0x00},
  {0x377a, 0x89},
  {0x377b, 0x00},
  {0x377c, 0x08},
  {0x377d, 0x01},
  {0x377e, 0x23},
  {0x377f, 0x02},
  {0x3780, 0xd9},
  {0x3781, 0x03},
  {0x3782, 0xf5},
  {0x3783, 0x06},
  {0x3784, 0xa5},
  {0x3788, 0x0f},
  {0x378a, 0xd9},
  {0x378b, 0x03},
  {0x378c, 0xeb},
  {0x378d, 0x05},
  {0x378e, 0x87},
  {0x378f, 0x06},
  {0x3790, 0xf5},
  {0x3792, 0x43},
  {0x3794, 0x7a},
  {0x3796, 0xa1},
  {0x37b0, 0x36},
  {0x3a00, 0x01},
};

static const struct regval mode_2l_10b_regs[] = {
  {0x3050, 0x00},
  {0x319D, 0x00},
  {0x341c, 0xff},
  {0x341d, 0x01},
  {0x3a01, 0x01},
};

static const struct regval inck_74Mhz_regs[] = {
  {0x300c, 0xB6},
  {0x300d, 0x7F},
  {0x314c, 0x80},
  {0x314d, 0x00},
  {0x315a, 0x03},
  {0x3168, 0x68},
  {0x316a, 0x7F},
};

static const struct regval inck_27Mhz_regs[] = {
  {0x300c, 0x42},
  {0x300d, 0x2E},
  {0x314c, 0xB0},
  {0x314d, 0x00},
  {0x315a, 0x02},
  {0x3168, 0x8F},
  {0x316a, 0x7E},
};

static const struct regval inck_24Mhz_regs[] = {
  {0x300c, 0x3B},
  {0x300d, 0x2A},
  {0x314c, 0xC6},
  {0x314d, 0x00},
  {0x315a, 0x02},
  {0x3168, 0xA0},
  {0x316a, 0x7E},
};

static const struct regval inck_18Mhz_regs[] = {
  {0x300c, 0x2D},
  {0x300d, 0x1F},
  {0x314c, 0x84},
  {0x314d, 0x00},
  {0x315a, 0x01},
  {0x3168, 0x6B},
  {0x316a, 0x7D},
};

static const struct regval inck_6Mhz_regs[] = {
  {0x300c, 0x0F},
  {0x300d, 0x0B},
  {0x314c, 0xC6},
  {0x314d, 0x00},
  {0x315a, 0x00},
  {0x3168, 0xA0},
  {0x316a, 0x7C},
};

static const struct regval framerate_10fps_regs[] = {
  {0x3030, 0xC0},
  {0x3031, 0x34},
};

static const struct regval framerate_15fps_regs[] = {
  {0x3030, 0x2A},
  {0x3031, 0x23},
};

static const struct regval framerate_20fps_regs[] = {
  {0x3030, 0x60},
  {0x3031, 0x1A},
};

static const struct regval framerate_25fps_regs[] = {
  {0x3030, 0x1A},
  {0x3031, 0x15},
};

static const struct regval framerate_30fps_regs[] = {
  {0x3030, 0x94},
  {0x3031, 0x11},
};

static const struct regval mirrorflip_mode_regs[][10] = {
  {
    {AREA3_ST_ADR_1_LSB, 0xc8}, //AREA3_ST_ADR_1 LSB
    {AREA3_ST_ADR_1_MSB, 0x00}, //AREA3_ST_ADR_1 MSB
    {IMX335_REG_HREVERSE, 0x00}, //HREVERSE 0
    {IMX335_REG_VREVERSE, 0x00}, //VREVERSE 0
    {0x3081, 0x02}, //RESERVED
    {0x3083, 0x02}, //RESERVED
    {0x30b6, 0x00}, //RESERVED
    {0x30b7, 0x00}, //RESERVED
    {0x3116, 0x08}, //RESERVED
    {0x3117, 0x00}, //RESERVED
  },
  {
    {AREA3_ST_ADR_1_LSB, 0xf8}, //AREA3_ST_ADR_1 LSB
    {AREA3_ST_ADR_1_MSB, 0x0f}, //AREA3_ST_ADR_1 MSB
    {IMX335_REG_HREVERSE, 0x00}, //HREVERSE 0
    {IMX335_REG_VREVERSE, 0x01}, //VREVERSE 1
    {0x3081, 0xfe}, //RESERVED
    {0x3083, 0xfe}, //RESERVED
    {0x30b6, 0xfa}, //RESERVED
    {0x30b7, 0x01}, //RESERVED
    {0x3116, 0x02}, //RESERVED
    {0x3117, 0x00}, //RESERVED
  },
  {
    {AREA3_ST_ADR_1_LSB, 0xc8}, //AREA3_ST_ADR_1 LSB
    {AREA3_ST_ADR_1_MSB, 0x00}, //AREA3_ST_ADR_1 MSB
    {IMX335_REG_HREVERSE, 0x01}, //HREVERSE 1
    {IMX335_REG_VREVERSE, 0x00}, //VREVERSE 0
    {0x3081, 0x02}, //RESERVED
    {0x3083, 0x02}, //RESERVED
    {0x30b6, 0x00}, //RESERVED
    {0x30b7, 0x00}, //RESERVED
    {0x3116, 0x08}, //RESERVED
    {0x3117, 0x00}, //RESERVED
  },
  {
    {AREA3_ST_ADR_1_LSB, 0xf8}, //AREA3_ST_ADR_1 LSB
    {AREA3_ST_ADR_1_MSB, 0x0f}, //AREA3_ST_ADR_1 MSB
    {IMX335_REG_HREVERSE, 0x01}, //HREVERSE 1
    {IMX335_REG_VREVERSE, 0x01}, //VREVERSE 1
    {0x3081, 0xfe}, //RESERVED
    {0x3083, 0xfe}, //RESERVED
    {0x30b6, 0xfa}, //RESERVED
    {0x30b7, 0x01}, //RESERVED
    {0x3116, 0x02}, //RESERVED
    {0x3117, 0x00}, //RESERVED
  },
};

static const struct regval test_pattern_enable_regs[] = {
  {0x3148, 0x10},
  {0x3280, 0x00},
  {0x329c, 0x01},
  {0x32a0, 0x11},
  {0x3302, 0x00},
  {0x3303, 0x00},
  {0x336c, 0x00},
};

static const struct regval test_pattern_disable_regs[] = {
  {0x3148, 0x00},
  {0x3280, 0x01},
  {0x329c, 0x00},
  {0x32a0, 0x10},
  {0x3302, 0x32},
  {0x3303, 0x00},
  {0x336c, 0x01},
};

static int32_t imx335_WriteTable(I2C_imx335_Drv_TypeDef *imx335_Drv, const struct regval *regs, uint32_t size)
{
uint32_t index;

	/* Set registers */
	for(index=0; index<size ; index++)
	{
		if ( HAL_I2C_Mem_Write(imx335_Drv->bus, imx335_Drv->device_address, regs->addr,2,(uint8_t *)&regs->val, 1,IMX335_TIMEOUT) )
			return 1;
	}
	return 0;
}

static int32_t imx335_WriteRegister(I2C_imx335_Drv_TypeDef *imx335_Drv, uint16_t reg, uint8_t *value, uint16_t length)
{
	return HAL_I2C_Mem_Write(imx335_Drv->bus, imx335_Drv->device_address, reg,2,value, length,IMX335_TIMEOUT);
}

static uint8_t imx335_ReadRegister(I2C_imx335_Drv_TypeDef *imx335_Drv, uint16_t reg, uint16_t length)
{
uint8_t	imx335_reg;
	if ( HAL_I2C_Mem_Read(imx335_Drv->bus, imx335_Drv->device_address, reg,2,&imx335_reg, length,IMX335_TIMEOUT) )
		return 0;
	return imx335_reg;
}

uint8_t imx335_ReadID(I2C_imx335_Drv_TypeDef *imx335_Drv)
{
	imx335_Drv->sensor_id = imx335_ReadRegister(imx335_Drv,IMX335_REG_ID,1);
	return 0;
}

uint8_t imx335_SetGain(I2C_imx335_Drv_TypeDef *imx335_Drv, uint32_t gain)
{
uint8_t hold = 1;
	if ((gain > IMX335_GAIN_MAX) || (gain < IMX335_GAIN_MIN))
	    return 1;

    gain /= IMX335_GAIN_UNIT_MDB;

	if ( imx335_WriteRegister(imx335_Drv,IMX335_REG_HOLD,&hold,1) == 0 )
	{
		if ( imx335_WriteRegister(imx335_Drv,IMX335_REG_GAIN,(uint8_t *)&gain,2) )
		    return 1;
		else
		{
			hold = 0;
			imx335_Drv->gain = gain;
			return imx335_WriteRegister(imx335_Drv,IMX335_REG_HOLD,&hold,1);
		}
	}
	return 1;
}

uint8_t imx335_SetExposure(I2C_imx335_Drv_TypeDef *imx335_Drv, uint32_t exposure)
{
uint8_t hold = 1;
uint32_t vmax, shutter;

	vmax = imx335_ReadRegister(imx335_Drv,IMX335_REG_VMAX,1);
    shutter = ( vmax - (exposure /((uint32_t) IMX335_1H_PERIOD_USEC)));

    if (shutter < IMX335_SHUTTER_MIN)
    	return 1;
    else
    {
    	if ( imx335_WriteRegister(imx335_Drv,IMX335_REG_HOLD,&hold,1) == 0 )
    	{
    		if ( imx335_WriteRegister(imx335_Drv,IMX335_REG_SHUTTER,(uint8_t *)&shutter,3) )
    		    return 1;
    		else
    		{
    			hold = 0;
    			return imx335_WriteRegister(imx335_Drv,IMX335_REG_HOLD,&hold,1);
    		}
    	}
    }
	return 1;
}

uint8_t imx335_SetFrequency(I2C_imx335_Drv_TypeDef *imx335_Drv, uint32_t frequency)
{
	  switch (frequency)
	  {
	    case IMX335_INCK_74MHZ:
	      return imx335_WriteTable(imx335_Drv, inck_74Mhz_regs, ARRAY_SIZE(inck_74Mhz_regs));
	      break;
	    case IMX335_INCK_27MHZ:
	    	return imx335_WriteTable(imx335_Drv, inck_27Mhz_regs, ARRAY_SIZE(inck_27Mhz_regs));
	      break;
	    case IMX335_INCK_24MHZ:
	    	return imx335_WriteTable(imx335_Drv, inck_24Mhz_regs, ARRAY_SIZE(inck_24Mhz_regs));
	      break;
	    case IMX335_INCK_18MHZ:
	    	return imx335_WriteTable(imx335_Drv, inck_18Mhz_regs, ARRAY_SIZE(inck_18Mhz_regs));
	      break;
	    default:
	      /* IMX335_INCK_6MHZ */
	    	return imx335_WriteTable(imx335_Drv, inck_6Mhz_regs, ARRAY_SIZE(inck_6Mhz_regs));
	      break;
	  }
	  return 1;
}

uint8_t imx335_SetFrameRate(I2C_imx335_Drv_TypeDef *imx335_Drv, uint32_t framerate)
{
	  switch (framerate)
	  {
	    case 10:
	    	return imx335_WriteTable(imx335_Drv, framerate_10fps_regs, ARRAY_SIZE(framerate_10fps_regs));
	      break;
	    case 15:
	    	return imx335_WriteTable(imx335_Drv, framerate_15fps_regs, ARRAY_SIZE(framerate_15fps_regs));
	      break;
	    case 20:
	    	return imx335_WriteTable(imx335_Drv, framerate_20fps_regs, ARRAY_SIZE(framerate_20fps_regs));
	      break;
	    case 25:
	    	return imx335_WriteTable(imx335_Drv, framerate_25fps_regs, ARRAY_SIZE(framerate_25fps_regs));
	      break;
	    default:
	      /* 30fps */
	    	return imx335_WriteTable(imx335_Drv, framerate_30fps_regs, ARRAY_SIZE(framerate_30fps_regs));
	      break;
	  };

}

uint8_t imx335_MirrorFlipConfig(I2C_imx335_Drv_TypeDef *imx335_Drv, uint32_t Config)
{
	  switch (Config)
	  {
	    case IMX335_FLIP:
	    	return imx335_WriteTable(imx335_Drv, mirrorflip_mode_regs[1], ARRAY_SIZE(mirrorflip_mode_regs[1]));
	      break;
	    case IMX335_MIRROR:
	    	return imx335_WriteTable(imx335_Drv, mirrorflip_mode_regs[2], ARRAY_SIZE(mirrorflip_mode_regs[2]));
	      break;
	    case IMX335_MIRROR_FLIP:
	    	return imx335_WriteTable(imx335_Drv, mirrorflip_mode_regs[3], ARRAY_SIZE(mirrorflip_mode_regs[3]));
	      break;
	    case IMX335_MIRROR_FLIP_NONE:
	    default:
	    	return imx335_WriteTable(imx335_Drv, mirrorflip_mode_regs[0], ARRAY_SIZE(mirrorflip_mode_regs[0]));
	      break;
	  }
}

uint8_t imx335_SetTestPattern(I2C_imx335_Drv_TypeDef *imx335_Drv, uint32_t mode)
{
uint8_t val;

	  if (mode >= 0)
	  {
	    /* Enable Test Pattern #mode */
	    val = mode;
	    if (imx335_WriteRegister(imx335_Drv, IMX335_REG_TPG, &val, 1))
	      return 1;
	    if (imx335_WriteTable(imx335_Drv, test_pattern_enable_regs, ARRAY_SIZE(test_pattern_enable_regs)))
		      return 1;
	  }
	  else
	  {
	    /* Disable Test Pattern */
		    if (imx335_WriteTable(imx335_Drv, test_pattern_disable_regs, ARRAY_SIZE(test_pattern_enable_regs)))
			      return 1;
	  }
	  return 0;
}

uint8_t	IMX335_1_I2C_ADDRESS_present;
uint8_t	IMX335_2_I2C_ADDRESS_present;
uint8_t	IMX335_3_I2C_ADDRESS_present;

ITCM_AREA_CODE uint32_t imx335_register(I2C_imx335_Drv_TypeDef *imx335_Drv)
{
I2C_DriverStruct_t 	*eptr, *pre_eptr;
uint32_t			ret_val;
uint8_t 			tmp;

	if ( imx335_Drv->bus == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( imx335_Drv->enable_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( imx335_Drv->reset_port == NULL)
		return DRIVER_REQUEST_FAILED;

	HAL_GPIO_WritePin(imx335_Drv->reset_port, imx335_Drv->reset_bit, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(imx335_Drv->reset_port, imx335_Drv->reset_bit, GPIO_PIN_SET);
	HAL_Delay(10);

	HAL_GPIO_WritePin(imx335_Drv->enable_port, imx335_Drv->enable_bit, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(imx335_Drv->enable_port, imx335_Drv->enable_bit, GPIO_PIN_SET);
	HAL_Delay(10);

	if ( HAL_I2C_IsDeviceReady(imx335_Drv->bus,imx335_Drv->device_address,5,IMX335_TIMEOUT) )
		return DRIVER_REQUEST_FAILED;
	if ( i2c_drv_ptr == NULL)
	{
		i2c_drv_ptr = (I2C_DriverStruct_t *)imx335_Drv;
		imx335_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = i2c_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (I2C_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)imx335_Drv;
		imx335_Drv->next_drv = NULL;
	}
	imx335_Drv->process = get_current_process();
    imx335_Drv->Config_Brightness    = 0;
    imx335_Drv->Config_Contrast      = 0;
    imx335_Drv->Config_HueDegree     = 0;
    imx335_Drv->Config_Gain          = 1;
    imx335_Drv->Config_Exposure      = 1;
    imx335_Drv->Config_ExposureMode  = 0;
    imx335_Drv->Config_LightMode     = 0;
    imx335_Drv->Config_MirrorFlip    = 1;
    imx335_Drv->Config_NightMode     = 0;
    imx335_Drv->Config_Resolution    = 0;
    imx335_Drv->Config_Saturation    = 0;
    imx335_Drv->Config_SpecialEffect = 0;
    imx335_Drv->Config_Zoom          = 0;
    imx335_Drv->Config_SensorInfo    = 1;
    imx335_Drv->Config_TestPattern   = 1;

    imx335_Drv->bayer_pattern = IMX335_BAYER_PATTERN;
    imx335_Drv->color_depth = IMX335_COLOR_DEPTH;
    imx335_Drv->width = IMX335_WIDTH;
    imx335_Drv->height = IMX335_HEIGHT;
    imx335_Drv->gain_min = IMX335_GAIN_MIN;
    imx335_Drv->gain_max = IMX335_GAIN_MAX;
    imx335_Drv->exposure_min = IMX335_EXPOSURE_MIN;
    imx335_Drv->exposure_max = IMX335_EXPOSURE_MAX;

    if ( imx335_Drv->Resolution != IMX335_R2592_1944)
    {
    	return 1;
    }
	ret_val = imx335_WriteTable(imx335_Drv, res_2592_1944_regs, ARRAY_SIZE(res_2592_1944_regs));
	if ( ret_val != 0)
		return 1;

	ret_val = imx335_WriteTable(imx335_Drv, mode_2l_10b_regs, ARRAY_SIZE(mode_2l_10b_regs));

    if ( ret_val != 0)
    	return 1;

    tmp = IMX335_MODE_STREAMING;
    if (imx335_WriteRegister(imx335_Drv, IMX335_REG_MODE_SELECT, &tmp, 1))
      return 1;

    IMX335_1_I2C_ADDRESS_present = HAL_I2C_IsDeviceReady(imx335_Drv->bus,IMX335_1_I2C_ADDRESS,5,IMX335_TIMEOUT);
    IMX335_2_I2C_ADDRESS_present = HAL_I2C_IsDeviceReady(imx335_Drv->bus,IMX335_2_I2C_ADDRESS,5,IMX335_TIMEOUT);
    IMX335_3_I2C_ADDRESS_present = HAL_I2C_IsDeviceReady(imx335_Drv->bus,0x52,5,IMX335_TIMEOUT);

	return 0;
}

#endif // #ifdef A_OS_I2C_ENABLED


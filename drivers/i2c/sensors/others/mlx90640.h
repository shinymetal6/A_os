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
 * mlx90640.h
 *
 *  Created on: Feb 24, 2026
 *      Author: fil
 */

#ifndef DRIVERS_I2C_SENSORS_OTHERS_MLX90640_H_
#define DRIVERS_I2C_SENSORS_OTHERS_MLX90640_H_

#define MLX90640_ADDRESS 				0x33
#define MLX90640_TIMEOUT 				100
#define MLX90640_EEPROM_START_ADDRESS 	0x2400
#define MLX90640_EEPROM_DUMP_NUM 		832

#define MLX90640_STATUS_REG 			0x8000
#define MLX90640_CTRL_REG 				0x800d
#define MLX90640_I2C_CONF_REG			0x800f

#define BIT_MASK(x) (1UL << (x))
#define REG_MASK(sbit,nbits) ~((~(~0UL << (nbits))) << (sbit))

#define MLX90640_EEPROM_START_ADDRESS 0x2400
#define MLX90640_EEPROM_DUMP_NUM 832
#define MLX90640_PIXEL_DATA_START_ADDRESS 0x0400
#define MLX90640_PIXEL_NUM 768
#define MLX90640_LINE_NUM 24
#define MLX90640_COLUMN_NUM 32
#define MLX90640_LINE_SIZE 32
#define MLX90640_COLUMN_SIZE 24
#define MLX90640_AUX_DATA_START_ADDRESS 0x0700
#define MLX90640_AUX_NUM 64
#define MLX90640_STATUS_REG 0x8000
#define MLX90640_INIT_STATUS_VALUE 0x0030
#define MLX90640_STAT_FRAME_MASK BIT_MASK(0)
#define MLX90640_GET_FRAME(reg_value) (reg_value & MLX90640_STAT_FRAME_MASK)
#define MLX90640_STAT_DATA_READY_MASK BIT_MASK(3)
#define MLX90640_GET_DATA_READY(reg_value) (reg_value & MLX90640_STAT_DATA_READY_MASK)

#define MLX90640_CTRL_TRIG_READY_MASK BIT_MASK(15)
#define MLX90640_CTRL_REFRESH_SHIFT 7
#define MLX90640_CTRL_REFRESH_MASK REG_MASK(MLX90640_CTRL_REFRESH_SHIFT,3)
#define MLX90640_CTRL_RESOLUTION_SHIFT 10
#define MLX90640_CTRL_RESOLUTION_MASK REG_MASK(MLX90640_CTRL_RESOLUTION_SHIFT,2)
#define MLX90640_CTRL_MEAS_MODE_SHIFT 12
#define MLX90640_CTRL_MEAS_MODE_MASK BIT_MASK(12)

#define MLX90640_MS_BYTE_SHIFT 8
#define MLX90640_MS_BYTE_MASK 0xFF00
#define MLX90640_LS_BYTE_MASK 0x00FF
#define MLX90640_MS_BYTE(reg16) ((reg16 & MLX90640_MS_BYTE_MASK) >> MLX90640_MS_BYTE_SHIFT)
#define MLX90640_LS_BYTE(reg16) (reg16 & MLX90640_LS_BYTE_MASK)
#define MLX90640_MSBITS_6_MASK 0xFC00
#define MLX90640_LSBITS_10_MASK 0x03FF
#define MLX90640_NIBBLE1_MASK 0x000F
#define MLX90640_NIBBLE2_MASK 0x00F0
#define MLX90640_NIBBLE3_MASK 0x0F00
#define MLX90640_NIBBLE4_MASK 0xF000
#define MLX90640_NIBBLE1(reg16) ((reg16 & MLX90640_NIBBLE1_MASK))
#define MLX90640_NIBBLE2(reg16) ((reg16 & MLX90640_NIBBLE2_MASK) >> 4)
#define MLX90640_NIBBLE3(reg16) ((reg16 & MLX90640_NIBBLE3_MASK) >> 8)
#define MLX90640_NIBBLE4(reg16) ((reg16 & MLX90640_NIBBLE4_MASK) >> 12)

#define POW2(x) pow(2, (double)x)

#define SCALEALPHA 0.000001

#define MLX90640_NO_ERROR 0

#define MLX90640_BROKEN_PIXELS_NUM_ERROR 3
#define MLX90640_OUTLIER_PIXELS_NUM_ERROR 4
#define MLX90640_BAD_PIXELS_NUM_ERROR 5
#define MLX90640_ADJACENT_BAD_PIXELS_ERROR 6
#define MLX90640_EEPROM_DATA_ERROR 7
#define MLX90640_FRAME_DATA_ERROR 8
#define MLX90640_MEAS_TRIGGER_ERROR 9

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
	uint16_t			mlx90640_found;
	uint16_t			*mlx90640_eeprom;	//[832]
    uint16_t 			*frame_data; 	//[768] MLX90640_PIXEL_NUM
    uint16_t 			*aux_data; 	//[64] MLX90640_AUX_NUM
    float	 			*to_data; 	//[768] MLX90640_AUX_NUM
    uint16_t 			*resulting_image; 	//[768] MLX90640_PIXEL_NUM
	uint16_t			mlx90640_status_reg;
	uint16_t			mlx90640_control_reg;
	uint16_t			mlx90640_i2c_conf_reg;
	uint32_t			mlx90640_resolutionRAM;
	uint32_t			mlx90640_refresh_rate;
	/* */
    int16_t 			kVdd;
    int16_t 			vdd25;
    float 				KvPTAT;
    float 				KtPTAT;
    uint16_t 			vPTAT25;
    float 				alphaPTAT;
    int16_t 			gainEE;
    float 				tgc;
    float 				cpKv;
    float 				cpKta;
    uint8_t 			resolutionEE;
    uint8_t 			calibrationModeEE;
    float 				KsTa;
    float 				ksTo[5];
    int16_t 			ct[5];
    uint16_t 			*alpha; 	//[768] MLX90640_PIXEL_NUM
    uint8_t 			alphaScale;
    int16_t 			*offset; 	//[768] MLX90640_PIXEL_NUM
    int8_t 				*kta;		//[768] MLX90640_PIXEL_NUM
    uint8_t 			ktaScale;
    int8_t 				*kv;	//[768] MLX90640_PIXEL_NUM
    uint8_t 			kvScale;
    float 				cpAlpha[2];
    int16_t 			cpOffset[2];
    float 				ilChessC[3];
    uint16_t 			brokenPixels[5];
    uint16_t 			outlierPixels[5];
    float 				*alphaTemp;	//[768] MLX90640_PIXEL_NUM
}I2C_Mlx90640_Drv_TypeDef;

extern void mlx9064_Calculate_To(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv, float emissivity, float tr);
extern int mlx90640_GetFrameData(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv);
extern void  mlx90640_GetImage(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv);
extern uint32_t mlx90640_run(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv,float emissivity,float eTa);

extern uint32_t mlx90640_register(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv);





#endif /* DRIVERS_I2C_SENSORS_OTHERS_MLX90640_H_ */

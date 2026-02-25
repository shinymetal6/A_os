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
 * mlx90640.c
 *
 *  Created on: Feb 24, 2026
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "../../i2c.h"
#include "mlx90640.h"

ITCM_AREA_CODE uint32_t mlx90640_read_eeprom(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
    return HAL_I2C_Mem_Read(mlx90640_Drv->bus, mlx90640_Drv->device_address, MLX90640_EEPROM_START_ADDRESS,2,(uint8_t *)mlx90640_Drv->mlx90640_eeprom,MLX90640_EEPROM_DUMP_NUM,100);
}

ITCM_AREA_CODE uint32_t mlx90640_read_frame_data(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
    return HAL_I2C_Mem_Read(mlx90640_Drv->bus, mlx90640_Drv->device_address, MLX90640_PIXEL_DATA_START_ADDRESS,2,(uint8_t *)mlx90640_Drv->frame_data,MLX90640_PIXEL_NUM,100);
}

ITCM_AREA_CODE uint32_t mlx90640_read_aux_data(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
    return HAL_I2C_Mem_Read(mlx90640_Drv->bus, mlx90640_Drv->device_address, MLX90640_AUX_DATA_START_ADDRESS,2,(uint8_t *)mlx90640_Drv->frame_data,MLX90640_AUX_NUM,100);
}

ITCM_AREA_CODE uint32_t mlx90640_read_register(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv,uint16_t mlx90640_register)
{
uint16_t mlx90640_reg;
    if ( HAL_I2C_Mem_Read(mlx90640_Drv->bus, mlx90640_Drv->device_address, mlx90640_register,2,(uint8_t *)&mlx90640_reg,2,MLX90640_TIMEOUT) )
    	return 0x100;
    return mlx90640_reg;
}

ITCM_AREA_CODE uint32_t mlx90640_write_register(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv,uint16_t mlx90640_register,uint16_t value)
{
	return HAL_I2C_Mem_Write(mlx90640_Drv->bus, mlx90640_Drv->device_address, mlx90640_register,2,(uint8_t *)&value, 2,MLX90640_TIMEOUT);
}

ITCM_AREA_CODE uint32_t mlx9064_GetCurResolution(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
    if ( mlx90640_read_register(mlx90640_Drv, MLX90640_CTRL_REG))
        return 1;
    mlx90640_Drv->mlx90640_resolutionRAM = (mlx90640_Drv->mlx90640_control_reg & ~MLX90640_CTRL_RESOLUTION_MASK) >> MLX90640_CTRL_RESOLUTION_SHIFT;
    return 0;
}

ITCM_AREA_CODE static float mlx906400_GetVdd(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
float vdd;
float resolutionCorrection;
uint16_t resolutionRAM;

    resolutionRAM = (mlx90640_Drv->frame_data[832] & ~MLX90640_CTRL_RESOLUTION_MASK) >> MLX90640_CTRL_RESOLUTION_SHIFT;
    resolutionCorrection = POW2(mlx90640_Drv->resolutionEE) / POW2(resolutionRAM);
    vdd = (resolutionCorrection * (int16_t)mlx90640_Drv->frame_data[810] - mlx90640_Drv->vdd25) / mlx90640_Drv->kVdd + 3.3;
    return vdd;
}

ITCM_AREA_CODE static float mlx906400_GetTa(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
int16_t ptat;
float ptatArt;
float vdd;
float ta;

    vdd = mlx906400_GetVdd(mlx90640_Drv);

    ptat = (int16_t)mlx90640_Drv->frame_data[800];

    ptatArt = (ptat / (ptat * mlx90640_Drv->alphaPTAT + (int16_t)mlx90640_Drv->frame_data[768])) * POW2(18);

    ta = (ptatArt / (1 + mlx90640_Drv->KvPTAT * (vdd - 3.3)) - mlx90640_Drv->vPTAT25);
    ta = ta / mlx90640_Drv->KtPTAT + 25;

    return ta;
}

ITCM_AREA_CODE uint32_t mlx9064_GetRefreshRate(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
    if ( mlx90640_read_register(mlx90640_Drv, MLX90640_CTRL_REG))
        return 1;
    mlx90640_Drv->mlx90640_refresh_rate = (mlx90640_Drv->mlx90640_control_reg & ~MLX90640_CTRL_REFRESH_MASK) >> MLX90640_CTRL_REFRESH_SHIFT;
    return 0;
}

ITCM_AREA_CODE uint32_t mlx9064_SetRefreshRate(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv,uint16_t refresh_rate)
{
uint16_t cntrl_reg,value;
	cntrl_reg = mlx90640_read_register(mlx90640_Drv, MLX90640_CTRL_REG);
    value = (refresh_rate << MLX90640_CTRL_REFRESH_SHIFT);
    value &= ~MLX90640_CTRL_REFRESH_MASK;
    value = (cntrl_reg & MLX90640_CTRL_REFRESH_MASK) | value;
    if ( mlx90640_write_register(mlx90640_Drv, MLX90640_CTRL_REG,value))
        return 1;
    mlx90640_Drv->mlx90640_refresh_rate = refresh_rate;
    return 0;
}

ITCM_AREA_CODE uint32_t mlx9064_SetInterleavedMode(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv,uint16_t refresh_rate)
{
uint16_t cntrl_reg,value;
	cntrl_reg = mlx90640_read_register(mlx90640_Drv, MLX90640_CTRL_REG);
    value = (cntrl_reg & ~MLX90640_CTRL_MEAS_MODE_MASK);
	if ( mlx90640_write_register(mlx90640_Drv, MLX90640_CTRL_REG,value))
		return 1;
	return 0;
}

ITCM_AREA_CODE uint32_t mlx9064_SetChessdMode(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv,uint16_t refresh_rate)
{
uint16_t cntrl_reg,value;
	cntrl_reg = mlx90640_read_register(mlx90640_Drv, MLX90640_CTRL_REG);
    value = (cntrl_reg | MLX90640_CTRL_MEAS_MODE_MASK);
	if ( mlx90640_write_register(mlx90640_Drv, MLX90640_CTRL_REG,value))
		return 1;
	return 0;
}

ITCM_AREA_CODE static void mlx9064_Extract_VDD_Parameters(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
int8_t kVdd;
int16_t vdd25;

    kVdd = MLX90640_MS_BYTE(mlx90640_Drv->mlx90640_eeprom[51]);

    vdd25 = MLX90640_LS_BYTE(mlx90640_Drv->mlx90640_eeprom[51]);
    vdd25 = ((vdd25 - 256) << 5) - 8192;

    mlx90640_Drv->kVdd = 32 * kVdd;
    mlx90640_Drv->vdd25 = vdd25;
}

ITCM_AREA_CODE static void mlx9064_Extract_PTAT_Parameters(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
float	KvPTAT;
float	KtPTAT;
int16_t	vPTAT25;
float	alphaPTAT;

    KvPTAT = (mlx90640_Drv->mlx90640_eeprom[50] & MLX90640_MSBITS_6_MASK) >> 10;
    if(KvPTAT > 31)
    {
        KvPTAT = KvPTAT - 64;
    }
    KvPTAT = KvPTAT/4096;

    KtPTAT = mlx90640_Drv->mlx90640_eeprom[50] & MLX90640_LSBITS_10_MASK;
    if(KtPTAT > 511)
    {
        KtPTAT = KtPTAT - 1024;
    }
    KtPTAT = KtPTAT/8;

    vPTAT25 = mlx90640_Drv->mlx90640_eeprom[49];

    alphaPTAT = (mlx90640_Drv->mlx90640_eeprom[16] & MLX90640_NIBBLE4_MASK) / POW2(14) + 8.0f;

    mlx90640_Drv->KvPTAT = KvPTAT;
    mlx90640_Drv->KtPTAT = KtPTAT;
    mlx90640_Drv->vPTAT25 = vPTAT25;
    mlx90640_Drv->alphaPTAT = alphaPTAT;
}

ITCM_AREA_CODE static void mlx9064_Extract_Gain_TGC_Resolution_Parameters(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
	mlx90640_Drv->gainEE = (int16_t)mlx90640_Drv->mlx90640_eeprom[48];
	mlx90640_Drv->tgc = (int8_t)MLX90640_LS_BYTE(mlx90640_Drv->mlx90640_eeprom[60]) / 32.0f;
	uint8_t resolutionEE = (mlx90640_Drv->mlx90640_eeprom[56] & 0x3000) >> 12;
    mlx90640_Drv->resolutionEE = resolutionEE;
}

ITCM_AREA_CODE static void mlx9064_Extract_KS_Parameters(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
int32_t KsToScale;
int8_t step;
	mlx90640_Drv->KsTa = (int8_t)MLX90640_MS_BYTE(mlx90640_Drv->mlx90640_eeprom[60]) / 8192.0f;

    step = ((mlx90640_Drv->mlx90640_eeprom[63] & 0x3000) >> 12) * 10;

    mlx90640_Drv->ct[0] = -40;
    mlx90640_Drv->ct[1] = 0;
    mlx90640_Drv->ct[2] = MLX90640_NIBBLE2(mlx90640_Drv->mlx90640_eeprom[63]);
    mlx90640_Drv->ct[3] = MLX90640_NIBBLE3(mlx90640_Drv->mlx90640_eeprom[63]);

    mlx90640_Drv->ct[2] = mlx90640_Drv->ct[2]*step;
    mlx90640_Drv->ct[3] = mlx90640_Drv->ct[2] + mlx90640_Drv->ct[3]*step;
    mlx90640_Drv->ct[4] = 400;

    KsToScale = MLX90640_NIBBLE1(mlx90640_Drv->mlx90640_eeprom[63]) + 8;
    KsToScale = 1UL << KsToScale;

    mlx90640_Drv->ksTo[0] = (int8_t)MLX90640_LS_BYTE(mlx90640_Drv->mlx90640_eeprom[61]) / (float)KsToScale;
    mlx90640_Drv->ksTo[1] = (int8_t)MLX90640_MS_BYTE(mlx90640_Drv->mlx90640_eeprom[61]) / (float)KsToScale;
    mlx90640_Drv->ksTo[2] = (int8_t)MLX90640_LS_BYTE(mlx90640_Drv->mlx90640_eeprom[62]) / (float)KsToScale;
    mlx90640_Drv->ksTo[3] = (int8_t)MLX90640_MS_BYTE(mlx90640_Drv->mlx90640_eeprom[62]) / (float)KsToScale;
    mlx90640_Drv->ksTo[4] = -0.0002;
}

ITCM_AREA_CODE static void mlx9064_Extract_CPP_Parameters(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
    float alphaSP[2];
    int16_t offsetSP[2];
    float cpKv;
    float cpKta;
    uint8_t alphaScale;
    uint8_t ktaScale1;
    uint8_t kvScale;

    alphaScale = MLX90640_NIBBLE4(mlx90640_Drv->mlx90640_eeprom[32]) + 27;

    offsetSP[0] = (mlx90640_Drv->mlx90640_eeprom[58] & MLX90640_LSBITS_10_MASK);
    if (offsetSP[0] > 511)
    {
        offsetSP[0] = offsetSP[0] - 1024;
    }

    offsetSP[1] = (mlx90640_Drv->mlx90640_eeprom[58] & MLX90640_MSBITS_6_MASK) >> 10;
    if (offsetSP[1] > 31)
    {
        offsetSP[1] = offsetSP[1] - 64;
    }
    offsetSP[1] = offsetSP[1] + offsetSP[0];

    alphaSP[0] = (mlx90640_Drv->mlx90640_eeprom[57] & MLX90640_LSBITS_10_MASK);
    if (alphaSP[0] > 511)
    {
        alphaSP[0] = alphaSP[0] - 1024;
    }
    alphaSP[0] = alphaSP[0] /  POW2(alphaScale);

    alphaSP[1] = (mlx90640_Drv->mlx90640_eeprom[57] & MLX90640_MSBITS_6_MASK) >> 10;
    if (alphaSP[1] > 31)
    {
        alphaSP[1] = alphaSP[1] - 64;
    }
    alphaSP[1] = (1 + alphaSP[1]/128) * alphaSP[0];

    cpKta = (int8_t)MLX90640_LS_BYTE(mlx90640_Drv->mlx90640_eeprom[59]);

    ktaScale1 = MLX90640_NIBBLE2(mlx90640_Drv->mlx90640_eeprom[56]) + 8;
    mlx90640_Drv->cpKta = cpKta / POW2(ktaScale1);

    cpKv = (int8_t)MLX90640_MS_BYTE(mlx90640_Drv->mlx90640_eeprom[59]);

    kvScale = MLX90640_NIBBLE3(mlx90640_Drv->mlx90640_eeprom[56]);
    mlx90640_Drv->cpKv = cpKv / POW2(kvScale);

    mlx90640_Drv->cpAlpha[0] = alphaSP[0];
    mlx90640_Drv->cpAlpha[1] = alphaSP[1];
    mlx90640_Drv->cpOffset[0] = offsetSP[0];
    mlx90640_Drv->cpOffset[1] = offsetSP[1];
}

ITCM_AREA_CODE static void mlx9064_Extract_Alpha_Parameters(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
int accRow[24];
int accColumn[32];
int p = 0;
int alphaRef;
uint8_t alphaScale;
uint8_t accRowScale;
uint8_t accColumnScale;
uint8_t accRemScale;
float temp;


    accRemScale = MLX90640_NIBBLE1(mlx90640_Drv->mlx90640_eeprom[32]);
    accColumnScale = MLX90640_NIBBLE2(mlx90640_Drv->mlx90640_eeprom[32]);
    accRowScale = MLX90640_NIBBLE3(mlx90640_Drv->mlx90640_eeprom[32]);
    alphaScale = MLX90640_NIBBLE4(mlx90640_Drv->mlx90640_eeprom[32]) + 30;
    alphaRef = mlx90640_Drv->mlx90640_eeprom[33];

    for(int i = 0; i < 6; i++)
    {
        p = i * 4;
        accRow[p + 0] = MLX90640_NIBBLE1(mlx90640_Drv->mlx90640_eeprom[34 + i]);
        accRow[p + 1] = MLX90640_NIBBLE2(mlx90640_Drv->mlx90640_eeprom[34 + i]);
        accRow[p + 2] = MLX90640_NIBBLE3(mlx90640_Drv->mlx90640_eeprom[34 + i]);
        accRow[p + 3] = MLX90640_NIBBLE4(mlx90640_Drv->mlx90640_eeprom[34 + i]);
    }

    for(int i = 0; i < MLX90640_LINE_NUM; i++)
    {
        if (accRow[i] > 7)
        {
            accRow[i] = accRow[i] - 16;
        }
    }

    for(int i = 0; i < 8; i++)
    {
        p = i * 4;
        accColumn[p + 0] = MLX90640_NIBBLE1(mlx90640_Drv->mlx90640_eeprom[40 + i]);
        accColumn[p + 1] = MLX90640_NIBBLE2(mlx90640_Drv->mlx90640_eeprom[40 + i]);
        accColumn[p + 2] = MLX90640_NIBBLE3(mlx90640_Drv->mlx90640_eeprom[40 + i]);
        accColumn[p + 3] = MLX90640_NIBBLE4(mlx90640_Drv->mlx90640_eeprom[40 + i]);
    }

    for(int i = 0; i < MLX90640_COLUMN_NUM; i++)
    {
        if (accColumn[i] > 7)
        {
            accColumn[i] = accColumn[i] - 16;
        }
    }

    for(int i = 0; i < MLX90640_LINE_NUM; i++)
    {
        for(int j = 0; j < MLX90640_COLUMN_NUM; j ++)
        {
            p = 32 * i +j;
            mlx90640_Drv->alphaTemp[p] = (mlx90640_Drv->mlx90640_eeprom[64 + p] & 0x03F0) >> 4;
            if (mlx90640_Drv->alphaTemp[p] > 31)
            {
            	mlx90640_Drv->alphaTemp[p] = mlx90640_Drv->alphaTemp[p] - 64;
            }
            mlx90640_Drv->alphaTemp[p] = mlx90640_Drv->alphaTemp[p]*(1 << accRemScale);
            mlx90640_Drv->alphaTemp[p] = (alphaRef + (accRow[i] << accRowScale) + (accColumn[j] << accColumnScale) + mlx90640_Drv->alphaTemp[p]);
            mlx90640_Drv->alphaTemp[p] = mlx90640_Drv->alphaTemp[p] / POW2(alphaScale);
            mlx90640_Drv->alphaTemp[p] = mlx90640_Drv->alphaTemp[p] - mlx90640_Drv->tgc * (mlx90640_Drv->cpAlpha[0] + mlx90640_Drv->cpAlpha[1])/2;
            mlx90640_Drv->alphaTemp[p] = SCALEALPHA/mlx90640_Drv->alphaTemp[p];
        }
    }

    temp = mlx90640_Drv->alphaTemp[0];
    for(int i = 1; i < MLX90640_PIXEL_NUM; i++)
    {
        if (mlx90640_Drv->alphaTemp[i] > temp)
        {
            temp = mlx90640_Drv->alphaTemp[i];
        }
    }

    alphaScale = 0;
    while(temp < 32767.4)
    {
        temp = temp*2;
        alphaScale = alphaScale + 1;
    }

    for(int i = 0; i < MLX90640_PIXEL_NUM; i++)
    {
        temp = mlx90640_Drv->alphaTemp[i] * POW2(alphaScale);
        mlx90640_Drv->alpha[i] = (temp + 0.5);

    }
    mlx90640_Drv->alphaScale = alphaScale;
}

ITCM_AREA_CODE static void mlx9064_Extract_Offset_Parameters(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
    int occRow[24];
    int occColumn[32];
    int p = 0;
    int16_t offsetRef;
    uint8_t occRowScale;
    uint8_t occColumnScale;
    uint8_t occRemScale;


    occRemScale = MLX90640_NIBBLE1(mlx90640_Drv->mlx90640_eeprom[16]);
    occColumnScale = MLX90640_NIBBLE2(mlx90640_Drv->mlx90640_eeprom[16]);
    occRowScale = MLX90640_NIBBLE3(mlx90640_Drv->mlx90640_eeprom[16]);
    offsetRef = (int16_t)mlx90640_Drv->mlx90640_eeprom[17];

    for(int i = 0; i < 6; i++)
    {
        p = i * 4;
        occRow[p + 0] = MLX90640_NIBBLE1(mlx90640_Drv->mlx90640_eeprom[18 + i]);
        occRow[p + 1] = MLX90640_NIBBLE2(mlx90640_Drv->mlx90640_eeprom[18 + i]);
        occRow[p + 2] = MLX90640_NIBBLE3(mlx90640_Drv->mlx90640_eeprom[18 + i]);
        occRow[p + 3] = MLX90640_NIBBLE4(mlx90640_Drv->mlx90640_eeprom[18 + i]);
    }

    for(int i = 0; i < MLX90640_LINE_NUM; i++)
    {
        if (occRow[i] > 7)
        {
            occRow[i] = occRow[i] - 16;
        }
    }

    for(int i = 0; i < 8; i++)
    {
        p = i * 4;
        occColumn[p + 0] = MLX90640_NIBBLE1(mlx90640_Drv->mlx90640_eeprom[24 + i]);
        occColumn[p + 1] = MLX90640_NIBBLE2(mlx90640_Drv->mlx90640_eeprom[24 + i]);
        occColumn[p + 2] = MLX90640_NIBBLE3(mlx90640_Drv->mlx90640_eeprom[24 + i]);
        occColumn[p + 3] = MLX90640_NIBBLE4(mlx90640_Drv->mlx90640_eeprom[24 + i]);
    }

    for(int i = 0; i < MLX90640_COLUMN_NUM; i ++)
    {
        if (occColumn[i] > 7)
        {
            occColumn[i] = occColumn[i] - 16;
        }
    }

    for(int i = 0; i < MLX90640_LINE_NUM; i++)
    {
        for(int j = 0; j < MLX90640_COLUMN_NUM; j ++)
        {
            p = 32 * i +j;
           mlx90640_Drv->offset[p] = (mlx90640_Drv->mlx90640_eeprom[64 + p] & MLX90640_MSBITS_6_MASK) >> 10;
            if (mlx90640_Drv->offset[p] > 31)
            {
               mlx90640_Drv->offset[p] =mlx90640_Drv->offset[p] - 64;
            }
           mlx90640_Drv->offset[p] =mlx90640_Drv->offset[p]*(1 << occRemScale);
           mlx90640_Drv->offset[p] = (offsetRef + (occRow[i] << occRowScale) + (occColumn[j] << occColumnScale) +mlx90640_Drv->offset[p]);
        }
    }

}

ITCM_AREA_CODE static void mlx9064_Extract_Kta_Pixel_Parameters(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
    int p = 0;
    int8_t KtaRC[4];
    uint8_t ktaScale1;
    uint8_t ktaScale2;
    uint8_t split;
    float ktaTemp[768];
    float temp;

    KtaRC[0] = (int8_t)MLX90640_MS_BYTE(mlx90640_Drv->mlx90640_eeprom[54]);;
    KtaRC[2] = (int8_t)MLX90640_LS_BYTE(mlx90640_Drv->mlx90640_eeprom[54]);;
    KtaRC[1] = (int8_t)MLX90640_MS_BYTE(mlx90640_Drv->mlx90640_eeprom[55]);;
    KtaRC[3] = (int8_t)MLX90640_LS_BYTE(mlx90640_Drv->mlx90640_eeprom[55]);;

    ktaScale1 = MLX90640_NIBBLE2(mlx90640_Drv->mlx90640_eeprom[56]) + 8;
    ktaScale2 = MLX90640_NIBBLE1(mlx90640_Drv->mlx90640_eeprom[56]);

    for(int i = 0; i < MLX90640_LINE_NUM; i++)
    {
        for(int j = 0; j < MLX90640_COLUMN_NUM; j ++)
        {
            p = 32 * i +j;
            split = 2*(p/32 - (p/64)*2) + p%2;
            ktaTemp[p] = (mlx90640_Drv->mlx90640_eeprom[64 + p] & 0x000E) >> 1;
            if (ktaTemp[p] > 3)
            {
                ktaTemp[p] = ktaTemp[p] - 8;
            }
            ktaTemp[p] = ktaTemp[p] * (1 << ktaScale2);
            ktaTemp[p] = KtaRC[split] + ktaTemp[p];
            ktaTemp[p] = ktaTemp[p] / POW2(ktaScale1);

        }
    }

    temp = fabs(ktaTemp[0]);
    for(int i = 1; i < MLX90640_PIXEL_NUM; i++)
    {
        if (fabs(ktaTemp[i]) > temp)
        {
            temp = fabs(ktaTemp[i]);
        }
    }

    ktaScale1 = 0;
    while(temp < 63.4)
    {
        temp = temp*2;
        ktaScale1 = ktaScale1 + 1;
    }

    for(int i = 0; i < MLX90640_PIXEL_NUM; i++)
    {
        temp = ktaTemp[i] * POW2(ktaScale1);
        if (temp < 0)
        {
        	mlx90640_Drv->kta[i] = (temp - 0.5);
        }
        else
        {
        	mlx90640_Drv->kta[i] = (temp + 0.5);
        }

    }

    mlx90640_Drv->ktaScale = ktaScale1;
}

ITCM_AREA_CODE static void mlx9064_Extract_Kv_Pixel_Parameters(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
    int p = 0;
    int8_t KvT[4];
    int8_t KvRoCo;
    int8_t KvRoCe;
    int8_t KvReCo;
    int8_t KvReCe;
    uint8_t kvScale;
    uint8_t split;
    float kvTemp[768];
    float temp;

    KvRoCo = MLX90640_NIBBLE4(mlx90640_Drv->mlx90640_eeprom[52]);
    if (KvRoCo > 7)
    {
        KvRoCo = KvRoCo - 16;
    }
    KvT[0] = KvRoCo;

    KvReCo = MLX90640_NIBBLE3(mlx90640_Drv->mlx90640_eeprom[52]);
    if (KvReCo > 7)
    {
        KvReCo = KvReCo - 16;
    }
    KvT[2] = KvReCo;

    KvRoCe = MLX90640_NIBBLE2(mlx90640_Drv->mlx90640_eeprom[52]);
    if (KvRoCe > 7)
    {
        KvRoCe = KvRoCe - 16;
    }
    KvT[1] = KvRoCe;

    KvReCe = MLX90640_NIBBLE1(mlx90640_Drv->mlx90640_eeprom[52]);
    if (KvReCe > 7)
    {
        KvReCe = KvReCe - 16;
    }
    KvT[3] = KvReCe;

    kvScale = MLX90640_NIBBLE3(mlx90640_Drv->mlx90640_eeprom[56]);


    for(int i = 0; i < MLX90640_LINE_NUM; i++)
    {
        for(int j = 0; j < MLX90640_COLUMN_NUM; j ++)
        {
            p = 32 * i +j;
            split = 2*(p/32 - (p/64)*2) + p%2;
            kvTemp[p] = KvT[split];
            kvTemp[p] = kvTemp[p] / POW2(kvScale);
        }
    }

    temp = fabs(kvTemp[0]);
    for(int i = 1; i < MLX90640_PIXEL_NUM; i++)
    {
        if (fabs(kvTemp[i]) > temp)
        {
            temp = fabs(kvTemp[i]);
        }
    }

    kvScale = 0;
    while(temp < 63.4)
    {
        temp = temp*2;
        kvScale = kvScale + 1;
    }

    for(int i = 0; i < MLX90640_PIXEL_NUM; i++)
    {
        temp = kvTemp[i] * POW2(kvScale);
        if (temp < 0)
        {
        	mlx90640_Drv->kv[i] = (temp - 0.5);
        }
        else
        {
        	mlx90640_Drv->kv[i] = (temp + 0.5);
        }

    }

    mlx90640_Drv->kvScale = kvScale;

}


ITCM_AREA_CODE static void mlx9064_Extract_Cilc_Pixel_Parameters(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
    float ilChessC[3];
    uint8_t calibrationModeEE;

    calibrationModeEE = (mlx90640_Drv->mlx90640_eeprom[10] & 0x0800) >> 4;
    calibrationModeEE = calibrationModeEE ^ 0x80;

    ilChessC[0] = (mlx90640_Drv->mlx90640_eeprom[53] & 0x003F);
    if (ilChessC[0] > 31)
    {
        ilChessC[0] = ilChessC[0] - 64;
    }
    ilChessC[0] = ilChessC[0] / 16.0f;

    ilChessC[1] = (mlx90640_Drv->mlx90640_eeprom[53] & 0x07C0) >> 6;
    if (ilChessC[1] > 15)
    {
        ilChessC[1] = ilChessC[1] - 32;
    }
    ilChessC[1] = ilChessC[1] / 2.0f;

    ilChessC[2] = (mlx90640_Drv->mlx90640_eeprom[53] & 0xF800) >> 11;
    if (ilChessC[2] > 15)
    {
        ilChessC[2] = ilChessC[2] - 32;
    }
    ilChessC[2] = ilChessC[2] / 8.0f;

    mlx90640_Drv->calibrationModeEE = calibrationModeEE;
    mlx90640_Drv->ilChessC[0] = ilChessC[0];
    mlx90640_Drv->ilChessC[1] = ilChessC[1];
    mlx90640_Drv->ilChessC[2] = ilChessC[2];

}

static int CheckAdjacentPixels(uint16_t pix1, uint16_t pix2)
{
int pixPosDif;
uint16_t lp1 = pix1 >> 5;
uint16_t lp2 = pix2 >> 5;
uint16_t cp1 = pix1 - (lp1 << 5);
uint16_t cp2 = pix2 - (lp2 << 5);

    pixPosDif = lp1 - lp2;
    if(pixPosDif > -2 && pixPosDif < 2)
    {
       pixPosDif = cp1 - cp2;
       if(pixPosDif > -2 && pixPosDif < 2)
       {
           return -MLX90640_ADJACENT_BAD_PIXELS_ERROR;
       }
    }
    return 0;
}

ITCM_AREA_CODE int32_t mlx9064_Extract_Deviating_Pixels(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
uint16_t pixCnt = 0;
uint16_t brokenPixCnt = 0;
uint16_t outlierPixCnt = 0;
int warn = 0;
int i;

    for(pixCnt = 0; pixCnt<5; pixCnt++)
    {
    	mlx90640_Drv->brokenPixels[pixCnt] = 0xFFFF;
    	mlx90640_Drv->outlierPixels[pixCnt] = 0xFFFF;
    }

    pixCnt = 0;
    while (pixCnt < MLX90640_PIXEL_NUM && brokenPixCnt < 5 && outlierPixCnt < 5)
    {
        if(mlx90640_Drv->mlx90640_eeprom[pixCnt+64] == 0)
        {
        	mlx90640_Drv->brokenPixels[brokenPixCnt] = pixCnt;
            brokenPixCnt = brokenPixCnt + 1;
        }
        else if((mlx90640_Drv->mlx90640_eeprom[pixCnt+64] & 0x0001) != 0)
        {
        	mlx90640_Drv->outlierPixels[outlierPixCnt] = pixCnt;
            outlierPixCnt = outlierPixCnt + 1;
        }

        pixCnt = pixCnt + 1;

    }

    if(brokenPixCnt > 4)
    {
        warn = -MLX90640_BROKEN_PIXELS_NUM_ERROR;
    }
    else if(outlierPixCnt > 4)
    {
        warn = -MLX90640_OUTLIER_PIXELS_NUM_ERROR;
    }
    else if((brokenPixCnt + outlierPixCnt) > 4)
    {
        warn = -MLX90640_BAD_PIXELS_NUM_ERROR;
    }
    else
    {
        for(pixCnt=0; pixCnt<brokenPixCnt; pixCnt++)
        {
            for(i=pixCnt+1; i<brokenPixCnt; i++)
            {
                warn = CheckAdjacentPixels(mlx90640_Drv->brokenPixels[pixCnt],mlx90640_Drv->brokenPixels[i]);
                if(warn != 0)
                {
                    return warn;
                }
            }
        }

        for(pixCnt=0; pixCnt<outlierPixCnt; pixCnt++)
        {
            for(i=pixCnt+1; i<outlierPixCnt; i++)
            {
                warn = CheckAdjacentPixels(mlx90640_Drv->outlierPixels[pixCnt],mlx90640_Drv->outlierPixels[i]);
                if(warn != 0)
                {
                    return warn;
                }
            }
        }

        for(pixCnt=0; pixCnt<brokenPixCnt; pixCnt++)
        {
            for(i=0; i<outlierPixCnt; i++)
            {
                warn = CheckAdjacentPixels(mlx90640_Drv->brokenPixels[pixCnt],mlx90640_Drv->outlierPixels[i]);
                if(warn != 0)
                {
                    return warn;
                }
            }
        }

    }
    return warn;
}

ITCM_AREA_CODE static uint32_t mlx9064_ExtractParameters(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
	if ( mlx90640_read_eeprom(mlx90640_Drv) )
		return 1;
	mlx9064_Extract_VDD_Parameters(mlx90640_Drv);
	mlx9064_Extract_PTAT_Parameters(mlx90640_Drv);
	mlx9064_Extract_Gain_TGC_Resolution_Parameters(mlx90640_Drv);
	mlx9064_Extract_KS_Parameters(mlx90640_Drv);
	mlx9064_Extract_CPP_Parameters(mlx90640_Drv);
	mlx9064_Extract_Alpha_Parameters(mlx90640_Drv);
	mlx9064_Extract_Offset_Parameters(mlx90640_Drv);
	mlx9064_Extract_Kta_Pixel_Parameters(mlx90640_Drv);
	mlx9064_Extract_Kv_Pixel_Parameters(mlx90640_Drv);
	mlx9064_Extract_Cilc_Pixel_Parameters(mlx90640_Drv);
	return 0;
}

ITCM_AREA_CODE static uint32_t IsPixelBad(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv,uint16_t pixel)
{
    for(uint32_t i=0; i<5; i++)
    {
        if(pixel == mlx90640_Drv->outlierPixels[i] || pixel == mlx90640_Drv->brokenPixels[i])
        {
            return 1;
        }
    }

    return 0;
}

static float GetMedian(float *values, int n)
 {
    float temp;

    for(int i=0; i<n-1; i++)
    {
        for(int j=i+1; j<n; j++)
        {
            if(values[j] < values[i])
            {
                temp = values[i];
                values[i] = values[j];
                values[j] = temp;
            }
        }
    }

    if(n%2==0)
    {
        return ((values[n/2] + values[n/2 - 1]) / 2.0);

    }
    else
    {
        return values[n/2];
    }

 }


ITCM_AREA_CODE void mlx9064_BadPixelsCorrection(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv,uint16_t *pixels, float *to, int mode)
{
float ap[4];
uint8_t pix;
uint8_t line;
uint8_t column;

    pix = 0;
    while(pixels[pix] != 0xFFFF)
    {
        line = pixels[pix]>>5;
        column = pixels[pix] - (line<<5);

        if(mode == 1)
        {
            if(line == 0)
            {
                if(column == 0)
                {
                    to[pixels[pix]] = to[33];
                }
                else if(column == 31)
                {
                    to[pixels[pix]] = to[62];
                }
                else
                {
                    to[pixels[pix]] = (to[pixels[pix]+31] + to[pixels[pix]+33])/2.0;
                }
            }
            else if(line == 23)
            {
                if(column == 0)
                {
                    to[pixels[pix]] = to[705];
                }
                else if(column == 31)
                {
                    to[pixels[pix]] = to[734];
                }
                else
                {
                    to[pixels[pix]] = (to[pixels[pix]-33] + to[pixels[pix]-31])/2.0;
                }
            }
            else if(column == 0)
            {
                to[pixels[pix]] = (to[pixels[pix]-31] + to[pixels[pix]+33])/2.0;
            }
            else if(column == 31)
            {
                to[pixels[pix]] = (to[pixels[pix]-33] + to[pixels[pix]+31])/2.0;
            }
            else
            {
                ap[0] = to[pixels[pix]-33];
                ap[1] = to[pixels[pix]-31];
                ap[2] = to[pixels[pix]+31];
                ap[3] = to[pixels[pix]+33];
                to[pixels[pix]] = GetMedian(ap,4);
            }
        }
        else
        {
            if(column == 0)
            {
                to[pixels[pix]] = to[pixels[pix]+1];
            }
            else if(column == 1 || column == 30)
            {
                to[pixels[pix]] = (to[pixels[pix]-1]+to[pixels[pix]+1])/2.0;
            }
            else if(column == 31)
            {
                to[pixels[pix]] = to[pixels[pix]-1];
            }
            else
            {
                if(IsPixelBad(mlx90640_Drv,pixels[pix]-2) == 0 && IsPixelBad(mlx90640_Drv,pixels[pix]+2) == 0)
                {
                    ap[0] = to[pixels[pix]+1] - to[pixels[pix]+2];
                    ap[1] = to[pixels[pix]-1] - to[pixels[pix]-2];
                    if(fabs(ap[0]) > fabs(ap[1]))
                    {
                        to[pixels[pix]] = to[pixels[pix]-1] + ap[1];
                    }
                    else
                    {
                        to[pixels[pix]] = to[pixels[pix]+1] + ap[0];
                    }
                }
                else
                {
                    to[pixels[pix]] = (to[pixels[pix]-1]+to[pixels[pix]+1])/2.0;
                }
            }
        }
        pix = pix + 1;
    }
}

ITCM_AREA_CODE void mlx9064_Calculate_To(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv, float emissivity, float tr)
{
float vdd;
float ta;
float ta4;
float tr4;
float taTr;
float gain;
float irDataCP[2];
float irData;
float alphaCompensated;
uint8_t mode;
int8_t ilPattern;
int8_t chessPattern;
int8_t pattern;
int8_t conversionPattern;
float Sx;
float To;
float alphaCorrR[4];
int8_t range;
uint16_t subPage;
float ktaScale;
float kvScale;
float alphaScale;
float kta;
float kv;

    subPage = mlx90640_Drv->frame_data[833];
    vdd = mlx906400_GetVdd(mlx90640_Drv);
    ta = mlx906400_GetTa(mlx90640_Drv);

    ta4 = (ta + 273.15);
    ta4 = ta4 * ta4;
    ta4 = ta4 * ta4;
    tr4 = (tr + 273.15);
    tr4 = tr4 * tr4;
    tr4 = tr4 * tr4;
    taTr = tr4 - (tr4-ta4)/emissivity;

    ktaScale = POW2(mlx90640_Drv->ktaScale);
    kvScale = POW2(mlx90640_Drv->kvScale);
    alphaScale = POW2(mlx90640_Drv->alphaScale);

    alphaCorrR[0] = 1 / (1 + mlx90640_Drv->ksTo[0] * 40);
    alphaCorrR[1] = 1 ;
    alphaCorrR[2] = (1 + mlx90640_Drv->ksTo[1] * mlx90640_Drv->ct[2]);
    alphaCorrR[3] = alphaCorrR[2] * (1 + mlx90640_Drv->ksTo[2] * (mlx90640_Drv->ct[3] - mlx90640_Drv->ct[2]));

//------------------------- Gain calculation -----------------------------------

    gain = (float)mlx90640_Drv->gainEE / (int16_t)mlx90640_Drv->frame_data[778];

//------------------------- To calculation -------------------------------------
    mode = (mlx90640_Drv->frame_data[832] & MLX90640_CTRL_MEAS_MODE_MASK) >> 5;

    irDataCP[0] = (int16_t)mlx90640_Drv->frame_data[776] * gain;
    irDataCP[1] = (int16_t)mlx90640_Drv->frame_data[808] * gain;

    irDataCP[0] = irDataCP[0] - mlx90640_Drv->cpOffset[0] * (1 + mlx90640_Drv->cpKta * (ta - 25)) * (1 + mlx90640_Drv->cpKv * (vdd - 3.3));
    if( mode ==  mlx90640_Drv->calibrationModeEE)
    {
        irDataCP[1] = irDataCP[1] - mlx90640_Drv->cpOffset[1] * (1 + mlx90640_Drv->cpKta * (ta - 25)) * (1 + mlx90640_Drv->cpKv * (vdd - 3.3));
    }
    else
    {
      irDataCP[1] = irDataCP[1] - (mlx90640_Drv->cpOffset[1] + mlx90640_Drv->ilChessC[0]) * (1 + mlx90640_Drv->cpKta * (ta - 25)) * (1 + mlx90640_Drv->cpKv * (vdd - 3.3));
    }

    for( int pixelNumber = 0; pixelNumber < 768; pixelNumber++)
    {
        ilPattern = pixelNumber / 32 - (pixelNumber / 64) * 2;
        chessPattern = ilPattern ^ (pixelNumber - (pixelNumber/2)*2);
        conversionPattern = ((pixelNumber + 2) / 4 - (pixelNumber + 3) / 4 + (pixelNumber + 1) / 4 - pixelNumber / 4) * (1 - 2 * ilPattern);

        if(mode == 0)
        {
          pattern = ilPattern;
        }
        else
        {
          pattern = chessPattern;
        }

        if(pattern == mlx90640_Drv->frame_data[833])
        {
            irData = (int16_t)mlx90640_Drv->frame_data[pixelNumber] * gain;

            kta = mlx90640_Drv->kta[pixelNumber]/ktaScale;
            kv = mlx90640_Drv->kv[pixelNumber]/kvScale;
            irData = irData - mlx90640_Drv->offset[pixelNumber]*(1 + kta*(ta - 25))*(1 + kv*(vdd - 3.3));

            if(mode !=  mlx90640_Drv->calibrationModeEE)
            {
              irData = irData + mlx90640_Drv->ilChessC[2] * (2 * ilPattern - 1) - mlx90640_Drv->ilChessC[1] * conversionPattern;
            }

            irData = irData - mlx90640_Drv->tgc * irDataCP[subPage];
            irData = irData / emissivity;

            alphaCompensated = SCALEALPHA*alphaScale/mlx90640_Drv->alpha[pixelNumber];
            alphaCompensated = alphaCompensated*(1 + mlx90640_Drv->KsTa * (ta - 25));

            Sx = alphaCompensated * alphaCompensated * alphaCompensated * (irData + alphaCompensated * taTr);
            Sx = sqrt(sqrt(Sx)) * mlx90640_Drv->ksTo[1];

            To = sqrt(sqrt(irData/(alphaCompensated * (1 - mlx90640_Drv->ksTo[1] * 273.15) + Sx) + taTr)) - 273.15;

            if(To < mlx90640_Drv->ct[1])
            {
                range = 0;
            }
            else if(To < mlx90640_Drv->ct[2])
            {
                range = 1;
            }
            else if(To < mlx90640_Drv->ct[3])
            {
                range = 2;
            }
            else
            {
                range = 3;
            }

            To = sqrt(sqrt(irData / (alphaCompensated * alphaCorrR[range] * (1 + mlx90640_Drv->ksTo[range] * (To - mlx90640_Drv->ct[range]))) + taTr)) - 273.15;

            mlx90640_Drv->to_data[pixelNumber] = To;
        }
    }
}

ITCM_AREA_CODE static int ValidateAuxData(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{

    if(mlx90640_Drv->aux_data[0] == 0x7FFF)
    	return -MLX90640_FRAME_DATA_ERROR;

    for(int i=8; i<19; i++)
    {
        if(mlx90640_Drv->aux_data[i] == 0x7FFF) return -MLX90640_FRAME_DATA_ERROR;
    }

    for(int i=20; i<23; i++)
    {
        if(mlx90640_Drv->aux_data[i] == 0x7FFF) return -MLX90640_FRAME_DATA_ERROR;
    }

    for(int i=24; i<33; i++)
    {
        if(mlx90640_Drv->aux_data[i] == 0x7FFF) return -MLX90640_FRAME_DATA_ERROR;
    }

    for(int i=40; i<51; i++)
    {
        if(mlx90640_Drv->aux_data[i] == 0x7FFF) return -MLX90640_FRAME_DATA_ERROR;
    }

    for(int i=52; i<55; i++)
    {
        if(mlx90640_Drv->aux_data[i] == 0x7FFF) return -MLX90640_FRAME_DATA_ERROR;
    }

    for(int i=56; i<64; i++)
    {
        if(mlx90640_Drv->aux_data[i] == 0x7FFF) return -MLX90640_FRAME_DATA_ERROR;
    }

    return MLX90640_NO_ERROR;

}

ITCM_AREA_CODE static int ValidateFrameData(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
    uint8_t line = 0;

    for(int i=0; i<MLX90640_PIXEL_NUM; i+=MLX90640_LINE_SIZE)
    {
        if((mlx90640_Drv->frame_data[i] == 0x7FFF) && (line%2 == mlx90640_Drv->frame_data[833]))
        	return -MLX90640_FRAME_DATA_ERROR;
        line = line + 1;
    }

    return MLX90640_NO_ERROR;
}

ITCM_AREA_CODE int mlx90640_GetFrameData(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
uint16_t dataReady = 0;
uint16_t statusRegister;
int error = 1;
uint16_t data[64];
uint8_t cnt = 0;

    while(dataReady == 0)
    {
    	statusRegister = mlx90640_read_register(mlx90640_Drv,MLX90640_STATUS_REG);
        if(statusRegister >= 0x100)
            return 1;
        dataReady = statusRegister & 0x0008;
    }

    if( mlx90640_write_register(mlx90640_Drv, MLX90640_STATUS_REG, MLX90640_INIT_STATUS_VALUE) )
        return 1;

    if ( mlx90640_read_frame_data(mlx90640_Drv) )
        return 1;

    if ( mlx90640_read_aux_data(mlx90640_Drv) )
        return 1;

    mlx90640_Drv->frame_data[832] = mlx90640_read_register(mlx90640_Drv,MLX90640_CTRL_REG);
    mlx90640_Drv->frame_data[833] = MLX90640_GET_FRAME(statusRegister);

    error = ValidateAuxData(mlx90640_Drv);
    if(error == MLX90640_NO_ERROR)
    {
        for(cnt=0; cnt<MLX90640_AUX_NUM; cnt++)
        {
        	mlx90640_Drv->frame_data[cnt+MLX90640_PIXEL_NUM] = data[cnt];
        }
    }

    error = ValidateFrameData(mlx90640_Drv);
    if (error != MLX90640_NO_ERROR)
    {
        return error;
    }

    return mlx90640_Drv->frame_data[833];
}


ITCM_AREA_CODE void  mlx90640_GetImage(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
float vdd;
float ta;
float gain;
float irDataCP[2];
float irData;
float alphaCompensated;
uint8_t mode;
int8_t ilPattern;
int8_t chessPattern;
int8_t pattern;
int8_t conversionPattern;
float image;
uint16_t subPage;
float ktaScale;
float kvScale;
float kta;
float kv;

	subPage = mlx90640_Drv->frame_data[833];
	vdd = mlx906400_GetVdd(mlx90640_Drv);
	ta = mlx906400_GetTa(mlx90640_Drv);


    ktaScale = POW2(mlx90640_Drv->ktaScale);
    kvScale = POW2(mlx90640_Drv->kvScale);

//------------------------- Gain calculation -----------------------------------

    gain = (float)mlx90640_Drv->gainEE / (int16_t)mlx90640_Drv->frame_data[778];

//------------------------- Image calculation -------------------------------------

    mode = (mlx90640_Drv->frame_data[832] & MLX90640_CTRL_MEAS_MODE_MASK) >> 5;

    irDataCP[0] = (int16_t)mlx90640_Drv->frame_data[776] * gain;
    irDataCP[1] = (int16_t)mlx90640_Drv->frame_data[808] * gain;

    irDataCP[0] = irDataCP[0] - mlx90640_Drv->cpOffset[0] * (1 + mlx90640_Drv->cpKta * (ta - 25)) * (1 + mlx90640_Drv->cpKv * (vdd - 3.3));
    if( mode ==  mlx90640_Drv->calibrationModeEE)
    {
        irDataCP[1] = irDataCP[1] - mlx90640_Drv->cpOffset[1] * (1 + mlx90640_Drv->cpKta * (ta - 25)) * (1 + mlx90640_Drv->cpKv * (vdd - 3.3));
    }
    else
    {
      irDataCP[1] = irDataCP[1] - (mlx90640_Drv->cpOffset[1] + mlx90640_Drv->ilChessC[0]) * (1 + mlx90640_Drv->cpKta * (ta - 25)) * (1 + mlx90640_Drv->cpKv * (vdd - 3.3));
    }

    for( int pixelNumber = 0; pixelNumber < 768; pixelNumber++)
    {
        ilPattern = pixelNumber / 32 - (pixelNumber / 64) * 2;
        chessPattern = ilPattern ^ (pixelNumber - (pixelNumber/2)*2);
        conversionPattern = ((pixelNumber + 2) / 4 - (pixelNumber + 3) / 4 + (pixelNumber + 1) / 4 - pixelNumber / 4) * (1 - 2 * ilPattern);

        if(mode == 0)
        {
          pattern = ilPattern;
        }
        else
        {
          pattern = chessPattern;
        }

        if(pattern == mlx90640_Drv->frame_data[833])
        {
            irData = (int16_t)mlx90640_Drv->frame_data[pixelNumber] * gain;

            kta = mlx90640_Drv->kta[pixelNumber]/ktaScale;
            kv = mlx90640_Drv->kv[pixelNumber]/kvScale;
            irData = irData - mlx90640_Drv->offset[pixelNumber]*(1 + kta*(ta - 25))*(1 + kv*(vdd - 3.3));

            if(mode !=  mlx90640_Drv->calibrationModeEE)
            {
              irData = irData + mlx90640_Drv->ilChessC[2] * (2 * ilPattern - 1) - mlx90640_Drv->ilChessC[1] * conversionPattern;
            }

            irData = irData - mlx90640_Drv->tgc * irDataCP[subPage];

            alphaCompensated = mlx90640_Drv->alpha[pixelNumber];

            image = irData*alphaCompensated;

            mlx90640_Drv->resulting_image[pixelNumber] = image;
        }
    }
}

ITCM_AREA_CODE uint32_t mlx90640_run(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv,float emissivity,float eTa)
{
	mlx90640_GetFrameData(mlx90640_Drv);
	mlx9064_Calculate_To(mlx90640_Drv,emissivity, eTa);
	return 0;
}


ITCM_AREA_CODE uint32_t mlx90640_register(I2C_Mlx90640_Drv_TypeDef *mlx90640_Drv)
{
I2C_DriverStruct_t *eptr, *pre_eptr;
	/* sanity check */

	if ( mlx90640_Drv->bus == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( HAL_I2C_IsDeviceReady(mlx90640_Drv->bus,mlx90640_Drv->device_address,5,MLX90640_TIMEOUT) )
		return DRIVER_REQUEST_FAILED;

	if ( mlx90640_Drv->mlx90640_eeprom == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( mlx90640_Drv->frame_data == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( mlx90640_Drv->resulting_image == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( mlx90640_Drv->alpha == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( mlx90640_Drv->offset == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( mlx90640_Drv->kta == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( mlx90640_Drv->kv == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( mlx90640_Drv->alphaTemp == NULL)
		return DRIVER_REQUEST_FAILED;

	if ( i2c_drv_ptr == NULL)
	{
		i2c_drv_ptr = (I2C_DriverStruct_t *)mlx90640_Drv;
		mlx90640_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = i2c_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (I2C_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)mlx90640_Drv;
		mlx90640_Drv->next_drv = NULL;
	}
	mlx90640_Drv->process = get_current_process();
	if ( mlx90640_Drv->device_address == 0 )
		mlx90640_Drv->device_address = MLX90640_ADDRESS;
	mlx90640_Drv->mlx90640_status_reg   = mlx90640_read_register(mlx90640_Drv,MLX90640_STATUS_REG);
	mlx90640_Drv->mlx90640_control_reg  = mlx90640_read_register(mlx90640_Drv,MLX90640_CTRL_REG);
	mlx90640_Drv->mlx90640_i2c_conf_reg = mlx90640_read_register(mlx90640_Drv,MLX90640_I2C_CONF_REG);
	mlx9064_ExtractParameters(mlx90640_Drv);
	mlx90640_Drv->mlx90640_found = 1;
	return 0;
}

#endif // #ifdef A_OS_I2C_ENABLED


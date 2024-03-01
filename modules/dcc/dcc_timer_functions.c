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
 * dcc_timer_functions.c
 *
 *  Created on: Mar 1, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef DDC_SYSTEM

static void dcc_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma)
{
  TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  if (hdma == htim->hdma[TIM_DMA_ID_CC1])
  {
    htim->Channel = HAL_TIM_ACTIVE_CHANNEL_1;

    if (hdma->Init.Mode == DMA_NORMAL)
    {
      TIM_CHANNEL_STATE_SET(htim, TIM_CHANNEL_1, HAL_TIM_CHANNEL_STATE_READY);
    }
  }
  else if (hdma == htim->hdma[TIM_DMA_ID_CC2])
  {
    htim->Channel = HAL_TIM_ACTIVE_CHANNEL_2;

    if (hdma->Init.Mode == DMA_NORMAL)
    {
      TIM_CHANNEL_STATE_SET(htim, TIM_CHANNEL_2, HAL_TIM_CHANNEL_STATE_READY);
    }
  }
  else if (hdma == htim->hdma[TIM_DMA_ID_CC3])
  {
    htim->Channel = HAL_TIM_ACTIVE_CHANNEL_3;

    if (hdma->Init.Mode == DMA_NORMAL)
    {
      TIM_CHANNEL_STATE_SET(htim, TIM_CHANNEL_3, HAL_TIM_CHANNEL_STATE_READY);
    }
  }
  else if (hdma == htim->hdma[TIM_DMA_ID_CC4])
  {
    htim->Channel = HAL_TIM_ACTIVE_CHANNEL_4;

    if (hdma->Init.Mode == DMA_NORMAL)
    {
      TIM_CHANNEL_STATE_SET(htim, TIM_CHANNEL_4, HAL_TIM_CHANNEL_STATE_READY);
    }
  }
  else
  {
    /* nothing to do */
  }

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
  htim->PWM_PulseFinishedCallback(htim);
#else
  HAL_TIM_PWM_PulseFinishedCallback(htim);
#endif /* USE_HAL_TIM_REGISTER_CALLBACKS */

  htim->Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
}

/**
  * @brief  Starts the TIM PWM signal generation in DMA mode.
  * @param  htim TIM PWM handle
  * @param  Channel TIM Channels to be enabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @param  pData The source Buffer address.
  * @param  Length The length of data to be transferred from memory to TIM peripheral
  * @retval HAL status
  */
HAL_StatusTypeDef dcc_HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, const uint32_t *pData, uint16_t Length)
{
HAL_StatusTypeDef status = HAL_OK;
	TIM_CHANNEL_STATE_SET(htim, Channel, HAL_TIM_CHANNEL_STATE_BUSY);

	/* Set the DMA compare callbacks */
	htim->hdma[TIM_DMA_ID_CC1]->XferCpltCallback = dcc_TIM_DMADelayPulseCplt;
	htim->hdma[TIM_DMA_ID_CC1]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

	/* Set the DMA error callback */
	htim->hdma[TIM_DMA_ID_CC1]->XferErrorCallback = TIM_DMAError ;

	/* Enable the DMA stream */
	//      if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)pData, (uint32_t)&htim->Instance->CCR1, Length) != HAL_OK)
	if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)pData, (uint32_t)&htim->Instance->PSC,Length) != HAL_OK)
	{
		/* Return error status */
		return HAL_ERROR;
	}

	/* Enable the TIM Capture/Compare 1 DMA request */
	__HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);

	/* Enable the Capture compare channel */
	TIM_CCxChannelCmd(htim->Instance, Channel, TIM_CCx_ENABLE);
	__HAL_TIM_ENABLE(htim);
	/* Return function status */
	return status;
}

#endif

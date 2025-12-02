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
 * sd_diskio.c
 *
 *  Created on: Nov 19, 2025
 *      Author: fil
 */


#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#ifdef A_OS_SDCARD_ENABLED

#include "../../modules/fat/ff_gen_drv.h"
#include "sd_diskio.h"
#include <string.h>


#if defined(ENABLE_SCRATCH_BUFFER)
#if defined (ENABLE_SD_DMA_CACHE_MAINTENANCE)
ALIGN_32BYTES(static uint8_t scratch[BLOCKSIZE]); // 32-Byte aligned for cache maintenance
#else
__ALIGN_BEGIN static uint8_t scratch[BLOCKSIZE] __ALIGN_END;
#endif
#endif

static volatile DSTATUS Stat = STA_NOINIT;

uint8_t  WriteStatus = 0, ReadStatus = 0;

static DSTATUS SD_CheckStatus(BYTE lun);
DSTATUS SD_initialize (BYTE);
DSTATUS SD_status (BYTE);
DRESULT SD_read (BYTE, BYTE*, DWORD, UINT);
#if _USE_WRITE == 1
	DRESULT SD_write (BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
	DRESULT SD_ioctl (BYTE, BYTE, void*);
#endif  /* _USE_IOCTL == 1 */

const Diskio_drvTypeDef  SD_Driver =
{
	SD_initialize,
	SD_status,
	SD_read,
	#if  _USE_WRITE == 1
	SD_write,
	#endif /* _USE_WRITE == 1 */
	#if  _USE_IOCTL == 1
	SD_ioctl,
#endif /* _USE_IOCTL == 1 */
};

static int SD_CheckStatusWithTimeout(uint32_t timeout)
{
  uint32_t timer = HAL_GetTick();
  /* block until SDIO IP is ready again or a timeout occur */
  while(HAL_GetTick() - timer < timeout)
  {
    if (SD_GetCardState() == SD_TRANSFER_OK)
    {
      return 0;
    }
  }

  return -1;
}

static DSTATUS SD_CheckStatus(BYTE lun)
{
  Stat = STA_NOINIT;

  if(SD_GetCardState() == MSD_OK)
  {
    Stat &= ~STA_NOINIT;
  }

  return Stat;
}

DSTATUS SD_initialize(BYTE lun)
{
	return SD_CheckStatus(lun);
}

DSTATUS SD_status(BYTE lun)
{
  return SD_CheckStatus(lun);
}

DRESULT SD_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR;
  uint32_t timeout;
#if defined(ENABLE_SCRATCH_BUFFER)
  uint8_t ret;
#endif
#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
  uint32_t alignedAddr;
#endif

  if (SD_CheckStatusWithTimeout(SD_TIMEOUT) < 0)
  {
    return res;
  }

#if defined(ENABLE_SCRATCH_BUFFER)
  if (!((uint32_t)buff & 0x3))
  {
#endif
    if(SD_ReadBlocks_DMA((uint32_t*)buff,
                             (uint32_t) (sector),
                             count) == MSD_OK)
    {
      ReadStatus = 0;
      /* Wait that the reading process is completed or a timeout occurs */
      timeout = HAL_GetTick();
      while((ReadStatus == 0) && ((HAL_GetTick() - timeout) < SD_TIMEOUT))
      {
      }
      /* in case of a timeout return error */
      if (ReadStatus == 0)
      {
        res = RES_ERROR;
      }
      else
      {
        ReadStatus = 0;
        timeout = HAL_GetTick();

        while((HAL_GetTick() - timeout) < SD_TIMEOUT)
        {
          if (SD_GetCardState() == SD_TRANSFER_OK)
          {
            res = RES_OK;
#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
            /*
            the SCB_InvalidateDCache_by_Addr() requires a 32-Byte aligned address,
            adjust the address and the D-Cache size to invalidate accordingly.
            */
            alignedAddr = (uint32_t)buff & ~0x1F;
            SCB_InvalidateDCache_by_Addr((uint32_t*)alignedAddr, count*BLOCKSIZE + ((uint32_t)buff - alignedAddr));
#endif
            break;
          }
        }
      }
    }
#if defined(ENABLE_SCRATCH_BUFFER)
  }
    else
    {
      /* Slow path, fetch each sector a part and memcpy to destination buffer */
      int i;

      for (i = 0; i < count; i++) {
        ret = BSP_SD_ReadBlocks_DMA((uint32_t*)scratch, (uint32_t)sector++, 1);
        if (ret == MSD_OK) {
          /* wait until the read is successful or a timeout occurs */

          timeout = HAL_GetTick();
          while((ReadStatus == 0) && ((HAL_GetTick() - timeout) < SD_TIMEOUT))
          {
          }
          if (ReadStatus == 0)
          {
            res = RES_ERROR;
            break;
          }
          ReadStatus = 0;

#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
          /*
          *
          * invalidate the scratch buffer before the next read to get the actual data instead of the cached one
          */
          SCB_InvalidateDCache_by_Addr((uint32_t*)scratch, BLOCKSIZE);
#endif
          memcpy(buff, scratch, BLOCKSIZE);
          buff += BLOCKSIZE;
        }
        else
        {
          break;
        }
      }

      if ((i == count) && (ret == MSD_OK))
        res = RES_OK;
    }
#endif

  return res;
}

#if _USE_WRITE == 1

DRESULT SD_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR;
  uint32_t timeout;
#if defined(ENABLE_SCRATCH_BUFFER)
  uint8_t ret;
  int i;
#endif

   WriteStatus = 0;
#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
  uint32_t alignedAddr;
#endif

  if (SD_CheckStatusWithTimeout(SD_TIMEOUT) < 0)
  {
    return res;
  }

#if defined(ENABLE_SCRATCH_BUFFER)
  if (!((uint32_t)buff & 0x3))
  {
#endif
#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)

    /*
    the SCB_CleanDCache_by_Addr() requires a 32-Byte aligned address
    adjust the address and the D-Cache size to clean accordingly.
    */
    alignedAddr = (uint32_t)buff &  ~0x1F;
    SCB_CleanDCache_by_Addr((uint32_t*)alignedAddr, count*BLOCKSIZE + ((uint32_t)buff - alignedAddr));
#endif

    if(SD_WriteBlocks_DMA((uint32_t*)buff,
                              (uint32_t)(sector),
                              count) == MSD_OK)
    {
      /* Wait that writing process is completed or a timeout occurs */

      timeout = HAL_GetTick();
      while((WriteStatus == 0) && ((HAL_GetTick() - timeout) < SD_TIMEOUT))
      {
      }
      /* in case of a timeout return error */
      if (WriteStatus == 0)
      {
        res = RES_ERROR;
      }
      else
      {
        WriteStatus = 0;
        timeout = HAL_GetTick();

        while((HAL_GetTick() - timeout) < SD_TIMEOUT)
        {
          if (SD_GetCardState() == SD_TRANSFER_OK)
          {
            res = RES_OK;
            break;
          }
        }
      }
    }
#if defined(ENABLE_SCRATCH_BUFFER)
  }
    else
    {
      /* Slow path, fetch each sector a part and memcpy to destination buffer */
#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
      /*
      * invalidate the scratch buffer before the next write to get the actual data instead of the cached one
      */
      SCB_InvalidateDCache_by_Addr((uint32_t*)scratch, BLOCKSIZE);
#endif

      for (i = 0; i < count; i++)
      {
        WriteStatus = 0;

        memcpy((void *)scratch, (void *)buff, BLOCKSIZE);
        buff += BLOCKSIZE;

        ret = BSP_SD_WriteBlocks_DMA((uint32_t*)scratch, (uint32_t)sector++, 1);
        if (ret == MSD_OK) {
          /* wait for a message from the queue or a timeout */
          timeout = HAL_GetTick();
          while((WriteStatus == 0) && ((HAL_GetTick() - timeout) < SD_TIMEOUT))
          {
          }
          if (WriteStatus == 0)
          {
            break;
          }

        }
        else
        {
          break;
        }
      }
      if ((i == count) && (ret == MSD_OK))
        res = RES_OK;
    }
#endif
  return res;
}
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  //BSP_SD_CardInfo CardInfo;
  HAL_SD_CardInfoTypeDef CardInfo;

  if (Stat & STA_NOINIT) return RES_NOTRDY;

  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;

  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
	  SD_GetCardInfo(&CardInfo);
    *(DWORD*)buff = CardInfo.LogBlockNbr;
    res = RES_OK;
    break;

  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
	  SD_GetCardInfo(&CardInfo);
    *(WORD*)buff = CardInfo.LogBlockSize;
    res = RES_OK;
    break;

  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
	  SD_GetCardInfo(&CardInfo);
    *(DWORD*)buff = CardInfo.LogBlockSize / SD_DEFAULT_BLOCK_SIZE;
    res = RES_OK;
    break;

  default:
    res = RES_PARERR;
  }

  return res;
}
#endif /* _USE_IOCTL == 1 */


#endif //#ifdef A_OS_SDCARD_ENABLED

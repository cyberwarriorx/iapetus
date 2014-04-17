/*  Copyright 2007,2013 Theo Berkau

    This file is part of Iapetus.

    Iapetus is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Iapetus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Iapetus; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "../iapetus.h"

#define SCUREG_D0R   (*(volatile u32 *)0x25FE0000)
#define SCUREG_D0W   (*(volatile u32 *)0x25FE0004)
#define SCUREG_D0C   (*(volatile u32 *)0x25FE0008)
#define SCUREG_D0AD  (*(volatile u32 *)0x25FE000C)
#define SCUREG_D0EN  (*(volatile u32 *)0x25FE0010)
#define SCUREG_D0MD  (*(volatile u32 *)0x25FE0014)

#define SCUREG_D1R   (*(volatile u32 *)0x25FE0020)
#define SCUREG_D1W   (*(volatile u32 *)0x25FE0024)
#define SCUREG_D1C   (*(volatile u32 *)0x25FE0028)
#define SCUREG_D1AD  (*(volatile u32 *)0x25FE002C)
#define SCUREG_D1EN  (*(volatile u32 *)0x25FE0030)
#define SCUREG_D1MD  (*(volatile u32 *)0x25FE0034)

#define SCUREG_D2R   (*(volatile u32 *)0x25FE0040)
#define SCUREG_D2W   (*(volatile u32 *)0x25FE0044)
#define SCUREG_D2C   (*(volatile u32 *)0x25FE0048)
#define SCUREG_D2AD  (*(volatile u32 *)0x25FE004C)
#define SCUREG_D2EN  (*(volatile u32 *)0x25FE0050)
#define SCUREG_D2MD  (*(volatile u32 *)0x25FE0054)

#define SCUREG_T0C   (*(volatile u32 *)0x25FE0090)
#define SCUREG_T1S   (*(volatile u32 *)0x25FE0094)
#define SCUREG_T1MD  (*(volatile u32 *)0x25FE0098)

#define SCUREG_IMS   (*(volatile u32 *)0x25FE00A0)
#define SCUREG_IST   (*(volatile u32 *)0x25FE00A4)

#define SCUREG_AIACK (*(volatile u32 *)0x25FE00A8)
#define SCUREG_ASR0  (*(volatile u32 *)0x25FE00B0)
#define SCUREG_ASR1  (*(volatile u32 *)0x25FE00B4)

#define SCUREG_RSEL  (*(volatile u32 *)0x25FE00C4)
#define SCUREG_VER   (*(volatile u32 *)0x25FE00C8)

//////////////////////////////////////////////////////////////////////////////

int scu_dma_init(void)
{
   scu_dma_stop_all();
   SCUREG_D0EN = 0;
   SCUREG_D1EN = 0;
   SCUREG_D2EN = 0;

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int scu_dma_start(int chan, void *src, void *dst, u32 size, u32 add, u32 mode)
{
   // Check first to make sure the channel has no pending DMA operation going
   switch (chan)
   {
      case 0:
         // Make sure everything is disabled
         SCUREG_D0EN = 0;
         // Write the read address
         SCUREG_D0R = (u32)src;
         // Write the write address
         SCUREG_D0W = (u32)dst;
         // Write the transfer number
         SCUREG_D0C = size;

         SCUREG_D0AD = add;
         SCUREG_D0MD = mode;

         if ((mode & 0x7) == 0x7)
            SCUREG_D0EN = 0x101;
         else
            SCUREG_D0EN = 0x100;
         break;
      case 1:
         // Make sure everything is disabled
         SCUREG_D1EN = 0;
         // Write the read address
         SCUREG_D1R = (u32)src;
         // Write the write address
         SCUREG_D1W = (u32)dst;
         // Write the transfer number
         SCUREG_D1C = size;

         SCUREG_D1AD = add;
         SCUREG_D1MD = mode;

         if ((mode & 0x7) == 0x7)
            SCUREG_D1EN = 0x101;
         else
            SCUREG_D1EN = 0x100;
         break;
      case 2:
         // Make sure everything is disabled
         SCUREG_D2EN = 0;
         // Write the read address
         SCUREG_D2R = (u32)src;
         // Write the write address
         SCUREG_D2W = (u32)dst;
         // Write the transfer number
         SCUREG_D2C = size;

         SCUREG_D2AD = add;
         SCUREG_D2MD = mode;

         if ((mode & 0x7) == 0x7)
            SCUREG_D2EN = 0x101;
         else
            SCUREG_D2EN = 0x100;
         break;
      default:
         return IAPETUS_ERR_INVALIDARG;
   }

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

BOOL is_scu_dma_running(int chan)
{
   switch (chan)
   {
      case 0:
//         if (SCUREG_DSTA & ?)
//            return IAPETUS_ERR_BUSY;
      case 1:
      case 2:
      default:
         return FALSE;
   }
}

//////////////////////////////////////////////////////////////////////////////

void scu_dma_stop_all(void)
{
   SCU_REG_DSTP = 1;
}

//////////////////////////////////////////////////////////////////////////////



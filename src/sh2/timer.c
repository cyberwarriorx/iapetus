/*  Copyright 2007 Theo Berkau

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

#define SH2_REG_WTCSR_R         (*(volatile u8 *)0xFFFFFE80)
#define SH2_REG_WTCSR_W(val)    (*((volatile u16 *)0xFFFFFE80) = 0xA500 | (u8)(val))
#define SH2_REG_WTCNT_R         (*(volatile u8 *)0xFFFFFE81)
#define SH2_REG_WTCNT_W(val)    (*((volatile u16 *)0xFFFFFE80) = 0x5A00 | (u8)(val))
                                                                          
#define SH2_REG_IPRA            (*(volatile u16 *)0xFFFFFEE2)
#define SH2_REG_VCRWDT          (*(volatile u16 *)0xFFFFFEE4)

#define SH2_REG_RTCSR_R           (*(volatile u32 *)0xFFFFFFF0)
#define SH2_REG_RTCSR_W(val)      (*((volatile u32 *)0xFFFFFFF0) = 0xA55A0000 | (u16)(val))
#define SH2_REG_RTCNT_R           (*(volatile u32 *)0xFFFFFFF4)
#define SH2_REG_RTCNT_W(val)      (*((volatile u32 *)0xFFFFFFF4) = 0xA55A0000 | (u16)(val))
#define SH2_REG_RTCOR_R           (*(volatile u32 *)0xFFFFFFF8)
#define SH2_REG_RTCOR_W(val)      (*((volatile u32 *)0xFFFFFFF8) = 0xA55A0000 | (u16)(val))

volatile u32 timercounter;
u32 timerfreq;

void TimerRTCIncrement(void) __attribute__ ((interrupt_handler));
void TimerFRTIncrement(void) __attribute__ ((interrupt_handler));
void TimerWDTIncrement(void) __attribute__ ((interrupt_handler));

//////////////////////////////////////////////////////////////////////////////

void TimerRTCIncrement(void)
{
   timercounter++;
}

//////////////////////////////////////////////////////////////////////////////

void TimerFRTIncrement(void)
{
   timercounter++;
}

//////////////////////////////////////////////////////////////////////////////

void TimerWDTIncrement(void)
{
   timercounter++;
}

//////////////////////////////////////////////////////////////////////////////

void TimerHBlankIncrement(void)
{
   timercounter++;
}

//////////////////////////////////////////////////////////////////////////////

int TimerSetup(int type, u32 *freq)
{
   u32 clock=0;
   int old_levelmask = InterruptGetLevelMask();

   if (freq == NULL)
      return LAPETUS_ERR_INVALIDPOINTER;

   freq[0] = 0;

   InterruptSetLevelMask(0xF);

   if (BIOS_GetClockSpeed == 0)
      clock = 26846587;
   else
      clock = 28636360;

   switch (type)
   {
      case TIMER_RTC:
         // Disable RTC
         SH2_REG_RTCSR_W(0);
         // Setup Interrupt
         BIOS_SetSH2Interrupt(0x7F, TimerRTCIncrement);
         // Setup vector
         SH2_REG_VCRWDT = 0x7F7F;
         // Setup level
         SH2_REG_IPRA = (0xF << 4);
         freq[0] = clock / 4;
         // Enable RTC
         SH2_REG_RTCNT_W(0);
         SH2_REG_RTCOR_W(0xFF);
         SH2_REG_RTCSR_W(0x40 | (0x7 << 3)); // I may change the increment speed
         break;
//      case TIMER_FRT:
         // Disable FRT

         // Setup so that FRC is cleared on compare match A
//         SH2_REG_FTCSR = 1; 
         // Setup Interrupt
//         BIOS_SetSH2Interrupt(0x7F, TimerFRTIncrement);
         // Setup vector
//         SH2_REG_VCRWDT = 0x7F7F;
         // Setup level
//         SH2_REG_IPRA = (0xF << 4);
//         freq[0] = clock / 4;
         // Enable FRT
//         SH2_REG_RTCNT = 0;
         // Setup the internal clock;
//         SH2_REG_TCR = 0; 
//         SH2_REG_FRC = 0;
//         SH2_REG_TIER = ???;
//         break;
      case TIMER_WDT:
         // Disable WDT interval timer
         SH2_REG_WTCSR_W(SH2_REG_WTCSR_R & 0x18);
         // Setup Interrupt
         BIOS_SetSH2Interrupt(0x7F, TimerWDTIncrement);
         // Setup vector
         SH2_REG_VCRWDT = 0x7F7F;
         // Setup level
         SH2_REG_IPRA = (0xF << 4);
         freq[0] = clock / 2 / 256; // double check this
         // Enable WDT interval timer
         SH2_REG_WTCNT_W(0);
         SH2_REG_WTCSR_W((SH2_REG_WTCSR_R & 0xDF) | 0x20 | 7);
         break;
      case TIMER_HBLANK:
         // Setup Interrupt
         BIOS_SetSCUInterrupt(0x42, TimerHBlankIncrement);
         BIOS_ChangeSCUInterruptMask(~MASK_HBLANKIN, 0);
         freq[0] = 224; // fix me

         if (old_levelmask > 0xC)
            old_levelmask = 0xC;

         break;
      default:
         return LAPETUS_ERR_INVALIDARG;
   }

   timercounter = 0;
   if (old_levelmask == 0xF)
      old_levelmask = 0xE;

   InterruptSetLevelMask(old_levelmask);

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

u32 TimerCounter()
{
   return timercounter;
}

//////////////////////////////////////////////////////////////////////////////

void TimerStop()
{
   BIOS_SetSH2Interrupt(0x7F, 0);  
}

//////////////////////////////////////////////////////////////////////////////


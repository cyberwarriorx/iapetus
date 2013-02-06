/*  Copyright 2005-2007 Theo Berkau

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

#include "iapetus.h"

//////////////////////////////////////////////////////////////////////

void InitIapetus(int res)
{
   int i;

   InterruptSetLevelMask(0xF);

   for (i = 0; i < 0x80; i++)
      BIOS_SetSH2Interrupt(i, 0);

   for (i = 0x40; i < 0x60; i++)
      BIOS_SetSCUInterrupt(i, 0);

   // Make sure all interrupts have been called
   BIOS_ChangeSCUInterruptMask(0, 0);
   BIOS_ChangeSCUInterruptMask(0xFFFFFFFF, 0xFFFFFFFF);

   vdp_init(res);
   PerInit();

   CommlinkStopService();

#ifdef DEBUG
   DebugInit();
#endif

   // If DSP is running, stop it
   if (DSPIsExec())
       DSPStop();

   if (InterruptGetLevelMask() > 0x7)
      InterruptSetLevelMask(0x7);
}

//////////////////////////////////////////////////////////////////////

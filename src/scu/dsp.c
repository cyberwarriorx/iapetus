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

//////////////////////////////////////////////////////////////////////////////

int dsp_load(u32 *program, u8 offset, u8 size)
{
   int i;
   u32 testval;

   // Clear out program control port
   testval = SCUREG_PPAF;

   if (size > (256 - offset))
      return LAPETUS_ERR_SIZE;

   // Make sure program is stopped, etc.
   SCUREG_PPAF = 0; 

   // Setup DSP so we can send a program to it
   SCUREG_PPAF = 0x8000 | offset;

   // Upload our program(Probably not the fastest, but meh, it's only 1K max)
   for (i = 0; i < size; i++)
   {
      SCUREG_PPD = program[i];
   }

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void dsp_exec(u8 PC)
{
   SCUREG_PPAF = 0;
   SCUREG_PPAF = 0x18000 | PC;
}

//////////////////////////////////////////////////////////////////////////////

int dsp_is_exec()
{
   return ((SCUREG_PPAF & 0x10000) >> 16);
}

//////////////////////////////////////////////////////////////////////////////

void dsp_stop()
{
   SCUREG_PPAF = 0;
}

//////////////////////////////////////////////////////////////////////////////

void dsp_pause()
{
   SCUREG_PPAF = SCUREG_PPAF | 0x2000000;
}

//////////////////////////////////////////////////////////////////////////////

void dsp_unpause()
{
   SCUREG_PPAF = SCUREG_PPAF | 0x4000000;
}

//////////////////////////////////////////////////////////////////////////////

void dsp_step()
{
   SCUREG_PPAF = (SCUREG_PPAF & 0xFF) | 0x20000;
}

//////////////////////////////////////////////////////////////////////////////


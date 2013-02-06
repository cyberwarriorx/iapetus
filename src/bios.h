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

#ifndef BIOS_H
#define BIOS_H

#define BIOS_RunCDPlayer() ((**(void(**)(void))0x0600026C)())
#define BIOS_IsMpegCardPresent(filtno) ((**(volatile int(**)(int))0x06000274)(filtno))
#define BIOS_GetMpegRom(index, size, addr) ((**(int(**)(u32, u32, u32))0x6000298)(index, size, addr))
#define BIOS_SetSCUInterrupt(vector, func) ((**(void(**)(u32, void *))0x06000300)((vector), (func)))
#define BIOS_GetSCUInterrupt(vector) ((void*)(**(void(*(**)(u32))(u32))0x6000304)(vector))
#define BIOS_SetSH2Interrupt(vector, func) ((**(void(**)(u32, void *))0x06000310)((vector), (func)))
#define BIOS_GetSH2Interrupt(vector) ((void*)(**(void(*(**)(u32))(u32))0x6000314)(vector))
#define BIOS_SetClockSpeed(mode) ((**(void(**)(u32))0x06000320)((mode)))
#define BIOS_GetClockSpeed (*(volatile u32*)0x6000324)
#define BIOS_SetSCUInterruptMask(bits) ((**(void(**)(u32))0x06000340)((bits)))
#define BIOS_ChangeSCUInterruptMask(mask, bits) ((**(volatile void(**)(u32, u32))0x06000344)((mask), (bits)))
#define BIOS_GetSCUInterruptMask() (*(volatile u32 *)0x06000348)

#endif

/*  Copyright 2005-2007,2013 Theo Berkau

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

#define bios_run_cd_player() ((**(void(**)(void))0x0600026C)())
#define bios_is_mpeg_card_present(filtno) ((**(int(**)(int))0x06000274)(filtno))
#define bios_get_mpeg_rom(index, size, addr) ((**(int(**)(u32, u32, u32))0x6000298)(index, size, addr))
#define bios_set_scu_interrupt(vector, func) ((**(void(**)(u32, void *))0x06000300)((vector), (func)))
#define bios_get_scu_interrupt(vector) ((void*)(**(void(*(**)(u32))(u32))0x6000304)(vector))
#define bios_set_sh2_interrupt(vector, func) ((**(void(**)(u32, void *))0x06000310)((vector), (func)))
#define bios_get_sh2_interrupt(vector) ((void*)(**(void(*(**)(u32))(u32))0x6000314)(vector))
#define bios_set_clock_speed(mode) ((**(void(**)(u32))0x06000320)((mode)))
#define bios_get_clock_speed (*(volatile u32*)0x6000324)
#define bios_set_scu_interrupt_mask(bits) ((**(void(**)(u32))0x06000340)((bits)))
#define bios_change_scu_interrupt_mask(mask, bits) ((**(void(**)(u32, u32))0x06000344)((mask), (bits)))
#define bios_get_scu_interrupt_mask() (*(volatile u32 *)0x06000348)

#endif

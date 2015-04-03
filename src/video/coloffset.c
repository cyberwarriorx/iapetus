/*  Copyright 2005-2007,2013,2015 Theo Berkau

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

extern vdp2_settings_struct vdp2_settings;

//////////////////////////////////////////////////////////////////////////////

void vdp_set_color_offset(u8 num, s16 r, s16 g, s16 b)
{
	if (num == 0)
	{
		// Offset A
		VDP2_REG_COAR = r & 0x1FF;
		VDP2_REG_COAG = g & 0x1FF;
		VDP2_REG_COAB = b & 0x1FF;
	}          
	else
	{
		// Offset B
		VDP2_REG_COBR = r & 0x1FF;
		VDP2_REG_COBG = g & 0x1FF;
		VDP2_REG_COBB = b & 0x1FF;
	}
}

//////////////////////////////////////////////////////////////////////////////

void vdp_enable_color_offset(u16 screen, int select)
{
	// Adjust select first
	screen = 1 << screen;

	if (select == 0)
		vdp2_settings.CLOFSL &= ~screen;
	else
		vdp2_settings.CLOFSL |= screen;

	// Adjust enable
	vdp2_settings.CLOFEN |= screen;

	// Write the new values to registers
	VDP2_REG_CLOFSL = vdp2_settings.CLOFSL;
	VDP2_REG_CLOFEN = vdp2_settings.CLOFEN;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_disable_color_offset(u16 screen)
{
	vdp2_settings.CLOFEN &= ~(1 << screen);
	VDP2_REG_CLOFEN = vdp2_settings.CLOFEN;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_fade_in(u16 screen, int num, int increment)
{
	int i;

	vdp_set_color_offset(num, 0x100, 0x100, 0x100);
	vdp_enable_color_offset(screen, num);
	for (i = 0; i < 256; i+=increment)
	{
		s16 offset;
		vdp_vsync();
		offset=0x100+i;
		vdp_set_color_offset(num, offset, offset, offset);
	}
	vdp_vsync();
	vdp_disable_color_offset(screen);
}

//////////////////////////////////////////////////////////////////////////////

void vdp_fade_out(u16 screen, int num, int increment)
{
	int i;

	vdp_set_color_offset(num, 0x1FF, 0x1FF, 0x1FF);
	vdp_enable_color_offset(screen, num);
	for (i = 0; i < 256; i+=increment)
	{
		s16 offset;
		vdp_vsync();
		offset=0x1FF-i;
		vdp_set_color_offset(num, offset, offset, offset);
	}
	vdp_vsync();
}

//////////////////////////////////////////////////////////////////////////////

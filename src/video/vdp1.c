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

int commandnum=0;

//////////////////////////////////////////////////////////////////////////////

void vdp_system_clipping(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(commandnum * 0x20));

   tbl->cmdctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0009;
   tbl->cmdlink = sprite->link_addr / 8;
   tbl->cmdxc = sprite->x;
   tbl->cmdyc = sprite->y;
   commandnum++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_user_clipping(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(commandnum * 0x20));

   tbl->cmdctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0008;
   tbl->cmdlink = sprite->link_addr / 8;
   tbl->cmdxa = sprite->x;
   tbl->cmdya = sprite->y;
   tbl->cmdxc = sprite->x2;
   tbl->cmdyc = sprite->y2;
   commandnum++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_local_coordinate(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(commandnum * 0x20));

   tbl->cmdctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x000A;
   tbl->cmdlink = sprite->link_addr / 8;
   tbl->cmdxa = sprite->x;
   tbl->cmdya = sprite->y;
   commandnum++;
}


//////////////////////////////////////////////////////////////////////////////

void vdp_draw_normal_sprite(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(commandnum * 0x20));

   tbl->cmdctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0000;
   tbl->cmdlink = sprite->link_addr / 8;
   tbl->cmdpmod.all = (u16)sprite->attr;
   tbl->cmdcolr = sprite->bank;
   tbl->cmdsrca = sprite->addr / 8;
   tbl->cmdsize = ((sprite->width / 8) << 8) | sprite->height;
   tbl->cmdxa = sprite->x;
   tbl->cmdya = sprite->y;
   tbl->cmdgrda = sprite->gouraud_addr / 8;
   commandnum++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_draw_scaled_sprite(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(commandnum * 0x20));

   tbl->cmdctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0001;
   tbl->cmdlink = sprite->link_addr / 8;
   tbl->cmdpmod.all = (u16)sprite->attr;
   tbl->cmdcolr = sprite->bank;
   tbl->cmdsrca = sprite->addr / 8;
   tbl->cmdsize = ((sprite->width / 8) << 8) | sprite->height;
   tbl->cmdxa = sprite->x;
   tbl->cmdya = sprite->y;
   tbl->cmdxb = sprite->x2;
   tbl->cmdyb = sprite->y2;
   tbl->cmdxc = sprite->x3;
   tbl->cmdyc = sprite->y3;
   tbl->cmdxd = sprite->x4;
   tbl->cmdyd = sprite->y4;
   tbl->cmdgrda = sprite->gouraud_addr / 8;
   commandnum++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_draw_distorted_sprite(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(commandnum * 0x20));

   tbl->cmdctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0002;
   tbl->cmdlink = sprite->link_addr / 8;
   tbl->cmdpmod.all = (u16)sprite->attr;
   tbl->cmdcolr = sprite->bank;
   tbl->cmdsrca = sprite->addr / 8;
   tbl->cmdsize = ((sprite->width / 8) << 8) | sprite->height;
   tbl->cmdxa = sprite->x;
   tbl->cmdya = sprite->y;
   tbl->cmdxb = sprite->x2;
   tbl->cmdyb = sprite->y2;
   tbl->cmdxc = sprite->x3;
   tbl->cmdyc = sprite->y3;
   tbl->cmdxd = sprite->x4;
   tbl->cmdyd = sprite->y4;
   tbl->cmdgrda = sprite->gouraud_addr / 8;
   commandnum++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_draw_polygon(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(commandnum * 0x20));

   tbl->cmdctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0004;
   tbl->cmdlink = sprite->link_addr / 8;
   tbl->cmdpmod.all = (u16)sprite->attr | 0xC0 ;
   tbl->cmdcolr = sprite->bank;
   tbl->cmdxa = sprite->x;
   tbl->cmdya = sprite->y;
   tbl->cmdxb = sprite->x2;
   tbl->cmdyb = sprite->y2;
   tbl->cmdxc = sprite->x3;
   tbl->cmdyc = sprite->y3;
   tbl->cmdxd = sprite->x4;
   tbl->cmdyd = sprite->y4;
   tbl->cmdgrda = sprite->gouraud_addr / 8;
   commandnum++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_draw_poly_line(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(commandnum * 0x20));

   tbl->cmdctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0005;
   tbl->cmdlink = sprite->link_addr / 8;
   tbl->cmdpmod.all = (u16)sprite->attr | 0xC0;
   tbl->cmdcolr = sprite->bank;
   tbl->cmdxa = sprite->x;
   tbl->cmdya = sprite->y;
   tbl->cmdxb = sprite->x2;
   tbl->cmdyb = sprite->y2;
   tbl->cmdxc = sprite->x3;
   tbl->cmdyc = sprite->y3;
   tbl->cmdxd = sprite->x4;
   tbl->cmdyd = sprite->y4;
   tbl->cmdgrda = sprite->gouraud_addr / 8;
   commandnum++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_draw_line(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(commandnum * 0x20));

   tbl->cmdctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0006;
   tbl->cmdlink = sprite->link_addr / 8;
   tbl->cmdpmod.all = (u16)sprite->attr | 0xC0;
   tbl->cmdcolr = sprite->bank;
   tbl->cmdxa = sprite->x;
   tbl->cmdya = sprite->y;
   tbl->cmdxb = sprite->x2;
   tbl->cmdyb = sprite->y2;
   tbl->cmdgrda = sprite->gouraud_addr / 8;
   commandnum++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_start_draw_kist()
{
   commandnum = 0;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_end_draw_list()
{
   *((volatile u16 *)(VDP1_RAM+(commandnum * 0x20))) = 0x8000;
}

//////////////////////////////////////////////////////////////////////////////

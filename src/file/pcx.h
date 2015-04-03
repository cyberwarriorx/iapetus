/*  Copyright 2015 Theo Berkau

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

#ifndef PCX_H
#define PCX_H

typedef struct
{
	u8 id;
	u8 version;
	u8 encoding;
	u8 bpp;
	u16 x;
	u16 y;
	u16 x2;
	u16 y2;
	u16 horz_res;
	u16 vert_res;
	u8 ega_pal[48];
	u8 reserved1;
	u8 num_planes;
	u16 bytes_per_line;
	u16 pal_type;
	u16 horz_size;
	u16 vert_size;
	u8 reserved2[54];
} pcx_header_struct;

enum IAPETUS_ERR pcx_load(u8 *pcx_data, size_t pcx_data_size, img_struct *img, u8 *static_buffer);


#endif

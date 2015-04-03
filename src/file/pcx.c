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

#include <string.h>
#include "../iapetus.h"

#define bswap16(x) (((x & 0xFF) << 8) | ((x >> 8) & 0xFF))

enum IAPETUS_ERR pcx_load(u8 *pcx_data, size_t pcx_data_size, img_struct *img, u8 *static_buffer)
{
   pcx_header_struct *header=(pcx_header_struct *)pcx_data;
	void *pal=pcx_data+pcx_data_size-768;
	int i;

	// Header Process
	if (header->id != 0x10 && header->id != 0x0A)
		return IAPETUS_ERR_BADHEADER;

	if (header->encoding != 1 || header->bpp != 8 || header->num_planes != 1)
		return IAPETUS_ERR_UNSUPPORTED;

	img->width = bswap16(header->x2)-bswap16(header->x)+1;
	img->height = bswap16(header->y2)-bswap16(header->y)+1;
	img->bpp = header->bpp;
	img->bytes_per_line = bswap16(header->bytes_per_line);
	if (static_buffer)
		img->data = static_buffer;
	else
	   img->data = malloc(img->width*img->height);

	if (img->data == NULL)
		return IAPETUS_ERR_OUTOFMEMORY;

	if (static_buffer)
		img->palette = static_buffer+(img->width*img->height);
	else
	   img->palette = malloc(256*3);

	if (img->palette == NULL)
	{
		free(img->data);
		return IAPETUS_ERR_OUTOFMEMORY;
	}

	pcx_data += sizeof(pcx_header_struct);
	i = 0;

	while(i <= img->width*img->height)
	{
		// Get next byte
		u8 data = pcx_data[0];
		pcx_data++;

		// Is it RLE?
		//if (data >= 192 && data <= 255)
		if ((data & 0xC0) == 0xC0)
		{
			int rle_length = data & 0x3F;

			// data to run
			data = pcx_data[0];
			pcx_data++;

			memset(img->data+i, data, rle_length);
			i+=rle_length;
		}
		else
			// Just copy the byte
			img->data[i++] = data;
	}

	memcpy(img->palette, pal, 256 * 3);

	return IAPETUS_ERR_OK;
}

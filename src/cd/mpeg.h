/*  Copyright 2006 Theo Berkau

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

#ifndef MPEG_H
#define MPEG_H

typedef struct
{
   u8 status;
   u8 playstatus;
   u16 vcounter;
   u8 pictureinfo;
   u8 mpegaudiostatus;
   u16 mpegvideostatus;
} mpegstat_struct;

BOOL IsMPEGCardPresent();
int IsMPEGAuth();
int MPEGAuth();
#endif

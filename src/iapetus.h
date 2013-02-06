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

#ifndef LAPETUS_H
#define LAPETUS_H

#include "bios.h"
#include "types.h"
#include "ar/arcomm.h"
#include "ar/commlink.h"
#include "cd/cd.h"
#include "cd/cdfs.h"
#include "cd/mpeg.h"
#include "debug/debug.h"
#include "modem/netlink.h"
#include "peripherals/smpc.h"
#include "scu/dsp.h"
#include "sh2/sh2dma.h"
#include "sh2/sh2int.h"
#include "sh2/sci.h"
#include "sh2/timer.h"
#include "sound/sound.h"
#include "ui/font.h"
#include "ui/gui.h"
#include "ui/text.h"
#include "video/vdp.h"

void InitIapetus(int res);

#define LAPETUS_ERR_OK                  0       // Everything is good
#define LAPETUS_ERR_COMM                -1      // Communication error
#define LAPETUS_ERR_HWNOTFOUND          -2      // Hardware not found
#define LAPETUS_ERR_SIZE                -3      // Invalid size specified
#define LAPETUS_ERR_INVALIDPOINTER      -4      // Invalid pointer passed
#define LAPETUS_ERR_INVALIDARG          -5      // Invalid argument passed
#define LAPETUS_ERR_BUSY                -6      // Hardware is busy
#define LAPETUS_ERR_UNKNOWN             -7      // Unknown error
#define LAPETUS_ERR_AUTH                -8      // Disc authentication error
#define LAPETUS_ERR_FILENOTFOUND        -9      // File not found error
#define LAPETUS_ERR_UNSUPPORTED         -10     // Unsupported feature

#endif

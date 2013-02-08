/*  Copyright 2006-2007,2013 Theo Berkau

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

#ifndef CD_H
#define CD_H

#include "../ui/font.h"

#define HIRQ_CMOK   0x0001
#define HIRQ_DRDY   0x0002
#define HIRQ_CSCT   0x0004
#define HIRQ_BFUL   0x0008
#define HIRQ_PEND   0x0010
#define HIRQ_DCHG   0x0020
#define HIRQ_ESEL   0x0040
#define HIRQ_EHST   0x0080
#define HIRQ_ECPY   0x0100
#define HIRQ_EFLS   0x0200
#define HIRQ_SCDQ   0x0400
#define HIRQ_MPED   0x0800
#define HIRQ_MPCM   0x1000
#define HIRQ_MPST   0x2000

#define STATUS_BUSY             0x00
#define STATUS_PAUSE            0x01
#define STATUS_STANDBY          0x02
#define STATUS_PLAY             0x03
#define STATUS_SEEK             0x04
#define STATUS_SCAN             0x05
#define STATUS_OPEN             0x06
#define STATUS_NODISC           0x07
#define STATUS_RETRY            0x08
#define STATUS_ERROR            0x09
#define STATUS_FATAL            0x0a
#define STATUS_PERIODIC         0x20
#define STATUS_TRANSFER         0x40
#define STATUS_WAIT             0x80
#define STATUS_REJECT           0xff

#define CDB_REG_HIRQ        *((volatile u16 *)0x25890008)
#define CDB_REG_HIRQMASK    *((volatile u16 *)0x2589000C)
#define CDB_REG_CR1         *((volatile u16 *)0x25890018)
#define CDB_REG_CR2         *((volatile u16 *)0x2589001C)
#define CDB_REG_CR3         *((volatile u16 *)0x25890020)
#define CDB_REG_CR4         *((volatile u16 *)0x25890024)
#define CDB_REG_DATATRNS    *((volatile u32 *)0x25818000)

typedef struct
{
   u16 CR1;
   u16 CR2;
   u16 CR3;
   u16 CR4;
} cd_cmd_struct;

typedef struct
{
   u8 status;
   u8 flag;
   u8 repeat_cnt;
   u8 ctrl_addr;
   u8 track;
   u8 index;
   u32 FAD;
} cd_stat_struct;

enum SECTORSIZE
{
   SECT_2048 = 0x0,
   SECT_2336 = 0x1,
   SECT_2340 = 0x2,
   SECT_2352 = 0x3
};

int cd_exec_command(u16 hirqmask, cd_cmd_struct *cd_cmd, cd_cmd_struct *cd_cmd_rs);
int cd_debug_exec_command(font_struct *font, u16 hirqmask, cd_cmd_struct *cd_cmd, cd_cmd_struct *cd_cmd_rs);
int cd_connect_cd_to_filter(int filternum);
int cd_init();
int is_cd_auth(u16 *disctypeauth);
int cd_auth();
int cd_stop_drive();
int cd_start_drive();
int is_cd_present();
int cd_read_sector(void *buffer, u32 FAD, int sectorsize, u32 numbytes);
int play_cd_audio(u8 audiotrack, u8 repeat, u8 vol_l, u8 vol_r);
int stop_cd_audio(void);
int cd_get_session_num(u8 *num);
#endif

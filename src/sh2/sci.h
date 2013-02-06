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

#ifndef SCI_H
#define SCI_H

#define SERIALMODE_ASYNC        0x00000000
#define SERIALMODE_CLOCKEDSYNC  0x00800000

int SciInit(u32 settings);
u8 SciTransferByteNowait(u8 val, BOOL *success);
void SciSendByte(u8 val);
u8 SciReceiveByte();
void SciSendWord(u16 val);
void SciSendLong(u32 val);
u16 SciReceiveWord();
u32 SciReceiveLong();
int SciRemoteExecute(void *buffer, u32 addr, u32 size);
int SciSyncOut();
int SciSyncIn();
void SciHandler();
#endif

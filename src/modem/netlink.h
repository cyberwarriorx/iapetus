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

#ifndef NETLINK_H
#define NETLINK_H

#define LSR_DATAAVAIL           0x01
#define LSR_OVERRUNERR          0x02
#define LSR_PARITYERR           0x04
#define LSR_FRAMINGERR          0x08
#define LSR_BSRECEIVED          0x10
#define LSR_THREMPTY            0x20
#define LSR_THREMPTYANDLINEIDLE 0x40
#define LSR_FIFODATAERROR       0x80

#define MSR_CTSCHANGE           0x01
#define MSR_DSRCHANGE           0x02
#define MSR_RITRAILEDGE         0x04
#define MSR_CDCHANGE            0x08
#define MSR_CTS                 0x10
#define MSR_DSR                 0x20
#define MSR_RI                  0x40
#define MSR_CD                  0x80

void WriteUART(u8 num, u8 val);
u8 ReadUART(u8 num);
int NetlinkInit();
void NetlinkTransmitCharacter(u8 val);
void NetlinkTransmitString(const char *string);
int NetlinkReceiveCharacter(u8 *data);

#define ReadRBR() ReadUART(0x0)
#define WriteTHR(n) WriteUART(0x0, n)

#define ReadIER() ReadUART(0x1)
#define WriteIER(n) WriteUART(0x1, n)

#define ReadIIR() ReadUART(0x2)
#define WriteFCR(n) WriteUART(0x2, n)

#define ReadLCR() ReadUART(0x3)
#define WriteLCR(n) WriteUART(0x3, n)

#define ReadMCR() ReadUART(0x4)
#define WriteMCR(n) WriteUART(0x4, n)

#define ReadLSR() ReadUART(0x5)

#define ReadMSR() ReadUART(0x6)

#define ReadSCR() ReadUART(0x7)
#define WriteSCR(n) WriteUART(0x7, n)

static inline u8 ReadDLL()
{
    u8 oldlcr;
    u8 ret;
    oldlcr = ReadLCR();
    WriteLCR(oldlcr | 0x80);
    ret = ReadUART(0x0);
    WriteLCR(oldlcr);
    return ret;
}
static inline void WriteDLL(u8 val)
{
    u8 oldlcr;
    oldlcr = ReadLCR();
    WriteLCR(oldlcr | 0x80);
    WriteUART(0x0, val);
    WriteLCR(oldlcr);
}

static inline u8 ReadDLM()
{
    u8 oldlcr;
    u8 ret;
    oldlcr = ReadLCR();
    WriteLCR(oldlcr | 0x80);
    ret = ReadUART(0x1);
    WriteLCR(oldlcr);
    return ret;
}
static inline void WriteDLM(u8 val)
{
    u8 oldlcr;
    oldlcr = ReadLCR();
    WriteLCR(oldlcr | 0x80);
    WriteUART(0x1, val);
    WriteLCR(oldlcr);
}

#endif

/*  Copyright 2006-2007 Theo Berkau

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

static int cdsectorsize = SECT_2048;
int sectorsizetbl[4] = { 2048, 2336, 2340, 2352 };

//////////////////////////////////////////////////////////////////////////////

void CDWriteCommand(cdcmd_struct *cdcmd)
{
   CDB_REG_CR1 = cdcmd->CR1;
   CDB_REG_CR2 = cdcmd->CR2;
   CDB_REG_CR3 = cdcmd->CR3;
   CDB_REG_CR4 = cdcmd->CR4;
}

//////////////////////////////////////////////////////////////////////////////

void CDReadReturnStatus(cdcmd_struct *cdcmdrs)
{
   cdcmdrs->CR1 = CDB_REG_CR1;
   cdcmdrs->CR2 = CDB_REG_CR2;
   cdcmdrs->CR3 = CDB_REG_CR3;
   cdcmdrs->CR4 = CDB_REG_CR4;
}         

//////////////////////////////////////////////////////////////////////////////

int CDWaitHirq(int flag)
{
   int i;
   u16 hirq_temp;
   
   for (i = 0; i < 0x240000; i++)
   {
      hirq_temp = CDB_REG_HIRQ;
      if (hirq_temp & flag)
         return TRUE;
   }
   return FALSE;
}

//////////////////////////////////////////////////////////////////////////////


int CDExecCommand(u16 hirqmask, cdcmd_struct *cdcmd, cdcmd_struct *cdcmdrs)
{
   int old_levelmask;
   u16 hirq_temp;
   u16 cdstatus;

   // Mask any interrupts, we don't need to be interrupted
   old_levelmask = InterruptGetLevelMask();
   InterruptSetLevelMask(0xF);

   hirq_temp = CDB_REG_HIRQ;

   // Make sure CMOK flag is set, or we can't continue
   if (!(hirq_temp & HIRQ_CMOK))
      return LAPETUS_ERR_BUSY;

   // Clear CMOK and any other user-defined flags
   CDB_REG_HIRQ = ~(hirqmask | HIRQ_CMOK);

   // Alright, time to execute the command
   CDWriteCommand(cdcmd);

   // Let's wait till the command operation is finished
   if (!CDWaitHirq(HIRQ_CMOK))
      return LAPETUS_ERR_BUSY;

   // Read return data
   CDReadReturnStatus(cdcmdrs);

   cdstatus = cdcmdrs->CR1 >> 8;

   // Was command good?
   if (cdstatus == STATUS_REJECT)
      return LAPETUS_ERR_BUSY;
   else if (cdstatus & STATUS_WAIT)
      return LAPETUS_ERR_BUSY;

   // return interrupts back to normal
   InterruptSetLevelMask(old_levelmask);

   // It's all good
   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int CDDebugExecCommand(font_struct *font, u16 hirqmask, cdcmd_struct *cdcmd, cdcmd_struct *cdcmdrs)
{
   int old_levelmask;
   u16 hirq_temp;

   // Mask any interrupts, we don't need to be interrupted
   old_levelmask = InterruptGetLevelMask();
   InterruptSetLevelMask(0xF);

   hirq_temp = CDB_REG_HIRQ;

   // Make sure CMOK flag is set, or we can't continue
   if (!(hirq_temp & HIRQ_CMOK))
      return LAPETUS_ERR_BUSY;

   // Clear CMOK and any other user-defined flags
   CDB_REG_HIRQ = ~(hirqmask | HIRQ_CMOK);

   // Alright, time to execute the command
   CDWriteCommand(cdcmd);

   // Go into an endless loop showing the HIRQ
   VdpPrintText(font, 2 * 8, 20 * 8, 15, "HIRQ = ");

   for (;;)
      VdpPrintf(font, 2 * 8, 20 * 8, 15, "%d", CDB_REG_HIRQ);

   // return interrupts back to normal
   InterruptSetLevelMask(old_levelmask);
}

//////////////////////////////////////////////////////////////////////////////

int CDCDBInit(int standby)
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   // CD Init Command
   cdcmd.CR1 = 0x0400;
   cdcmd.CR2 = standby;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x040F;

   return CDExecCommand(0, &cdcmd, &cdcmdrs);
}

//////////////////////////////////////////////////////////////////////////////

int CDEndTransfer()
{
   int ret;
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   cdcmd.CR1 = 0x0600;
   cdcmd.CR2 = 0x0000;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x0000;

   ret = CDExecCommand(0, &cdcmd, &cdcmdrs);

   CDB_REG_HIRQ = (~HIRQ_DRDY) | HIRQ_CMOK;

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int CDPlayFAD(int playmode, int startfad, int numsectors)
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;
   int ret;

   // Clear flags
   CDB_REG_HIRQ = ~(HIRQ_PEND|HIRQ_CSCT) | HIRQ_CMOK;

   cdcmd.CR1 = 0x1080 | (startfad >> 16);
   cdcmd.CR2 = startfad;
   cdcmd.CR3 = (playmode << 8) | 0x80 | (numsectors >> 16);
   cdcmd.CR4 = numsectors;

   ret = CDExecCommand(0, &cdcmd, &cdcmdrs);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int CDSeekFAD(int seekfad)
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   cdcmd.CR1 = 0x1180 | (seekfad >> 16);
   cdcmd.CR2 = seekfad;
   cdcmd.CR3 = 0;
   cdcmd.CR4 = 0;

   return CDExecCommand(0, &cdcmd, &cdcmdrs);
}

//////////////////////////////////////////////////////////////////////////////

int CDConnectCDToFilter(int filternum)
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   cdcmd.CR1 = 0x3000;
   cdcmd.CR2 = 0x0000;
   cdcmd.CR3 = filternum << 8;
   cdcmd.CR4 = 0x0000;

   return CDExecCommand(HIRQ_ESEL, &cdcmd, &cdcmdrs);
}

//////////////////////////////////////////////////////////////////////////////

static int CDSetFilterMode(int filternum, int mode)
{
   int ret;
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   cdcmd.CR1 = 0x4400 | (mode & 0xFF);
   cdcmd.CR2 = 0;
   cdcmd.CR3 = (filternum << 8);
   cdcmd.CR4 = 0;

   ret = CDExecCommand(HIRQ_ESEL, &cdcmd, &cdcmdrs);

   // Wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int CDSetFilterSubheaderConditions(int filternum) // fix me
{
   int ret;
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   cdcmd.CR1 = 0x4200 | 0x00; // fix me
   cdcmd.CR2 = 0x0000; // fix me
   cdcmd.CR3 = (filternum << 8) | 0x00; // fix me
   cdcmd.CR4 = 0x0000; // fix me

   ret = CDExecCommand(HIRQ_ESEL, &cdcmd, &cdcmdrs);

   // Wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int CDSetFilterConnection(int filternum, int connectflag, int truecon, int falsecon)
{
   int ret;
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   cdcmd.CR1 = 0x4600 | (connectflag & 0xFF);
   cdcmd.CR2 = (truecon << 8) | (falsecon & 0xFF);
   cdcmd.CR3 = (filternum << 8);
   cdcmd.CR4 = 0;

   ret = CDExecCommand(HIRQ_ESEL, &cdcmd, &cdcmdrs);

   // Wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int CDSetFilter(int filternum, int mode, int truecon, int falsecon)
{
   int ret;

   if ((ret = CDSetFilterMode(filternum, mode)) != 0)
      return ret;

   // fix me
//   if ((ret = CDSetFilterSubheaderConditions(filternum)) != 0)
//      return ret;

   // Connect filter 0's true condition to selector 0, false condition to selector NULL
   if ((ret = CDSetFilterConnection(filternum, 0x03, truecon, falsecon)) != 0)
      return ret;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

static int CDResetSelector(int resetflags, int selnum)
{
   int ret;
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   // Reset Selector Command
   cdcmd.CR1 = 0x4800 | ((u8)resetflags);
   cdcmd.CR2 = 0x0000;
   cdcmd.CR3 = (selnum << 8);
   cdcmd.CR4 = 0x0000;

   if ((ret = CDExecCommand(HIRQ_EFLS, &cdcmd, &cdcmdrs)) != 0)
      return ret;

   // wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int CDResetSelectorOne(int selnum)
{
   return CDResetSelector(0, selnum);
}

//////////////////////////////////////////////////////////////////////////////

int CDResetSelectorAll()
{
   return CDResetSelector(0xFC, 0);
}

//////////////////////////////////////////////////////////////////////////////

int CDIsDataReady(int selnum)
{
   int ret;
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   cdcmd.CR1 = 0x5100;
   cdcmd.CR2 = 0;
   cdcmd.CR3 = (selnum << 8);
   cdcmd.CR4 = 0;

   if ((ret = CDExecCommand(0, &cdcmd, &cdcmdrs)) != 0)
      return LAPETUS_ERR_OK;

   // Return the number of sectors ready
   return cdcmdrs.CR4;
}

//////////////////////////////////////////////////////////////////////////////

int CDSetSectorSize(int size)
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   cdsectorsize = size;

   cdcmd.CR1 = 0x6000 | (size & 0xFF);
   cdcmd.CR2 = size << 8;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x0000;

   return CDExecCommand(HIRQ_ESEL, &cdcmd, &cdcmdrs);
}

//////////////////////////////////////////////////////////////////////////////

int CDGetThenDeleteSectorData(int selnum, int sectorpos, int numsectors)
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   cdcmd.CR1 = 0x6300;
   cdcmd.CR2 = sectorpos;
   cdcmd.CR3 = selnum << 8;
   cdcmd.CR4 = numsectors;

   return CDExecCommand(HIRQ_EHST, &cdcmd, &cdcmdrs);
}

//////////////////////////////////////////////////////////////////////////////

int CDAbortFile()
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   // Abort File Command
   cdcmd.CR1 = 0x7500;
   cdcmd.CR2 = 0x0000;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x0000;
  
   return CDExecCommand(HIRQ_EFLS, &cdcmd, &cdcmdrs);
}

//////////////////////////////////////////////////////////////////////////////

int CDTransferData(u32 numsectors, u32 *buffer)
{
   u32 i;
   int ret;

   // Setup a transfer from cd buffer to wram, then delete data
   // from cd buffer
   if ((ret = CDGetThenDeleteSectorData(0, 0, numsectors)) != 0)
      return ret;

   // Wait till data is ready
   if (!CDWaitHirq(HIRQ_DRDY | HIRQ_EHST))
   	   return LAPETUS_ERR_BUSY;

   // Do transfer
   for (i = 0; i < ((numsectors * sectorsizetbl[cdsectorsize]) / 4); i++)
      buffer[i] = CDB_REG_DATATRNS; // this can also be done in word units as well

   if ((ret = CDEndTransfer()) != 0)
      return ret;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int CDTransferDataBytes(u32 numbytes, u32 *buffer)
{
   u32 i;
   int ret;
   int numsectors=numbytes / sectorsizetbl[cdsectorsize];

   if (numbytes % sectorsizetbl[cdsectorsize])
      numsectors++;

   // Setup a transfer from cd buffer to wram, then delete data
   // from cd buffer
   if ((ret = CDGetThenDeleteSectorData(0, 0, numsectors)) != 0)
      return ret;

   // Wait till data is ready
   if (!CDWaitHirq(HIRQ_DRDY | HIRQ_EHST))
   	   return LAPETUS_ERR_BUSY;

   // Do transfer
   for (i = 0; i < (numbytes >> 2); i++)
   {
      buffer[0] = CDB_REG_DATATRNS; // this can also be done in word units as well
      buffer++;
   }

   // Get the remainder
   if (numbytes % 4)
   {
      u32 data;
      u8 *datapointer=(u8 *)&data;

      data = CDB_REG_DATATRNS;

      for (i = 0; i < (numbytes % 4); i++)
         ((u8 *)buffer)[i] = datapointer[i];
   }

   if ((ret = CDEndTransfer()) != 0)
      return ret;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int CDInit()
{
   int ret;

   // Abort any file transfers that may be currently going
   if ((ret = CDAbortFile()) != 0)
      return ret;

   // Init CD Block
   if ((ret = CDCDBInit(0)) != 0)
      return ret;

   // End any previous cd buffer data transfers
   if ((ret = CDEndTransfer()) != 0)
      return ret;

   // Reset all buffer partitions, partition output connectors, all filter
   // conditions, all filter input connectors, etc.
   if ((ret = CDResetSelectorAll()) != 0)
      return ret;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int CDGetStat(cdstat_struct *cdstatus)
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;
   int ret;

   cdcmd.CR1 = 0x0000;
   cdcmd.CR2 = 0x0000;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x0000;

   if ((ret = CDExecCommand(0, &cdcmd, &cdcmdrs)) != 0)
      return ret;

   cdstatus->status = cdcmdrs.CR1 >> 8;
   cdstatus->flag = (cdcmdrs.CR1 >> 4) & 0xF;
   cdstatus->repeatcnt = cdcmdrs.CR1 & 0xF;
   cdstatus->ctrladdr = cdcmdrs.CR2 >> 8;
   cdstatus->track = cdcmdrs.CR2 & 0xFF;
   cdstatus->index = cdcmdrs.CR3 >> 8;
   cdstatus->FAD = ((cdcmdrs.CR3 & 0xFF) << 16) | cdcmdrs.CR4;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int IsCDAuth(u16 *disctypeauth)
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   cdcmd.CR1 = 0xE100;
   cdcmd.CR2 = 0x0000;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x0000;

   // If command fails, assume it's not authenticated
   if (CDExecCommand(0, &cdcmd, &cdcmdrs) != LAPETUS_ERR_OK)
      return FALSE;

   if (disctypeauth)
      *disctypeauth = cdcmdrs.CR2;

   // Disc type Authenticated:
   // 0x00: No CD/Not Authenticated
   // 0x01: Audio CD
   // 0x02: Regular Data CD(not Saturn disc)
   // 0x03: Copied/Pirated Saturn Disc
   // 0x04: Original Saturn Disc
   if (cdcmdrs.CR2 != 0x04 && cdcmdrs.CR2 != 0x02)
      return FALSE;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

int CDAuth()
{
   int ret;
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;
   u16 auth;
   cdstat_struct cdstatus;
   int i;

   // Clear hirq flags
   CDB_REG_HIRQ = ~(HIRQ_DCHG | HIRQ_EFLS);

   // Authenticate disc
   cdcmd.CR1 = 0xE000;
   cdcmd.CR2 = 0x0000;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x0000;

   if ((ret = CDExecCommand(HIRQ_EFLS, &cdcmd, &cdcmdrs)) != 0)
      return ret;

   // wait till operation is finished
   while (!(CDB_REG_HIRQ & HIRQ_EFLS)) {}

   // Wait until drive has finished seeking
   for (;;)
   {
      // wait a bit
      for (i = 0; i < 100000; i++) { }

      if (CDGetStat(&cdstatus) != 0) continue;

      if (cdstatus.status == STATUS_PAUSE) break;
      else if (cdstatus.status == STATUS_FATAL) return LAPETUS_ERR_UNKNOWN;
   }

   // Was Authentication successful?
   if (!IsCDAuth(&auth))
      return LAPETUS_ERR_AUTH;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int CDStopDrive()
{
   int ret;
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;
   cdstat_struct cdstatus;
   int i;

   // CD Init Command
   cdcmd.CR1 = 0x0400;
   cdcmd.CR2 = 0x0001;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x040F;

   if ((ret = CDExecCommand(0, &cdcmd, &cdcmdrs)) != 0)
      return ret;

   // Wait till operation is finished(fix me)

   // Wait till drive is stopped
   for (;;)
   {
      // wait a bit
      for (i = 0; i < 100000; i++) { }

      if (CDGetStat(&cdstatus) != 0) continue;

      if (cdstatus.status == STATUS_STANDBY) break;
      else if (cdstatus.status == STATUS_FATAL) return LAPETUS_ERR_UNKNOWN;
   }

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int CDStartDrive()
{
   int ret;
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   // CD Init Command
   cdcmd.CR1 = 0x0400;
   cdcmd.CR2 = 0x0000;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x040F;

   if ((ret = CDExecCommand(0, &cdcmd, &cdcmdrs)) != 0)
      return ret;

   // wait till operation is finished(fix me)

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int IsCDPresent()
{
   cdstat_struct cdstatus;

   // If command fails, assume disc isn't present
   if (CDGetStat(&cdstatus) != 0)
      return FALSE;

   // Check status
   switch (cdstatus.status & 0xF)
   {
      case STATUS_BUSY:
         return FALSE;
      case STATUS_OPEN:
         return FALSE;
      case STATUS_NODISC:
         return FALSE;
      default: break;
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

int CDReadSector(void *buffer, u32 FAD, int sectorsize, u32 numbytes)
{
   int ret;
   int done=0;
   // Figure out how many sectors we actually have to read
   int numsectors=numbytes / sectorsizetbl[cdsectorsize];

   if (numbytes % sectorsizetbl[cdsectorsize] != 0)
      numsectors++;

   if ((ret = CDSetSectorSize(sectorsize)) != 0)
      return ret;

   // Set Filter
/*
//   if ((ret = CDSetFilter(0, 1, 0, 0xFF)) != 0)
//   if ((ret = CDSetFilter(0, 0x80, 0, 0xFF)) != 0)
   if ((ret = CDSetFilter(0, 0x10, 0, 1)) != 0)
      return ret;
*/
   // Clear partition 0
   if ((ret = CDResetSelectorOne(0)) != 0)
      return ret;

   // Connect CD device to filter 0
   if ((ret = CDConnectCDToFilter(0)) != 0)
      return ret;

   // Start reading sectors
   if ((ret = CDPlayFAD(0, FAD, numsectors)) != 0)
      return ret;

   while (!done)
   {
      u32 sectorstoread=0;
      u32 bytestoread;

      // Wait until there's data ready
      while ((sectorstoread = CDIsDataReady(0)) == 0) {}

      if ((sectorstoread * sectorsizetbl[cdsectorsize]) > numbytes)
         bytestoread = numbytes;
      else
         bytestoread = sectorstoread * sectorsizetbl[cdsectorsize];

      // Setup a transfer from cd buffer to wram, then delete data
      // from cd buffer
      if ((ret = CDTransferDataBytes(bytestoread, buffer)) != LAPETUS_ERR_OK)
         return ret;

      numbytes -= bytestoread;
      buffer += bytestoread;

      if (numbytes == 0)
         done = 1;
   }

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int PlayCDAudio(u8 audiotrack, u8 repeat, u8 vol_l, u8 vol_r)
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;
   int ret;

   SoundExternalAudioEnable(vol_l, vol_r);

   if ((ret = CDSetSectorSize(2048)) != 0)
      return ret;

   // Clear partition 0
   if ((ret = CDResetSelectorOne(0)) != 0)
      return ret;

   // Connect CD device to filter 0
   if ((ret = CDConnectCDToFilter(0)) != 0)
      return ret;

   // Clear flags
   CDB_REG_HIRQ = ~(HIRQ_PEND|HIRQ_CSCT) | HIRQ_CMOK;

   cdcmd.CR1 = 0x1000;
   cdcmd.CR2 = (audiotrack << 8) | 0x01;
   cdcmd.CR3 = repeat << 8;
   cdcmd.CR4 = (audiotrack << 8) | 0x63;

   ret = CDExecCommand(0, &cdcmd, &cdcmdrs);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int StopCDAudio(void)
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   // Do a default seek, that should stop the cd from playing
   cdcmd.CR1 = 0x11FF;
   cdcmd.CR2 = 0xFFFF;
   cdcmd.CR3 = 0xFFFF;
   cdcmd.CR4 = 0xFFFF;

   return CDExecCommand(0, &cdcmd, &cdcmdrs);
}

//////////////////////////////////////////////////////////////////////////////

int CDGetSessionNum(u8 *num)
{
   int ret;
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   // Get Session Info
   cdcmd.CR1 = 0x0300;
   cdcmd.CR2 = 0x0000;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x0000;

   if ((ret = CDExecCommand(0, &cdcmd, &cdcmdrs)) != 0)
      return ret;

   num[0] = cdcmdrs.CR3 >> 8;
   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

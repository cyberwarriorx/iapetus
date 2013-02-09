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

#include "../iapetus.h"

//////////////////////////////////////////////////////////////////////////////

static int cd_sector_size = SECT_2048;
int sector_size_tbl[4] = { 2048, 2336, 2340, 2352 };

//////////////////////////////////////////////////////////////////////////////

void cd_write_command(cd_cmd_struct *cd_cmd)
{
   CDB_REG_CR1 = cd_cmd->CR1;
   CDB_REG_CR2 = cd_cmd->CR2;
   CDB_REG_CR3 = cd_cmd->CR3;
   CDB_REG_CR4 = cd_cmd->CR4;
}

//////////////////////////////////////////////////////////////////////////////

void cd_read_return_status(cd_cmd_struct *cd_cmd_rs)
{
   cd_cmd_rs->CR1 = CDB_REG_CR1;
   cd_cmd_rs->CR2 = CDB_REG_CR2;
   cd_cmd_rs->CR3 = CDB_REG_CR3;
   cd_cmd_rs->CR4 = CDB_REG_CR4;
}         

//////////////////////////////////////////////////////////////////////////////

int cd_wait_hirq(int flag)
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

int cd_exec_command(u16 hirq_mask, cd_cmd_struct *cd_cmd, cd_cmd_struct *cd_cmd_rs)
{
   int old_level_mask;
   u16 hirq_temp;
   u16 cd_status;

   // Mask any interrupts, we don't need to be interrupted
   old_level_mask = interrupt_get_level_mask();
   interrupt_set_level_mask(0xF);

   hirq_temp = CDB_REG_HIRQ;

   // Make sure CMOK flag is set, or we can't continue
   if (!(hirq_temp & HIRQ_CMOK))
      return LAPETUS_ERR_BUSY;

   // Clear CMOK and any other user-defined flags
   CDB_REG_HIRQ = ~(hirq_mask | HIRQ_CMOK);

   // Alright, time to execute the command
   cd_write_command(cd_cmd);

   // Let's wait till the command operation is finished
   if (!cd_wait_hirq(HIRQ_CMOK))
      return LAPETUS_ERR_BUSY;

   // Read return data
   cd_read_return_status(cd_cmd_rs);

   cd_status = cd_cmd_rs->CR1 >> 8;

   // Was command good?
   if (cd_status == STATUS_REJECT)
      return LAPETUS_ERR_BUSY;
   else if (cd_status & STATUS_WAIT)
      return LAPETUS_ERR_BUSY;

   // return interrupts back to normal
   interrupt_set_level_mask(old_level_mask);

   // It's all good
   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void cd_get_info_data(int size, u16 *data)
{
   int i;
   for (i = 0; i < size; i++)
      data[i] = *((volatile u16 *)0x25898000);
}

//////////////////////////////////////////////////////////////////////////////

int cd_debug_exec_command(font_struct *font, u16 hirq_mask, cd_cmd_struct *cd_cmd, cd_cmd_struct *cd_cmd_rs)
{
   int old_level_mask;
   u16 hirq_temp;

   // Mask any interrupts, we don't need to be interrupted
   old_level_mask = interrupt_get_level_mask();
   interrupt_set_level_mask(0xF);

   hirq_temp = CDB_REG_HIRQ;

   // Make sure CMOK flag is set, or we can't continue
   if (!(hirq_temp & HIRQ_CMOK))
      return LAPETUS_ERR_BUSY;

   // Clear CMOK and any other user-defined flags
   CDB_REG_HIRQ = ~(hirq_mask | HIRQ_CMOK);

   // Alright, time to execute the command
   cd_write_command(cd_cmd);

   // Go into an endless loop showing the HIRQ
   vdp_print_text(font, 2 * 8, 20 * 8, 15, "HIRQ = ");

   for (;;)
      vdp_printf(font, 2 * 8, 20 * 8, 15, "%d", CDB_REG_HIRQ);

   // return interrupts back to normal
   interrupt_set_level_mask(old_level_mask);
}

//////////////////////////////////////////////////////////////////////////////

int cd_cdb_init(int standby)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   // CD Init Command
   cd_cmd.CR1 = 0x0400;
   cd_cmd.CR2 = standby;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x040F;

   return cd_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int cd_end_transfer()
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x0600;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs);

   CDB_REG_HIRQ = (~HIRQ_DRDY) | HIRQ_CMOK;

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int cd_play_fad(int playmode, int startfad, int numsectors)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   int ret;

   // Clear flags
   CDB_REG_HIRQ = ~(HIRQ_PEND|HIRQ_CSCT) | HIRQ_CMOK;

   cd_cmd.CR1 = 0x1080 | (startfad >> 16);
   cd_cmd.CR2 = startfad;
   cd_cmd.CR3 = (playmode << 8) | 0x80 | (numsectors >> 16);
   cd_cmd.CR4 = numsectors;

   ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int cd_seek_fad(int seekfad)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x1180 | (seekfad >> 16);
   cd_cmd.CR2 = seekfad;
   cd_cmd.CR3 = 0;
   cd_cmd.CR4 = 0;

   return cd_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int cd_get_subcode(enum SUBCODE_TYPE type, u16 *data, u16 *flags)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x2000 | (type & 0xFF);
   cd_cmd.CR2 = 0;
   cd_cmd.CR3 = 0;
   cd_cmd.CR4 = 0;

   ret = cd_exec_command(HIRQ_DRDY, &cd_cmd, &cd_cmd_rs);

   if (ret == LAPETUS_ERR_OK)
   {
      // Wait for data to be ready
      if (!cd_wait_hirq(HIRQ_DRDY))
         return LAPETUS_ERR_BUSY;

      cd_get_info_data(cd_cmd_rs.CR2, data);
      *flags = cd_cmd_rs.CR4;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int cd_connect_cd_to_filter(int filternum)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x3000;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = filternum << 8;
   cd_cmd.CR4 = 0x0000;

   return cd_exec_command(HIRQ_ESEL, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

static int cd_set_filter_mode(int filternum, int mode)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x4400 | (mode & 0xFF);
   cd_cmd.CR2 = 0;
   cd_cmd.CR3 = (filternum << 8);
   cd_cmd.CR4 = 0;

   ret = cd_exec_command(HIRQ_ESEL, &cd_cmd, &cd_cmd_rs);

   // Wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int cd_set_filter_subheader_conditions(int filter_num) // fix me
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x4200 | 0x00; // fix me
   cd_cmd.CR2 = 0x0000; // fix me
   cd_cmd.CR3 = (filter_num << 8) | 0x00; // fix me
   cd_cmd.CR4 = 0x0000; // fix me

   ret = cd_exec_command(HIRQ_ESEL, &cd_cmd, &cd_cmd_rs);

   // Wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int cd_set_filter_connection(int filter_num, int connect_flag, int true_con, int false_con)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x4600 | (connect_flag & 0xFF);
   cd_cmd.CR2 = (true_con << 8) | (false_con & 0xFF);
   cd_cmd.CR3 = (filter_num << 8);
   cd_cmd.CR4 = 0;

   ret = cd_exec_command(HIRQ_ESEL, &cd_cmd, &cd_cmd_rs);

   // Wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int cd_set_filter(int filternum, int mode, int truecon, int falsecon)
{
   int ret;

   if ((ret = cd_set_filter_mode(filternum, mode)) != 0)
      return ret;

   // fix me
//   if ((ret = CDSetFilterSubheaderConditions(filternum)) != 0)
//      return ret;

   // Connect filter 0's true condition to selector 0, false condition to selector NULL
   if ((ret = cd_set_filter_connection(filternum, 0x03, truecon, falsecon)) != 0)
      return ret;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

static int cd_reset_selector(int resetflags, int selnum)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   // Reset Selector Command
   cd_cmd.CR1 = 0x4800 | ((u8)resetflags);
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = (selnum << 8);
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(HIRQ_EFLS, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   // wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cd_reset_selector_one(int sel_num)
{
   return cd_reset_selector(0, sel_num);
}

//////////////////////////////////////////////////////////////////////////////

int cd_reset_selector_all()
{
   return cd_reset_selector(0xFC, 0);
}

//////////////////////////////////////////////////////////////////////////////

int cd_is_data_ready(int selnum)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x5100;
   cd_cmd.CR2 = 0;
   cd_cmd.CR3 = (selnum << 8);
   cd_cmd.CR4 = 0;

   if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != 0)
      return LAPETUS_ERR_OK;

   // Return the number of sectors ready
   return cd_cmd_rs.CR4;
}

//////////////////////////////////////////////////////////////////////////////

int cd_set_sector_size(int size)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_sector_size = size;

   cd_cmd.CR1 = 0x6000 | (size & 0xFF);
   cd_cmd.CR2 = size << 8;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   return cd_exec_command(HIRQ_ESEL, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int cd_get_then_delete_sector_data(int selnum, int sectorpos, int numsectors)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x6300;
   cd_cmd.CR2 = sectorpos;
   cd_cmd.CR3 = selnum << 8;
   cd_cmd.CR4 = numsectors;

   return cd_exec_command(HIRQ_EHST, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int cd_abort_file()
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   // Abort File Command
   cd_cmd.CR1 = 0x7500;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;
  
   return cd_exec_command(HIRQ_EFLS, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int cd_transfer_data(u32 num_sectors, u32 *buffer)
{
   u32 i;
   int ret;

   // Setup a transfer from cd buffer to wram, then delete data
   // from cd buffer
   if ((ret = cd_get_then_delete_sector_data(0, 0, num_sectors)) != 0)
      return ret;

   // Wait till data is ready
   if (!cd_wait_hirq(HIRQ_DRDY | HIRQ_EHST))
   	   return LAPETUS_ERR_BUSY;

   // Do transfer
   for (i = 0; i < ((num_sectors * sector_size_tbl[cd_sector_size]) / 4); i++)
      buffer[i] = CDB_REG_DATATRNS; // this can also be done in word units as well

   if ((ret = cd_end_transfer()) != 0)
      return ret;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cd_transfer_data_bytes(u32 num_bytes, u32 *buffer)
{
   u32 i;
   int ret;
   int num_sectors=num_bytes / sector_size_tbl[cd_sector_size];

   if (num_bytes % sector_size_tbl[cd_sector_size])
      num_sectors++;

   // Setup a transfer from cd buffer to wram, then delete data
   // from cd buffer
   if ((ret = cd_get_then_delete_sector_data(0, 0, num_sectors)) != 0)
      return ret;

   // Wait till data is ready
   if (!cd_wait_hirq(HIRQ_DRDY | HIRQ_EHST))
   	   return LAPETUS_ERR_BUSY;

   // Do transfer
   for (i = 0; i < (num_bytes >> 2); i++)
   {
      buffer[0] = CDB_REG_DATATRNS; // this can also be done in word units as well
      buffer++;
   }

   // Get the remainder
   if (num_bytes % 4)
   {
      u32 data;
      u8 *datapointer=(u8 *)&data;

      data = CDB_REG_DATATRNS;

      for (i = 0; i < (num_bytes % 4); i++)
         ((u8 *)buffer)[i] = datapointer[i];
   }

   if ((ret = cd_end_transfer()) != 0)
      return ret;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cd_init()
{
   int ret;

   // Abort any file transfers that may be currently going
   if ((ret = cd_abort_file()) != 0)
      return ret;

   // Init CD Block
   if ((ret = cd_cdb_init(0)) != 0)
      return ret;

   // End any previous cd buffer data transfers
   if ((ret = cd_end_transfer()) != 0)
      return ret;

   // Reset all buffer partitions, partition output connectors, all filter
   // conditions, all filter input connectors, etc.
   if ((ret = cd_reset_selector_all()) != 0)
      return ret;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cd_get_stat(cd_stat_struct *cd_status)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   int ret;

   cd_cmd.CR1 = 0x0000;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   cd_status->status = cd_cmd_rs.CR1 >> 8;
   cd_status->flag = (cd_cmd_rs.CR1 >> 4) & 0xF;
   cd_status->repeat_cnt = cd_cmd_rs.CR1 & 0xF;
   cd_status->ctrl_addr = cd_cmd_rs.CR2 >> 8;
   cd_status->track = cd_cmd_rs.CR2 & 0xFF;
   cd_status->index = cd_cmd_rs.CR3 >> 8;
   cd_status->FAD = ((cd_cmd_rs.CR3 & 0xFF) << 16) | cd_cmd_rs.CR4;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int is_cd_auth(u16 *disc_type_auth)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0xE100;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   // If command fails, assume it's not authenticated
   if (cd_exec_command(0, &cd_cmd, &cd_cmd_rs) != LAPETUS_ERR_OK)
      return FALSE;

   if (disc_type_auth)
      *disc_type_auth = cd_cmd_rs.CR2;

   // Disc type Authenticated:
   // 0x00: No CD/Not Authenticated
   // 0x01: Audio CD
   // 0x02: Regular Data CD(not Saturn disc)
   // 0x03: Copied/Pirated Saturn Disc
   // 0x04: Original Saturn Disc
   if (cd_cmd_rs.CR2 != 0x04 && cd_cmd_rs.CR2 != 0x02)
      return FALSE;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

int cd_auth()
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   u16 auth;
   cd_stat_struct cd_status;
   int i;

   // Clear hirq flags
   CDB_REG_HIRQ = ~(HIRQ_DCHG | HIRQ_EFLS);

   // Authenticate disc
   cd_cmd.CR1 = 0xE000;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(HIRQ_EFLS, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   // wait till operation is finished
   while (!(CDB_REG_HIRQ & HIRQ_EFLS)) {}

   // Wait until drive has finished seeking
   for (;;)
   {
      // wait a bit
      for (i = 0; i < 100000; i++) { }

      if (cd_get_stat(&cd_status) != 0) continue;

      if (cd_status.status == STATUS_PAUSE) break;
      else if (cd_status.status == STATUS_FATAL) return LAPETUS_ERR_UNKNOWN;
   }

   // Was Authentication successful?
   if (!is_cd_auth(&auth))
      return LAPETUS_ERR_AUTH;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cd_stop_drive()
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   cd_stat_struct cd_status;
   int i;

   // CD Init Command
   cd_cmd.CR1 = 0x0400;
   cd_cmd.CR2 = 0x0001;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x040F;

   if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   // Wait till operation is finished(fix me)

   // Wait till drive is stopped
   for (;;)
   {
      // wait a bit
      for (i = 0; i < 100000; i++) { }

      if (cd_get_stat(&cd_status) != 0) continue;

      if (cd_status.status == STATUS_STANDBY) break;
      else if (cd_status.status == STATUS_FATAL) return LAPETUS_ERR_UNKNOWN;
   }

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cd_start_drive()
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   // CD Init Command
   cd_cmd.CR1 = 0x0400;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x040F;

   if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   // wait till operation is finished(fix me)

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int is_cd_present()
{
   cd_stat_struct cd_status;

   // If command fails, assume disc isn't present
   if (cd_get_stat(&cd_status) != 0)
      return FALSE;

   // Check status
   switch (cd_status.status & 0xF)
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

int cd_read_sector(void *buffer, u32 FAD, int sector_size, u32 num_bytes)
{
   int ret;
   int done=0;
   // Figure out how many sectors we actually have to read
   int num_sectors=num_bytes / sector_size_tbl[cd_sector_size];

   if (num_bytes % sector_size_tbl[cd_sector_size] != 0)
      num_sectors++;

   if ((ret = cd_set_sector_size(sector_size)) != 0)
      return ret;

   // Set Filter
/*
//   if ((ret = CDSetFilter(0, 1, 0, 0xFF)) != 0)
//   if ((ret = CDSetFilter(0, 0x80, 0, 0xFF)) != 0)
   if ((ret = CDSetFilter(0, 0x10, 0, 1)) != 0)
      return ret;
*/
   // Clear partition 0
   if ((ret = cd_reset_selector_one(0)) != 0)
      return ret;

   // Connect CD device to filter 0
   if ((ret = cd_connect_cd_to_filter(0)) != 0)
      return ret;

   // Start reading sectors
   if ((ret = cd_play_fad(0, FAD, num_sectors)) != 0)
      return ret;

   while (!done)
   {
      u32 sectorstoread=0;
      u32 bytes_to_read;

      // Wait until there's data ready
      while ((sectorstoread = cd_is_data_ready(0)) == 0) {}

      if ((sectorstoread * sector_size_tbl[cd_sector_size]) > num_bytes)
         bytes_to_read = num_bytes;
      else
         bytes_to_read = sectorstoread * sector_size_tbl[cd_sector_size];

      // Setup a transfer from cd buffer to wram, then delete data
      // from cd buffer
      if ((ret = cd_transfer_data_bytes(bytes_to_read, buffer)) != LAPETUS_ERR_OK)
         return ret;

      num_bytes -= bytes_to_read;
      buffer += bytes_to_read;

      if (num_bytes == 0)
         done = 1;
   }

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int play_cd_audio(u8 audio_track, u8 repeat, u8 vol_l, u8 vol_r)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   int ret;

   sound_external_audio_enable(vol_l, vol_r);

   if ((ret = cd_set_sector_size(2048)) != 0)
      return ret;

   // Clear partition 0
   if ((ret = cd_reset_selector_one(0)) != 0)
      return ret;

   // Connect CD device to filter 0
   if ((ret = cd_connect_cd_to_filter(0)) != 0)
      return ret;

   // Clear flags
   CDB_REG_HIRQ = ~(HIRQ_PEND|HIRQ_CSCT) | HIRQ_CMOK;

   cd_cmd.CR1 = 0x1000;
   cd_cmd.CR2 = (audio_track << 8) | 0x01;
   cd_cmd.CR3 = repeat << 8;
   cd_cmd.CR4 = (audio_track << 8) | 0x63;

   ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int stop_cd_audio(void)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   // Do a default seek, that should stop the cd from playing
   cd_cmd.CR1 = 0x11FF;
   cd_cmd.CR2 = 0xFFFF;
   cd_cmd.CR3 = 0xFFFF;
   cd_cmd.CR4 = 0xFFFF;

   return cd_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int cd_get_session_num(u8 *num)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   // Get Session Info
   cd_cmd.CR1 = 0x0300;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   num[0] = cd_cmd_rs.CR3 >> 8;
   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

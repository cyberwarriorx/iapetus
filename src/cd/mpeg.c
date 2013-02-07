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

BOOL is_mpeg_card_present()
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x0100;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if (cd_exec_command(0, &cd_cmd, &cd_cmd_rs) != LAPETUS_ERR_OK)
      return FALSE;

   // Is MPEG card available?
   if (((cd_cmd_rs.CR2 >> 8) & 0x02) == 0)
   	   return FALSE;
   
   // Has MPEG card been authenticated?
   if ((cd_cmd_rs.CR3 & 0xFF) == 0)
   {
       if (!bios_is_mpeg_card_present(0))
       {
          if (!bios_is_mpeg_card_present(0))
          	  return FALSE;
       }
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOL is_mpeg_auth()
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0xE100;
   cd_cmd.CR2 = 0x0001;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   // If command fails, assume it's not authenticated
   if (cd_exec_command(0, &cd_cmd, &cd_cmd_rs) != LAPETUS_ERR_OK)
      return FALSE;

   // Disc type Authenticated:
   // 0x00: No MPEG Card/Not Authenticated
   // 0x02: Some kind of MPEG card
   if (cd_cmd_rs.CR2 != 0x02)
      return FALSE;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_auth()
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   u16 auth;

   // Clear hirq flags
   CDB_REG_HIRQ = ~(HIRQ_MPED);

   // Authenticate disc
   cd_cmd.CR1 = 0xE000;
   cd_cmd.CR2 = 0x0001;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(HIRQ_EFLS, &cd_cmd, &cd_cmd_rs)) != LAPETUS_ERR_OK)
      return ret;

   // wait till operation is finished
   while (!(CDB_REG_HIRQ & HIRQ_MPED)) {}

   // Was Authentication successful?
   if (!is_mpeg_auth(&auth))
      return -1;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_init ()
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   screen_settings_struct settings;

   // Make sure MPEG card is authenticated
   if (!is_mpeg_auth())
      mpeg_auth();

   // Now Initialize MPEG card
   cd_cmd.CR1 = 0x9300;
   cd_cmd.CR2 = 0x0001; // might have to change this
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(HIRQ_MPED, &cd_cmd, &cd_cmd_rs)) != LAPETUS_ERR_OK)
      return ret;

   // Do a MPEG Set Mode here

   // Get MPEG stats here

   // Get MPEG Connection here

   // Enable the external audio through SCSP
   sound_external_audio_enable(7, 7);

   // Setup NBG1 as EXBG
   settings.is_bitmap = TRUE;
   settings.bitmap_size = BG_BITMAP512x256;
   settings.transparent_bit = 0;
   settings.color = BG_32786COLOR;
   settings.special_priority = 0;
   settings.special_color_calc = 0;
   settings.extra_palette_num = 0;
   settings.map_offset = 0;

   return vdp_exbg_init(&settings);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_play(file_struct *file)
{
   // Setup CD filters here

   // Setup MPEG connections here

   // Start CD transfer here

   // Start MPEG decoding here

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_pause(file_struct *file)
{
   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_stop(file_struct *file)
{
   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

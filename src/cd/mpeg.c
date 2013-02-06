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

BOOL IsMPEGCardPresent()
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   cdcmd.CR1 = 0x0100;
   cdcmd.CR2 = 0x0000;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x0000;

   if (CDExecCommand(0, &cdcmd, &cdcmdrs) != LAPETUS_ERR_OK)
      return FALSE;

   // Is MPEG card available?
   if (((cdcmdrs.CR2 >> 8) & 0x02) == 0)
   	   return FALSE;
   
   // Has MPEG card been authenticated?
   if ((cdcmdrs.CR3 & 0xFF) == 0)
   {
       if (!BIOS_IsMpegCardPresent(0))
       {
          if (!BIOS_IsMpegCardPresent(0))
          	  return FALSE;
       }
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOL IsMPEGAuth()
{
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;

   cdcmd.CR1 = 0xE100;
   cdcmd.CR2 = 0x0001;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x0000;

   // If command fails, assume it's not authenticated
   if (CDExecCommand(0, &cdcmd, &cdcmdrs) != LAPETUS_ERR_OK)
      return FALSE;

   // Disc type Authenticated:
   // 0x00: No MPEG Card/Not Authenticated
   // 0x02: Some kind of MPEG card
   if (cdcmdrs.CR2 != 0x02)
      return FALSE;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

int MPEGAuth()
{
   int ret;
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;
   u16 auth;

   // Clear hirq flags
   CDB_REG_HIRQ = ~(HIRQ_MPED);

   // Authenticate disc
   cdcmd.CR1 = 0xE000;
   cdcmd.CR2 = 0x0001;
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x0000;

   if ((ret = CDExecCommand(HIRQ_EFLS, &cdcmd, &cdcmdrs)) != LAPETUS_ERR_OK)
      return ret;

   // wait till operation is finished
   while (!(CDB_REG_HIRQ & HIRQ_MPED)) {}

   // Was Authentication successful?
   if (!IsMPEGAuth(&auth))
      return -1;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int MPEGInit ()
{
   int ret;
   cdcmd_struct cdcmd;
   cdcmd_struct cdcmdrs;
   screen_settings_struct settings;

   // Make sure MPEG card is authenticated
   if (!IsMPEGAuth())
      MPEGAuth();

   // Now Initialize MPEG card
   cdcmd.CR1 = 0x9300;
   cdcmd.CR2 = 0x0001; // might have to change this
   cdcmd.CR3 = 0x0000;
   cdcmd.CR4 = 0x0000;

   if ((ret = CDExecCommand(HIRQ_MPED, &cdcmd, &cdcmdrs)) != LAPETUS_ERR_OK)
      return ret;

   // Do a MPEG Set Mode here

   // Get MPEG stats here

   // Get MPEG Connection here

   // Enable the external audio through SCSP
   SoundExternalAudioEnable(7, 7);

   // Setup NBG1 as EXBG
   settings.isbitmap = TRUE;
   settings.bitmapsize = BG_BITMAP512x256;
   settings.transparentbit = 0;
   settings.color = BG_32786COLOR;
   settings.specialpriority = 0;
   settings.specialcolorcalc = 0;
   settings.extrapalettenum = 0;
   settings.mapoffset = 0;

   return vdp_exbg_init(&settings);
}

//////////////////////////////////////////////////////////////////////////////

int MpegPlay(file_struct *file)
{
   // Setup CD filters here

   // Setup MPEG connections here

   // Start CD transfer here

   // Start MPEG decoding here

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int MpegPause(file_struct *file)
{
   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int MpegStop(file_struct *file)
{
   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

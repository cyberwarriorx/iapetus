/*  Copyright 2009 Theo Berkau
    
    Flash code based on code by Ex-Cyber

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

#define AR_5555         *((volatile u16 *)0x2200AAAA)
#define AR_AAAA         *((volatile u16 *)0x22005554)
#define AR_VENDOR       *((volatile u16 *)0x22000000)
#define AR_DEVICE       *((volatile u16 *)0x22000002)

#define CMD_PID_ENTRY   0x9090
#define CMD_PID_EXIT    0xF0F0
#define CMD_PAGE_WRITE  0xA0A0

typedef struct
{
   char *name;
   u32 pid;
   int pagesize; // in words
   int romsize; // in words
} flashlist_struct;

static flashlist_struct flashlist[] = {
   { "Silicon Storage Technology SST29EE010", 0xBFBF0707, 128, 131072 },
   { "Atmel AT29C010", 0x1F1FD5D5, 128, 131072 },
};

static int numsupportedflash=sizeof(flashlist)/sizeof(flashlist_struct);

static int arpagesize=0;
static int arromsize=0;

void ARCLInitHandler(int vector, u32 patchaddr, u16 patchinst, u32 codeaddr)
{
   int i;

   // Copy over AR handler from 0x02003024-0x02003147 to codeaddr+8
   for (i = 0; i < 292; i+=4)
       *((u32 *)(codeaddr+8+i)) = *((u32 *)(0x02003024+i));
   
   // Patch code address+8 so the bra call is correct(fix me)
   *((u16 *)(codeaddr+0x8)) = *((u16 *)(codeaddr+0xA));
   *((u16 *)(codeaddr+0xA)) = 0x0009;

   // Patch interrupt with AR handler
   *((u32 *)(0x06000000 + (vector << 2))) = codeaddr + 0x8;

   // Read patch address, write it to code address+0xC
   *((u16 *)(codeaddr+0xC)) = *((u16 *)patchaddr);

   // Write patch instruction to patch address
   *((u16 *)patchaddr) = patchinst;

   // Write a dummy code(not sure if this is needed)
   *((u32 *)(codeaddr)) = 0x06000000;
   *((u32 *)(codeaddr+4)) = 0x00000000; // flag to keep handler from running multiple times at the same time
   *((u32 *)(codeaddr+0x11C)) = codeaddr;   
}

void ARCommand(u16 cmd)
{
  // Unlock Flash
  AR_5555 = 0xAAAA;
  AR_AAAA = 0x5555;
  // Write Command
  AR_5555 = cmd;
}

void ARDelay10ms()
{
   // This should be good enough. Ideally though, you'd want something a little more accurate using WDT, FRT or something
   vdp_vsync();
   vdp_vsync();
}

void ARGetProductID(u16 *vendorid, u16 *deviceid)
{
  ARCommand(CMD_PID_ENTRY);
  ARDelay10ms();
  *vendorid = AR_VENDOR;
  *deviceid = AR_DEVICE;
  ARCommand(CMD_PID_EXIT);
  ARDelay10ms();
}

int ARInitFlashIO()
{
   u16 vendorid, deviceid;
   u32 pid;
   int i;

   ARGetProductID(&vendorid, &deviceid);
   pid = (vendorid << 16) | deviceid;

   if (pid == 0xFFFFFFFF)
   {
      arpagesize = 0;
      arromsize = 0;
      return LAPETUS_ERR_HWNOTFOUND;
   }

   // Make sure vendor id and device id are supported here
   for (i = 0; i < numsupportedflash; i++)
   {
      if (flashlist[i].pid == pid)
      {
         arpagesize=flashlist[i].pagesize;
         arromsize=flashlist[i].romsize;
         return LAPETUS_ERR_OK;
      }
   }

   arpagesize = 0;
   arromsize = 0;
   return LAPETUS_ERR_UNSUPPORTED;
}

// Untested
void AREraseFlash(volatile u16 *page, int numpages)
{
  int i,j;

  for (i = 0; i < numpages; i++)
  {
     ARCommand(CMD_PAGE_WRITE);
     for(j = 0; j < arpagesize; j++)
     {
        page[0] = 0xFFFF;
        page++;
     }
     ARDelay10ms();
  }
}

void ARWriteFlash(volatile u16 *page, u16 *data, int numpages)
{
  int i,j;

  for (i = 0; i < numpages; i++)
  {
     ARCommand(CMD_PAGE_WRITE);
     for(j = 0; j < arpagesize; j++)
     {
        page[0] = data[0];
        page++;
        data++;
     }
     ARDelay10ms();
  }
}

// Untested
int ARVerifyWriteFlash(volatile u16 *page, u16 *data, int numpages)
{
   int i;

   for (i = 0; i < (numpages * arpagesize); i++)
   {
      if (page[i] != data[i])
         return FALSE;
   }
   return TRUE;
}

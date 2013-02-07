/*  Copyright 2009, 2013 Theo Berkau
    
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
} flash_list_struct;

static flash_list_struct flash_list[] = {
   { "Silicon Storage Technology SST29EE010", 0xBFBF0707, 128, 131072 },
   { "Atmel AT29C010", 0x1F1FD5D5, 128, 131072 },
};

static int numsupportedflash=sizeof(flash_list)/sizeof(flash_list_struct);

static int arpagesize=0;
static int arromsize=0;

void arcl_init_handler(int vector, u32 patchaddr, u16 patchinst, u32 codeaddr)
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

void ar_command(u16 cmd)
{
  // Unlock Flash
  AR_5555 = 0xAAAA;
  AR_AAAA = 0x5555;
  // Write Command
  AR_5555 = cmd;
}

void ar_delay_10ms()
{
   // This should be good enough. Ideally though, you'd want something a little more accurate using WDT, FRT or something
   vdp_vsync();
   vdp_vsync();
}

void ar_get_product_id(u16 *vendorid, u16 *deviceid)
{
  ar_command(CMD_PID_ENTRY);
  ar_delay_10ms();
  *vendorid = AR_VENDOR;
  *deviceid = AR_DEVICE;
  ar_command(CMD_PID_EXIT);
  ar_delay_10ms();
}

int ar_init_flash_io()
{
   u16 vendorid, deviceid;
   u32 pid;
   int i;

   ar_get_product_id(&vendorid, &deviceid);
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
      if (flash_list[i].pid == pid)
      {
         arpagesize=flash_list[i].pagesize;
         arromsize=flash_list[i].romsize;
         return LAPETUS_ERR_OK;
      }
   }

   arpagesize = 0;
   arromsize = 0;
   return LAPETUS_ERR_UNSUPPORTED;
}

// Untested
void ar_erase_flash(volatile u16 *page, int numpages)
{
  int i,j;

  for (i = 0; i < numpages; i++)
  {
     ar_command(CMD_PAGE_WRITE);
     for(j = 0; j < arpagesize; j++)
     {
        page[0] = 0xFFFF;
        page++;
     }
     ar_delay_10ms();
  }
}

void ar_write_flash(volatile u16 *page, u16 *data, int numpages)
{
  int i,j;

  for (i = 0; i < numpages; i++)
  {
     ar_command(CMD_PAGE_WRITE);
     for(j = 0; j < arpagesize; j++)
     {
        page[0] = data[0];
        page++;
        data++;
     }
     ar_delay_10ms();
  }
}

// Untested
int ar_verify_write_flash(volatile u16 *page, u16 *data, int numpages)
{
   int i;

   for (i = 0; i < (numpages * arpagesize); i++)
   {
      if (page[i] != data[i])
         return FALSE;
   }
   return TRUE;
}

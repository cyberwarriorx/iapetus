/*  Copyright 2009, 2013-2014 Theo Berkau
    
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
	{ "Silicon Storage Technology SST29EE020", 0xBFBF1010, 128, 262144 },
   { "Atmel AT29C010", 0x1F1FD5D5, 128, 131072 },
	{ "AMD AM29F010B", 0x01012020, 128, 131072 },
};

static int num_supported_flash=sizeof(flash_list)/sizeof(flash_list_struct);

static int ar_page_size=0;
static int ar_rom_size=0;

void arcl_init_handler(int vector, u32 patch_addr, u16 patch_inst, u32 code_addr)
{
   int i;

   // Copy over AR handler from 0x02003024-0x02003147 to codeaddr+8
   for (i = 0; i < 292; i+=4)
       *((u32 *)(code_addr+8+i)) = *((u32 *)(0x02003024+i));
   
   // Patch code address+8 so the bra call is correct(fix me)
   *((u16 *)(code_addr+0x8)) = *((u16 *)(code_addr+0xA));
   *((u16 *)(code_addr+0xA)) = 0x0009;

   // Patch interrupt with AR handler
   *((u32 *)(0x06000000 + (vector << 2))) = code_addr + 0x8;

   // Read patch address, write it to code address+0xC
   *((u16 *)(code_addr+0xC)) = *((u16 *)patch_addr);

   // Write patch instruction to patch address
   *((u16 *)patch_addr) = patch_inst;

   // Write a dummy code(not sure if this is needed)
   *((u32 *)(code_addr)) = 0x06000000;
   *((u32 *)(code_addr+4)) = 0x00000000; // flag to keep handler from running multiple times at the same time
   *((u32 *)(code_addr+0x11C)) = code_addr;   
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

void ar_get_product_id(u16 *vendor_id, u16 *device_id)
{
	ar_command(CMD_PID_ENTRY);
	ar_delay_10ms();
	*vendor_id = AR_VENDOR;
	*device_id = AR_DEVICE;
	ar_command(CMD_PID_EXIT);
	ar_delay_10ms();
}

int ar_get_product_index(u16 vendorid, u16 deviceid)
{
	int i;
	u32 pid = (vendorid << 16) | deviceid;
	for (i = 0; i < num_supported_flash; i++)
	{
		if (flash_list[i].pid == pid)
			return i;
	}
	return -1;
}

int ar_get_product_name(u16 vendor_id, u16 device_id, char **name)
{
	int i;

	if (name == NULL)
		return IAPETUS_ERR_INVALIDARG;

	i = ar_get_product_index(vendor_id, device_id);

	if (i >= 0)
	{
		*name=flash_list[i].name;
		return IAPETUS_ERR_OK;
	}

	if (vendor_id == 0xBFBF)
		*name = "Unknown Silicon Storage Technology";

	return IAPETUS_ERR_UNSUPPORTED;
}

int ar_init_flash_io()
{
   u16 vendor_id, device_id;
   int i;

   ar_get_product_id(&vendor_id, &device_id);

	i = ar_get_product_index(vendor_id, device_id);
   // Make sure vendor id and device id are supported
   if (i < 0)
   {
      ar_page_size = 0;
      ar_rom_size = 0;

      if (vendor_id == 0xFFFF && device_id == 0xFFFF)
         return IAPETUS_ERR_HWNOTFOUND;
      else
         return IAPETUS_ERR_UNSUPPORTED;
   }
	else
	{
      ar_page_size=flash_list[i].pagesize;
      ar_rom_size=flash_list[i].romsize;
		return IAPETUS_ERR_OK;
	}
}

// Untested
void ar_erase_flash(volatile u16 *page, int num_pages)
{
  int i,j;

  for (i = 0; i < num_pages; i++)
  {
     ar_command(CMD_PAGE_WRITE);
     for(j = 0; j < ar_page_size; j++)
     {
        page[0] = 0xFFFF;
        page++;
     }
     ar_delay_10ms();
  }
}

void ar_write_flash(volatile u16 *page, u16 *data, int num_pages)
{
  int i,j;

  for (i = 0; i < num_pages; i++)
  {
     ar_command(CMD_PAGE_WRITE);
     for(j = 0; j < ar_page_size; j++)
     {
        page[0] = data[0];
        page++;
        data++;
     }
     ar_delay_10ms();
  }
}

int ar_verify_write_flash(volatile u16 *page, u16 *data, int num_pages)
{
   int i;

   for (i = 0; i < (num_pages * ar_page_size); i++)
   {
      if (page[i] != data[i])
         return FALSE;
   }
   return TRUE;
}

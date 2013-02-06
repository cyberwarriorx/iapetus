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

#include "../iapetus.h"

volatile int commlinkservice;

//////////////////////////////////////////////////////////////////////////////

#define PAR_OUTPORT *((volatile u8 *)0x22080001)
#define PAR_STATPORT *((volatile u8 *)0x22100001)
#define PAR_INPORT *((volatile u8 *)0x22180001)

//////////////////////////////////////////////////////////////////////////////

u8 CLExchangeByte(u8 val)
{
   u8 ret;
   while (!(PAR_STATPORT & 0x1)) {}
   ret = PAR_INPORT;
   PAR_OUTPORT = val;
   return ret;
}

//////////////////////////////////////////////////////////////////////////////

u8 CLReceiveByte(void)
{
   u8 ret;
   while (!(PAR_STATPORT & 0x1)) {}
   ret = PAR_INPORT;
   PAR_OUTPORT = ret;
   return ret;
}

//////////////////////////////////////////////////////////////////////////////

void CLSendLong(u32 val)
{
   CLExchangeByte(val >> 24);
   CLExchangeByte(val >> 16);
   CLExchangeByte(val >> 8);
   CLExchangeByte(val & 0xFF);
}

//////////////////////////////////////////////////////////////////////////////

u32 CLReceiveLong(void)
{
   u32 ret;
   ret = CLExchangeByte(0x00) << 24;
   ret |= CLExchangeByte(0x00) << 16;
   ret |= CLExchangeByte(0x00) << 8;
   ret |= CLExchangeByte(0x00);
   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int CLCheck1(void)
{
   u8 buf[2];
   s32 addr;
   s32 size;
   u16 chksum;
   u16 calcchksum;
   u8 data;

   // Clear outport
   PAR_OUTPORT = 0;

   // Check to see if someone's trying to send something
   if (PAR_INPORT == 'W')
   {
      for (;;)
      {
         buf[0] = CLExchangeByte('R');
         buf[1] = CLExchangeByte('W');

         if (buf[0] == 'W' && buf[1] == 'B')
            break;
      }

      // First byte seems to be unused
      CLReceiveByte();

      addr = CLReceiveByte() << 24;
      addr |= CLReceiveByte() << 16;
      addr |= CLReceiveByte() << 8;
      addr |= CLReceiveByte();

      size = CLReceiveByte() << 24;
      size |= CLReceiveByte() << 16;
      size |= CLReceiveByte() << 8;
      size |= CLReceiveByte();

      calcchksum = 0;

      // Receive data
      while (size > 0)
      {
         data = CLReceiveByte();
         *((volatile u8 *)addr) = data;
         addr++;
         calcchksum += data;
         calcchksum &= 0xFFF;
         size--;
      }

      chksum = CLReceiveByte() << 8;
      chksum |= CLReceiveByte();

      if (chksum == calcchksum)
      {
         PAR_OUTPORT = 0;
         ((void (*)())addr)();
         return 0;
      }
   }

   return 1;
}

//////////////////////////////////////////////////////////////////////////////

void CLCheck2(u8 val)
{
   u32 addr;
   u32 size;
   u8 exec;
   u8 data=val;

   if (PAR_INPORT != 'I')
      return;

   // Sync with commlink
   for (;;)
   {
      if (CLExchangeByte('D') != 'I')
         continue;

      if (CLExchangeByte('O') == 'N')
         break;
   }

   switch(CLExchangeByte(0x00))
   {
      case 0x01:
      {
         u8 chksum;

         // Download Memory
         CLSendLong(val);

         for (;;)
         {
            data = 0;
            data = CLExchangeByte(data);
            addr = data << 24;
            data = CLExchangeByte(data);
            addr |= data << 16;
            data = CLExchangeByte(data);
            addr |= data << 8;
            data = CLExchangeByte(data);
            addr |= data;

            data = CLExchangeByte(data);
            size = data << 24;
            data = CLExchangeByte(data);
            size |= data << 16;
            data = CLExchangeByte(data);
            size |= data << 8;
            data = CLExchangeByte(data);
            size |= data;

            // Transfer data
            if (size == 0)
               break;

            chksum = 0;

            while (size > 0)
            {
               data = *((volatile u8 *)addr);
               chksum += data;
               CLExchangeByte(data);
               addr++;
               size--;
            }

            // Send the checksum
            CLExchangeByte(chksum);
         }

         CLExchangeByte('0');
         CLExchangeByte('K');

         break;
      }
      case 0x02:
         // Set some kind of flag(FIXME)
         break;
      case 0x03:
         // Receive 5 long's, and do stuff(FIXME)
         CLReceiveLong();
         CLReceiveLong();
         CLReceiveLong();
         CLReceiveLong();
         CLReceiveLong();
         break;
      case 0x04:
         // Send 5 long's(FIXME), used by AR software's download/upload saves features
         CLSendLong(0);
         CLSendLong(0);
         CLSendLong(0);
         CLSendLong(0);
         CLSendLong(0);
         break;
      case 0x05:
         // Exchange byte, write received byte to 0x060FFE20(FIXME), used by AR software's download/upload saves features
         CLExchangeByte(0x00);
         break;
      case 0x06:
         // Receive 2 long's, write the first one to 0x060FE000, the second one to 0x060FE004(fix me), used by AR software's download/upload saves features(FIXME)
         CLReceiveLong();
         CLReceiveLong();
         break;
      case 0x07:
         // Send whatever is in R9. Looks like some kind of conditional variable. In the main menu it's 0x1, in other instances, it's 0x2, used by AR software's download/upload saves features
         CLSendLong(val);
         break;
      case 0x08:
      {         
         // Write byte to memory
         addr = CLReceiveLong();
         *((volatile u8 *)addr) = CLExchangeByte(0x00);
         break;
      }
      case 0x09:
      {
         u32 execaddr;

         // Upload(and execute) to Memory
         execaddr = addr = CLReceiveLong();
         size = CLReceiveLong();
         exec = CLExchangeByte(0x00);

         // Transfer data
         while (size > 0)
         {
            data = CLExchangeByte(data);
            *((volatile u8 *)addr) = data;
            addr++;
            size--;
         }

         if (exec)
            ((void (*)())execaddr)();

         break;
      }
      default: break;
   }
}

//////////////////////////////////////////////////////////////////////////////

void CommlinkStartService(void)
{
   commlinkservice = TRUE;
}

//////////////////////////////////////////////////////////////////////////////

void CommlinkStopService()
{
   commlinkservice = FALSE;
}

//////////////////////////////////////////////////////////////////////////////








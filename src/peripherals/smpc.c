/*  Copyright 2005-2007 Theo Berkau

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

#include <string.h>
#include "../iapetus.h"

perdata_struct per[MAX_PERIPHERALS];

static u16 oldperpush[MAX_PERIPHERALS]; // fix me

//////////////////////////////////////////////////////////////////////////////

void SmpcCommand (u8 cmd)
{
   SmpcWaitTillReady();
   SmpcIssueCommand(cmd);
}

//////////////////////////////////////////////////////////////////////////////

void perhandler(void)
{
   // Issue Intback command(we only want peripheral data)
   SmpcWaitTillReady();
   SMPC_REG_IREG(0) = 0x00; // no intback status
   SMPC_REG_IREG(1) = 0x0A; // 15-byte mode, peripheral data returned, time optimized
   SMPC_REG_IREG(2) = 0xF0; // ???
   SmpcIssueCommand(SMPC_CMD_INTBACK);
}

//////////////////////////////////////////////////////////////////////////////

void smpchandler()
{
   /* Port Status:
   0x04 - Sega-tap is connected
   0x16 - Multi-tap is connected
   0x21-0x2F - Clock serial peripheral is connected
   0xF0 - Not Connected or Unknown Device
   0xF1 - Peripheral is directly connected

   Peripheral ID:
   0x02 - Digital Device Standard Format
   0x13 - Racing Device Standard Format
   0x15 - Analog Device Standard Format
   0x23 - Pointing Device Standard Format
   0x23 - Shooting Device Standard Format
   0x34 - Keyboard Device Standard Format
   0xE1 - Mega Drive 3-Button Pad
   0xE2 - Mega Drive 6-Button Pad
   0xE3 - Saturn Mouse
   0xFF - Not Connected

   Special Notes:

   If a peripheral is disconnected from a port, SMPC only returns 1 byte for
   that port(which is the port status 0xF0), at the next OREG is the port
   status of the next port.
 
   e.g. If Port 1 has nothing connected, and Port 2 has a controller
        connected:

   OREG0 = 0xF0
   OREG1 = 0xF1
   OREG2 = 0x02
   etc.
   */

   u16 perdata;
   u8 portcon;
   u8 perid;
   u8 oregcounter=0;
   u8 percounter=0;
   int i, i2;

   // See what is actually connected to Port 1/2
   for (i2 = 0; i2 < 2; i2++)
   {
      portcon = SMPC_REG_OREG(oregcounter);
      oregcounter++;

      if ((portcon & 0xF0) != 0x20)
      {
         for (i = 0; i < (portcon & 0x0F); i++)
         {
            per[i+percounter].id = perid = SMPC_REG_OREG(oregcounter);
            oregcounter++;

//            if (perid == 0x02)
//            {
//               oldperpush[i+percounter] = per[i+percounter].butpush;
//               perdata = (SMPC_REG_OREG(oregcounter) << 8) | SMPC_REG_OREG(oregcounter+1);
//               oregcounter += 2;
//            }
//            else
//               oregcounter += (perid & 0xF);
            switch (perid >> 4)
            {
               case 0x0: // Standard Pad type
               case 0x1: // Analog type(Racing wheel/Analog pad)
               case 0x3: // Keyboard type
               {
                  oldperpush[i+percounter] = per[i+percounter].butpush;
                  perdata = (SMPC_REG_OREG(oregcounter) << 8) | SMPC_REG_OREG(oregcounter+1);
                  oregcounter += 2;

                  switch (perid & 0xF)
                  {
                     case 0x3: // Racing
                        oregcounter += 1;
                        break;
                     case 0x4: // Keyboard
                        ((keyboarddata_struct *)per)[i+percounter].kbdtype = perdata & 0x7;
                        perdata |= 0x7;
                        ((keyboarddata_struct *)per)[i+percounter].flags = SMPC_REG_OREG(oregcounter);
                        ((keyboarddata_struct *)per)[i+percounter].key = SMPC_REG_OREG(oregcounter+1);
                        if (((keyboarddata_struct *)per)[i+percounter].flags != 0x6)
                        {
                           if (((keyboarddata_struct *)per)[i+percounter].key == KEY_LEFTSHIFT ||
                               ((keyboarddata_struct *)per)[i+percounter].key == KEY_RIGHTSHIFT)
                                 ((keyboarddata_struct *)per)[i+percounter].extrastate[0] = (((keyboarddata_struct *)per)[i+percounter].flags >> 3) & 0x1;
                        }
                        oregcounter += 2;
                        break;
                     case 0x5: // Analog Pad
                        oregcounter += 3;
                        break;
                     default: break;
                  }

                  per[i+percounter].butpush = perdata ^ 0xFFFF;
                  per[i+percounter].butpushonce = (oldperpush[i+percounter] ^ per[i+percounter].butpush) & per[i+percounter].butpush;

                  break;
               }
               case 0x2: // Pointer type(Mouse/Gun)
               case 0xE: // Other type(Mega Drive 3/6 button pads/Shuttle Mouse)
               {
                  switch (perid & 0xF)
                  {
                     case 0x2: // Mega Drive 6-button Pad
                     case 0x1: // Mega Drive 3-button Pad
                        break;
                     case 0x3: // Saturn/Shuttle Mouse
                     {
                        perdata = SMPC_REG_OREG(oregcounter);
                        per[i+percounter].butpush = (perdata & 0xF) << 8; // fix me
                        per[i+percounter].butpushonce = (oldperpush[i+percounter] ^ per[i+percounter].butpush) & per[i+percounter].butpush;
                        ((mousedata_struct *)per)[i+percounter].flags = perdata >> 4;

                        ((mousedata_struct *)per)[i+percounter].x = SMPC_REG_OREG(oregcounter+1);
                        ((mousedata_struct *)per)[i+percounter].y = SMPC_REG_OREG(oregcounter+2);

                        // X Overflow
                        if (((mousedata_struct *)per)[i+percounter].flags & 0x4)
                           ((mousedata_struct *)per)[i+percounter].x++;

                        // X Sign
                        if (((mousedata_struct *)per)[i+percounter].flags & 0x1)
                           ((mousedata_struct *)per)[i+percounter].x = 0 - ((mousedata_struct *)per)[i+percounter].x - 1;

                        // Y Overflow
                        if (((mousedata_struct *)per)[i+percounter].flags & 0x8)
                           ((mousedata_struct *)per)[i+percounter].y++;

                        // Y Sign
                        if (((mousedata_struct *)per)[i+percounter].flags & 0x2)
                           ((mousedata_struct *)per)[i+percounter].y = 0 - ((mousedata_struct *)per)[i+percounter].y - 1;

                        break;
                     }
                     default: break;
                  }

                  oregcounter += (perid & 0xF);
                  break;
               }
               default: break;
            }
            percounter++;
         }
      }
   }

   // Issue break
   SMPC_REG_IREG(0) = 0x40;
}

//////////////////////////////////////////////////////////////////////////////

void PerInit()
{
   // Make sure vblank-out's and SMPC's interrupts are disabled
   BIOS_ChangeSCUInterruptMask(0xFFFFFFF, MASK_VBLANKOUT | MASK_SYSTEMMANAGER);

   SMPC_REG_DDR1 = 0;
   SMPC_REG_EXLE = 0;
   SMPC_REG_IOSEL = 0; // Set both ports to SMPC control mode

   // Add our vblank-out interrupt
   BIOS_SetSCUInterrupt(0x41, perhandler);

   // Add our SMPC interrupt
   BIOS_SetSCUInterrupt(0x47, smpchandler);

   // Clear internal variables
   memset(oldperpush, 0, MAX_PERIPHERALS * sizeof(u16));
   memset(per, 0, sizeof(perdata_struct) * MAX_PERIPHERALS);

   // Unmask vblank-out interrupt
   BIOS_ChangeSCUInterruptMask(~(MASK_VBLANKOUT | MASK_SYSTEMMANAGER), 0);
}

//////////////////////////////////////////////////////////////////////////////

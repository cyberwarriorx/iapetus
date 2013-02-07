/*  Copyright 2006-2009, 2013 Theo Berkau

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
#include "gui.h"

void gui_clear_scr(font_struct *font)
{
   vdp_disp_off();
   vdp_clear_screen(font);
   vdp_disp_on();
}

int gui_do_menu(menu_item_struct *menu, font_struct *font, int x, int y, const char *title, int flags, int num_lines)
{
   int cursel=0;
   int nummenu=0;
   int i;
   int menux, menuy;
   int width=320, height=224; // TODO: Should be autodetected
   int win_width, win_height;
   int len, maxlen=0;

   // Setup Priorities
   VDP2_REG_PRIR = 0x0002;
   VDP2_REG_PRISA = 0x0101;
   VDP2_REG_PRISB = 0x0101;
   VDP2_REG_PRISC = 0x0101;
   VDP2_REG_PRISD = 0x0101;

   // Make sure current screen is clear
   gui_window_init();

   // Figure out how many menu items there are, and get the longest one
   for (;;)
   {     
      if (menu[nummenu].name[0] == '\0') break;
      len = strlen((char *)menu[nummenu].name);
      if (len > maxlen)
         maxlen = len;
      nummenu++;
   }

   // Make sure title isn't longer either
   len = strlen(title);
   if (len > maxlen)
      maxlen = len;

   win_width = 6 + 8 + 6 + (maxlen * font->width);
   if (num_lines == -1)
      win_height = 5+5+5+8+nummenu*8;
   else
   	  win_height = 5+5+5+8+num_lines*8;

   if (flags & MTYPE_CENTER)
   {
      x = ((width - win_width) >> 1) + x;
      y = ((height - win_height) >> 1) + y;
   }
   else if (flags & MTYPE_RIGHTALIGN)
   {
      x = width - win_width - x;
      y = height - win_height - y;
   }

   gui_window_draw(x, y, win_width, win_height, 0, RGB16(26, 26, 25) | 0x8000);

   // Draw title
   vdp_printf(font, x+6, y+5, 15, "%s", title);

   menux = x + 6;
   menuy = y + 5 + 8 + 5;

   // Add Menu Sprite to draw list
   for (i = 0; i < nummenu; i++)
   {
      if (num_lines != -1 && i >= num_lines)
         break;
      vdp_printf(font, menux + (1 * 8) + 1, menuy + (i * 8)+1, 0x10, (char *)menu[i].name);
      vdp_printf(font, menux + (1 * 8), menuy + (i * 8), 0xF, (char *)menu[i].name);
   }

   // Add Selected Menu Item(should always be first item) sprite to draw list
   vdp_printf(font, menux, menuy+(cursel * 8), 0xF, ">");

   for (;;)
   {
      vdp_vsync(); 

      // poll joypad(if menu item is selected, return)
      if (per[0].but_push_once & PAD_UP)
      {
         vdp_printf(font, menux, menuy + (cursel * 8), 0, ">");

         if (cursel != 0)
            cursel--;
         else
            cursel = (nummenu - 1);

         vdp_printf(font, menux, menuy + (cursel * 8), 0xF, ">");
      }
      else if (per[0].but_push_once & PAD_DOWN)
      {
         vdp_printf(font, menux, menuy + (cursel * 8), 0x0, ">");

         if (cursel != (nummenu - 1))
            cursel++;
         else
            cursel = (nummenu - 1);

         vdp_printf(font, menux, menuy + (cursel * 8), 0xF, ">");
      }

      if (per[0].but_push_once & PAD_A)
      {
         gui_clear_scr(font);

         vdp_start_draw_kist();
         vdp_end_draw_list();

         if (menu[cursel].func)
            menu[cursel].func();
         return cursel;
      }
      else if (per[0].but_push_once & PAD_B)
      {
         gui_clear_scr(font);

         vdp_start_draw_kist();
         vdp_end_draw_list();
         return -1;
      }
   }
}


//////////////////////////////////////////////////////////////////////////////

void gui_window_init(void)
{
}

//////////////////////////////////////////////////////////////////////////////

void gui_window_draw(int x, int y, int width, int height, u16 fgcolor, u16 bgcolor)
{
   sprite_struct localcoord;
   sprite_struct sprite;

   localcoord.attr = 0;
   localcoord.x = 0;
   localcoord.y = 0;

   vdp_start_draw_kist();
   vdp_local_coordinate(&localcoord);

   sprite.attr = SPRITE_PRECLIPENABLE | SPRITE_HIGHSPEEDSHRINKDISABLE |
                 SPRITE_CLIPDISABLE | SPRITE_ENDCODEDISABLE | SPRITE_16BPP;
   sprite.bank = bgcolor;
   sprite.x = x;
   sprite.y = y;
   sprite.x2 = x+width;
   sprite.y2 = y;
   sprite.x3 = x+width;
   sprite.y3 = y+height;
   sprite.x4 = x;
   sprite.y4 = y+height;

   vdp_draw_polygon(&sprite);

   // White lines
   sprite.x += 1;
   sprite.y += 1;
   sprite.x2 -= 1;
   sprite.y2 += 1;
   sprite.bank = RGB16(31, 31, 31) | 0X8000;
   vdp_draw_line(&sprite);

   sprite.x2 = sprite.x;
   sprite.y2 = sprite.y4-1;
   vdp_draw_line(&sprite);

   // Gray lines
   sprite.x = sprite.x2;
   sprite.y = sprite.y2;
   sprite.x2 = sprite.x3-1;
   sprite.y2 = sprite.y3-1;
   sprite.bank = RGB16(16, 16, 16) | 0X8000;
   vdp_draw_line(&sprite);

   sprite.x = sprite.x2;
   sprite.y = y+1;
   vdp_draw_line(&sprite);

   // Dark lines
   sprite.x+=1;
   sprite.y-=1;
   sprite.x2+=1;
   sprite.y2+=1;
   sprite.bank = RGB16(8, 8, 8) | 0X8000;
   vdp_draw_line(&sprite);

   sprite.x = x;
   sprite.y = sprite.y2;
   vdp_draw_line(&sprite);

   // Caption
   sprite.x = x+3;
   sprite.y = y+3;
   sprite.x2 = x+width-3;
   sprite.y2 = sprite.y;
   sprite.x3 = sprite.x2;
   sprite.y3 = y+12;
   sprite.x4 = sprite.x;
   sprite.y4 = sprite.y3;
   sprite.bank = RGB16(1, 4, 13) | 0X8000;
   vdp_draw_polygon(&sprite);

   vdp_end_draw_list();
}

//////////////////////////////////////////////////////////////////////////////


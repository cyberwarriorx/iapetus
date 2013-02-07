/*  Copyright 2006-2008,2013 Theo Berkau

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

#ifndef GUI_H
#define GUI_H

typedef struct
{
   int x;
   int y;
   int width;
   int height;
   int fgcolor;
   int bgcolor;
} window_struct;

typedef struct
{
   s8 name[26];
   void (*func)();
} menu_item_struct;

#define MTYPE_LEFTALIGN         0x00000000
#define MTYPE_CENTER            0x00000001
#define MTYPE_RIGHTALIGN        0x00000002

int gui_do_menu(menu_item_struct *menu, font_struct *font, int x, int y, const char *title, int flags, int num_lines);
int MessageBox(const char *header, const char *body, int type);
void gui_clear_scr(font_struct *font);

void gui_window_init(void);
void gui_window_draw(int x, int y, int width, int height, u16 fgcolor, u16 bgcolor);

extern font_struct testdispfont;

#endif

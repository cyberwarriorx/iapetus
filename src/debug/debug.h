/*  Copyright 2009 Theo Berkau

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

#ifndef DEBUG_H
#define DEBUG_H

int RemoteDebuggerStart(void *addr);
int RemoteDebuggerStop(void);
int DebuggerStart(void);
void DebuggerSetCodeBreakpoint(u32 addr);
void DebuggerClearCodeBreakpoint();
void DebuggerSetMemoryBreakpoint(u32 addr, u32 val, u32 valmask, int rw, int size);
void DebuggerClearMemoryBreakpoint(u32 addr);
int DebuggerStop(void);

#endif

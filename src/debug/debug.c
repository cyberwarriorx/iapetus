// Almost everything is done here. I just need to finish the AR polling code
// which should use the UBC. Also the UBC handler needs to be implemented
// (in remoteex.s). I think I may remove the Local debugger altogether
// I may also consider adding some profiler code

#include "../iapetus.h"

#define BARA        (*(volatile u32 *)0xFFFFFF40)
#define BAMRA       (*(volatile u32 *)0xFFFFFF44)
#define BBRA        (*(volatile u16 *)0xFFFFFF48)
#define BARB        (*(volatile u32 *)0xFFFFFF60)
#define BAMRB       (*(volatile u32 *)0xFFFFFF64)
#define BBRB        (*(volatile u16 *)0xFFFFFF68)
#define BDRB        (*(volatile u32 *)0xFFFFFF70)
#define BDMRB       (*(volatile u32 *)0xFFFFFF74)
#define BRCR        (*(volatile u16 *)0xFFFFFF78)

#if 0
extern u32 RemoteDebugStart;
extern u32 RemoteDebugSize;
extern u32 RemoteExGeneralIllegalInstruction;
extern u32 RemoteExSlotIllegalInstruction;
extern u32 RemoteExCPUAddressError;
extern u32 RemoteExDMAAddressError;
extern u32 RemoteUBCHandler;
//void RemoteUBCHandler(void) __attribute__ ((interrupt_handler)); 
#endif

void ExGeneralIllegalInstruction(void);
void ExSlotIllegalInstruction(void);
void ExCPUAddressError(void);
void ExDMAAddressError(void);
void UBCHandler(void) __attribute__ ((interrupt_handler));

#if 0
/*
//////////////////////////////////////////////////////////////////////////////

// This will be move to asm
void RemoteUBCHandler(void)
{
   // Let's establish which interrupt was triggered
   if (BRCR & 0x8000)
   {
      // Channel A(Code Breakpoint)
      // Send data back to computer notifying it that we're in a breakpoint(use 0x02 return command)

   }
   else if (BRCR & 0x80)
   {
      // Channel B(Memory Breakpoint)
      // Send data back to computer notifying it that we're in a breakpoint(use 0x02 return command)
   }

   // Clear A and B condition matches
   BRCR = BRCR & 0x3F3F;
}
*/

//////////////////////////////////////////////////////////////////////////////

int RemoteDebuggerStart(void *addr)
{
   u32 *outbuf=(u32 *)addr;
   u32 *buf=(u32 *)RemoteDebugStart;
   u32 i;

   // Copy over code
   for (i = 0; i < RemoteDebugSize; i++)
      outbuf[i] = buf[i];

   // Setup exception functions
   BIOS_SetSH2Interrupt(0x4, addr+RemoteExGeneralIllegalInstruction);
   BIOS_SetSH2Interrupt(0x6, addr+RemoteExSlotIllegalInstruction);
   BIOS_SetSH2Interrupt(0x9, addr+RemoteExCPUAddressError);
   BIOS_SetSH2Interrupt(0xA, addr+RemoteExDMAAddressError);
   BIOS_SetSH2Interrupt(0xC, addr+RemoteUBCHandler);

   // Hijack interrupt handling function here
//   if (so and so word == ?)
//      replace word with bra instruction

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int RemoteDebuggerStop(void)
{
   BIOS_SetSH2Interrupt(0x4, 0);
   BIOS_SetSH2Interrupt(0x6, 0);
   BIOS_SetSH2Interrupt(0x9, 0);
   BIOS_SetSH2Interrupt(0xA, 0);
   BIOS_SetSH2Interrupt(0xC, 0);

   // Give interrupt handling function control back here

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////
#endif

void UBCHandler(void)
{
//   if (BRCR & ?)
}

//////////////////////////////////////////////////////////////////////////////

int DebuggerStart(void)
{
   // This function basically sets up exception handlers, disables remote
   // debugging, and sets things up to allows for breakpoints
   int ret;

   if ((ret = DebuggerStop()) != LAPETUS_ERR_OK)
      return ret;

   BIOS_SetSH2Interrupt(0x4, ExGeneralIllegalInstruction);
   BIOS_SetSH2Interrupt(0x6, ExSlotIllegalInstruction);
   BIOS_SetSH2Interrupt(0x9, ExCPUAddressError);
   BIOS_SetSH2Interrupt(0xA, ExDMAAddressError);

   // Clear the UBC registers
   BBRA = 0;
   BBRB = 0;
   BDRB = 0;
   BRCR = 0;
   BIOS_SetSH2Interrupt(0xC, UBCHandler);

   // Adjust level mask
   if (InterruptGetLevelMask() > 0xE)
      InterruptSetLevelMask(0xE);

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void DebuggerSetCodeBreakpoint(u32 addr)
{
   BARA = addr;
   BAMRA = 0;

   // break on cpu cycles, instruction fetch cycles, read/write cycles
   BBRA = (0x1 << 6) | (0x1 << 4) | (0x3 << 2) | 0; 

   // Clear the condition match flags, break before instruction execution
   BRCR = (BRCR & 0x3BFF);
}

//////////////////////////////////////////////////////////////////////////////

void DebuggerClearCodeBreakpoint()
{
   BBRA = 0;
}

//////////////////////////////////////////////////////////////////////////////

void DebuggerSetMemoryBreakpoint(u32 addr, u32 val, u32 valmask, int rw, int size)
{
   BARB = addr;
   BAMRB = 0;
   BDRB = val;
   BDMRB = valmask;

   // break on cpu cycles, data access cycles, etc. 
   BBRB = (0x1 << 6) | (0x2 << 4) | ((rw & 0x3) << 2) | (size & 0x3);
  
   // Clear the condition match flags, break before instruction execution
   BRCR = (BRCR & 0xFF33);
}

//////////////////////////////////////////////////////////////////////////////

void DebuggerClearMemoryBreakpoint(u32 addr)
{
   BBRB = 0;
}

//////////////////////////////////////////////////////////////////////////////

int DebuggerStop(void)
{
   BIOS_SetSH2Interrupt(0x4, 0);
   BIOS_SetSH2Interrupt(0x6, 0);
   BIOS_SetSH2Interrupt(0x9, 0);
   BIOS_SetSH2Interrupt(0xA, 0);
   BIOS_SetSH2Interrupt(0xC, 0);

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

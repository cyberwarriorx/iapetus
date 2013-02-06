// A Fairly simple "Hello World!" program

#include <lapetus.h>

//////////////////////////////////////////////////////////////////////////////

int main()
{
   screensettings_struct settings;
   font_struct font;

   // This should always be called at the begining of your program.
   InitLapetus(RES_320x224);

   // Setup a screen for us draw on
   settings.isbitmap = TRUE;
   settings.bitmapsize = BG_BITMAP512x256;
   settings.transparentbit = 0;
   settings.color = BG_256COLOR;
   settings.specialpriority = 0;
   settings.specialcolorcalc = 0;
   settings.extrapalettenum = 0;
   settings.mapoffset = 0;
   settings.rotationmode = 0;
   settings.parameteraddr = 0x25E60000;
   VdpRBG0Init(&settings);

   // Use the default palette
   VdpSetDefaultPalette();

   // Setup the default 8x16 1BPP font
   VdpSetDefaultFont(SCREEN_RBG0, &font);

   // Print some text on the screen
   VdpPrintf(&font, 8, 8, 15, "Hello World!");

   // Display everything
   VdpDispOn();

   // Phew, we're all done! Time to rest
   for(;;)
      VdpVsync();
}

//////////////////////////////////////////////////////////////////////////////



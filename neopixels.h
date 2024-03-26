#ifndef __NEOPIXELS
#define __NEOPIXELS

#include "config.h"

class Neopixels {
  public:
  
  // Set the pin modes for the display.
  static void ledSetup();
  
  // Send a pixel to the display.
  static void sendPixel(unsigned char r, unsigned char g , unsigned char b);
  
  // Wait long enough without sending any bits to cause the pixels to latch and display the last sent frame.
  static void show();
};

#endif

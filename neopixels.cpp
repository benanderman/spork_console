#include "neopixels.h"

#ifndef EMULATOR

/*
 This is adapted from an example of how simple driving a Neopixel can be
 This code is optimized for understandability and changability rather than raw speed
 More info at http://wp.josh.com/2014/05/11/ws2812-neopixels-made-easy/
*/

// These are the timing constraints taken mostly from the WS2812 datasheets 
// These are chosen to be conservative and avoid problems rather than for maximum throughput 

#define T1H  900    // Width of a 1 bit in ns = 14.4 cycles
#define T1L  600    // Width of a 1 bit in ns = 9.6 cycles

#define T0H  330    // Width of a 0 bit in ns = 6.4 cycles
#define T0L  900    // Width of a 0 bit in ns = 14.4 cycles

#define RES 6000    // Width of the low gap between bits to cause a frame to latch = 96 cycles

// Here are some convience defines for using nanoseconds specs to generate actual CPU delays

#define NS_PER_SEC (1000000000L)          // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives

#define CYCLES_PER_SEC (F_CPU)

#define NS_PER_CYCLE ( NS_PER_SEC / CYCLES_PER_SEC ) // 62.5 at 16MHz

#define NS_TO_CYCLES(n) ( (n) / NS_PER_CYCLE )

// Send a bit to the string. We must to drop to asm to ensure that the compiler does
// not reorder things and make it so the delay happens in the wrong place.
static inline void sendBit(bool bitVal) {
  if (bitVal) {        // 1 bit
    asm volatile (
      "sbi %[port], %[bit] \n\t"        // Set the output bit
      ".rept %[onCycles] \n\t"          // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"        // Clear the output bit
      ".rept %[offCycles] \n\t"         // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT),
      [onCycles]  "I" (NS_TO_CYCLES(T1H) - 2),    // 1-bit width less overhead for the actual bit setting, note that this delay could be longer and everything would still work
      [offCycles]   "I" (NS_TO_CYCLES(T1L) - 2)   // Minimum interbit delay. Note that we probably don't need this at all since the loop overhead will be enough, but here for correctness
    );
  } else {          // 0 bit
    asm volatile (
      "sbi %[port], %[bit] \n\t"       // Set the output bit
      ".rept %[onCycles] \n\t"         // Now timing actually matters. The 0-bit must be long enough to be detected but not too long or it will be a 1-bit
      "nop \n\t"                       // Execute NOPs to delay exactly the specified number of cycles
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"       // Clear the output bit
      ".rept %[offCycles] \n\t"        // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]      "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]       "I" (PIXEL_BIT),
      [onCycles]  "I" (NS_TO_CYCLES(T0H) - 2),
      [offCycles] "I" (NS_TO_CYCLES(T0L) - 2)
    );
  }
    
    // Note that the inter-bit gap can be as long as you want as long as it doesn't exceed the 5us reset timeout (which is A long time).
    // Here I have been generous and not tried to squeeze the gap tight but instead erred on the side of lots of extra time.
}

// Send bit to PIXEL_PORT / PIXEL_BIT, and PERIPHERAL_PIXEL_PORT / PERIPHERAL_PIXEL_BIT at the same time.
static inline void dualSendBit(bool bitVal, bool peripheralBitVal) {
  if (bitVal) {
    if (peripheralBitVal) {
      asm volatile (
        "sbi %[port], %[bit] \n\t"                      // Set the output bit
        "sbi %[peripheralPort], %[peripheralBit] \n\t"  // Set the peripheral output bit
        ".rept %[onCycles] \n\t"                        // Execute NOPs to delay exactly the specified number of cycles
        "nop \n\t"
        ".endr \n\t"
        "cbi %[port], %[bit] \n\t"                      // Clear the output bit
        "cbi %[peripheralPort], %[peripheralBit] \n\t"  // Clear the peripheral output bit
        ".rept %[offCycles] \n\t"                       // Execute NOPs to delay exactly the specified number of cycles
        "nop \n\t"
        ".endr \n\t"
        ::
        [port]           "I" (_SFR_IO_ADDR(PIXEL_PORT)),
        [bit]            "I" (PIXEL_BIT),
        [peripheralPort] "I" (_SFR_IO_ADDR(PERIPHERAL_PIXEL_PORT)),
        [peripheralBit]  "I" (PERIPHERAL_PIXEL_BIT),
        [onCycles]       "I" (NS_TO_CYCLES(T1H) - 4),    // 1-bit width less overhead  for the actual bit setting, note that this delay could be longer and everything would still work
        [offCycles]      "I" (NS_TO_CYCLES(T1L) - 4)     // Minimum interbit delay. Note that we probably don't need this at all since the loop overhead will be enough, but here for correctness
      );
    } else {
      asm volatile (
        "sbi %[port], %[bit] \n\t"                      // Set the output bit
        "sbi %[peripheralPort], %[peripheralBit] \n\t"  // Set the peripheral output bit
        ".rept %[onCycles] \n\t"                        // Execute NOPs to delay exactly the specified number of cycles
        "nop \n\t"
        ".endr \n\t"
        "cbi %[peripheralPort], %[peripheralBit] \n\t"  // Clear the peripheral output bit
        ".rept %[extraOnCycles] \n\t"
        "nop \n\t"
        ".endr \n\t"
        "cbi %[port], %[bit] \n\t"                      // Clear the output bit
        ".rept %[offCycles] \n\t"
        "nop \n\t"
        ".endr \n\t"
        ::
        [port]           "I" (_SFR_IO_ADDR(PIXEL_PORT)),
        [bit]            "I" (PIXEL_BIT),
        [peripheralPort] "I" (_SFR_IO_ADDR(PERIPHERAL_PIXEL_PORT)),
        [peripheralBit]  "I" (PERIPHERAL_PIXEL_BIT),
        [onCycles]       "I" (NS_TO_CYCLES(T0H) - 2),        // 1-bit width less overhead  for the actual bit setting, note that this delay could be longer and everything would still work
        [extraOnCycles]  "I" (NS_TO_CYCLES(T1H - T0H) - 2),  // Cycles after turning the first pin off
        [offCycles]      "I" (NS_TO_CYCLES(T0L) - 4)         // Minimum interbit delay. Note that we probably don't need this at all since the loop overhead will be enough, but here for correctness
      );
    }
  } else {
    if (peripheralBitVal) {
      asm volatile (
        "sbi %[port], %[bit] \n\t"                      // Set the output bit
        "sbi %[peripheralPort], %[peripheralBit] \n\t"  // Set the peripheral output bit
        ".rept %[onCycles] \n\t"                        // Execute NOPs to delay exactly the specified number of cycles
        "nop \n\t"
        ".endr \n\t"
        "cbi %[port], %[bit] \n\t"                      // Clear the output bit
        ".rept %[extraOnCycles] \n\t"
        "nop \n\t"
        ".endr \n\t"
        "cbi %[peripheralPort], %[peripheralBit] \n\t"  // Clear the peripheral output bit
        ".rept %[offCycles] \n\t"                       // Execute NOPs to delay exactly the specified number of cycles
        "nop \n\t"
        ".endr \n\t"
        ::
        [port]           "I" (_SFR_IO_ADDR(PIXEL_PORT)),
        [bit]            "I" (PIXEL_BIT),
        [peripheralPort] "I" (_SFR_IO_ADDR(PERIPHERAL_PIXEL_PORT)),
        [peripheralBit]  "I" (PERIPHERAL_PIXEL_BIT),
        [onCycles]       "I" (NS_TO_CYCLES(T0H) - 2),        // 1-bit width less overhead  for the actual bit setting, note that this delay could be longer and everything would still work
        [extraOnCycles]  "I" (NS_TO_CYCLES(T1H - T0H) - 2),  // Cycles after turning the first pin off
        [offCycles]      "I" (NS_TO_CYCLES(T0L) - 4)         // Minimum interbit delay. Note that we probably don't need this at all since the loop overhead will be enough, but here for correctness
      );
    } else {
      asm volatile (
        "sbi %[port], %[bit] \n\t"                      // Set the output bit
        "sbi %[peripheralPort], %[peripheralBit] \n\t"  // Set the peripheral output bit
        ".rept %[onCycles] \n\t"                        // Now timing actually matters. The 0-bit must be long enough to be detected but not too long or it will be a 1-bit
        "nop \n\t"                                      // Execute NOPs to delay exactly the specified number of cycles
        ".endr \n\t"
        "cbi %[port], %[bit] \n\t"                      // Clear the output bit
        "cbi %[peripheralPort], %[peripheralBit] \n\t"  // Clear the peripheral output bit
        ".rept %[offCycles] \n\t"                       // Execute NOPs to delay exactly the specified number of cycles
        "nop \n\t"
        ".endr \n\t"
        ::
        [port]           "I" (_SFR_IO_ADDR(PIXEL_PORT)),
        [bit]            "I" (PIXEL_BIT),
        [peripheralPort] "I" (_SFR_IO_ADDR(PERIPHERAL_PIXEL_PORT)),
        [peripheralBit]  "I" (PERIPHERAL_PIXEL_BIT),
        [onCycles]       "I" (NS_TO_CYCLES(T0H) - 4),
        [offCycles]      "I" (NS_TO_CYCLES(T0L) - 4)
      );
    }
  }
}
  
static void Neopixels::sendByte(unsigned char byte, unsigned char peripheralByte) {
  for (unsigned char bit = 0; bit < 8; bit++ ) {
    // Neopixel wants bit in highest-to-lowest order
    // so send highest bit (bit #7 in an 8-bit byte since they start at 0)
    // and then shift left so bit 6 moves into 7, 5 moves into 6, etc.
    dualSendBit(bitRead(byte, 7), bitRead(peripheralByte, 7));
    byte <<= 1;
    peripheralByte <<= 1;
  }
} 
  
static void Neopixels::ledSetup() {
  bitSet(PIXEL_DDR, PIXEL_BIT);
}

static void Neopixels::sendPixel(unsigned char r, unsigned char g, unsigned char b)  {
  cli();
  sendByte(g, g); // Neopixel wants colors in green then red then blue order
  sendByte(r, r);
  sendByte(b, b);
  sei();
}


// Just wait long enough without sending any bits to cause the pixels to latch and display the last sent frame
static void Neopixels::show() {
  _delay_us( (RES / 1000UL) + 1); // Round up since the delay must be _at_least_ this long (too short might not work, too long not a problem)
}

#endif

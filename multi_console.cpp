#include "multi_console.h"
#include "config.h"

void MultiConsole::run_peripheral_mode() {
  pinMode(CONTROLLER_SHLD_PIN, INPUT);
  pinMode(CONTROLLER_CLK_PIN, INPUT);
  pinMode(CONTROLLER_2_SER_PIN, OUTPUT);
  pinMode(CONTROLLER_2_CONN_PIN, OUTPUT);
  digitalWrite(CONTROLLER_2_CONN_PIN, HIGH);

  cli();
  asm volatile (
    "in r16, %[peripheralSEROutPort] \n\t"
    "mov r17, r16 \n\t"
    "andi r16, (~(1 << %[peripheralSEROutBit]))\n\t" // r16 = turn SER bit off
    "ori r17, (1 << %[peripheralSEROutBit])\n\t"     // r17 = turn SER bit on

    "in r18, %[pixelOutPort] \n\t"
    "mov r19, r18 \n\t"
    "andi r18, (~(1 << %[pixelOutBit]))\n\t" // r18 = turn pixel bit off
    "ori r19, (1 << %[pixelOutBit])\n\t"     // r19 = turn pixel bit on

    "startMirroringDisplay: \n\t"
      "sbis %[pixelInPin], %[pixelInBit] \n\t"
      "rjmp turnDisplayBitOff \n\t"
      "rjmp turnDisplayBitOn \n\t"

    "turnDisplayBitOff: \n\t"
      "out %[pixelOutPort], r18 \n\t"
    "displayBitOffLoop: \n\t"
      "sbis %[controllerSHLDPin], %[controllerSHLDBit] \n\t"
      "rjmp startForwardingController \n\t"
      "sbis %[pixelInPin], %[pixelInBit] \n\t"
      "rjmp displayBitOffLoop \n\t"
      "rjmp turnDisplayBitOn \n\t"

    "turnDisplayBitOn: \n\t"
      "out %[pixelOutPort], r19 \n\t"
    "displayBitOnLoop: \n\t"
      "sbic %[pixelInPin], %[pixelInBit] \n\t"
      "rjmp displayBitOnLoop \n\t"
      "rjmp turnDisplayBitOff \n\t"
    
    "startForwardingController: \n\t"
      "waitForSHLDSet: \n\t"
        "sbic %[controllerSHLDPin], %[controllerSHLDBit] \n\t"
        "rjmp waitForSHLDSet \n\t"
      
      "ldi r20, 9 \n\t" // Count from 9 to 1

      "controllerShiftLoop: \n\t"
        "sbic %[peripheralSERInPin], %[peripheralSERInBit] \n\t"
        "out %[peripheralSEROutPort], r17 \n\t"
        "sbis %[peripheralSERInPin], %[peripheralSERInBit] \n\t"
        "out %[peripheralSEROutPort], r16 \n\t"

        "waitForCLKSet: \n\t"
          "sbic %[controllerCLKPin], %[controllerCLKBit] \n\t"
          "rjmp waitForCLKSet \n\t"
        
        "waitForCLKClear: \n\t"
          "sbis %[controllerCLKPin], %[controllerCLKBit] \n\t"
          "rjmp waitForCLKClear \n\t"

        "dec r20 \n\t"
        "breq startMirroringDisplay \n\t" // Jump if r20 is 1
        "rjmp controllerShiftLoop \n\t"
    ::
    [controllerSHLDPin]    "I" (_SFR_IO_ADDR(CONTROLLER_SHLD_IN_PIN)),
    [controllerSHLDBit]    "I" (CONTROLLER_SHLD_BIT),
    [controllerCLKPin]     "I" (_SFR_IO_ADDR(CONTROLLER_CLK_IN_PIN)),
    [controllerCLKBit]     "I" (CONTROLLER_CLK_BIT),
    [pixelOutPort]         "I" (_SFR_IO_ADDR(PIXEL_PORT)),
    [pixelOutBit]          "I" (PIXEL_BIT),
    [pixelInPin]           "I" (_SFR_IO_ADDR(PERIPHERAL_PIXEL_PIN)),
    [pixelInBit]           "I" (PERIPHERAL_PIXEL_BIT),
    [peripheralSERInPin]   "I" (_SFR_IO_ADDR(PERIPHERAL_SER_IN_PIN)),
    [peripheralSERInBit]   "I" (PERIPHERAL_SER_IN_BIT),
    [peripheralSEROutPort] "I" (_SFR_IO_ADDR(PERIPHERAL_SER_OUT_PORT)),
    [peripheralSEROutBit]  "I" (PERIPHERAL_SER_OUT_BIT)
  );
  sei();

  digitalWrite(CONTROLLER_SHLD_PIN, LOW);
  pinMode(CONTROLLER_SHLD_PIN, OUTPUT);
  digitalWrite(CONTROLLER_CLK_PIN, LOW);
  pinMode(CONTROLLER_CLK_PIN, OUTPUT);

  pinMode(CONTROLLER_2_SER_PIN, INPUT);
  pinMode(CONTROLLER_2_CONN_PIN, INPUT);
}

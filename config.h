#define DISPLAY_RCLK_PIN      0
#define DISPLAY_SRCLK_PIN     1
#define DISPLAY_SER_PIN       2
#define DISPLAY_OE_PIN        11
#define AUDIO_PIN             11 // Only audio or display OE can be enabled, not both
#define DISPLAY_NEOPIXEL_PIN  12

#define CONSOLE_LEFT_BUTTON_PIN   3
#define CONSOLE_RIGHT_BUTTON_PIN  4

#define CONTROLLER_SHLD_PIN 5
#define CONTROLLER_CLK_PIN  6

#define CONTROLLER_1_SER_PIN    7
#define CONTROLLER_1_CONN_PIN   8
#define CONTROLLER_2_SER_PIN    9
#define CONTROLLER_2_CONN_PIN   10

#define MAX_DISPLAY_PIXELS  200
#define DISPLAY_INITIAL_BRIGHTNESS 8

#define RGB_CONSOLE 1

#define AUDIO_ENABLED       0
#define BRIGHTNESS_ENABLED  !AUDIO_ENABLED

#define TONE_IF_ENABLED(note, duration) if (AUDIO_ENABLED) tone(AUDIO_PIN, note, duration)

/* 
  Settings for neopixel display; should match DISPLAY_NEOPIXEL_PIN:
*/

// These values depend on which pin your string is connected to and what board you are using 
// More info on how to find these at http://www.arduino.cc/en/Reference/PortManipulation

// These values are for the pin that connects to the Data Input pin on the LED strip. They correspond to...

// Arduino Yun:     Digital Pin 8
// DueMilinove/UNO: Digital Pin 12
// Arduino MeagL    PWM Pin 4

// You'll need to look up the port/bit combination for other boards. 

// Note that you could also include the DigitalWriteFast header file to not need to do this lookup.

#define PIXEL_PORT  PORTB  // Port of the pin the pixels are connected to
#define PIXEL_DDR   DDRB   // Port of the pin the pixels are connected to
#define PIXEL_BIT   4      // Bit of the pin the pixels are connected to


#if RGB_CONSOLE
  #define BRIGHTNESS_ENABLED 0
  #define DISPLAY_NEOPIXEL_PIN  2
  #define PIXEL_PORT  PORTD
  #define PIXEL_DDR   DDRD
  #define PIXEL_BIT   2
#endif

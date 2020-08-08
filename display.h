#ifndef __DISPLAY
#define __DISPLAY

#include "config.h"
#include "arduino.h"

typedef uint32_t (*get_pixel_func_t)(int x, int y);

class Display {
  public:
  enum Mode {
    rows,
    U
  };

  int RCLK_PIN;
  int SRCLK_PIN;
  int SER_PIN;
  int OE_PIN;

  int height;
  int width;
  Mode mode;

  byte (*palette)[3];

  Display(Mode mode, int width, int height,
    int rclk_pin, int srclk_pin, int ser_pin, int oe_pin, bool neopixels) :
    mode(mode), width(width), height(height),
    RCLK_PIN(rclk_pin), SRCLK_PIN(srclk_pin), SER_PIN(ser_pin), OE_PIN(oe_pin),
    neopixels(neopixels), palette(NULL) {}

  // Returns whether the pixel was on before
  byte set_pixel(int x, int y, byte val);
  
  // Returns whether any pixel being set was set before
  bool set_rect(int x, int y, int width, int height, byte val);
  
  byte get_pixel(int x, int y);
  void clear_all();
  void refresh(get_pixel_func_t get_pixel_func = NULL);
  void set_brightness(byte brightness);
  byte get_brightness();
  
  bool neopixels;

  private:
  byte brightness;
  byte state[MAX_DISPLAY_PIXELS];
};

#endif

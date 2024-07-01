#ifndef __DISPLAY
#define __DISPLAY

#include "config.h"
#include "arduino.h"

typedef uint32_t (*get_pixel_func_t)(int x, int y);

class Display {
  public:
  enum Mode {
    rows, // Left to right, top to bottom rows
    U, // Two columns of rows, top to bottom, then bottom to top
    N // Columns that alternate top to bottom, bottom to top
  } __attribute__((packed));

  Mode mode;
  int width;
  int height;

  int RCLK_PIN;
  int SRCLK_PIN;
  int SER_PIN;
  int OE_PIN;

  uint8_t (*palette)[3];

  Display(Mode mode, int width, int height,
    int rclk_pin, int srclk_pin, int ser_pin, int oe_pin, bool neopixels) :
    mode(mode), width(width), height(height),
    RCLK_PIN(rclk_pin), SRCLK_PIN(srclk_pin), SER_PIN(ser_pin), OE_PIN(oe_pin),
    palette(NULL), neopixels(neopixels) {}

  // Returns whether the pixel was on before
  uint8_t set_pixel(int x, int y, uint8_t val);
  
  // Returns whether any pixel being set was set before
  bool set_rect(int x, int y, int width, int height, uint8_t val);
  
  uint8_t get_pixel(int x, int y);
  void clear_all();
  void refresh(get_pixel_func_t get_pixel_func = NULL);
  void set_brightness(uint8_t brightness);
  uint8_t get_brightness();
  
  bool neopixels;

  private:
  uint8_t brightness;
  uint8_t state[MAX_DISPLAY_PIXELS];
};

#endif

#ifndef __DISPLAY
#define __DISPLAY

#include "config.h"
#include "arduino.h"

class Display {
  public:
  enum Mode {
    rows, // Left to right, top to bottom rows
    U, // Two columns of rows, top to bottom, then bottom to top
    N // Columns that alternate top to bottom, bottom to top
  } __attribute__((packed));

  Mode mode;
  uint8_t width;
  uint8_t height;

  uint8_t RCLK_PIN;
  uint8_t SRCLK_PIN;
  uint8_t SER_PIN;
  uint8_t OE_PIN;

  uint8_t (*palette)[3];

  Display(Mode mode, uint8_t width, uint8_t height,
    uint8_t rclk_pin, uint8_t srclk_pin, uint8_t ser_pin, uint8_t oe_pin, bool neopixels) :
    mode(mode), width(width), height(height),
    RCLK_PIN(rclk_pin), SRCLK_PIN(srclk_pin), SER_PIN(ser_pin), OE_PIN(oe_pin),
    palette(NULL), neopixels(neopixels) {}

  // Returns whether the pixel was on before
  uint8_t set_pixel(int8_t x, int8_t y, uint8_t val);
  
  // Returns whether any pixel being set was set before
  bool set_rect(int8_t x, int8_t y, int8_t width, int8_t height, uint8_t val);
  
  uint8_t get_pixel(int8_t x, int8_t y);
  void clear_all();
  void refresh(bool multi_display = false);
  void set_brightness(uint8_t brightness);
  uint8_t get_brightness();
  
  bool neopixels;

  private:
  uint8_t brightness;
  uint8_t state[MAX_DISPLAY_PIXELS];
};

#endif

#ifndef __DISPLAY
#define __DISPLAY

#include "config.h"
#include "arduino.h"

class Display {
  public:
  enum Mode {
    rows,
    U
  };

  int RCLK_PIN;
  int SRCLK_PIN;
  int SER_PIN;

  int height;
  int width;
  Mode mode;

  Display(Mode mode, int width, int height,
    int rclk_pin, int srclk_pin, int ser_pin) :
    mode(mode), width(width), height(height),
    RCLK_PIN(rclk_pin), SRCLK_PIN(srclk_pin), SER_PIN(ser_pin) {}

  // Returns whether the pixel was on before
  bool set_pixel(int x, int y, bool val);
  
  // Returns whether any pixel being set was set before
  bool set_rect(int x, int y, int width, int height, bool val);
  
  bool get_pixel(int x, int y);
  void clear_all();
  void refresh();

  private:
  bool state[MAX_DISPLAY_PIXELS];
};

#endif

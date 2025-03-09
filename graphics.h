#ifndef __GRAPHICS
#define __GRAPHICS

#include "display.h"

class Controller;

class Graphics {
  public:
  static void clear_rows(Display& disp);
  static void animate_color_rows(Display& disp, uint8_t exclude_value, uint8_t value);
  static void explode_pixels(Display& disp, uint8_t swap_index, uint8_t max_value);

  // Returns whether the game should exit
  static bool end_game(Display &disp, Controller *controllers, int controller_count, uint8_t color, uint8_t swap_index);
};

#endif

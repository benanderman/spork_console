#ifndef __GRAPHICS
#define __GRAPHICS

#include "display.h"

class Controller;

class Graphics {
  public:
  static void clear_rows(Display& disp);
  static void animate_color_rows(Display& disp, byte exclude_value, byte value);
  static void explode_pixels(Display& disp, byte (*palette)[3], byte swap_index, byte max_value);

  // Returns whether the game should exit
  static bool end_game(Display &disp, Controller *controllers, int controller_count, byte color, byte (*palette)[3], byte swap_index);
};

#endif

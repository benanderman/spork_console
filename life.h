#ifndef __LIFE
#define __LIFE

#include "display.h"
#include "controller.h"
#include "input_processor.h"

class Life: public InputProcessor {
  public:
  Display& disp;

  Life(Display& disp, Controller *controllers, uint8_t controller_count);

  // Returns whether to exit to menu
  bool play();

  bool handle_button_down(Controller::Button button, uint8_t controller_index);

  private:
  void cycle();
  // If controller_index != -1, paint with it if A or B is pressed
  void move_cursor(int8_t x, int8_t y, uint8_t controller_index);
  void set_cell(uint8_t x, uint8_t y, bool value);
  
  bool paused;
  int8_t cursor_x;
  int8_t cursor_y;
};

#endif

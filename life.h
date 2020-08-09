#ifndef __LIFE
#define __LIFE

#include "display.h"
#include "controller.h"
#include "input_processor.h"

class Life: public InputProcessor {
  public:
  Display& disp;

  Life(Display& disp, Controller *controllers, int controller_count);

  // Returns whether to exit to menu
  bool play();

  bool handle_button_down(Controller::Button button, int controller_index);

  private:
  void cycle();
  // If controller_index != -1, paint with it if A or B is pressed
  void move_cursor(int x, int y, int controller_index = -1);
  void set_cell(int x, int y, bool value);
  
  bool paused;
  int cursor_x;
  int cursor_y;
};

#endif

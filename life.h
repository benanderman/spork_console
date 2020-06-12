#ifndef __LIFE
#define __LIFE

#include "display.h"
#include "controller.h"

class Life {
  public:
  Display& disp;
  Controller controller;

  Life(Display& disp, Controller controller):
    disp(disp), controller(controller), paused(true), cursor_x(disp.width / 2), cursor_y(disp.height / 2) {}

  // Returns whether to exit to menu
  bool play();

  private:
  bool handle_input();
  void cycle();
  void move_cursor(int x, int y);
  
  bool paused;
  int cursor_x;
  int cursor_y;
};

#endif

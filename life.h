#ifndef __LIFE
#define __LIFE

#include "display.h"
#include "controller.h"

class Life {
  public:
  Display& disp;
  Controller controller;

  Life(Display& disp, Controller controller):
    disp(disp), controller(controller) {}

  // Returns whether to exit to menu
  bool play();

  private:
  bool handle_input();
};

#endif

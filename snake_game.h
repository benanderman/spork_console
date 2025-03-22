#ifndef __SNAKE_GAME
#define __SNAKE_GAME

#include "display.h"
#include "controller.h"
#include "menu.h"

class SnakeGame {
  public:
  Display& disp;
  Controller *controllers;
  int controller_count;

  SnakeGame(Display& disp, Controller *controllers, int controller_count):
    disp(disp), controllers(controllers), controller_count(controller_count) {}

  static MenuOption menuOption();
  static void setPalette(Display& disp);
  static bool run(Display& disp, Controller *controllers, uint8_t controller_count);

  bool play();

  private:
  bool handle_input();
  void cycle();
};

#endif

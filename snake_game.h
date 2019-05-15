#ifndef __SNAKE_GAME
#define __SNAKE_GAME

#include "display.h"
#include "controller.h"

class SnakeGame {
  public:
  Display disp;
  Controller *controllers;
  int controller_count;

  SnakeGame(Display disp, Controller *controllers, int controller_count):
    disp(disp), controllers(controllers), controller_count(controller_count) {}

  void play();

  private:
  void handle_input();
  void cycle();
};

#endif

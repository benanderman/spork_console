#ifndef __MENU
#define __MENU

#include "display.h"
#include "controller.h"

enum MenuChoice {
  snake = 0,
  obstacle,
  sporktris,
  life,
  dice,
  chess,
  peripheral
};

class Menu {
  public:
  Display& disp;
  Controller *controllers;
  int controller_count;
  int LEFT_BUTTON_PIN;
  int RIGHT_BUTTON_PIN;

  Menu(Display& disp, Controller *controllers, int controller_count, int left_button_pin, int right_button_pin):
    disp(disp), controllers(controllers), controller_count(controller_count),
    LEFT_BUTTON_PIN(left_button_pin), RIGHT_BUTTON_PIN(right_button_pin),
    option_index(0) {}
  
  MenuChoice choose(MenuChoice initial_option = MenuChoice::snake);

  private:
  int option_index;
};

#endif

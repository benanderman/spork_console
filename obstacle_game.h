#ifndef __OBSTACLE_GAME
#define __OBSTACLE_GAME

#include "display.h"
#include "controller.h"

class ObstacleGame {
  public:
  Display& disp;
  Controller controller;
  int LEFT_BUTTON_PIN;
  int RIGHT_BUTTON_PIN;

  ObstacleGame(Display& disp, int left_button_pin, int right_button_pin, Controller controller):
    disp(disp), LEFT_BUTTON_PIN(left_button_pin), RIGHT_BUTTON_PIN(right_button_pin),
    controller(controller) {}

  bool play();

  private:
  int player_x, player_y;
  bool handle_input();
  void draw_obstacles(int cycle);
};

#endif

#ifndef __OBSTACLE_GAME
#define __OBSTACLE_GAME

#include "display.h"
#include "controller.h"

class ObstacleGame {
  public:
  Display& disp;
  Controller controller;
  uint8_t LEFT_BUTTON_PIN;
  uint8_t RIGHT_BUTTON_PIN;

  ObstacleGame(Display& disp, uint8_t left_button_pin, uint8_t right_button_pin, Controller controller):
    disp(disp), controller(controller), LEFT_BUTTON_PIN(left_button_pin),
    RIGHT_BUTTON_PIN(right_button_pin) {}

  bool play();

  private:
  int8_t player_x, player_y;
  int8_t obstacles[16];
  bool handle_input();
  void draw_obstacles(long cycle, int level);
};

#endif

#ifndef __OBSTACLE_GAME
#define __OBSTACLE_GAME

#include "display.h"
#include "controller.h"
#include "menu.h"

class ObstacleGame {
  public:
  Display& disp;
  Controller *controllers;
  int controller_count;

  ObstacleGame(Display& disp, Controller *controllers, int controller_count):
    disp(disp), controllers(controllers), controller_count(controller_count) {}

  static MenuOption menuOption();
  static void setPalette(Display& disp);
  static bool run(Display& disp, Controller *controllers, uint8_t controller_count);

  bool play();

  private:
  int8_t player_x, player_y;
  int8_t obstacles[16];
  bool handle_input();
  void draw_obstacles(long cycle, int level);
};

#endif

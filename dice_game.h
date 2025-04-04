#ifndef __DICE_GAME
#define __DICE_GAME

#include "display.h"
#include "controller.h"
#include "input_processor.h"
#include "graphics.h"
#include "menu.h"

struct Obstacles {
  int rows[10][5]; 
  int first_row = 0;
  int num_displayed = 0;

  Obstacles();
  void fill_row(int row);
  bool descend();
  void recycle_lowest_row();
  void resolve_collision(int column, int projectile_color);
  int row_to_index(int row);
  void draw(Display& disp);
};

struct Projectile {
  int x; //0-4, on the grid. -1 signifies a non-existent projectile
  int y;
  int color;

  bool draw(Display& disp);

  Projectile();
  Projectile(int x, int y, int color);
};

struct Projectiles {
  Projectile projectile_list[5];

  void add_projectile(int x, int y, int color);
  void remove_projectile(int index);
  void ascend_all();
  void draw(Display& disp, Obstacles& obstacles);

  Projectiles();
};

class DiceGame: public InputProcessor {
  public:
  Display& disp;
  Controller *controllers;
  int controller_count;

  DiceGame(Display& disp, Controller *controllers, uint8_t controller_count);

  static MenuOption menuOption();
  static void setPalette(Display& disp);
  static bool run(Display& disp, Controller *controllers, uint8_t controller_count);

  bool play();

  private:
  bool handle_button_down(Controller::Button button, uint8_t controller_index);
  bool paused;

  Obstacles obstacles;
  Projectiles projectiles;

  Projectile player; //turns out the player has the exact same attributes as a projectile. Can change later
  void draw_dice();

};

#endif
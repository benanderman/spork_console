#ifndef __SPORKTRIS
#define __SPORKTRIS

#include "display.h"
#include "controller.h"

struct ButtonState {
  bool pressed;
  unsigned long last_change;
  unsigned long last_register;

  ButtonState(): pressed(false), last_change(0), last_register(0) {}
};

struct Tetromino {
  struct Rect {
    byte x, y, x2, y2;
    byte center_x();
  };
  
  byte positions;
  byte axis_x;
  byte axis_y;
  byte cur_pos;
  byte color;
  byte points[4][2];

  Tetromino rotated(bool cw);
  Rect bounding_rect();

  static Tetromino random_piece();
};

class Sporktris {
  public:
  Display& disp;
  Controller controller;

  Sporktris(Display& disp, Controller controller):
    disp(disp), controller(controller) {}

  bool play();

  private:
  byte *board;
  
  Tetromino cur_piece;
  short piece_x;
  short piece_y;
  bool need_new_piece;
  bool clearing_lines;

  int line_count;

  bool paused;

  ButtonState button_states[Controller::Button::__count];

  void update_button_states(unsigned long now);
  bool handle_input(unsigned long now);
  bool handle_button_press(Controller::Button button);
  bool cycle();
  void draw();

  bool is_valid_position(Tetromino piece, int x, int y);
};

#endif

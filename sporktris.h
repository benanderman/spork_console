#ifndef __SPORKTRIS
#define __SPORKTRIS

#include "display.h"
#include "controller.h"

struct Tetromino {
  struct Rect {
    byte x, y, x2, y2;
    byte center_x();
  };
  
  byte positions;
  byte axis_x;
  byte axis_y;
  byte cur_pos;
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
  bool *board;
  
  Tetromino cur_piece;
  short piece_x;
  short piece_y;
  bool need_new_piece;
  bool clearing_lines;

  int line_count;

  bool paused;

  unsigned long last_input_cycles[8];
  
  bool handle_input();
  bool cycle();
  void draw();

  bool is_valid_position(Tetromino piece, int x, int y);
};

#endif

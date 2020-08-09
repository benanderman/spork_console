#ifndef __SPORKTRIS
#define __SPORKTRIS

#include "display.h"
#include "controller.h"
#include "input_processor.h"

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

class Sporktris: public InputProcessor {
  public:
  Display& disp;

  Sporktris(Display& disp, Controller *controllers, int controller_count);

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
  
  bool handle_button_down(Controller::Button button, int controller_index);
  bool cycle();
  void draw();

  bool is_valid_position(Tetromino piece, int x, int y);
};

#endif

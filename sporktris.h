#ifndef __SPORKTRIS
#define __SPORKTRIS

#include "display.h"
#include "controller.h"
#include "input_processor.h"

struct Tetromino {
  struct Rect {
    uint8_t x, y, x2, y2;
    uint8_t center_x();
  };
  
  uint8_t positions;
  uint8_t axis_x;
  uint8_t axis_y;
  uint8_t cur_pos;
  uint8_t color;
  uint8_t points[4][2];

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
  uint8_t *board;
  
  Tetromino cur_piece;
  int8_t piece_x;
  int8_t piece_y;
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

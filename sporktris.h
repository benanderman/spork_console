#ifndef __SPORKTRIS
#define __SPORKTRIS

#include "display.h"
#include "controller.h"
#include "input_processor.h"

#define PIECE_COUNT 7

struct Tetromino {
  struct Rect {
    int8_t x, y, x2, y2;
    int8_t center_x();
  };
  
  uint8_t positions;
  uint8_t axis_x;
  uint8_t axis_y;
  uint8_t cur_pos;
  uint8_t color;
  int8_t points[4][2];

  Tetromino rotated(bool cw);
  Rect bounding_rect();

  static Tetromino piece_of_type(uint8_t type);
};

struct PlayerState {
  PlayerState();
  Tetromino get_next_piece();

  int line_count;
  uint8_t level;

  uint8_t next_piece_types[PIECE_COUNT * 2];
  uint8_t next_piece_index;
  Tetromino cur_piece;
  int8_t piece_x, piece_y;
  bool need_new_piece;
  bool clearing_lines;

  bool alive;
  unsigned long last_cycle;
  unsigned long cycle_length;
};

class Sporktris: public InputProcessor {
  public:
  Display& disp;

  Sporktris(Display& disp, Controller *controllers, uint8_t controller_count);

  bool play();

  private:
  uint8_t board[MAX_DISPLAY_PIXELS];
  PlayerState player_states[2];

  bool paused;
  
  uint8_t get_board_cell(uint8_t x, uint8_t y);
  void set_board_cell(uint8_t x, uint8_t y, uint8_t value);
  uint8_t get_board_cell(uint8_t player_index, uint8_t x, uint8_t y);
  void set_board_cell(uint8_t player_index, uint8_t x, uint8_t y, uint8_t value);

  bool handle_button_down(Controller::Button button, uint8_t controller_index);
  void cycle(uint8_t player_index);
  void draw();

  bool is_valid_position(uint8_t player_index, Tetromino piece, int8_t x, int8_t y);
};

#endif

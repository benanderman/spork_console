#include "string.h"

#include "sporktris.h"
#include "config.h"
#include "graphics.h"
#include "pitches.h"

// Minimum milliseconds between button state changes
#define BUTTON_DEBOUNCE_THRESHOLD 20

Tetromino Tetromino::random_piece() {
  Tetromino pieces[] = {
    { // long
      .positions = 2,
      .axis_x = 1,
      .axis_y = 1,
      .cur_pos = 0,
      .color = 1,
      .points = { {1,0}, {1,1}, {1,2}, {1,3} }
    },
    { // L
      .positions = 4,
      .axis_x = 1,
      .axis_y = 2,
      .cur_pos = 0,
      .color = 2,
      .points = { {0,2}, {1,2}, {2,2}, {2,3} }
    },
    { // S
      .positions = 2,
      .axis_x = 1,
      .axis_y = 2,
      .cur_pos = 0,
      .color = 3,
      .points = { {1,2}, {2,2}, {0,3}, {1,3} }
    },
    { // #
      .positions = 1,
      .axis_x = 1,
      .axis_y = 2,
      .cur_pos = 0,
      .color = 4,
      .points = { {1,2}, {2,2}, {1,3}, {2,3} }
    },
    { // L
      .positions = 4,
      .axis_x = 1,
      .axis_y = 2,
      .cur_pos = 0,
      .color = 5,
      .points = { {0,2}, {1,2}, {2,2}, {0,3} }
    },
    { // S
      .positions = 2,
      .axis_x = 1,
      .axis_y = 2,
      .cur_pos = 0,
      .color = 6,
      .points = { {0,2}, {1,2}, {1,3}, {2,3} }
    },
    { // T
      .positions = 4,
      .axis_x = 1,
      .axis_y = 2,
      .cur_pos = 0,
      .color = 7,
      .points = { {0,2}, {1,2}, {2,2}, {1,3} }
    }
  };
  const byte pieces_count = sizeof(pieces) / sizeof(*pieces);

  static byte random_set[pieces_count * 2];
  const byte random_set_count = sizeof(random_set) / sizeof(*random_set);
  static byte index = 0;
  if (index == 0) {
    for (byte i = 0; i < random_set_count; i++) {
      random_set[i] = i % pieces_count;
    }
    for (byte i = 0; i < random_set_count; i++) {
      byte temp = random_set[i];
      byte swap_index = random(random_set_count - i) + i;
      random_set[i] = random_set[swap_index];
      random_set[swap_index] = temp;
    }
  }

  Tetromino result = pieces[random_set[index]];
  index = (index + 1) % random_set_count;

  return result;
}

Tetromino Tetromino::rotated(bool cw) {
  if (positions == 1) {
    return *this;
  } else if (positions == 2) {
    cw = cur_pos == 0;
  }
  
  Tetromino new_piece = *this;
  new_piece.cur_pos = (cur_pos + 1) % positions;
  for (int i = 0; i < sizeof(points) / sizeof(*points); i++) {
    new_piece.points[i][0] = axis_x + (axis_y - points[i][1]) * (cw ? -1 : 1);
    new_piece.points[i][1] = axis_y + (axis_x - points[i][0]) * (cw ? 1 : -1);
  }
  
  return new_piece;
}

Tetromino::Rect Tetromino::bounding_rect() {
  Tetromino::Rect rect = {.x = 3, .y = 3, .x2 = 0, .y2 = 0};
  for (int i = 0; i < sizeof(points) / sizeof(*points); i++) {
    rect.x = min(rect.x, points[i][0]);
    rect.y = min(rect.y, points[i][1]);
    rect.x2 = max(rect.x2, points[i][0]);
    rect.y2 = max(rect.y2, points[i][1]);
  }
  return rect;
}

byte Tetromino::Rect::center_x() {
  return (x + x2) / 2;
}

Sporktris::Sporktris(Display& disp, Controller *controllers, int controller_count):
  InputProcessor(controllers, controller_count), disp(disp) {
  button_conf[Controller::Button::b] = { .initial = 500, .subsequent = 500};
  button_conf[Controller::Button::a] = { .initial = 500, .subsequent = 500};
  button_conf[Controller::Button::select] = { .initial = 0, .subsequent = 0};
  button_conf[Controller::Button::start] = { .initial = 0, .subsequent = 0};
  button_conf[Controller::Button::down] = { .initial = 100, .subsequent = 50};
  button_conf[Controller::Button::right] = { .initial = 200, .subsequent = 20};
  button_conf[Controller::Button::up] = { .initial = 500, .subsequent = 30};
  button_conf[Controller::Button::left] = { .initial = 200, .subsequent = 20};
}

bool Sporktris::play() {
  randomSeed(analogRead(A0));
  random(7);
  byte palette[][3] = {
    {0, 0, 0},
    {16, 4, 4},
    {4, 16, 4},
    {4, 4, 16},
    {10, 10, 4},
    {10, 4, 10},
    {4, 10, 10},
    {8, 8, 8},
    {24, 0, 0},
    {0, 0, 0}
  };
  disp.palette = palette;
  
  byte board[MAX_DISPLAY_PIXELS];
  memset(board, false, sizeof(board));
  this->board = board;
  need_new_piece = true;
  clearing_lines = false;
  line_count = 0;
  paused = false;

  bool alive = true;
  unsigned long last_cycle = millis();
  unsigned long cycle_length = 500;
  int level = 1;
  while (alive) {
    unsigned long now = millis();

    bool should_exit = handle_input(now);
    if (should_exit) {
      Graphics::clear_rows(disp);
      disp.palette = NULL;
      return true;
    }

    if (paused) {
      continue;
    }

    if (need_new_piece) {
      cur_piece = Tetromino::random_piece();
      Tetromino::Rect rect = cur_piece.bounding_rect();
      piece_x = disp.width / 2 - rect.x - rect.center_x();
      // TODO: Figure out why this doesn't work with negative numbers
      piece_y = -rect.y2 - 1;
      need_new_piece = false;
    }

    if (now > last_cycle + cycle_length) {
      alive = cycle();
      last_cycle = now;
      if (line_count / 10 > level) {
        level++;
        cycle_length = floor(float(cycle_length + 35) / 1.5);
      }
    }

    draw();
  }

  return Graphics::end_game(disp, controllers, controller_count, 8, palette, 9);
}

bool Sporktris::handle_button_down(Controller::Button button, int controller_index) {
  if (paused && button != Controller::Button::start && button != Controller::Button::select) {
    return false;
  }
  
  switch (button) {
    case Controller::Button::start: {
      return true;
    }

    case Controller::Button::select: {
      paused = !paused;
      return false;
    }

    case Controller::Button::left: {
      if (is_valid_position(cur_piece, piece_x - 1, piece_y)) {
        piece_x--;
      }
      return false;
    }

    case Controller::Button::right: {
      if (is_valid_position(cur_piece, piece_x + 1, piece_y)) {
        piece_x++;
      }
      return false;
    }

    case Controller::Button::down: {
      if (is_valid_position(cur_piece, piece_x, piece_y + 1)) {
        piece_y++;
      }
      return false;
    }

    case Controller::Button::up: {
      while (is_valid_position(cur_piece, piece_x, piece_y + 1)) {
        piece_y++;
      }
      return false;
    }

    case Controller::Button::a: {
      Tetromino new_piece = cur_piece.rotated(true);
      if (is_valid_position(new_piece, piece_x, piece_y)) {
        cur_piece = new_piece;
      }
      return false;
    }

    case Controller::Button::b: {
      Tetromino new_piece = cur_piece.rotated(false);
      if (is_valid_position(new_piece, piece_x, piece_y)) {
        cur_piece = new_piece;
      }
      return false;
    }
  }
}

bool Sporktris::cycle() {
  bool alive = true;

  // Move all the lines down to fill the cleared lines, instead of moving the piece
  if (clearing_lines) {
    int top = disp.height - 1;
    for (int y = disp.height - 1; y >= 0; y--) {
      bool is_clear = true;
      for (int x = 0; x < disp.width; x++) {
        if (board[y * disp.width + x]) {
          is_clear = false;
          break;
        }
      }
      
      if (top != y) {
        memcpy(&board[top * disp.width], &board[y * disp.width], disp.width);
        memset(&board[y * disp.width], 0, disp.width);
      }
      
      if (!is_clear) {
        top--;
      }
    }
    clearing_lines = false;
    return true;
  }
  
  if (is_valid_position(cur_piece, piece_x, piece_y + 1)) {
    piece_y++;
  } else {
    // Apply piece to the board
    int top = disp.height;
    int lines_just_cleared = 0;
    for (int i = 0; i < sizeof(cur_piece.points) / sizeof(*cur_piece.points); i++) {
      int px = cur_piece.points[i][0] + piece_x;
      int py = cur_piece.points[i][1] + piece_y;
      top = min(py, top);
      if (py < 0) {
        continue;
      }
      board[py * disp.width + px] = cur_piece.color;
      
      bool cleared_line = true;
      for (int x = 0; x < disp.width; x++) {
        if (!board[py * disp.width + x]) {
          cleared_line = false;
        }
      }
      if (cleared_line) {
        memset(board + py * disp.width, false, disp.width);
        clearing_lines = true;
        line_count++;
        lines_just_cleared++;
      }
    }
    
    int tones[] = {NOTE_B0, NOTE_C6, NOTE_E6, NOTE_G6, NOTE_A6};
    TONE_IF_ENABLED(tones[lines_just_cleared], 100);
    
    // The player is only alive if the top of the piece that just landed is fully on the board
    alive = top >= 0;
    
    need_new_piece = true;
  }
  
  return alive;
}

void Sporktris::draw() {
  disp.clear_all();

  // Draw blocks on the board
  for (int x = 0; x < disp.width; x++) {
    for (int y = 0; y < disp.height; y++) {
      disp.set_pixel(x, y, board[y * disp.width + x]);
    }
  }

  // Draw current piece
  for (int i = 0; i < sizeof(cur_piece.points) / sizeof(*cur_piece.points); i++) {
    int px = cur_piece.points[i][0] + piece_x;
    int py = cur_piece.points[i][1] + piece_y;
    disp.set_pixel(px, py, cur_piece.color);
  }
  
  disp.refresh();
}

bool Sporktris::is_valid_position(Tetromino piece, int x, int y) {
  Tetromino::Rect rect = piece.bounding_rect();
  if (rect.x + x < 0 || rect.x2 + x >= disp.width ||
      rect.y2 + y >= disp.height) {
    return false;
  }
  for (int i = 0; i < sizeof(piece.points) / sizeof(*piece.points); i++) {
    int px = piece.points[i][0] + x;
    int py = piece.points[i][1] + y;
    if (py >= 0 && board[py * disp.width + px]) {
      return false;
    }
  }
  return true;
}

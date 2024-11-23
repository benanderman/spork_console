#include "string.h"

#include "sporktris.h"
#include "config.h"
#include "graphics.h"
#include "pitches.h"

// Minimum milliseconds between button state changes
#define BUTTON_DEBOUNCE_THRESHOLD 20

Tetromino Tetromino::piece_of_type(uint8_t type) {
  switch (type) {
    case 0: return { // long
      .positions = 2,
      .axis_x = 1,
      .axis_y = 1,
      .cur_pos = 0,
      .color = 1,
      .points = { {1,0}, {1,1}, {1,2}, {1,3} }
    };
    case 1: return { // L
      .positions = 4,
      .axis_x = 1,
      .axis_y = 2,
      .cur_pos = 0,
      .color = 2,
      .points = { {0,2}, {1,2}, {2,2}, {2,3} }
    };
    case 2: return { // S
      .positions = 2,
      .axis_x = 1,
      .axis_y = 2,
      .cur_pos = 0,
      .color = 3,
      .points = { {1,2}, {2,2}, {0,3}, {1,3} }
    };
    case 3: return { // #
      .positions = 1,
      .axis_x = 1,
      .axis_y = 2,
      .cur_pos = 0,
      .color = 4,
      .points = { {1,2}, {2,2}, {1,3}, {2,3} }
    };
    case 4: return { // L
      .positions = 4,
      .axis_x = 1,
      .axis_y = 2,
      .cur_pos = 0,
      .color = 5,
      .points = { {0,2}, {1,2}, {2,2}, {0,3} }
    };
    case 5: return { // S
      .positions = 2,
      .axis_x = 1,
      .axis_y = 2,
      .cur_pos = 0,
      .color = 6,
      .points = { {0,2}, {1,2}, {1,3}, {2,3} }
    };
    case 6: return { // T
      .positions = 4,
      .axis_x = 1,
      .axis_y = 2,
      .cur_pos = 0,
      .color = 7,
      .points = { {0,2}, {1,2}, {2,2}, {1,3} }
    };
  };
}

Tetromino Tetromino::rotated(bool cw) {
  if (positions == 1) {
    return *this;
  } else if (positions == 2) {
    cw = cur_pos == 0;
  }
  
  Tetromino new_piece = *this;
  new_piece.cur_pos = (cur_pos + 1) % positions;
  for (uint8_t i = 0; i < sizeof(points) / sizeof(*points); i++) {
    new_piece.points[i][0] = axis_x + (axis_y - points[i][1]) * (cw ? -1 : 1);
    new_piece.points[i][1] = axis_y + (axis_x - points[i][0]) * (cw ? 1 : -1);
  }
  
  return new_piece;
}

Tetromino::Rect Tetromino::bounding_rect() {
  Tetromino::Rect rect = {.x = 3, .y = 3, .x2 = 0, .y2 = 0};
  for (uint8_t i = 0; i < sizeof(points) / sizeof(*points); i++) {
    rect.x = min(rect.x, points[i][0]);
    rect.y = min(rect.y, points[i][1]);
    rect.x2 = max(rect.x2, points[i][0]);
    rect.y2 = max(rect.y2, points[i][1]);
  }
  return rect;
}

int8_t Tetromino::Rect::center_x() {
  return (x + x2) / 2;
}

PlayerState::PlayerState() {
  next_piece_index = 0;
  line_count = 0;
  level = 1;

  memset(next_piece_types, 0, sizeof(next_piece_types));
  next_piece_index = 0;
  cur_piece = Tetromino::piece_of_type(0);
  piece_x = 0;
  piece_y = 0;
  need_new_piece = true;
  clearing_lines = false;

  last_cycle = millis();
  cycle_length = 500;
}

Tetromino PlayerState::get_next_piece() {
  const uint8_t next_piece_types_count = sizeof(next_piece_types) / sizeof(*next_piece_types);
  if (next_piece_index == 0) {
    for (uint8_t i = 0; i < next_piece_types_count; i++) {
      next_piece_types[i] = i % PIECE_COUNT;
    }
    for (uint8_t i = 0; i < next_piece_types_count; i++) {
      uint8_t temp = next_piece_types[i];
      uint8_t swap_index = random(next_piece_types_count - i) + i;
      next_piece_types[i] = next_piece_types[swap_index];
      next_piece_types[swap_index] = temp;
    }
  }

  Tetromino result = Tetromino::piece_of_type(next_piece_types[next_piece_index]);
  next_piece_index = (next_piece_index + 1) % next_piece_types_count;

  return result;
}

Sporktris::Sporktris(Display& disp, Controller *controllers, uint8_t controller_count):
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
  uint8_t palette[][3] = {
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

  player_states[0].alive = controllers[0].is_connected();
  player_states[1].alive = controller_count > 1 && controllers[1].is_connected();
  
  memset(board, 0, sizeof(board));
  paused = false;

  while (player_states[0].alive || player_states[1].alive) {
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

    for (uint8_t p = 0; p < sizeof(player_states) / sizeof(*player_states); p++) {
      PlayerState& player = player_states[p];
      if (player.need_new_piece) {
        player.cur_piece = player.get_next_piece();
        Tetromino::Rect rect = player.cur_piece.bounding_rect();
        player.piece_x = disp.width / 2 - rect.x - rect.center_x();
        player.piece_y = -rect.y2 - 1;
        player.need_new_piece = false;
      }

      if (now > player.last_cycle + player.cycle_length) {
        cycle(p);
        player.last_cycle = now;
        if (player.line_count / 10 + 1 > player.level) {
          player.level++;
          player.cycle_length = (unsigned long)(float(player.cycle_length + 35) / 1.5);
        }
      }
    }

    draw();
  }

  return Graphics::end_game(disp, controllers, controller_count, 8, palette, 9);
}

bool Sporktris::handle_button_down(Controller::Button button, uint8_t controller_index) {
  if (paused && button != Controller::Button::start && button != Controller::Button::select) {
    return false;
  }

  PlayerState& player_state = player_states[controller_index];
  
  switch (button) {
    case Controller::Button::start: {
      return true;
    }

    case Controller::Button::select: {
      paused = !paused;
      return false;
    }

    case Controller::Button::left: {
      if (is_valid_position(controller_index, player_state.cur_piece, player_state.piece_x - 1, player_state.piece_y)) {
        player_state.piece_x--;
      }
      return false;
    }

    case Controller::Button::right: {
      if (is_valid_position(controller_index, player_state.cur_piece, player_state.piece_x + 1, player_state.piece_y)) {
        player_state.piece_x++;
      }
      return false;
    }

    case Controller::Button::down: {
      if (is_valid_position(controller_index, player_state.cur_piece, player_state.piece_x, player_state.piece_y + 1)) {
        player_state.piece_y++;
      }
      return false;
    }

    case Controller::Button::up: {
      while (is_valid_position(controller_index, player_state.cur_piece, player_state.piece_x, player_state.piece_y + 1)) {
        player_state.piece_y++;
      }
      return false;
    }

    case Controller::Button::a: {
      Tetromino new_piece = player_state.cur_piece.rotated(true);
      if (is_valid_position(controller_index, new_piece, player_state.piece_x, player_state.piece_y)) {
        player_state.cur_piece = new_piece;
      }
      return false;
    }

    case Controller::Button::b: {
      Tetromino new_piece = player_state.cur_piece.rotated(false);
      if (is_valid_position(controller_index, new_piece, player_state.piece_x, player_state.piece_y)) {
        player_state.cur_piece = new_piece;
      }
      return false;
    }
  }
}

void Sporktris::cycle(uint8_t player_index) {
  PlayerState& player_state = player_states[player_index];
  if (!player_state.alive) {
    return;
  }

  // Move all the lines down to fill the cleared lines, instead of moving the piece
  if (player_state.clearing_lines) {
    int top = disp.height - 1;
    for (int y = disp.height - 1; y >= 0; y--) {
      bool is_clear = true;
      for (int x = 0; x < disp.width; x++) {
        if (get_board_cell(player_index, x, y)) {
          is_clear = false;
          break;
        }
      }
      
      if (top != y) {
        for (int x = 0; x < disp.width; x++) {
          set_board_cell(player_index, x, top, get_board_cell(player_index, x, y));
          set_board_cell(player_index, x, y, 0);
        }
      }
      
      if (!is_clear) {
        top--;
      }
    }
    player_state.clearing_lines = false;
    return;
  }
  
  Tetromino& cur_piece = player_state.cur_piece;
  if (is_valid_position(player_index, cur_piece, player_state.piece_x, player_state.piece_y + 1)) {
    player_state.piece_y++;
  } else {
    // Apply piece to the board
    int8_t top = disp.height;
    uint8_t lines_just_cleared = 0;
    for (uint8_t i = 0; i < sizeof(cur_piece.points) / sizeof(*cur_piece.points); i++) {
      int8_t px = cur_piece.points[i][0] + player_state.piece_x;
      int8_t py = cur_piece.points[i][1] + player_state.piece_y;
      top = min(py, top);
      if (py < 0) {
        continue;
      }
      set_board_cell(player_index, px, py, cur_piece.color);
      
      bool cleared_line = true;
      for (int x = 0; x < disp.width; x++) {
        if (!get_board_cell(player_index, x, py)) {
          cleared_line = false;
        }
      }
      if (cleared_line) {
        for (int x = 0; x < disp.width; x++) {
          set_board_cell(player_index, x, py, 0);
        }
        player_state.clearing_lines = true;
        player_state.line_count++;
        lines_just_cleared++;
      }
    }
    
    int tones[] = {NOTE_B0, NOTE_C6, NOTE_E6, NOTE_G6, NOTE_A6};
    TONE_IF_ENABLED(tones[lines_just_cleared], 100);
    
    // The player is only alive if the top of the piece that just landed is fully on the board
    player_state.alive = top >= 0;
    
    player_state.need_new_piece = true;
  }
}

void Sporktris::draw() {
  disp.clear_all();

  // Draw blocks on the board
  for (int8_t x = 0; x < disp.width; x++) {
    for (int8_t y = 0; y < disp.height; y++) {
      disp.set_pixel(x, y, get_board_cell(x, y));
    }
  }

  // Draw current piece for each player
  for (uint8_t player_index = 0; player_index < 2; player_index++) {
    PlayerState& player_state = player_states[player_index];
    uint8_t offset = player_index * 4;
    uint8_t mask = 0xF << offset;

    const Tetromino& cur_piece = player_state.cur_piece;
    for (int8_t i = 0; i < sizeof(cur_piece.points) / sizeof(*cur_piece.points); i++) {
      int8_t px = cur_piece.points[i][0] + player_state.piece_x;
      int8_t py = cur_piece.points[i][1] + player_state.piece_y;
      uint8_t pixel = disp.get_pixel(px, py);
      pixel = (pixel & (~mask)) | ((cur_piece.color << offset) & mask);
      disp.set_pixel(px, py, pixel);
    }
  }
  
  disp.refresh(true);
}

bool Sporktris::is_valid_position(uint8_t player_index, Tetromino piece, int8_t x, int8_t y) {
  Tetromino::Rect rect = piece.bounding_rect();
  if (rect.x + x < 0 || rect.x2 + x >= disp.width ||
      rect.y2 + y >= disp.height) {
    return false;
  }
  for (int8_t i = 0; i < sizeof(piece.points) / sizeof(*piece.points); i++) {
    int8_t px = piece.points[i][0] + x;
    int8_t py = piece.points[i][1] + y;
    if (py >= 0 && get_board_cell(player_index, px, py)) {
      return false;
    }
  }
  return true;
}

uint8_t Sporktris::get_board_cell(uint8_t x, uint8_t y) {
  return board[y * disp.width + x];
}

void Sporktris::set_board_cell(uint8_t x, uint8_t y, uint8_t value) {
  board[y * disp.width + x] = value;
}

uint8_t Sporktris::get_board_cell(uint8_t player_index, uint8_t x, uint8_t y) {
  uint8_t offset = player_index * 4;
  uint8_t cell = get_board_cell(x, y);
  return (cell & (0xF << offset)) >> offset;
}

void Sporktris::set_board_cell(uint8_t player_index, uint8_t x, uint8_t y, uint8_t value) {
  uint8_t offset = player_index * 4;
  uint8_t& cell = board[y * disp.width + x];
  uint8_t mask = 0xF << offset;
  cell = (cell & (~mask)) | ((value << offset) & mask);
}

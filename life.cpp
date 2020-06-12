#include "life.h"
#include "graphics.h"

bool Life::play() {
  byte palette[][3] = {
    {0, 0, 0},  // still dead
    {1, 1, 1}, // newly dead
    {8, 8, 8}, // newly alive
    {6, 6, 6}, // still alive
    // With cursor
    {0, 0, 20},  // still dead
    {1, 1, 20}, // newly dead
    {4, 4, 20}, // newly alive
    {2, 2, 18}, // still alive
  };
  disp.palette = palette;

  move_cursor(cursor_x, cursor_y);

  disp.refresh();

  unsigned long last_cycle = millis();
  unsigned long last_move = millis();
  unsigned long cycle_rate = 300;
  unsigned long move_rate = 200;
  
  while (true) {
    unsigned long now = millis();

    if (now > last_move + move_rate) {
      bool exit_game = handle_input();
      if (exit_game) {
        break;
      }
      last_move = now;
    }

    if (!paused && now > last_cycle + cycle_rate) {
      cycle();
      last_cycle = now;
    }
    
    disp.refresh();
    delay(1);
  }

  disp.palette = NULL;
  return true;
}

bool Life::handle_input() {
  Controller::update_state(&controller, 1);

  if (controller[Controller::Button::up]) {
    move_cursor(cursor_x, cursor_y - 1);
  }

  if (controller[Controller::Button::down]) {
    move_cursor(cursor_x, cursor_y + 1);
  }

  if (controller[Controller::Button::left]) {
    move_cursor(cursor_x - 1, cursor_y);
  }

  if (controller[Controller::Button::right]) {
    move_cursor(cursor_x + 1, cursor_y);
  }

  if (controller[Controller::Button::a]) {
    byte cell = disp.get_pixel(cursor_x, cursor_y);
    cell = (cell & ~2) | (cell & 2 ? 0 : 2);
    disp.set_pixel(cursor_x, cursor_y, cell);
  }

  if (controller[Controller::Button::select]) {
    paused = !paused;
  }
  
  return controller[Controller::Button::start];
}

void Life::move_cursor(int x, int y) {
  byte pixel = disp.get_pixel(cursor_x, cursor_y);
  pixel &= ~(1 << 2);
  disp.set_pixel(cursor_x, cursor_y, pixel);
  
  cursor_x = (x + disp.width) % disp.width;
  cursor_y = (y + disp.height) % disp.height;
  
  pixel = disp.get_pixel(cursor_x, cursor_y);
  pixel |= 1 << 2;
  disp.set_pixel(cursor_x, cursor_y, pixel);
}

void Life::cycle() {
  struct Offset {
    int x;
    int y;
  };
  Offset neighbors[] = {
    {-1, -1}, {0, -1}, {1, -1},
    {-1,  0},          {1,  0},
    {-1,  1}, {0,  1}, {1,  1},
  };

  // 3 is 00000011
  for (int x = 0; x < disp.width; x++) {
    for (int y = 0; y < disp.height; y++) {
      byte cell = disp.get_pixel(x, y);
      cell = ((cell & 3) >> 1) | (cell & ~3);
      disp.set_pixel(x, y, cell);
    }
  }
  
  for (int x = 0; x < disp.width; x++) {
    for (int y = 0; y < disp.height; y++) {
      int alive_neighbors = 0;
      for (int i = 0; i < sizeof(neighbors) / sizeof(*neighbors); i++) {
        if (disp.get_pixel((x + neighbors[i].x + disp.width) % disp.width, (y + neighbors[i].y + disp.height) % disp.height) & 1) {
          alive_neighbors++;
        }
      }
      byte cell = disp.get_pixel(x, y);
      bool alive = cell & 1;
      alive = alive_neighbors == 3 || (alive && alive_neighbors == 2);
      disp.set_pixel(x, y, cell | (alive ? 1 : 0) << 1);
    }
  }
}

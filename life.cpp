#include "life.h"
#include "graphics.h"

Life::Life(Display& disp, Controller *controllers, uint8_t controller_count):
  InputProcessor(controllers, controller_count),
  disp(disp), paused(true), cursor_x(disp.width / 2), cursor_y(disp.height / 2) {
  button_conf[Controller::Button::b] = { .initial = 0, .subsequent = 0};
  button_conf[Controller::Button::a] = { .initial = 0, .subsequent = 0};
  button_conf[Controller::Button::select] = { .initial = 0, .subsequent = 0};
  button_conf[Controller::Button::start] = { .initial = 0, .subsequent = 0};
  button_conf[Controller::Button::down] = { .initial = 200, .subsequent = 100};
  button_conf[Controller::Button::right] = { .initial = 200, .subsequent = 100};
  button_conf[Controller::Button::up] = { .initial = 200, .subsequent = 100};
  button_conf[Controller::Button::left] = { .initial = 200, .subsequent = 100};
}

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

  move_cursor(cursor_x, cursor_y, 0);

  disp.refresh();

  unsigned long last_cycle = millis();
  unsigned long cycle_rate = 300;
  
  while (true) {
    unsigned long now = millis();

    bool exit_game = handle_input(now);
    if (exit_game) {
      break;
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

bool Life::handle_button_down(Controller::Button button, uint8_t controller_index) {
  switch (button) {
    case Controller::Button::start: {
      return true;
    }

    case Controller::Button::select: {
      paused = !paused;
      break;
    }

    case Controller::Button::up: {
      move_cursor(cursor_x, cursor_y - 1, controller_index);
      break;
    }

    case Controller::Button::down: {
      move_cursor(cursor_x, cursor_y + 1, controller_index);
      break;
    }

    case Controller::Button::left: {
      move_cursor(cursor_x - 1, cursor_y, controller_index);
      break;
    }

    case Controller::Button::right: {
      move_cursor(cursor_x + 1, cursor_y, controller_index);
      break;
    }

    case Controller::Button::a: {
      set_cell(cursor_x, cursor_y, true);
      break;
    }

    case Controller::Button::b: {
      set_cell(cursor_x, cursor_y, false);
      break;
    }
  }
  
  return false;
}

void Life::move_cursor(int8_t x, int8_t y, uint8_t controller_index) {
  uint8_t pixel = disp.get_pixel(cursor_x, cursor_y);
  pixel &= ~(1 << 2);
  disp.set_pixel(cursor_x, cursor_y, pixel);
  
  cursor_x = (x + disp.width) % disp.width;
  cursor_y = (y + disp.height) % disp.height;
  
  pixel = disp.get_pixel(cursor_x, cursor_y);
  pixel |= 1 << 2;
  disp.set_pixel(cursor_x, cursor_y, pixel);

  if (controllers[controller_index].handler_states[Controller::Button::a].is_pressed()) {
    set_cell(cursor_x, cursor_y, true);
  } else if (controllers[controller_index].handler_states[Controller::Button::b].is_pressed()) {
    set_cell(cursor_x, cursor_y, false);
  }
}

void Life::set_cell(uint8_t x, uint8_t y, bool value) {
  byte cell = disp.get_pixel(x, y);
  cell = value ? cell | 2 : cell & ~2;
  disp.set_pixel(x, y, cell);
}

void Life::cycle() {
  struct Offset {
    short x;
    short y;
  };
  Offset neighbors[] = {
    {-1, -1}, {0, -1}, {1, -1},
    {-1,  0},          {1,  0},
    {-1,  1}, {0,  1}, {1,  1},
  };

  // 3 is 00000011
  for (uint8_t x = 0; x < disp.width; x++) {
    for (uint8_t y = 0; y < disp.height; y++) {
      byte cell = disp.get_pixel(x, y);
      cell = ((cell & 3) >> 1) | (cell & ~3);
      disp.set_pixel(x, y, cell);
    }
  }
  
  for (uint8_t x = 0; x < disp.width; x++) {
    for (uint8_t y = 0; y < disp.height; y++) {
      uint8_t alive_neighbors = 0;
      for (uint8_t i = 0; i < sizeof(neighbors) / sizeof(*neighbors); i++) {
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

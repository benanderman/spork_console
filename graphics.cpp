#include "string.h"

#include "graphics.h"
#include "controller.h"

void Graphics::clear_rows(Display& disp) {
  for (int y = 0; y < disp.height; y++) {
    disp.set_rect(0, y, disp.width, 1, 0);
    disp.refresh();
    delay(5);
  }
}

void Graphics::animate_color_rows(Display& disp, uint8_t exclude_value, uint8_t value) {
  for (int y = 0; y < disp.height; y++) {
    for (int x = 0; x < disp.width; x++) {
      if (disp.get_pixel(x, y) != exclude_value) {
        disp.set_pixel(x, y, value);
      }
    }
    disp.refresh();
    delay(5);
  }
}

void Graphics::explode_pixels(Display& disp, uint8_t swap_index, uint8_t max_value) {
  for (int y = 0; y < disp.height; y++) {
    for (int x = 0; x < disp.width; x++) {
      uint8_t pixel = disp.get_pixel(x, y);
      if (pixel == 0) {
        continue;
      }
      disp.palette[swap_index] = disp.palette[pixel];
      disp.set_pixel(x, y, swap_index);
    }
    
    while (disp.palette[swap_index].r < max_value &&
           disp.palette[swap_index].g < max_value &&
           disp.palette[swap_index].b < max_value) {
      disp.palette[swap_index].r = min(max_value, (uint8_t)((disp.palette[swap_index].r + 1) * 1.2));
      disp.palette[swap_index].g = min(max_value, (uint8_t)((disp.palette[swap_index].g + 1) * 1.2));
      disp.palette[swap_index].b = min(max_value, (uint8_t)((disp.palette[swap_index].b + 1) * 1.2));
      disp.refresh();
      delayMicroseconds(300);
    }
    
    for (int x = 0; x < disp.width; x++) {
      disp.set_pixel(x, y, 0);
    }
  }
}

bool Graphics::end_game(Display& disp, Controller *controllers, int controller_count, uint8_t color, uint8_t swap_index) {
  if (disp.neopixels) {
    animate_color_rows(disp, 0, color);
  }
  bool should_exit = false;
  bool done = false;
  while (true) {
    Controller::update_state(controllers, controller_count);
    for (int i = 0; i < controller_count; i++) {
      if (controllers[i][Controller::Button::select] || controllers[i][Controller::Button::a]) {
        done = true;
      }
      if (controllers[i][Controller::Button::start]) {
        should_exit = true;
        done = true;
      }
    }
    if (done) {
      break;
    }
    delay(1);
  }
  
  Graphics::explode_pixels(disp, swap_index, 128);
  
  return should_exit;
}

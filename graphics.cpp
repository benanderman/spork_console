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

void Graphics::animate_color_rows(Display& disp, byte exclude_value, byte value) {
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

void Graphics::explode_pixels(Display& disp, byte (*palette)[3], byte swap_index, byte max_value) {
  for (int y = 0; y < disp.height; y++) {
    for (int x = 0; x < disp.width; x++) {
      byte pixel = disp.get_pixel(x, y);
      if (pixel == 0) {
        continue;
      }
      memcpy(palette[swap_index], palette[disp.get_pixel(x, y)], 3);
      disp.set_pixel(x, y, swap_index);
    }
    
    while (palette[swap_index][0] < max_value &&
           palette[swap_index][1] < max_value &&
           palette[swap_index][2] < max_value) {
      palette[swap_index][0] = min(max_value, (uint8_t)((palette[swap_index][0] + 1) * 1.2));
      palette[swap_index][1] = min(max_value, (uint8_t)((palette[swap_index][1] + 1) * 1.2));
      palette[swap_index][2] = min(max_value, (uint8_t)((palette[swap_index][2] + 1) * 1.2));
      disp.refresh();
      delayMicroseconds(300);
    }
    
    for (int x = 0; x < disp.width; x++) {
      disp.set_pixel(x, y, 0);
    }
  }
}

bool Graphics::end_game(Display& disp, Controller *controllers, int controller_count, byte color, byte (*palette)[3], byte swap_index) {
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
  
  Graphics::explode_pixels(disp, palette, swap_index, 128);
  disp.palette = NULL;
  
  return should_exit;
}

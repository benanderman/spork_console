#include "menu.h"
#include "graphics.h"

void MenuOption::setPalette(Display& disp) {
  this->setPaletteFunction(disp);
}

bool MenuOption::run(Display& disp, Controller *controllers, uint8_t controller_count) {
  return this->runFunction(disp, controllers, controller_count);
}

void MenuOption::draw(Display& disp, int8_t origin_x, int8_t origin_y, int8_t override_color) {
  for (int8_t y = 0; y < 10; y++) {
    for (int8_t x = 0; x < 10; x++) {
      char pixel = pgm_read_byte(&graphic[y * 10 + x]) - '0';
      if (pixel < 0 || pixel > 9) {
        pixel = 0;
      }
      if (pixel != 0 && override_color != -1) {
        pixel = override_color;
      }
      disp.set_pixel(x + origin_x, y + origin_y, pixel);
    }
  }
}

void Menu::run() {
  while (true) {
    option_index = choose(option_index);
    MenuOption& option = options[option_index];
    bool quit = false;
    while (!quit) {
      quit = option.run(disp, controllers, controller_count);
    }
  }
}

uint8_t Menu::choose(uint8_t initial_option = 0) {
  disp.palette[15] = RGB(4,  4,  4);

  bool chosen = false;
  unsigned long last_change = millis();
  unsigned long last_brightness_change = millis();
  while (!chosen) {
    unsigned long now = millis();
    int old_index = option_index;
    Controller::update_state(controllers, controller_count);

    if (now > last_change + 300) {
      for (int c = 0; c < controller_count; c++) {
        if (!controllers[c].is_connected()) {
          continue;
        }
        if (controllers[c][Controller::Button::up]) {
          option_index = (option_count + option_index - 1) % option_count;
        }
        if (controllers[c][Controller::Button::down]) {
          option_index = (option_index + 1) % option_count;
        }
        if (controllers[c][Controller::Button::select]) {
          chosen = true;
        }
      }
      if (LEFT_RIGHT_BUTTONS_ENABLED && digitalRead(CONSOLE_RIGHT_BUTTON_PIN)) {
        chosen = true;
      }
      if (LEFT_RIGHT_BUTTONS_ENABLED && digitalRead(CONSOLE_LEFT_BUTTON_PIN)) {
        option_index = (option_index + 1) % option_count;
      }
      if (old_index != option_index) {
        last_change = now;
      }
    }

    // Update brightness with start + left and right
    if (now > last_brightness_change + 400) {
      byte brightness = disp.get_brightness();
      bool left = false;
      bool right = false;
      for (int c = 0; c < controller_count; c++) {
        if (!controllers[c].is_connected() || !controllers[c][Controller::Button::start]) {
          continue;
        }
        left = left || controllers[c][Controller::Button::left];
        right = right || controllers[c][Controller::Button::right];
      }
      if (left || right) {
        brightness += DISPLAY_MAX_BRIGHTNESS;
        brightness += left * -1 + right * 1;
        brightness = brightness % DISPLAY_MAX_BRIGHTNESS;
        disp.set_brightness(brightness);
        last_brightness_change = now;
      }
    }

    bool looping = abs(option_index - old_index) > 1;
    int offset = (option_index - old_index) * 10;
    offset = looping ? -offset / option_count : offset;
    int increment = offset > 0 ? -1 : 1;
    for (; offset != 0; offset += increment) {
      disp.clear_all();
      for (int i = option_index - 2; i <= option_index + 2; i++) {
        int looped_i = (i + option_count) % option_count;
        int8_t override_color = looped_i == old_index ? -1 : 15;
        options[looped_i].draw(disp, 0, 10 * (i - option_index) + (disp.height / 4) + offset, override_color);
      }
      disp.refresh();
      delay(15);
    }

    options[option_index].setPalette(disp);

    disp.clear_all();
    for (int i = option_index - 1; i <= option_index + 1; i++) {
      int looped_i = (i + option_count) % option_count;
      int8_t override_color = looped_i == option_index ? -1 : 15;
      options[looped_i].draw(disp, 0, 10 * (i - option_index) + (disp.height / 4), override_color);
    }
    
    disp.refresh(false);
    delay(4);
  }

  // Animate clearing the display, to avoid a power surge
  Graphics::clear_rows(disp);
  
  return option_index;
}

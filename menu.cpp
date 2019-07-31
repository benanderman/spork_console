#include "menu.h"
#include "graphics.h"

static const char PROGMEM snake_graphic[] =
      "__________"
      "_OOOOO__O_"
      "_____O____"
      "__OOOO____"
      "__O_____O_"
      "_OO__OOOO_"
      "_O___O____"
      "_OOOOO____"
      "__________"
      "__________";

static const char PROGMEM obstacle_graphic[] =
      "__________"
      "_OO_______"
      "_OO_______"
      "__________"
      "______OO__"
      "______OO__"
      "__________"
      "__OO______"
      "__OO_OO___"
      "_____OO___";

static const char PROGMEM sporktris_graphic[] =
      "__________"
      "__________"
      "__________"
      "__OO______"
      "__O_______"
      "__O_______"
      "________OO"
      "O____O__OO"
      "OO_O_OOOOO"
      "OO_OOOOO_O";

struct Option {
  MenuChoice choice;

  Option(MenuChoice choice): choice(choice) {}

  void draw(Display& disp, int x, int y, bool invert) {
    int x_origin = x;
    int y_origin = y;
    for (int i = 0; i < 100; i++) {
      int x = x_origin + i % 10;
      int y = y_origin + i / 10;
      bool val = read_graphic(i) != '_';
      disp.set_pixel(x, y, invert ? !val : val);
    }
  }

  char read_graphic(int index) {
    switch (choice) {
      case snake: return pgm_read_byte(&snake_graphic[index]);
      case obstacle: return pgm_read_byte(&obstacle_graphic[index]);
      case sporktris: return pgm_read_byte(&sporktris_graphic[index]);
    }
  }
};

MenuChoice Menu::choose() {
  Option options[] = {
    Option(MenuChoice::snake),
    Option(MenuChoice::obstacle),
    Option(MenuChoice::sporktris)
  };

  bool chosen = false;
  int option_count = sizeof(options) / sizeof(*options);
  unsigned long last_change = millis();
  unsigned long last_brightness_change = millis();
  while (!chosen) {
    unsigned long now = millis();
    Controller::update_state(controllers, controller_count);
    if (now > last_change + 300) {
      int old_index = option_index;
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
      if (digitalRead(RIGHT_BUTTON_PIN)) {
        chosen = true;
      }
      if (digitalRead(LEFT_BUTTON_PIN)) {
        option_index = (option_index + 1) % option_count;
      }
      if (option_index - display_offset < 0) {
        display_offset = option_index;
      }
      if (option_index - display_offset >= 2) {
        display_offset = option_index - 1;
      }
      if (old_index != option_index) {
        last_change = now;
      }
    }

    // Update brightness with left and right
    if (now > last_brightness_change + 100) {
      byte brightness = disp.get_brightness();
      bool left = false;
      bool right = false;
      for (int c = 0; c < controller_count; c++) {
        if (!controllers[c].is_connected()) {
          continue;
        }
        left = left || controllers[c][Controller::Button::left];
        right = right || controllers[c][Controller::Button::right];
      }
      if (left || right) {
        brightness += left * -1 + right * 1;
        disp.set_brightness(byte(brightness));
        last_brightness_change = now;
      }
    }
    
    for (int i = 0; i < sizeof(options) / sizeof(*options); i++) {
      options[i].draw(disp, 0, 10 * (i - display_offset), i == option_index);
    }
    disp.refresh();
    delay(10);
  }

  // Animate clearing the display, to avoid a power surge
  Graphics::clear_rows(disp);
  
  return options[option_index].choice;
}

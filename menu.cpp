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

static const char PROGMEM life_graphic[] =
      "__________"
      "__________"
      "__0_______"
      "___00_____"
      "__00______"
      "_______0__"
      "_____0_0__"
      "______00__"
      "__________"
      "__________";

struct Option {
  MenuChoice choice;

  Option(MenuChoice choice): choice(choice) {}

  void draw(Display& disp, int x, int y, bool selected) {
    int x_origin = x;
    int y_origin = y;
    for (int i = 0; i < 100; i++) {
      int x = x_origin + i % 10;
      int y = y_origin + i / 10;
      bool val = read_graphic(i) != '_';
      byte color = selected ? !val : val;
      if (disp.neopixels) {
        if (val) {
          color = selected ? 2 + choice : 1;
        } else {
          color = selected ? 6 : 0;
        }
      }
      disp.set_pixel(x, y, color);
    }
  }

  char read_graphic(int index) {
    switch (choice) {
      case snake: return pgm_read_byte(&snake_graphic[index]);
      case obstacle: return pgm_read_byte(&obstacle_graphic[index]);
      case sporktris: return pgm_read_byte(&sporktris_graphic[index]);
      case life: return pgm_read_byte(&life_graphic[index]);
    }
  }
};

MenuChoice Menu::choose() {
  Option options[] = {
    Option(MenuChoice::snake),
    Option(MenuChoice::obstacle),
    Option(MenuChoice::sporktris),
    Option(MenuChoice::life)
  };

  byte palette[][3] = {
    {0, 0, 0},
    {8, 8, 8},
    {0, 0, 24},
    {0, 24, 0},
    {24, 0, 0},
    {8, 16, 0},
    {1, 1, 1},
  };
  disp.palette = palette;

  get_pixel_func_t get_pixel_func = NULL;

  bool chosen = false;
  const int option_count = sizeof(options) / sizeof(*options);
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
        if (controllers[c][Controller::Button::a]) {
          get_pixel_func = &Graphics::rainbow_cycle;
        }
        if (controllers[c][Controller::Button::b]) {
          get_pixel_func = NULL;
        }
      }
      if (!disp.neopixels && digitalRead(RIGHT_BUTTON_PIN)) {
        chosen = true;
      }
      if (!disp.neopixels && digitalRead(LEFT_BUTTON_PIN)) {
        option_index = (option_index + 1) % option_count;
      }
      if (old_index != option_index) {
        last_change = now;
      }
    }

    // Update brightness with left and right
    if (now > last_brightness_change + 500) {
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

    bool looping = abs(option_index - old_index) > 1;
    int offset = (option_index - old_index) * 10;
    offset = looping ? -offset / option_count : offset;
    int increment = offset > 0 ? -1 : 1;
    for (; offset != 0; offset += increment) {
      disp.clear_all();
      for (int i = option_index - 2; i <= option_index + 2; i++) {
        int looped_i = (i + option_count) % option_count;
        options[looped_i].draw(disp, 0, 10 * (i - option_index) + (disp.height / 4) + offset, false);
      }
      disp.refresh();
      delay(15);
    }

    disp.clear_all();
    for (int i = option_index - 1; i <= option_index + 1; i++) {
      int looped_i = (i + option_count) % option_count;
      options[looped_i].draw(disp, 0, 10 * (i - option_index) + (disp.height / 4), i == option_index);
    }
    
    disp.refresh(get_pixel_func);
    delay(4);
  }

  // Animate clearing the display, to avoid a power surge
  Graphics::clear_rows(disp);
  disp.palette = NULL;
  
  return options[option_index].choice;
}

#include "menu.h"
#include "graphics.h"

struct Option {
  MenuChoice choice;
  const char *graphic;

  Option(MenuChoice choice, const char *graphic): choice(choice), graphic(graphic) {}

  void draw(Display& disp, int x, int y, bool invert) {
    int i = 0;
    int x_origin = x;
    while (graphic[i] != 0) {
      if (graphic[i] == '|') {
        x = x_origin;
        y++;
      } else {
        bool val = graphic[i] != '_';
        disp.set_pixel(x, y, invert ? !val : val);
        x++;
      }
      i++;
    }
  }
};

MenuChoice Menu::choose() {
  Option options[] = {
    Option(MenuChoice::snake,
      "__________|"
      "_OOOOO__O_|"
      "_____O____|"
      "__OOOO____|"
      "__O_____O_|"
      "_OO__OOOO_|"
      "_O___O____|"
      "_OOOOO____|"
      "__________|"
      "__________|"
    ),
    Option(MenuChoice::obstacle,
      "__________|"
      "_OO_______|"
      "_OO_______|"
      "__________|"
      "______OO__|"
      "______OO__|"
      "__________|"
      "__OO______|"
      "__OO_OO___|"
      "_____OO___|"
    ),
    Option(MenuChoice::sporktris,
      "__________|"
      "__________|"
      "__________|"
      "__OO______|"
      "__O_______|"
      "__O_______|"
      "________OO|"
      "O____O__OO|"
      "OO_O_OOOOO|"
      "OO_OOOOO_O|"
    )
  };

  bool chosen = false;
  int option_index = 0;
  int display_offset = 0;
  int option_count = sizeof(options) / sizeof(*options);
  unsigned long last_change = millis();
  while (!chosen) {
    unsigned long now = millis();
    if (now > last_change + 300) {
      Controller::update_state(controllers, controller_count);
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

    disp.clear_all();
    for (int i = 0; i < sizeof(options) / sizeof(*options); i++) {
      options[i].draw(disp, 0, 10 * (i - display_offset), i == option_index);
    }
    disp.refresh();
    delay(1);
  }

  // Animate clearing the display, to avoid a power surge
  Graphics::clear_rows(disp);
  
  return options[option_index].choice;
}

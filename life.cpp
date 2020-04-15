#include "life.h"
#include "graphics.h"

bool Life::play() {
  randomSeed(millis());
  
  byte palette[][3] = {
    {0, 0, 0},
    {4, 16, 4},
    {8, 0, 16},
    {16, 8, 0},
  };
  disp.palette = palette;

  unsigned long last_pixel = millis();
  unsigned long new_pixel_rate = 50;
  
  while (1) {
    unsigned long now = millis();

    bool exit_game = handle_input();
    if (exit_game) {
      break;
    }

    // Set a random pixel to a random value 0-3 every new_pixel_rate milliseconds
    if (now > last_pixel + new_pixel_rate) {
      last_pixel = now;
      int x = random(disp.width);
      int y = random(disp.height);
      char value = random(3) + 1;
      disp.set_pixel(x, y, value);
    }
    
    disp.refresh();
    delay(1);
  }

  disp.palette = NULL;
  return true;
}

bool Life::handle_input() {
  Controller::update_state(&controller, 1);
  return controller[Controller::Button::start];
}

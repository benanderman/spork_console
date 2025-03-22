#ifndef __MENU
#define __MENU

#include "display.h"
#include "controller.h"

class MenuOption {
  public:
  MenuOption(const char *graphic, void (*setPaletteFunction)(Display& disp),
    bool (*runFunction)(Display& disp, Controller *controllers, uint8_t controller_count)):
    graphic(graphic), setPaletteFunction(setPaletteFunction), runFunction(runFunction) {}

  void setPalette(Display& disp);
  void draw(Display& disp, int8_t origin_x, int8_t origin_y, int8_t override_color);
  // Returns whether it should be run again.
  bool run(Display& disp, Controller *controllers, uint8_t controller_count);

  private:
  void (*setPaletteFunction)(Display& disp);
  bool (*runFunction)(Display& disp, Controller *controllers, uint8_t controller_count);
  // 10x10 graphic stored in PROGMEM.
  const char *graphic;
};

class Menu {
  public:
  Display& disp;
  Controller *controllers;
  uint8_t controller_count;
  MenuOption *options;
  uint8_t option_count;

  Menu(Display& disp, Controller *controllers, uint8_t controller_count, MenuOption *options, uint8_t option_count):
    disp(disp), controllers(controllers), controller_count(controller_count),
    options(options), option_count(option_count), option_index(0) {}
  
  uint8_t choose(uint8_t initial_option = 0);
  void run();

  private:
  int option_index;
};

#endif

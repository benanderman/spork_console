#ifndef __SPORK_CONSOLE
#define __SPORK_CONSOLE

#include "config.h"
#include "controller.h"
#include "display.h"
#include "menu.h"

// Base class that doesn't store its Display or Controllers, and can use
// configurable ones.
class ConfigurableSporkConsole {
  public:
  ConfigurableSporkConsole(Display& display, Controller *controllers, uint8_t controller_count):
    display(display), controllers(controllers), _controller_count(controller_count) {}
  void begin();
  void run(MenuOption *menuOptions, uint8_t menuOptionCount);

  Display& display;
  Controller *controllers;
  uint8_t controller_count();

  private:
  uint8_t _controller_count;
};

// Subclass that stores its Display and Controllers, configured in a default way.
class SporkConsole: public ConfigurableSporkConsole {
  public:
  SporkConsole();

  private:
  Display _display;
  Controller _controllers[2];
};

#endif

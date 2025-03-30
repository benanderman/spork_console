#include "spork_console.h"
#include "config.h"
#include "controller.h"
#include "display.h"
#include "menu.h"
#include "neopixels.h"
#include "multi_console.h"

static void setup_neopixels() {
  // Clear the internal display
  if (SHIFT_REGISTER_DISPLAY) {
    digitalWrite(DISPLAY_SER_PIN, LOW);
    for (int i = 0; i < MAX_DISPLAY_PIXELS; i++) {
      digitalWrite(DISPLAY_SRCLK_PIN, HIGH);
      digitalWrite(DISPLAY_SRCLK_PIN, LOW);
    }
    digitalWrite(DISPLAY_RCLK_PIN, HIGH);
    digitalWrite(DISPLAY_RCLK_PIN, LOW);
  }
  
  pinMode(DISPLAY_NEOPIXEL_PIN, OUTPUT);

  Neopixels::ledSetup();
}

SporkConsole::SporkConsole():
  _display(NEOPIXEL_DISPLAY ? Display::Mode::rows : Display::Mode::U, 10, 20,
    DISPLAY_RCLK_PIN, DISPLAY_SRCLK_PIN, DISPLAY_SER_PIN, DISPLAY_OE_PIN, NEOPIXEL_DISPLAY),
  _controllers({
    Controller(CONTROLLER_1_SER_PIN, CONTROLLER_1_CONN_PIN),
    Controller(CONTROLLER_2_SER_PIN, CONTROLLER_2_CONN_PIN),
  }),
  ConfigurableSporkConsole(
    _display, _controllers, HARDWARE == ARCADE_CABINET ? 1 : sizeof(_controllers) / sizeof(*_controllers)
  )
{}

void ConfigurableSporkConsole::begin() {
  if (SHIFT_REGISTER_DISPLAY) {
    pinMode(DISPLAY_RCLK_PIN, OUTPUT);
    pinMode(DISPLAY_SRCLK_PIN, OUTPUT);
    pinMode(DISPLAY_SER_PIN, OUTPUT);
    pinMode(DISPLAY_OE_PIN, OUTPUT);
  }

  if (CONTROLLER_AUX_ENABLED) {
    pinMode(CONTROLLER_AUX_CONN_PIN, INPUT_PULLUP);
    pinMode(CONTROLLER_AUX_SHLD_PIN, OUTPUT);
    pinMode(CONTROLLER_AUX_CLK_PIN, OUTPUT);
    pinMode(CONTROLLER_AUX_SER_PIN, INPUT);
  }

  if (LEFT_RIGHT_BUTTONS_ENABLED) {
    pinMode(CONSOLE_LEFT_BUTTON_PIN, INPUT);
    pinMode(CONSOLE_RIGHT_BUTTON_PIN, INPUT);
  }

  pinMode(CONTROLLER_SHLD_PIN, OUTPUT);
  pinMode(CONTROLLER_CLK_PIN, OUTPUT);
  digitalWrite(CONTROLLER_SHLD_PIN, LOW);
  digitalWrite(CONTROLLER_CLK_PIN, LOW);
  pinMode(CONTROLLER_1_SER_PIN, INPUT);
  pinMode(CONTROLLER_1_CONN_PIN, INPUT);
  pinMode(CONTROLLER_2_SER_PIN, INPUT);
  pinMode(CONTROLLER_2_CONN_PIN, INPUT);

  if (NEOPIXEL_DISPLAY) {
    setup_neopixels();
  }
  
  this->display.set_brightness(DISPLAY_INITIAL_BRIGHTNESS);
}

uint8_t ConfigurableSporkConsole::controller_count() {
  return _controller_count;
}

void ConfigurableSporkConsole::run(MenuOption *menuOptions, uint8_t menuOptionCount) {
  Menu menu(display, controllers, controller_count(), menuOptions, menuOptionCount);
  menu.run();
}

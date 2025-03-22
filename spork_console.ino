#include "config.h"
#include "controller.h"
#include "display.h"
#include "menu.h"
#include "obstacle_game.h"
#include "snake_game.h"
#include "sporktris.h"
#include "life.h"
#include "neopixels.h"
#include "dice_game.h"
#include "chess.h"
#include "multi_console.h"

#if CONTROLLER_AUX_ENABLED
// For controlling via internet, etc.; not a real controller
Controller aux_controller = Controller(CONTROLLER_AUX_SER_PIN, CONTROLLER_AUX_CONN_PIN, NULL);
Controller *aux_controller_ptr = &aux_controller;
#else
Controller *aux_controller_ptr = NULL;
#endif

Controller controllers[] = {
  Controller(CONTROLLER_1_SER_PIN, CONTROLLER_1_CONN_PIN, aux_controller_ptr),
  Controller(CONTROLLER_2_SER_PIN, CONTROLLER_2_CONN_PIN, NULL)
};

int controller_count = sizeof(controllers) / sizeof(*controllers);
#if HARDWARE == ARCADE_CABINET
controller_count = 1;
#endif

void setup() {
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
}

bool neopixels_connected() {
  if (HARDWARE == RGB_CONSOLE) {
    return true;
  }
  // Determine whether CONTROLLER_2_SER_PIN is connected to a pull-down resistor
  pinMode(DISPLAY_NEOPIXEL_PIN, INPUT_PULLUP);
  bool read_val = digitalRead(DISPLAY_NEOPIXEL_PIN);
  pinMode(DISPLAY_NEOPIXEL_PIN, OUTPUT);
  return !read_val;
}

void setup_neopixels() {
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

void loop() {
  bool use_neopixels = neopixels_connected();

  if (use_neopixels) {
    setup_neopixels();
  }

  Display::Mode disp_mode = use_neopixels ? Display::Mode::rows : Display::Mode::U;
  Display disp(disp_mode, 10, 20,
    DISPLAY_RCLK_PIN, DISPLAY_SRCLK_PIN, DISPLAY_SER_PIN, DISPLAY_OE_PIN, use_neopixels);
  
  disp.set_brightness(DISPLAY_INITIAL_BRIGHTNESS);

  MenuOption menuOptions[] = {
    SnakeGame::menuOption(),
    ObstacleGame::menuOption(),
    Sporktris::menuOption(),
    Life::menuOption(),
    DiceGame::menuOption(),
    Chess::menuOption(),
  };

  Menu menu(disp, controllers, controller_count, menuOptions, sizeof(menuOptions) / sizeof(*menuOptions));
  menu.run();
}

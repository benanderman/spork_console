#include "config.h"
#include "controller.h"
#include "display.h"
#include "menu.h"
#include "obstacle_game.h"
#include "snake_game.h"

Controller controllers[2] = {
  Controller(CONTROLLER_1_SER_PIN, CONTROLLER_1_CONN_PIN),
  Controller(CONTROLLER_2_SER_PIN, CONTROLLER_2_CONN_PIN)
};

Display main_display = Display(Display::Mode::U, 10, 20,
  DISPLAY_RCLK_PIN, DISPLAY_SRCLK_PIN, DISPLAY_SER_PIN);

void setup() {
  pinMode(DISPLAY_RCLK_PIN, OUTPUT);
  pinMode(DISPLAY_SRCLK_PIN, OUTPUT);
  pinMode(DISPLAY_SER_PIN, OUTPUT);
  
  pinMode(CONSOLE_LEFT_BUTTON_PIN, INPUT);
  pinMode(CONSOLE_RIGHT_BUTTON_PIN, INPUT);

  pinMode(CONTROLLER_SHLD_PIN, OUTPUT);
  pinMode(CONTROLLER_CLK_PIN, OUTPUT);
  pinMode(CONTROLLER_1_SER_PIN, INPUT);
  pinMode(CONTROLLER_1_CONN_PIN, INPUT);
  pinMode(CONTROLLER_2_SER_PIN, INPUT);
  pinMode(CONTROLLER_2_CONN_PIN, INPUT);
}

void loop() {
  Menu menu(main_display, controllers, sizeof(controllers) / sizeof(*controllers), CONSOLE_LEFT_BUTTON_PIN, CONSOLE_RIGHT_BUTTON_PIN);
  ObstacleGame obstacle_game(main_display, CONSOLE_LEFT_BUTTON_PIN, CONSOLE_RIGHT_BUTTON_PIN, controllers[0]);
  SnakeGame snake_game(main_display, controllers, sizeof(controllers) / sizeof(*controllers));

  while (true) {
    MenuChoice choice = menu.choose();
    switch (choice) {
      case MenuChoice::snake: {
        while (!snake_game.play()) {}
        break;
      }
      case MenuChoice::obstacle: {
        while (!obstacle_game.play()) {}
        break;
      }
    }
  }
}

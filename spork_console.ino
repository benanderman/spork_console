#include "config.h"
#include "controller.h"
#include "display.h"
#include "obstacle_game.h"

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
  ObstacleGame obstacle_game(main_display, CONSOLE_LEFT_BUTTON_PIN, CONSOLE_RIGHT_BUTTON_PIN, controllers[0]);
  obstacle_game.play();
  
  for (int cycle = 0; cycle < 1000; cycle++) {
    for (int i = 0; i < 200; i++) {
      bool val = ((i + 1) % (cycle % 20)) == 0;
      digitalWrite(DISPLAY_SER_PIN, val);
      digitalWrite(DISPLAY_SRCLK_PIN, HIGH);
      digitalWrite(DISPLAY_SRCLK_PIN, LOW);
    }
    digitalWrite(DISPLAY_RCLK_PIN, HIGH);
    digitalWrite(DISPLAY_RCLK_PIN, LOW);
    int extraDelay = digitalRead(CONSOLE_LEFT_BUTTON_PIN) ? -100 : 0;
    extraDelay += digitalRead(CONSOLE_RIGHT_BUTTON_PIN) ? 1000 : 0;
    delay(200 + extraDelay);
  }
}

#include "config.h"
#include "controller.h"
#include "display.h"
#include "menu.h"
#include "obstacle_game.h"
#include "snake_game.h"
#include "sporktris.h"

Controller controllers[2] = {
  Controller(CONTROLLER_1_SER_PIN, CONTROLLER_1_CONN_PIN),
  Controller(CONTROLLER_2_SER_PIN, CONTROLLER_2_CONN_PIN)
};

Display main_display = Display(Display::Mode::U, 10, 20,
  DISPLAY_RCLK_PIN, DISPLAY_SRCLK_PIN, DISPLAY_SER_PIN, DISPLAY_OE_PIN);

void setup() {
  pinMode(DISPLAY_RCLK_PIN, OUTPUT);
  pinMode(DISPLAY_SRCLK_PIN, OUTPUT);
  pinMode(DISPLAY_SER_PIN, OUTPUT);
  pinMode(DISPLAY_OE_PIN, OUTPUT);
  
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
  if (neopixels_connected()) {
    loop_neopixels();
  } else {
    loop_internal_disp();
  }
}

bool neopixels_connected() {
  // Determine whether CONTROLLER_2_SER_PIN is connected to a pull-down resistor
  pinMode(CONTROLLER_2_SER_PIN, INPUT_PULLUP);
  bool read_val = digitalRead(CONTROLLER_2_SER_PIN);
  pinMode(CONTROLLER_2_SER_PIN, INPUT);
  return !read_val;
}

void loop_neopixels() {
  // Clear the internal display
  digitalWrite(DISPLAY_SER_PIN, LOW);
  for (int i = 0; i < MAX_DISPLAY_PIXELS; i++) {
    digitalWrite(DISPLAY_SRCLK_PIN, HIGH);
    digitalWrite(DISPLAY_SRCLK_PIN, LOW);
  }
  digitalWrite(DISPLAY_RCLK_PIN, HIGH);
  digitalWrite(DISPLAY_RCLK_PIN, LOW);
  
  pinMode(CONTROLLER_2_SER_PIN, OUTPUT);
  Adafruit_NeoPixel neopixels(MAX_DISPLAY_PIXELS, CONTROLLER_2_SER_PIN, NEO_GRB + NEO_KHZ800);
  neopixels.begin();
  neopixels.show();
  
  neopixels.setBrightness(50);
  
  Display disp(Display::Mode::rows, 10, 20,
    DISPLAY_RCLK_PIN, DISPLAY_SRCLK_PIN, DISPLAY_SER_PIN, DISPLAY_OE_PIN, &neopixels);
  loop(disp);
}

void loop_internal_disp() {
  Display disp(Display::Mode::U, 10, 20,
    DISPLAY_RCLK_PIN, DISPLAY_SRCLK_PIN, DISPLAY_SER_PIN, DISPLAY_OE_PIN);
  loop(disp);
}

void loop(Display& disp) {
  
  disp.set_brightness(DISPLAY_INITIAL_BRIGHTNESS);
  
  Menu menu(disp, controllers, sizeof(controllers) / sizeof(*controllers), CONSOLE_LEFT_BUTTON_PIN, CONSOLE_RIGHT_BUTTON_PIN);

  while (true) {
    MenuChoice choice = menu.choose();
    switch (choice) {
      case MenuChoice::snake: {
        SnakeGame snake_game(disp, controllers, sizeof(controllers) / sizeof(*controllers));
        while (!snake_game.play()) {}
        break;
      }
      case MenuChoice::obstacle: {
        ObstacleGame obstacle_game(disp, CONSOLE_LEFT_BUTTON_PIN, CONSOLE_RIGHT_BUTTON_PIN, controllers[0]);
        while (!obstacle_game.play()) {}
        break;
      }
      case MenuChoice::sporktris: {
        Sporktris sporktris(disp, controllers[0]);
        while (!sporktris.play()) {}
        break;
      }
    }
  }
}

#include "config.h"
#include "controller.h"
#include "display.h"

Controller controllers[2] = {
  Controller(CONTROLLER_1_SER_PIN, CONTROLLER_1_CONN_PIN),
  Controller(CONTROLLER_2_SER_PIN, CONTROLLER_2_CONN_PIN)
};

Display main_display = Display(Display::Mode::U, 10, 20,
  DISPLAY_RCLK_PIN, DISPLAY_SRCLK_PIN, DISPLAY_SER_PIN);

void setup() {
  // put your setup code here, to run once:
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

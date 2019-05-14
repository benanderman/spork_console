#include "controller.h"

Controller::Controller(int ser_pin, int connected_pin) :
  SER_PIN(ser_pin), CONNECTED_PIN(connected_pin), button_state(0)
{}
  
bool Controller::operator[](Button button) {
  return (button_state >> button) & 1;
}

bool Controller::is_connected() {
  return digitalRead(CONNECTED_PIN);
}

void Controller::update_state(Controller controllers[], int count) {
  digitalWrite(CONTROLLER_SHLD_PIN, LOW);
  digitalWrite(CONTROLLER_CLK_PIN, HIGH);
  digitalWrite(CONTROLLER_CLK_PIN, LOW);
  digitalWrite(CONTROLLER_SHLD_PIN, HIGH);

  for (int i = 0; i < 8; i++) {
    for (int c = 0; c < count; c++) {
      Controller *con = &controllers[c];
      con->button_state = (con->button_state << 1) | (digitalRead(con->SER_PIN) & 1);
    }
    
    digitalWrite(CONTROLLER_CLK_PIN, HIGH);
    digitalWrite(CONTROLLER_CLK_PIN, LOW);
  }
}

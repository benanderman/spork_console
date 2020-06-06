#include "controller.h"

Controller::Controller(int ser_pin, int connected_pin, Controller *aux_controller) :
  SER_PIN(ser_pin), CONNECTED_PIN(connected_pin), aux(aux_controller), button_state(0)
{}
  
bool Controller::operator[](Button button) const {
  return (button_state >> button) & 1;
}

bool Controller::is_connected() {
  if (RGB_CONSOLE && CONNECTED_PIN == CONTROLLER_1_CONN_PIN) {
    return true;
  }
  if (CONTROLLER_AUX_ENABLED && CONNECTED_PIN == CONTROLLER_AUX_CONN_PIN) {
    return !digitalRead(CONNECTED_PIN); // it uses INPUT_PULLUP
  }
  return digitalRead(CONNECTED_PIN);
}

void Controller::update_state(Controller controllers[], int count) {
  digitalWrite(CONTROLLER_SHLD_PIN, LOW);
  digitalWrite(CONTROLLER_CLK_PIN, HIGH);
  digitalWrite(CONTROLLER_CLK_PIN, LOW);
  digitalWrite(CONTROLLER_SHLD_PIN, HIGH);

  if (CONTROLLER_AUX_ENABLED) {
    digitalWrite(CONTROLLER_AUX_SHLD_PIN, LOW);
    digitalWrite(CONTROLLER_AUX_CLK_PIN, HIGH);
    digitalWrite(CONTROLLER_AUX_CLK_PIN, LOW);
    digitalWrite(CONTROLLER_AUX_SHLD_PIN, HIGH);
  }

  for (int i = 0; i < 8; i++) {
    for (int c = 0; c < count; c++) {
      Controller *con = &controllers[c];
      con->button_state = (con->button_state << 1) | (digitalRead(con->SER_PIN) & 1);
      if (con->aux && con->aux->is_connected()) {
        con->button_state |= digitalRead(con->aux->SER_PIN) & 1;
      }
    }
    
    digitalWrite(CONTROLLER_CLK_PIN, HIGH);
    digitalWrite(CONTROLLER_CLK_PIN, LOW);
    if (CONTROLLER_AUX_ENABLED) {
      digitalWrite(CONTROLLER_AUX_CLK_PIN, HIGH);
      digitalWrite(CONTROLLER_AUX_CLK_PIN, LOW);
    }
  }
}

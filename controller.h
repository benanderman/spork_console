#ifndef __CONTROLLER
#define __CONTROLLER

#include "config.h"
#include "arduino.h"

class Controller {
  public:
  
  enum Button {
    b = 0,
    a,
    select,
    start,
    down,
    right,
    up,
    left
  };
  
  Controller(int ser_pin, int connected_pin);
  
  bool operator[](Button button) const;
  
  // Unreliable due to lack of pull-down resistor
  bool is_connected();
  
  static void update_state(Controller controllers[], int count);
  
  private:
  int SER_PIN;
  int CONNECTED_PIN;
  
  byte button_state;
};

#endif

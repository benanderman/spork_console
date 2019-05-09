#include "config.h"
#include "arduino.h"

class Controller {
  public:
  
  enum Button {
    left = 0,
    up,
    right,
    down,
    start,
    select,
    a,
    b
  };
  
  Controller(int ser_pin, int connected_pin);
  
  bool operator[](Button button);
  
  // Unreliable due to lack of pull-down resistor
  bool is_connected();
  
  static void update_state(Controller controllers[], int count);
  
  private:
  int SER_PIN;
  int CONNECTED_PIN;
  
  byte button_state;
};

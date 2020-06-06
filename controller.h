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
    left,
    __count
  };
  
  Controller(int ser_pin, int connected_pin, Controller *aux);
  
  bool operator[](Button button) const;
  
  bool is_connected();
  
  static void update_state(Controller controllers[], int count);
  
  private:
  int SER_PIN;
  int CONNECTED_PIN;

  Controller *aux;
  
  byte button_state;
};

#endif

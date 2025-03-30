#ifndef __CONTROLLER
#define __CONTROLLER

#include "arduino.h"
#include "config.h"

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

  struct ButtonHandlerState {
    enum PressedState {
      not_pressed = 0,
      pressed_zero_registers,
      pressed_one_register,
      pressed_more_registers
    } __attribute__((packed));

    PressedState pressed_state;
    uint8_t since_last_change;
    unsigned short since_last_register;

    bool is_pressed() {
      return pressed_state != not_pressed;
    }

    ButtonHandlerState(): pressed_state(not_pressed), since_last_change(0), since_last_register(0) {}
  };

  struct ButtonHandlerStateSet {
    ButtonHandlerState states[Button::__count];

    ButtonHandlerState& operator[](Button button) {
      return states[(int)button];
    }
  };

  // This is managed by InputProcessor; this class does not update it.
  ButtonHandlerStateSet handler_states;

  Controller(int ser_pin, int connected_pin, Controller *aux = NULL);
  
  bool operator[](Button button) const;
  
  bool is_connected();
  
  static void update_state(Controller controllers[], int count);
  
  private:
  uint8_t SER_PIN;
  uint8_t CONNECTED_PIN;

  Controller *aux;
  
  uint8_t button_state;
};

#endif

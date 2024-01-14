#ifndef __INPUT_PROCESSOR
#define __INPUT_PROCESSOR

#include "controller.h"

enum ButtonPressedState {
  not_pressed,
  pressed_zero_registers,
  pressed_one_register,
  pressed_more_registers,
} __attribute__((packed));

struct ButtonState {
  ButtonPressedState pressed_state;
  uint8_t since_last_change;
  unsigned short since_last_register;

  bool is_pressed() {
    return pressed_state != ButtonPressedState::not_pressed;
  }

  ButtonState(): pressed_state(not_pressed), since_last_change(0), since_last_register(0) {}
};

struct ButtonRepeatDelays {
  unsigned short initial;
  unsigned short subsequent;
};

struct ButtonStateSet {
  ButtonState states[Controller::Button::__count];

  ButtonState& operator[](Controller::Button button) {
    return states[(int)button];
  }
};

class InputProcessor {
  public:
  ButtonRepeatDelays button_conf[Controller::Button::__count];
  ButtonStateSet *button_states;
  Controller *controllers;
  int controller_count;
  
  InputProcessor(Controller *controllers, int controller_count);

  bool handle_input(unsigned long now);

  // Override to handle button presses
  virtual bool handle_button_down(Controller::Button button, int controller_index);
  virtual bool handle_button_up(Controller::Button button, int controller_index);

  private:
  unsigned long last_process;
  void update_button_states(unsigned long now);
};

#endif

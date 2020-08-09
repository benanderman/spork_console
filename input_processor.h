#ifndef __INPUT_PROCESSOR
#define __INPUT_PROCESSOR

#include "controller.h"

// This is a WIP; want to refactor Sporktris to inherit from this, then
// use the same functionality in other games

struct ButtonState {
  bool pressed;
  unsigned long last_change;
  unsigned long last_register;

  ButtonState(): pressed(false), last_change(0), last_register(0) {}
};

struct ButtonRepeatDelays {
  short initial;
  short subsequent;
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
  void update_button_states(unsigned long now);
};

#endif

#ifndef __INPUT_PROCESSOR
#define __INPUT_PROCESSOR

#include "controller.h"

// Delay before first key repeat, and subsequent key repeats.
// 0 means no repeat, and there should not be a subsequent repeat without initial repeat.
struct ButtonRepeatDelays {
  unsigned short initial;
  unsigned short subsequent;
};

// Handles updating state from controllers, debouncing button presses, and key repeat.
// Subclasses initialize with controllers, configure key repeat delays, then call `handle_input`
// with the current time (from `millis()`) in their run loop. `handle_button_down` will be
// called when a button is first pressed and for each key repeat registered, and `handle_button_up`
// will be called when a button stops being pushed.
class InputProcessor {
  public:
  ButtonRepeatDelays button_conf[Controller::Button::__count];
  Controller *controllers;
  int controller_count;
  
  InputProcessor(Controller *controllers, int controller_count);

  // Call from run loop; handles getting state from controllers, debouncing, and key repeat.
  // Calls `handle_button_down` and `handle_button_up`.
  bool handle_input(unsigned long now);

  // Override to handle button presses, return whether to exit.
  virtual bool handle_button_down(Controller::Button button, int controller_index);
  virtual bool handle_button_up(Controller::Button button, int controller_index);

  private:
  unsigned long last_process;
  void update_button_states(unsigned long now);
};

#endif

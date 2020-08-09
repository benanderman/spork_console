#include "input_processor.h"

// Minimum milliseconds between button state changes
#define BUTTON_DEBOUNCE_THRESHOLD 20

InputProcessor::InputProcessor(Controller *controllers, int controller_count):
  controllers(controllers), controller_count(controller_count) {
  button_states = new ButtonStateSet[controller_count];
  
  button_conf[Controller::Button::b] = { .initial = 500, .subsequent = 500};
  button_conf[Controller::Button::a] = { .initial = 500, .subsequent = 500};
  button_conf[Controller::Button::select] = { .initial = 0, .subsequent = 0};
  button_conf[Controller::Button::start] = { .initial = 0, .subsequent = 0};
  button_conf[Controller::Button::down] = { .initial = 100, .subsequent = 50};
  button_conf[Controller::Button::right] = { .initial = 200, .subsequent = 20};
  button_conf[Controller::Button::up] = { .initial = 500, .subsequent = 30};
  button_conf[Controller::Button::left] = { .initial = 200, .subsequent = 20};
}

void InputProcessor::update_button_states(unsigned long now) {
  Controller::update_state(controllers, controller_count);

  for (int c = 0; c < controller_count; c++) {
    bool is_connected = controllers[c].is_connected();
    for (int b = 0; b < Controller::Button::__count; b++) {
      bool new_state = is_connected && controllers[c][(Controller::Button)b];
      if (new_state != button_states[c][b].pressed) {
        if (now - button_states[c][b].last_change > BUTTON_DEBOUNCE_THRESHOLD) {
          button_states[c][b].pressed = new_state;
          button_states[c][b].last_change = now;
          button_states[c][b].last_register = 0;
          if (!new_state) {
            handle_button_up((Controller::Button)b, c);
          }
        }
      }
    }
  }
}

bool InputProcessor::handle_input(unsigned long now) {
  update_button_states(now);

  bool should_exit = false;

  for (int c = 0; c < controller_count; c++) {
    for (int b = 0; b < Controller::Button::__count; b++) {
      ButtonState& state = button_states[c][b];
      ButtonRepeatDelays conf = button_conf[b];
      
      bool should_register = false;
      if (state.pressed) {
        unsigned long since_last_register = now - state.last_register;
  
        // First register
        if (state.last_register < state.last_change) {
          should_register = true;
          state.last_register = state.last_change;
  
        // Second register
        } else if (state.last_register == state.last_change) {
          if (conf.initial && since_last_register >= conf.initial) {
            should_register = true;
            state.last_register += conf.initial;
          }
  
        // Third+ register
        } else if (conf.subsequent && since_last_register >= conf.subsequent) {
          should_register = true;
          state.last_register += conf.subsequent;
        }
      }
  
      if (should_register) {
        should_exit = handle_button_down((Controller::Button)b, c);
      }
    }
  }
  
  return should_exit;
}

bool InputProcessor::handle_button_down(Controller::Button button, int controller_index) {
  return false;
}

bool InputProcessor::handle_button_up(Controller::Button button, int controller_index) {
  return false;
}

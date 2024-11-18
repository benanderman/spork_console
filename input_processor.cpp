#include "input_processor.h"
#include "controller.h"

// Minimum milliseconds between button state changes
#define BUTTON_DEBOUNCE_THRESHOLD (unsigned long)20

InputProcessor::InputProcessor(Controller *controllers, uint8_t controller_count):
  controllers(controllers), controller_count(controller_count), last_process(0) {
  
  button_conf[Controller::Button::b] = { .initial = 500, .subsequent = 500};
  button_conf[Controller::Button::a] = { .initial = 500, .subsequent = 500};
  button_conf[Controller::Button::select] = { .initial = 0, .subsequent = 0};
  button_conf[Controller::Button::start] = { .initial = 0, .subsequent = 0};
  button_conf[Controller::Button::down] = { .initial = 100, .subsequent = 50};
  button_conf[Controller::Button::right] = { .initial = 200, .subsequent = 20};
  button_conf[Controller::Button::up] = { .initial = 500, .subsequent = 30};
  button_conf[Controller::Button::left] = { .initial = 200, .subsequent = 20};
}

// Updates controllers, and handles debouncing, button up calls, and adding the time since
// `last_process` to each `since_last_change` and `since_last_register`.
void InputProcessor::update_button_states(unsigned long now) {
  Controller::update_state(controllers, controller_count);

  unsigned long since_last_process = now - last_process;

  for (int c = 0; c < controller_count; c++) {
    bool is_connected = controllers[c].is_connected();
    for (int b = 0; b < Controller::Button::__count; b++) {
      Controller::ButtonHandlerState& state = controllers[c].handler_states[(Controller::Button)b];
      bool new_is_pressed = is_connected && controllers[c][(Controller::Button)b];
      state.since_last_change = min(
        state.since_last_change + since_last_process,
        BUTTON_DEBOUNCE_THRESHOLD + 1
      );
      state.since_last_register += since_last_process;
      if (new_is_pressed != state.is_pressed() && state.since_last_change > BUTTON_DEBOUNCE_THRESHOLD) {
        state.pressed_state = Controller::ButtonHandlerState::PressedState::pressed_zero_registers;
        state.since_last_change = 0;
        state.since_last_register = 0;
        if (!new_is_pressed) {
          state.pressed_state = Controller::ButtonHandlerState::PressedState::not_pressed;
          handle_button_up((Controller::Button)b, c);
        }
      }
    }
  }
}

// Handles button down calls, and returns whether to exit.
bool InputProcessor::handle_input(unsigned long now) {
  update_button_states(now);
  unsigned long since_last_process = now - last_process;
  last_process = now;

  bool should_exit = false;

  for (int c = 0; c < controller_count; c++) {
    for (int b = 0; b < Controller::Button::__count; b++) {
      Controller::ButtonHandlerState& state = controllers[c].handler_states[(Controller::Button)b];
      ButtonRepeatDelays conf = button_conf[b];
      
      bool should_register = false;
      switch (state.pressed_state) {
        case Controller::ButtonHandlerState::PressedState::not_pressed: {
          break;
        }

        // First register
        case Controller::ButtonHandlerState::PressedState::pressed_zero_registers: {
          should_register = true;
          state.since_last_register = 0;
          state.pressed_state = Controller::ButtonHandlerState::PressedState::pressed_one_register;
          break;
        }

        // Second register
        case Controller::ButtonHandlerState::PressedState::pressed_one_register: {
          if (conf.initial && state.since_last_register >= conf.initial) {
            should_register = true;
            state.since_last_register -= conf.initial;
            state.pressed_state = Controller::ButtonHandlerState::PressedState::pressed_more_registers;
          }
          break;
        }

        // Third+ register
        case Controller::ButtonHandlerState::PressedState::pressed_more_registers: {
          if (conf.subsequent && state.since_last_register >= conf.subsequent) {
            should_register = true;
            state.since_last_register -= conf.subsequent;
            state.pressed_state = Controller::ButtonHandlerState::PressedState::pressed_more_registers;
          }
          break;
        }
      }
  
      if (should_register) {
        should_exit = handle_button_down((Controller::Button)b, c);
      }
    }
  }
  
  return should_exit;
}

bool InputProcessor::handle_button_down(Controller::Button button, uint8_t controller_index) {
  return false;
}

bool InputProcessor::handle_button_up(Controller::Button button, uint8_t controller_index) {
  return false;
}

#include "display.h"

bool Display::set_pixel(int x, int y, bool val) {
  int index = y * width + x;
  if (index < 0 || index >= MAX_DISPLAY_PIXELS) {
    return false;
  }
  bool old_value = state[index];
  state[index] = val;
  return old_value;
}

bool Display::set_rect(int x, int y, int width, int height, bool val) {
  bool result = false;
  for (int cx = x; cx < x + width; cx++) {
    for (int cy = y; cy < y + height; cy++) {
      result = set_pixel(cx, cy, val) || result;
    }
  }
  return result;
}

bool Display::get_pixel(int x, int y) {
  return state[y * width + x];
}

void Display::clear_all() {
  memset(&state, 0, MAX_DISPLAY_PIXELS);
}

void Display::refresh() {
  int total_pixels = width * height;
  for (int i = total_pixels - 1; i >= 0; i--) {
    int x, y;
    switch (mode) {
      case Mode::rows: {
        x = i % width;
        y = i / width;
        break;
      }
      case Mode::U: {
        if (i < total_pixels / 2) {
          x = i % (width / 2);
          y = i / (width / 2);
        } else {
          x = width / 2 + i % (width / 2);
          y = (height - 1) - (i - total_pixels / 2) / (width / 2);
        }
        break;
      }
    }
    
    
    bool val = get_pixel(x, y);
    digitalWrite(SER_PIN, val);
    digitalWrite(SRCLK_PIN, HIGH);
    digitalWrite(SRCLK_PIN, LOW);
  }
  digitalWrite(RCLK_PIN, HIGH);
  digitalWrite(RCLK_PIN, LOW);
}

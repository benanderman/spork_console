#include "string.h"

#include "display.h"
#include "neopixels.h"

byte Display::get_pixel(int x, int y) {
  int index = y * width + x;
  return state[index];
}

byte Display::set_pixel(int x, int y, byte val) {
  int index = y * width + x;
  if (index < 0 || index >= MAX_DISPLAY_PIXELS) {
    return false;
  }
  byte old_value = state[index];
  state[index] = val;
  
  return old_value;
}

bool Display::set_rect(int x, int y, int width, int height, byte val) {
  bool result = false;
  for (int cx = x; cx < x + width; cx++) {
    for (int cy = y; cy < y + height; cy++) {
      result = set_pixel(cx, cy, val) || result;
    }
  }
  return result;
}

void Display::clear_all() {
  memset(&state, 0, MAX_DISPLAY_PIXELS);
}

void Display::refresh(get_pixel_func_t get_pixel_func) {
  int total_pixels = width * height;
  for (int p = 0; p < total_pixels; p++) {
    int i = p;
    if (!neopixels) {
      i = total_pixels - 1 - i;
    }
    
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
      case Mode::N: {
        x = i / height;
        y = i % height;
        if (x % 2 == 1) {
          y = height - y;
        }
        break;
      }
    }
    
    byte val = get_pixel(x, y);

    if (neopixels) {
      if (palette) {
        byte m = brightness;
        byte *pixel = palette[val];
        byte r = pixel[0];
        byte g = pixel[1];
        byte b = pixel[2];
        if (get_pixel_func) {
          uint32_t p = get_pixel_func(x, y);
          r = (p >> 24) & 0xFF;
          g = (p >> 16) & 0xFF;
          b = (p >>  8) & 0xFF;
        }
        Neopixels::sendPixel(r * m, g * m, b * m);
      } else {
        byte pixel_brightness = (val ? brightness : 0);
        Neopixels::sendPixel(pixel_brightness, pixel_brightness, pixel_brightness);
      }
    } else {
      digitalWrite(SER_PIN, val);
      digitalWrite(SRCLK_PIN, HIGH);
      digitalWrite(SRCLK_PIN, LOW);
    }
  }
  if (neopixels) {
    Neopixels::show();
  } else {
    digitalWrite(RCLK_PIN, HIGH);
    digitalWrite(RCLK_PIN, LOW);
  }
}

void Display::set_brightness(byte brightness) {
  // Output enable on 595 shift registers is on when grounded
  this->brightness = brightness;
  if (BRIGHTNESS_ENABLED) {
    analogWrite(OE_PIN, 255 - this->brightness);
  }
}

byte Display::get_brightness() {
  return brightness;
}

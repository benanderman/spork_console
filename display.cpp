#include "string.h"

#include "display.h"
#include "neopixels.h"

uint8_t Display::get_pixel(int8_t x, int8_t y) {
  int index = y * width + x;
  if (index < 0 || index >= MAX_DISPLAY_PIXELS) {
    return 0;
  }
  return state[index];
}

uint8_t Display::set_pixel(int8_t x, int8_t y, uint8_t val) {
  int16_t index = y * width + x;
  if (index < 0 || index >= MAX_DISPLAY_PIXELS) {
    return false;
  }
  uint8_t old_value = state[index];
  state[index] = val;
  
  return old_value;
}

bool Display::set_rect(int8_t x, int8_t y, int8_t width, int8_t height, uint8_t val) {
  bool result = false;
  for (int8_t cx = x; cx < x + width; cx++) {
    for (int8_t cy = y; cy < y + height; cy++) {
      result = set_pixel(cx, cy, val) || result;
    }
  }
  return result;
}

void Display::clear_all() {
  memset(&state, 0, MAX_DISPLAY_PIXELS);
}

void Display::refresh(bool multi_display) {
  uint8_t total_pixels = width * height;
  uint8_t r_shift = 0;
  uint8_t l_shift = 0;
  switch (brightness) {
    case 0:
    r_shift = 7;
    break;
    case 1:
    r_shift = 2;
    break;
    case 2:
    r_shift = 1;
    break;
    default:
    l_shift = brightness - 3;
    break;
  };
  for (int p = 0; p < total_pixels; p++) {
    int i = p;
    if (!neopixels) {
      i = total_pixels - 1 - i;
    }
    
    int8_t x, y;
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
    
    uint8_t val = get_pixel(x, y);

    if (neopixels) {
      if (palette) {
        if (multi_display) {
          // In multi-display mode, the host display's pixel is in the low 4 bits, and
          // the peripheral display's pixel is in the high 4 bits.
          uint8_t *pixel1 = palette[val & 0xF];
          uint8_t *pixel2 = palette[val >> 4];
          // WS2812-compatible LEDs expect colors in GRB order. We use the sendByte function
          // here instead of sendPixel, to have less time between bits sent; if it's too long
          // (5 microseconds), the screen will refresh.
          cli();
          uint8_t g1 = pixel1[1];
          uint8_t g2 = pixel2[1];
          Neopixels::sendByte((g1 >> r_shift) << l_shift, (g2 >> r_shift) << l_shift);
          uint8_t r1 = pixel1[0];
          uint8_t r2 = pixel2[0];
          Neopixels::sendByte((r1 >> r_shift) << l_shift, (r2 >> r_shift) << l_shift);
          uint8_t b1 = pixel1[2];
          uint8_t b2 = pixel2[2];
          Neopixels::sendByte((b1 >> r_shift) << l_shift, (b2 >> r_shift) << l_shift);
          sei();
        } else {
          uint8_t *pixel = palette[val];
          uint8_t r = pixel[0];
          uint8_t g = pixel[1];
          uint8_t b = pixel[2];
          Neopixels::sendPixel((r >> r_shift) << l_shift, (g >> r_shift) << l_shift, (b >> r_shift) << l_shift);
        }
      } else {
        uint8_t pixel_brightness = (val ? brightness : 0);
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

void Display::set_brightness(uint8_t brightness) {
  // Output enable on 595 shift registers is on when grounded
  this->brightness = brightness;
  if (BRIGHTNESS_ENABLED) {
    analogWrite(OE_PIN, 255 - this->brightness);
  }
}

uint8_t Display::get_brightness() {
  return brightness;
}

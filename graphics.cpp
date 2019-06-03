#include "graphics.h"

void Graphics::clear_rows(Display& disp) {
  for (int y = 0; y < disp.height; y++) {
    disp.set_rect(0, y, disp.width, 1, false);
    disp.refresh();
    delay(5);
  }
}

#include "config.h"
#include "arduino.h"

class Display {
  public:
  enum Mode {
    rows,
    U
  };

  int RCLK_PIN;
  int SRCLK_PIN;
  int SER_PIN;

  int row_count;
  int col_count;
  Mode mode;

  Display(Mode mode, int row_count, int col_count,
    int rclk_pin, int srclk_pin, int ser_pin) :
    mode(mode), row_count(row_count), col_count(col_count),
    RCLK_PIN(rclk_pin), SRCLK_PIN(srclk_pin), SER_PIN(ser_pin) {}

  void set_pixel(int row, int col);
  bool get_pixel(int row, int col);
  void refresh();

  private:
  byte state[(MAX_DISPLAY_PIXELS + 7) / 8];
};

#include "arduino.h"
#include "obstacle_game.h"
#include "graphics.h"
#include "config.h"

#define ENTITY_SIZE 2

static bool ObstacleGame::run(Display& disp, Controller *controllers, uint8_t controller_count) {
  ObstacleGame game = ObstacleGame(disp, controllers, controller_count);
  return game.play();
}

static MenuOption ObstacleGame::menuOption() {
  static const char PROGMEM graphic[] =
    "__________"
    "_22_______"
    "_22_______"
    "__________"
    "______22__"
    "______22__"
    "__________"
    "__22______"
    "__22_11___"
    "_____11___";
  return MenuOption(graphic, ObstacleGame::setPalette, ObstacleGame::run);
}

static void ObstacleGame::setPalette(Display &disp) {
  disp.palette[0] = RGB(0,  0,  0);
  disp.palette[1] = RGB(4,  16, 4);
  disp.palette[2] = RGB(16, 4,  4);
  disp.palette[3] = RGB(4,  4,  16);
  disp.palette[4] = RGB(10, 10, 4);
  disp.palette[5] = RGB(10, 4,  10);
  disp.palette[6] = RGB(4,  10, 10);
  disp.palette[7] = RGB(8,  8,  8);
  disp.palette[8] = RGB(0,  24, 0);
  disp.palette[9] = RGB(0,  0,  0);
}

bool ObstacleGame::play() {
  ObstacleGame::setPalette(disp);

  randomSeed(millis());
  
  player_x = disp.width / 2 - ENTITY_SIZE / 2;
  player_y = disp.height - ENTITY_SIZE;
  bool alive = true;

  long obstacle_cycle = 0;
  unsigned long last_obstacle_cycle = millis();
  unsigned long last_move = millis();
  int obstacle_speed = 250;
  int move_speed = 35;
  int level = 0;
  
  while (alive) {
    unsigned long now = millis();

    if (now > last_move + move_speed) {
      bool exit_game = handle_input();
      if (exit_game) {
        return true;
      }
      last_move = now;
    }

    if (now > last_obstacle_cycle + obstacle_speed) {
      obstacle_cycle++;
      last_obstacle_cycle = now;

      if (obstacle_cycle % 35 == 0) {
        obstacle_speed *= 0.9;
        level++;
        randomSeed(analogRead(A0) + millis());
      }
    }

    disp.clear_all();

    draw_obstacles(obstacle_cycle, level);
    alive = !disp.set_rect(player_x, player_y, ENTITY_SIZE, ENTITY_SIZE, 1);
    
    disp.refresh();
    delay(1);
  }

  return Graphics::end_game(disp, controllers, controller_count, 8, 9);
}

bool ObstacleGame::handle_input() {
  bool left = LEFT_RIGHT_BUTTONS_ENABLED && digitalRead(CONSOLE_LEFT_BUTTON_PIN);
  bool right = LEFT_RIGHT_BUTTONS_ENABLED && digitalRead(CONSOLE_LEFT_BUTTON_PIN);
  bool up = false;
  bool down = false;
  bool start = false;

  for (int i = 0; i < controller_count; i++) {
    Controller& controller = controllers[i];
    if (controller.is_connected()) {
      Controller::update_state(&controller, 1);
      left = left || controller[Controller::Button::left];
      right = right || controller[Controller::Button::right];
      up = up || controller[Controller::Button::up];
      down = down || controller[Controller::Button::down];
      start = start || controller[Controller::Button::start];
    }
  }

  player_x += left ? -1 : 0;
  player_x += right ? 1 : 0;
  player_x = max(0, min(disp.width - ENTITY_SIZE, player_x));
  
  player_y += up ? -1 : 0;
  player_y += down ? 1 : 0;
  player_y = max(0, min(disp.height - ENTITY_SIZE, player_y));

  return start;
}

void ObstacleGame::draw_obstacles(long cycle, int level) {
  int obstacle_count = sizeof(obstacles) / sizeof(*obstacles);
  for (int o = 0; o < obstacle_count; o++) {
    int total_height = (obstacle_count * 5);
    int y = (cycle - ENTITY_SIZE - o * 5) % total_height;
    if (y < 1) {
      obstacles[o] = random(disp.width - ENTITY_SIZE + 1);
    }
    byte color = 2 + level % 6;
    disp.set_rect(obstacles[o], y, ENTITY_SIZE, ENTITY_SIZE, color);
  }
}

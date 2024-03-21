#include "arduino.h"
#include "obstacle_game.h"
#include "graphics.h"

int entity_size = 2;
int obstacles[] = {0, 8, 3, 5, 1, 7, 2, 4, 8, 5, 1, 5, 2, 6, 2, 8, 4};

bool ObstacleGame::play() {
  randomSeed(millis());
  
  byte palette[][3] = {
    {0, 0, 0},
    {4, 16, 4},
    {16, 4, 4},
    {4, 4, 16},
    {10, 10, 4},
    {10, 4, 10},
    {4, 10, 10},
    {8, 8, 8},
    {0, 24, 0},
    {0, 0, 0}
  };
  disp.palette = palette;
  
  player_x = disp.width / 2 - entity_size / 2;
  player_y = disp.height - entity_size;
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
        disp.palette = NULL;
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
    alive = !disp.set_rect(player_x, player_y, entity_size, entity_size, 1);
    
    disp.refresh();
    delay(1);
  }

  return Graphics::end_game(disp, &controller, 1, 8, palette, 9);
}

bool ObstacleGame::handle_input() {
  bool left = !disp.neopixels && digitalRead(LEFT_BUTTON_PIN);
  bool right = !disp.neopixels && digitalRead(RIGHT_BUTTON_PIN);
  bool up = false;
  bool down = false;

  if (controller.is_connected()) {
    Controller::update_state(&controller, 1);
    left = left || controller[Controller::Button::left];
    right = right || controller[Controller::Button::right];
    up = controller[Controller::Button::up];
    down = controller[Controller::Button::down];
  }
  
  player_x += left ? -1 : 0;
  player_x += right ? 1 : 0;
  player_x = max(0, min(disp.width - entity_size, player_x));
  
  player_y += up ? -1 : 0;
  player_y += down ? 1 : 0;
  player_y = max(0, min(disp.height - entity_size, player_y));

  return controller[Controller::Button::start];
}

void ObstacleGame::draw_obstacles(long cycle, int level) {
  int obstacle_count = sizeof(obstacles) / sizeof(*obstacles);
  for (int o = 0; o < obstacle_count; o++) {
    int total_height = (obstacle_count * 5);
    int y = (cycle - entity_size - o * 5) % total_height;
    if (y < 1) {
      obstacles[o] = random(disp.width - entity_size + 1);
    }
    byte color = 2 + level % 6;
    disp.set_rect(obstacles[o], y, entity_size, entity_size, color);
  }
}

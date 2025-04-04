#include "arduino.h"
#include "snake_game.h"
#include "graphics.h"
#include "pitches.h"

static bool SnakeGame::run(Display& disp, Controller *controllers, uint8_t controller_count) {
  SnakeGame game = SnakeGame(disp, controllers, controller_count);
  return game.play();
}

static MenuOption SnakeGame::menuOption() {
  static const char PROGMEM graphic[] =
    "__________"
    "_11111__3_"
    "_____1____"
    "__1111____"
    "__1_____1_"
    "_11__1111_"
    "_1___1____"
    "_11111____"
    "__________"
    "__________"
  ;
  return MenuOption(graphic, SnakeGame::setPalette, SnakeGame::run);
}

enum Dir {
  left,
  right,
  up,
  down
};

struct Point {
  byte x;
  byte y;

  Point(byte x = 0, byte y = 0): x(x), y(y) {}
  
  Point shifted_by(Dir dir, byte dist = 1) const {
    switch (dir) {
      case left: return Point(x - dist, y); break;
      case right: return Point(x + dist, y); break;
      case up: return Point(x, y - dist); break;
      case down: return Point(x, y + dist); break;
    }
  }

  static Point rand_in_disp(const Display& disp) {
    return Point(random(disp.width), random(disp.height));
  }

  bool operator==(const Point& p) {
    return p.x == x && p.y == y;
  }
};

class Player {
  public:
  Dir facing;
  Dir last_facing;
  Point points[200];
  byte points_start;
  byte points_len;
  bool alive;
  Display& disp;
  byte color;
  
  Player(int x, int y, int len, Dir facing, Display& disp, byte color):
    facing(facing), last_facing(facing), alive(true), disp(disp), color(color) {
    points_start = 0;
    points_len = len;
    Point point = Point(x, y);
    for (int i = 0; i < len; i++) {
      points[i] = point;
      point = point.shifted_by(facing);
    }
  }

  void handle_input(const Controller& con) {
    if (con[Controller::Button::left] && last_facing != Dir::right) {
      facing = Dir::left;
    }
    if (con[Controller::Button::right] && last_facing != Dir::left) {
      facing = Dir::right;
    }
    if (con[Controller::Button::up] && last_facing != Dir::down) {
      facing = Dir::up;
    }
    if (con[Controller::Button::down] && last_facing != Dir::up) {
      facing = Dir::down;
    }
  }

  Point head() {
    int total_points = sizeof(points) / sizeof(*points);
    int head_index = (points_start + points_len - 1) % total_points;
    return points[head_index];
  }

  // Return whether food was eaten
  bool cycle(Point food, bool wrap = false) {
    int total_points = sizeof(points) / sizeof(*points);
    int head_index = (points_start + points_len - 1) % total_points;
    int next_head = (head_index + 1) % total_points;
    Point new_point = points[head_index].shifted_by(facing);

    bool ate_food = false;
    if (new_point.x >= disp.width || new_point.y >= disp.height) {
      if (!wrap) {
        alive = false;
      }
      new_point.x = new_point.x == 255 ? disp.width - 1 : new_point.x % disp.width;
      new_point.y = new_point.y == 255 ? disp.height - 1 : new_point.y % disp.height;
    }
    if (new_point == food) {
      points_len++;
      ate_food = true;
    } else {
      points_start = (points_start + 1) % total_points;
    }
    points[next_head] = new_point;
    last_facing = facing;
    
    return ate_food;
  }

  void draw(Player &other_player, bool can_overlap) {
    Point other_head = other_player.head();
    int total_points = sizeof(points) / sizeof(*points);
    for (int i = 0; i < points_len; i++) {
      Point point = points[(points_start + i) % total_points];
      if (point == other_head && other_player.alive) {
        other_player.alive = false;
        if (point == head()) {
          alive = false;
        }
      }
      if (!can_overlap && disp.get_pixel(point.x, point.y) == color) {
        alive = false;
      }
      disp.set_pixel(point.x, point.y, color);
    }
  }
};

static void SnakeGame::setPalette(Display &disp) {
  disp.palette[0] = RGB(0,  0,  0);
  disp.palette[1] = RGB(4,  16, 4);   // Player 1
  disp.palette[2] = RGB(4,  4,  16);  // Player 2
  disp.palette[3] = RGB(16, 4,  4);   // Food
  disp.palette[4] = RGB(4,  10, 10);  // Tie winner
}

Player *players[2];

bool SnakeGame::play() {
  SnakeGame::setPalette(disp);

  randomSeed(analogRead(A0));
  
  uint8_t start_len = 3;
  Player player1(disp.width / 2 - 1, 0, start_len, Dir::down, disp, 1);
  Player player2(disp.width / 2, disp.height - 1, start_len, Dir::up, disp, 2);
  players[0] = &player1;
  players[1] = &player2;
  player1.alive = controllers[0].is_connected();
  player2.alive = controller_count > 1 && controllers[1].is_connected();

  Point food = Point::rand_in_disp(disp);

  unsigned long last_cycle = millis();
  unsigned long last_input = millis();
  int game_speed = 150;
  int input_speed = 5;
  int winner = -1;
  
  while (player1.alive || player2.alive) {
    unsigned long now = millis();
    bool single_player = player1.alive ^ player2.alive;

    if (single_player) {
      winner = player1.alive ? 0 : 1;
    }
    
    disp.clear_all();

    if (now > last_input + input_speed) {
      bool exit_game = handle_input();
      if (exit_game) {
        return true;
      }
      last_input = now;
    }

    bool food_was_eaten = false;

    if (now > last_cycle + game_speed) {
      last_cycle = now;
      for (int i = 0; i < 2; i++) {
        if (players[i]->alive) {
          food_was_eaten = players[i]->cycle(food, !single_player) || food_was_eaten;
          if (food_was_eaten && AUDIO_ENABLED) {
            tone(AUDIO_PIN, i ? NOTE_F4 : NOTE_D5, 10);
          }
        }
      }
    }

    if (food_was_eaten) {
      game_speed *= 0.98;
    }

    for (int i = 0; i < 2; i++) {
      if (players[i]->alive || i == winner) {
        players[i]->draw(*players[1 - i], !single_player);
      }
    }

    while (disp.get_pixel(food.x, food.y) != 0) {
      food = Point::rand_in_disp(disp);
    }
    disp.set_pixel(food.x, food.y, 3);
    
    disp.refresh();
    delay(1);
  }

  // If the winner is -1, that means it was a tie.
  uint8_t winner_color = winner == -1 ? 4 : players[winner]->color;
  return Graphics::end_game(disp, controllers, controller_count, winner_color, 5);
}

bool SnakeGame::handle_input() {
  Controller::update_state(controllers, controller_count);
  for (int i = 0; i < controller_count; i++) {
    if (controllers[i].is_connected()) {
      if (controllers[i][Controller::Button::start]) {
        return true;
      }
      players[i]->handle_input(controllers[i]);
    }
  }
  return false;
}

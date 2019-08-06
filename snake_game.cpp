#include "snake_game.h"

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
  bool cycle(Point food) {
    int total_points = sizeof(points) / sizeof(*points);
    int head_index = (points_start + points_len - 1) % total_points;
    int next_head = (head_index + 1) % total_points;
    Point new_point = points[head_index].shifted_by(facing);
    points[next_head] = new_point;
    last_facing = facing;
    
    if (new_point.x < 0 || new_point.y < 0 || new_point.x >= disp.width || new_point.y >= disp.height) {
      alive = false;
      return false;
    } else if (new_point == food) {
      points_len++;
      return true;
    } else {
      points_start = (points_start + 1) % total_points;
      return false;
    }
  }

  void draw(Player &other_player) {
    Point other_head = other_player.head();
    int total_points = sizeof(points) / sizeof(*points);
    for (int i = 0; i < points_len; i++) {
      Point point = points[(points_start + i) % total_points];
      if (point == other_head) {
        other_player.alive = false;
      }
      disp.set_pixel(point.x, point.y, color);
    }
  }
};

Player *players[2];

bool SnakeGame::play() {
  randomSeed(analogRead(A0));

  byte palette[][3] = {
    {0, 0, 0},
    {4, 16, 4},
    {4, 4, 16},
    {16, 4, 4},
  };
  disp.palette = palette;
  
  byte start_len = 3;
  Player player1(disp.width / 2 - 1, 0, start_len, Dir::down, disp, 1);
  Player player2(disp.width / 2, disp.height - 1, start_len, Dir::up, disp, 2);
  players[0] = &player1;
  players[1] = &player2;
  player1.alive = controllers[0].is_connected();
  player2.alive = controller_count > 1 && controllers[1].is_connected();

  Point food = Point::rand_in_disp(disp);

  unsigned long last_cycle = millis();
  unsigned long last_input = millis();
  int game_speed = 130;
  int input_speed = 5;
  
  while (player1.alive || player2.alive) {
    unsigned long now = millis();
    
    disp.clear_all();

    if (now > last_input + input_speed) {
      bool exit_game = handle_input();
      if (exit_game) {
        disp.palette = NULL;
        return true;
      }
      last_input = now;
    }

    bool need_new_food = false;

    if (now > last_cycle + game_speed) {
      last_cycle = now;
      for (int i = 0; i < 2; i++) {
        if (players[i]->alive) {
          need_new_food = need_new_food || players[i]->cycle(food);
        }
      }
    }

    for (int i = 0; i < 2; i++) {
      if (players[i]->alive) {
        players[i]->draw(*players[1 - i]);
      }
    }

    while (need_new_food) {
      food = Point::rand_in_disp(disp);
      if (!disp.get_pixel(food.x, food.y)) {
        need_new_food = false;
      }
    }
    disp.set_pixel(food.x, food.y, 3);
    
    disp.refresh();
    delay(1);
  }
  
  disp.palette = NULL;
  return false;
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

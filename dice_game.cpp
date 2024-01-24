#include "dice_game.h"
#include "config.h"
#include "graphics.h"

bool DiceGame::play() {
  byte palette[][3] = {
    {0, 0, 0},
    {8, 8, 24},
    {5, 24, 5},
    {24, 12, 12},
    {12, 0, 12},
    {15, 15, 0},
    {15, 0, 0},
    {0, 0, 0},
  };
  disp.palette = palette;
  
  player.x = 2;
  player.y = disp.height-2;
  player.color = 1;
  paused = false;

  unsigned long last_frame = millis();
  int frame_speed = 66;
  int decend_cooldown = 20; //unit: frames

  bool alive = true;
  while (alive) {
    unsigned long now = millis();
    
    bool should_exit = handle_input(now);
    if (should_exit) {
      Graphics::clear_rows(disp);
      disp.palette = NULL;
      return true;
    }
    
    if (paused) {
      continue;
    }

    if (now > last_frame + frame_speed) {
      last_frame = now;
      decend_cooldown--;
      if (decend_cooldown == 0) {
        decend_cooldown = 20;
        obstacles.decend();
      }
      projectiles.ascend_all();
    }

    disp.clear_all();
    obstacles.draw(disp);
    projectiles.draw(disp, obstacles);
    player.draw(disp);
    disp.refresh();
    delay(1);
  }


}

bool DiceGame::handle_button_down(Controller::Button button, int controller_index) {
  if (paused && button != Controller::Button::start && button != Controller::Button::select) {
    return false;
  }
  
  switch (button) {
    case Controller::Button::start: {
      return true;
    }

    case Controller::Button::select: {
      paused = !paused;
      return false;
    }

    case Controller::Button::left: {
      if (player.x > 0) {
        player.x--;
      }
      return false;
    }

    case Controller::Button::right: {
      if (player.x < 4) {
        player.x++;
      }
      return false;
    }

    case Controller::Button::down: {
      return false;
    }

    case Controller::Button::up: {
      projectiles.add_projectile(player.x, player.y - 2, player.color);
      return false;
    }

    case Controller::Button::a: {
      player.color = (player.color % 6) + 1; //constrained 1-6
      return false;
    }

    case Controller::Button::b: {
      player.color = ((player.color - 2 + 6) % 6) + 1; //constrained 1-6
      return false;
    }
  }
}


Obstacles::Obstacles() {
  for (int i = 0; i < sizeof(rows) / sizeof(*rows); i++) {
    fill_row(i);
  }
}

void Obstacles::fill_row(int row) {
  //randomSeed(analogRead(A0) + millis());
  for(int i = 0; i < sizeof(rows[0]) / sizeof(rows[0][0]); i++) {
    rows[row][i] = random(1, 7); //1-6
  }
}

void Obstacles::draw(Display& disp) {
  for (int row = 0; row < num_displayed; row++) {
    for (int column = 0; column < sizeof(rows[0]) / sizeof(rows[0][0]); column++) {
      disp.set_rect(column * 2, row * 2, 2, 2, rows[row_to_index(row)][column]);
    }
  }
}

void Obstacles::decend() {
  int row_count = sizeof(rows)/sizeof(rows[0]);
  first_row = (first_row - 1 + row_count) % row_count;
  num_displayed += 1;
}

void Obstacles::recycle_lowest_row() {
  fill_row((first_row + num_displayed) % (sizeof(rows) / sizeof(rows[0])));
  num_displayed -= 1;
}

void Obstacles::resolve_collision(int column, int projectile_color){
  for (int i = 0; i < num_displayed; i++) {
    if (rows[row_to_index(i)][column] == 0) {
      
      //collision is detected on the row above the first 0 found
      if (rows[row_to_index(i-1)][column] == projectile_color){
        rows[row_to_index(i-1)][column] = 0;
        
        //check for empty row
        bool has_value = false;
        for(int j = 0; j<sizeof(rows[0])/sizeof(rows[0][0]); j++){
          if(rows[row_to_index(i-1)][j] != 0){
            has_value = true;
            break;
          }
        }
        if(!has_value){
          recycle_lowest_row();
        }
      }
      break;
    }
  }
}

int Obstacles::row_to_index(int row) {
  int row_count = sizeof(rows) / sizeof(rows[0]);
  return (first_row + row + row_count) % row_count;
}

bool Projectile::draw(Display& disp) {
  return disp.set_rect(x * 2, y, 2, 2, color);
}

void Projectiles::add_projectile(int x, int y, int color) {
  for (int i = 0; i < sizeof(projectile_list) / sizeof(projectile_list[0]); i++){
    if (projectile_list[i].x == -1) {
      projectile_list[i].x = x;
      projectile_list[i].y = y;
      projectile_list[i].color = color;
      break;
    }
  }
}

void Projectiles::remove_projectile(int index) {
  projectile_list[index].x = -1;
}

void Projectiles::ascend_all() {
  for(int i = 0; i < sizeof(projectile_list) / sizeof(projectile_list[0]); i++){
    projectile_list[i].y -= 1;
    if (projectile_list[i].y <= 0) {
      remove_projectile(i);
    }
  }
}

void Projectiles::draw(Display& disp, Obstacles& obstacles){
  for (int i = 0; i < sizeof(projectile_list) / sizeof(projectile_list[0]); i++) {
    if (projectile_list[i].x != -1) {
      if (projectile_list[i].draw(disp)) { //if collision
        obstacles.resolve_collision(projectile_list[i].x, projectile_list[i].color);
        remove_projectile(i);
      }
    }
  }
}

DiceGame::DiceGame(Display& disp, Controller *controllers, int controller_count):
  InputProcessor(controllers, controller_count), disp(disp) {
  button_conf[Controller::Button::b] = { .initial = 500, .subsequent = 500};
  button_conf[Controller::Button::a] = { .initial = 500, .subsequent = 500};
  button_conf[Controller::Button::select] = { .initial = 0, .subsequent = 0};
  button_conf[Controller::Button::start] = { .initial = 0, .subsequent = 0};
  button_conf[Controller::Button::down] = { .initial = 100, .subsequent = 50};
  button_conf[Controller::Button::right] = { .initial = 200, .subsequent = 20};
  button_conf[Controller::Button::up] = { .initial = 200, .subsequent = 200};
  button_conf[Controller::Button::left] = { .initial = 200, .subsequent = 20};
}

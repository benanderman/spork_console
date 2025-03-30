#include "spork_console.h"
#include "obstacle_game.h"
#include "snake_game.h"
#include "sporktris.h"
#include "life.h"
#include "dice_game.h"
#include "chess.h"

SporkConsole console = SporkConsole();

void setup() {
  console.begin();
}

void loop() {
  MenuOption menuOptions[] = {
    SnakeGame::menuOption(),
    ObstacleGame::menuOption(),
    Sporktris::menuOption(),
    Life::menuOption(),
    DiceGame::menuOption(),
    Chess::menuOption(),
  };

  console.run(menuOptions, sizeof(menuOptions) / sizeof(*menuOptions));
}

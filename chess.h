#ifndef __CHESS
#define __CHESS

#include "arduino.h"
#include "controller.h"
#include "input_processor.h"
#include "display.h"

enum PieceType {
  none = 0,
  pawn,
  knight,
  bishop,
  rook,
  queen,
  king,
  __count,
} __attribute__((packed));

enum Side {
  white = 0,
  black = 1,
} __attribute__((packed));

struct Position {
  uint8_t x() {
    return data & 0xf;
  }
  uint8_t y() {
    return data >> 4;
  }

  Position(uint8_t x, uint8_t y) {
    data = (x & 0xf) | ((y & 0xf) << 4); 
  }

  private:
  uint8_t data;
};

struct Piece {
  Side side(){
    return (data >> 4) & 0x1;
  }
  PieceType type(){
    return data & 0xf;
  }

  Piece(Side side, PieceType type){
    data = ((side & 0x1) << 4) | (type & 0xf);
  }
  Piece() : Piece(Side::white, PieceType::none) {}  
  
  private:
  uint8_t data;
};

struct Board {
  Piece board[8][8];

  Board();

  int width(){
    return sizeof(*board)/sizeof(**board);
  }
  int height(){
    return sizeof(board)/sizeof(*board);
  }
};

struct GameState {
  Piece board[8][8];
  Side turn = Side::white;
  Position cursor_pos = Position(0,0);


  GameState();

  int board_width(){
    return sizeof(*board)/sizeof(**board);
  }
  int board_height(){
    return sizeof(board)/sizeof(*board);
  }

};

class Chess: public InputProcessor {
  public:
  Display& disp;
  Controller *controllers;
  int controller_count;

  Chess(Display& disp, Controller *controllers, int controller_count);

  bool play();

  private:
  bool handle_button_down(Controller::Button button, int controller_index);
  
  Piece board[8][8];
  Side turn = Side::white;
  Position cursor_pos = Position(0,0);
};

#endif
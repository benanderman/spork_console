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
  void set_x (int val) {
    data = (data & 0xf0) | val;
  }
  void set_y (int val) {
    data = (data & 0xf) | (val << 4);
  }

  bool operator==(Position other) {
    return (other.x() == x() && other.y() == y());
  }

  bool operator!=(Position other) {
    return !(operator==(other));
  }

  Position(uint8_t x, uint8_t y) {
    data = (x & 0xf) | ((y & 0xf) << 4); 
  }

  private:
  uint8_t data;
};

struct Piece {
  Side side() {
    return (data >> 4) & 0x1;
  }
  PieceType type() {
    return data & 0xf;
  }
  int color() {
    return type();
  }

  Piece(Side side, PieceType type) {
    data = ((side & 0x1) << 4) | (type & 0xf);
  }
  Piece() : Piece(Side::white, PieceType::none) {}  
  
  private:
  uint8_t data;
};

struct Board {
  Piece board[8][8];
  Position origin = Position(1,6);
  Side turn = Side::white;
  bool game_over = false;
  int en_passant_column = -1;
  uint8_t castling_pieces_moves = 0; //bit vector

  int animation_time = 2000; //milliseconds
  float max_dim = 0.6;

  int width(){
    return sizeof(*board)/sizeof(**board);
  }
  int height(){
    return sizeof(board)/sizeof(*board);
  }
  Piece piece(Position pos) {
    return board[pos.y()][pos.x()];
  }

  bool square_threatened(Position pos);
  bool valid_castle(int column);
  bool valid_move(Position start, Position end);
  bool line_is_empty(Position start, Position end);
  void move(Position start, Position end);

  void draw(Display& disp);

  Board();
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
  
  Board board;
  bool selected = false;
  Position selected_pos = Position(0,0);
  Position cursor_pos = Position(4,4);
};

#endif
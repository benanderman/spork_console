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
  int8_t x() {
    return x_;
  }
  int8_t y() {
    return y_;
  }
  void set_x (int8_t val) {
    x_ = val;
  }
  void set_y (int8_t val) {
    y_ = val;
  }

  bool operator==(Position other) {
    return (other.x() == x() && other.y() == y());
  }

  bool operator!=(Position other) {
    return !(operator==(other));
  }

  Position(int8_t x, int8_t y) {
    x_ = x;
    y_ = y;
  }
  private:
  int8_t x_;
  int8_t y_;
};

struct Piece {
  Side side() {
    return (Side)((data >> 4) & 0x1);
  }
  PieceType type() {
    return (PieceType)(data & 0xf);
  }
  uint8_t color() {
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
  int8_t en_passant_column = -1;
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

  Chess(Display& disp, Controller *controllers, uint8_t controller_count);

  bool play();

  private:
  bool handle_button_down(Controller::Button button, uint8_t controller_index);
  
  Board board;
  bool selected = false;
  Position selected_pos = Position(0,0);
  Position cursor_pos = Position(4,4);
};

#endif
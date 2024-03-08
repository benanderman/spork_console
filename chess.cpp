#include "chess.h"
#include "config.h"
#include "graphics.h"


bool Chess::play() {
  byte palette[][3] = {
    {0, 0, 0},
    {15, 4, 4},
    {24, 10, 0},
    {0, 24, 0},
    {0, 0, 24},
    {24, 24, 24},
    {24, 0, 24},
    {0, 0, 0},
  };
  disp.palette = palette;

  while(true) {
    disp.clear_all();
    board.draw(disp, turn);
    disp.set_rect(cursor_pos.x() + board.origin.x(), cursor_pos.y() + board.origin.y(), 1, 1, 3);
    disp.refresh();
    delay(1);
  }
}

Board::Board() {
  typedef PieceType t;
  typedef Side s;
  t piece_order[] = {t::rook, t::knight, t::bishop, t::queen, t::king, t::bishop, t::knight, t::rook};
  
  for(int i = 0; i < width(); i++){
    board[0][i] = Piece(s::black, piece_order[i]);
    board[1][i] = Piece(s::black, t::pawn);
    board[height() - 1][i] = Piece(s::white, piece_order[i]);
    board[height() - 2][i] = Piece(s::white, t::pawn);
  }
}

void Board::draw(Display& disp, Side turn) {
  unsigned long now = millis();

  for (int x = 0; x < width(); x++) {
    for (int y = 0; y < height(); y++){
      if (board[y][x].type() != PieceType::none) {
        if (board[y][x].side() == turn) {
          if (now % animation_time < animation_time * 4/5) {
            disp.set_pixel(x + origin.x(), y + origin.y(), board[y][x].type());
          }
      
        //memcpy(disp.palette[PieceType::__count], disp.palette[board[y][x].color()], 3);
        //for(int i = 0; i < 3; i++){
        //  disp.palette[PieceType::__count][i] *= float(abs((animation_time/2) - (now % animation_time))) / (animation_time/2) * (1 - max_dim) + max_dim;
        //}
        //disp.set_rect(x + origin.x(), y + origin.y(), 1, 1, PieceType::__count);

        } else {
          disp.set_rect(x + origin.x(), y + origin.y(), 1, 1, board[y][x].color());
        }
      }
    }
  }
}


Chess::Chess(Display& disp, Controller *controllers, int controller_count):
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

bool Chess::handle_button_down(Controller::Button button, int controller_index){};
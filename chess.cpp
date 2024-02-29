#include "chess.h"
#include "config.h"
#include "graphics.h"
/*
bool GameState::validate_move(tuple<int, int> start, tuple<int, int> end) {
  start_piece = board[start[0]][start[1]];
  end_piece = board[end[1]][end[1]]
  if (start == end
    || start_piece == 0
    || (turn < 0) != (start_piece < 0)
    || (turn < 0) == (end_piece < 0)

  ) {
    return false;
  }
  if start
  if ((start_piece < 0) == (end_piece < 0)) {
    return false;
  }
}
*/
bool Chess::play(){
  byte palette[][3] = {
    {0, 0, 0},
    {24, 0, 0},
    {24, 6, 0},
    {12, 12, 0},
    {0, 20, 0},
    {4, 4, 24},
    {12, 0, 12},
    {24, 0, 0},
    {0, 0, 0},
  };
  disp.palette = palette;

  while(true){
    disp.clear_all();
    disp.set_rect(cursor_pos.x(), cursor_pos.y(), 1, 1, 3);
    disp.refresh();
    delay(1);
  }
}

Board::Board(){
  typedef PieceType t;
  typedef Side s;
  t piece_order[] = {t::rook, t::knight, t::bishop, t::queen, t::king, t::bishop, t::knight, t::rook};

  //board[0] = {Piece(s::black, t::rook), Piece(s::black, t::knight), Piece(s::black, t::bishop), Piece(s::black, t::queen), Piece(s::black, t::king), Piece(s::black, t::queen), Piece(s::black, t::bishop), Piece(s::black, t::knight), Piece(s::black, t::rook)};
  //board[height()] = {Piece(s::white, t::rook), Piece(s::white, t::knight), Piece(s::white, t::bishop), Piece(s::white, t::queen), Piece(s::white, t::king), Piece(s::white, t::queen), Piece(s::white, t::bishop), Piece(s::white, t::knight), Piece(s::white, t::rook)};
  
  for(int i = 0; i < width() + 1; i++){
    board[0][i] = Piece(s::black, piece_order[i]);
    board[1][i] = Piece(s::black, t::pawn);
    board[height() - 1][i] = Piece(s::white, piece_order[i]);
    board[height() - 1][i] = Piece(s::white, t::pawn);
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
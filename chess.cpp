#include "chess.h"
#include "config.h"
#include "graphics.h"


bool Chess::play() {
  disp.palette[0] = RGB(0,  0,  0);
  disp.palette[1] = RGB(15, 4,  4);
  disp.palette[2] = RGB(24, 10, 0);
  disp.palette[3] = RGB(0,  24, 0);
  disp.palette[4] = RGB(0,  0,  24);
  disp.palette[5] = RGB(24, 24, 24);
  disp.palette[6] = RGB(24, 0,  24);
  disp.palette[7] = RGB(16, 16, 0);
  disp.palette[8] = RGB(0,  0,  0);

  while(!board.game_over) {
    unsigned long now = millis();

    bool should_exit = handle_input(now);
    if (should_exit) {
      Graphics::clear_rows(disp);
      return true;
    }
    disp.clear_all();
    board.draw(disp);
    disp.set_pixel(cursor_pos.x() + board.origin.x(), cursor_pos.y() + board.origin.y(), 7);
    disp.refresh();
    delay(1);
  }

  return Graphics::end_game(disp, controllers, controller_count, 4, 8);
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

void Board::move(Position start, Position end) {
  turn = turn == Side::white ? Side::black : Side::white;
  game_over = (piece(end).type() == PieceType::king);
  board[end.y()][end.x()] = piece(start);
  board[start.y()][start.x()] = Piece();
}

bool Board::line_is_empty(Position start, Position end) {
  Position offset = Position(end.x() - start.x(), end.y() - start.y());
  int8_t increment_x = offset.x() == 0 ? 0 : (offset.x() < 0 ? -1 : 1);
  int8_t increment_y = offset.y() == 0 ? 0 : (offset.y() < 0 ? -1 : 1);

  Position current_pos = Position(start.x() + increment_x, start.y() + increment_y);
  while (current_pos != end) {
    if (piece(current_pos).type() != PieceType::none) {
      return false;
    }

    current_pos.set_x(current_pos.x() + increment_x);
    current_pos.set_y(current_pos.y() + increment_y);
  }

  return true;
}

bool Board::square_threatened(Position pos) {
  Position orthagonal_directions[] = {Position(1, 0), Position(0, 1), Position(-1, 0), Position(0, -1)};
  Position diagonal_directions[] = {Position(1,1), Position(1, -1), Position(-1, 1), Position(-1, -1)};
  Position knight_moves[] = {Position(1, 2), Position(2, 1), Position(-1, 2), Position(-2, 1), Position(1, -2), Position(2, -1), Position(-1, -2), Position(-2, -1)};
  for (int i = 0; i < sizeof(knight_moves) / sizeof(*knight_moves); i++) {
    int x = pos.x() + knight_moves[i].x();
    int y = pos.y() + knight_moves[i].y();
    if (x < 0 || y < 0 || x > width() - 1 || y > height() - 1) {
      continue;
    }


  }
  return false;
}

bool Board::valid_move(Position start, Position end) {
  Piece start_piece = piece(start);
  Piece end_piece = piece(end);
  if (start == end) {
    return false;
  }
  if (start_piece.side() != turn) {
    return false;
  }
  if (end_piece.type() != PieceType::none && end_piece.side() == turn) { //can't capture your own piece
    return false;
  }

  Position offset = Position(end.x() - start.x() , end.y() - start.y() );
  switch (start_piece.type()) {
    case PieceType::knight:
      if ((abs(offset.x()) == 1 && abs(offset.y()) == 2) || (abs(offset.x()) == 2 && abs(offset.y()) == 1)) {
        return true;
      }
      break;
    case PieceType::pawn: {
      int8_t direction = start_piece.side() == Side::white ? -1 : 1;
      int8_t start_line = start_piece.side() == Side::white ? 6 : 1;
      if (abs(offset.x()) == 1) {
        // TODO: This en_passant_column is clearly wrong, but it's not straightforward to get right
        if ((offset.y() == 1*direction) && (end_piece.type() != PieceType::none || offset.x() == en_passant_column)) {
          return true;
        }
      } else if (offset.x() == 0) {
        bool square_is_empty = end_piece.type() == PieceType::none;
        bool is_one_forward = offset.y() == direction;
        bool is_two_forward = offset.y() == direction * 2;
        bool is_on_start_line = start.y() == start_line;
        bool can_move_two = is_on_start_line && line_is_empty(start, end);
        if (square_is_empty && (is_one_forward || (is_two_forward && can_move_two))) {
          return true;
        }
      }
      break;
    }
    case PieceType::bishop:
      if (abs(offset.x()) == abs(offset.y()) && line_is_empty(start, end)) {
        return true;
      } 
      break;
    case PieceType::rook:
      if ((offset.x() == 0 || offset.y() == 0) && line_is_empty(start, end)) {
        return true;
      }     
      break;
    case PieceType::queen:
      if ((offset.x() == 0 || offset.y() == 0 || abs(offset.x()) == abs(offset.y())) && line_is_empty(start, end)){
        return true;
      }
      break;
    case PieceType::king:
      if ((abs(offset.x()) <= 1 && abs(offset.y()) <= 1) || ((abs(offset.x()) == 2) && offset.y() == 0 && valid_castle(end.y())) ){
        return true;
      }
      break;
    
  }
  return false;
}

bool Board::valid_castle(int column) {
  return false;
}

void Board::draw(Display& disp) {
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


Chess::Chess(Display& disp, Controller *controllers, uint8_t controller_count):
  InputProcessor(controllers, controller_count), disp(disp) {
  button_conf[Controller::Button::b]      = { .initial = 500, .subsequent = 500};
  button_conf[Controller::Button::a]      = { .initial = 500, .subsequent = 500};
  button_conf[Controller::Button::select] = { .initial = 0,   .subsequent = 0};
  button_conf[Controller::Button::start]  = { .initial = 0,   .subsequent = 0};
  button_conf[Controller::Button::down]   = { .initial = 150, .subsequent = 50};
  button_conf[Controller::Button::right]  = { .initial = 150, .subsequent = 50};
  button_conf[Controller::Button::up]     = { .initial = 150, .subsequent = 50};
  button_conf[Controller::Button::left]   = { .initial = 150, .subsequent = 50};
}

bool Chess::handle_button_down(Controller::Button button, uint8_t controller_index) {
  switch (button) {
    case Controller::Button::start: {
      return true;
    }

    case Controller::Button::select: {
      return false;
    }

    case Controller::Button::left: {
      int x = cursor_pos.x();
      if (x > 0) {
        cursor_pos.set_x(x - 1);
      }
      return false;
    }

    case Controller::Button::right: {
      int x = cursor_pos.x();
      if (x < board.width() - 1) {
        cursor_pos.set_x(x + 1);
      }
      return false;
    }

    case Controller::Button::down: {
      int y = cursor_pos.y();
      if (y < board.height() - 1) {
        cursor_pos.set_y(y + 1);
      }
      return false;
    }

    case Controller::Button::up: {
      int y = cursor_pos.y();
      if (y > 0) {
        cursor_pos.set_y(y - 1);
      }
      return false;
    }

    case Controller::Button::a: {
      Piece piece = board.piece(cursor_pos);
      if (selected) {
        if (board.valid_move(selected_pos, cursor_pos)) {
          board.move(selected_pos, cursor_pos);
        }
        selected = false;

      } else if (piece.type() != PieceType::none && piece.side() == board.turn) {
        selected = true;
        selected_pos = cursor_pos;
      }
      return false;
    }

    case Controller::Button::b: {
      return false;
    }
  }
};
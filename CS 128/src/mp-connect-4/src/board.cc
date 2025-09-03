#include "board.hpp"

// you might need additional includes
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

/**************************************************************************/
/* your function definitions                                              */
/**************************************************************************/

void InitBoard(Board& b) {
  // below was minimal to get tests to actually compile and given test case to
  // fail
  for (int x = 0; x < Board::kBoardWidth; x++) {
    for (int y = 0; y < Board::kBoardHeight; y++) {
      b.board[y][x] = DiskType::kEmpty;
    }
  }
}

// Board is Y-X based
void DropDiskToBoard(Board& b, DiskType disk, int col) {
  if (!BoardLocationInBounds(0, col)) {
    throw std::runtime_error("The specified column is out of bounds");
  }
  for (int height = 0; height < Board::kBoardHeight; height++) {
    if (b.board[height][col] == DiskType::kEmpty) {
      b.board[height][col] = disk;
      return;
    }
  }
  throw std::runtime_error("The specified column is already filled");
}

bool CheckForWinner(Board& b, DiskType disk) {
  return ((SearchForWinner(b, disk, WinningDirection::kHorizontal) ||
           SearchForWinner(b, disk, WinningDirection::kLeftDiag) ||
           SearchForWinner(b, disk, WinningDirection::kRightDiag) ||
           SearchForWinner(b, disk, WinningDirection::kVertical)));
}

bool SearchForWinner(Board& b, DiskType disk, WinningDirection to_check) {
  int change_x = 0;
  int change_y = 0;
  switch (to_check) {
    case (WinningDirection::kHorizontal): {
      change_x = 1;
      break;
    }
    case (WinningDirection::kLeftDiag): {
      change_x = 1;
      change_y = -1;
      break;
    }
    case (WinningDirection::kRightDiag): {
      change_x = 1;
      change_y = 1;
      break;
    }
    case (WinningDirection::kVertical): {
      change_y = 1;
      break;
    }
  }
  return SearchThroughBoard(b, disk, change_x, change_y);
}

bool SearchThroughBoard(Board& b, DiskType disk, int change_x, int change_y) {
  for (unsigned int x = 0; x < Board::kBoardWidth; x++) {
    for (unsigned int y = 0; y < Board::kBoardHeight; y++) {
      bool found_connect_four = true;
      for (int i = 0; i < 4; i++) {
        if (!BoardLocationInBounds(y + change_y * i, x + change_x * i)) {
          found_connect_four = false;
          break;
        }
        if (b.board[y + change_y * i][x + change_x * i] != disk) {
          found_connect_four = false;
          break;
        }
      }
      if (found_connect_four) {
        return true;
      }
    }
  }
  return false;
}

bool BoardLocationInBounds(int row, int col) {
  return (row >= 0 && col >= 0 && col < Board::kBoardWidth &&
          row < Board::kBoardHeight);
}

/**************************************************************************/
/* provided to you                                                        */
/**************************************************************************/
std::string BoardToStr(const Board& b) {
  constexpr int kTotalWidth = Board::kBoardWidth * 11 - Board::kBoardHeight - 1;
  std::stringstream ss;
  ss << std::endl;
  for (int row = Board::kBoardHeight; row > 0; --row) {
    ss << " |";
    for (int col = 0; col < Board::kBoardWidth; ++col) {
      std::string value_here;
      value_here.push_back(static_cast<char>(b.board[row - 1][col]));
      ss << ' ' << CenterStr(value_here, Board::kBoardWidth + 1) << '|';
    }
    ss << std::endl;
    ss << " |" << std::setw(kTotalWidth) << std::setfill('-') << '|'
       << std::endl;
  }
  ss << " |";
  for (int col = 0; col < Board::kBoardWidth; ++col) {
    ss << ' ' << CenterStr(std::to_string(col), Board::kBoardWidth + 1) << '|';
  }
  return ss.str();
}

std::string CenterStr(const std::string& str, int col_width) {
  // quick and easy (but error-prone) implementation
  auto padl = (col_width - str.length()) / 2;
  auto padr = (col_width - str.length()) - padl;
  std::string strf = std::string(padl, ' ') + str + std::string(padr, ' ');
  return strf;
}
// clang-format off
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//  Written By : Michael R. Nowak                Environment : ubuntu:latest               //
//  Date ......: 2022/02/07                      Compiler ...: clang-10                    //
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// clang-format on
/////////////////////////////////////////////////////////////////////////////////////////////
//                             Framework Set-up //
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
#endif

#include "catch.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
//                                 Includes //
/////////////////////////////////////////////////////////////////////////////////////////////
#include <stdexcept>
#include <vector>

#include "board.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
//                             Helpers/Constants //
/////////////////////////////////////////////////////////////////////////////////////////////
constexpr int kBoardWidth = 7;
constexpr int kBoardHeight = 6;

bool AreEqual(DiskType solution[][kBoardWidth],
              DiskType student[][kBoardWidth]) {
  for (int i = 0; i < kBoardHeight; ++i) {
    for (int j = 0; j < kBoardWidth; ++j) {
      if (solution[i][j] != student[i][j]) return false;
    }
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                Test Cases //
/////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Board initialization", "[board_init]") {
  // SECTION("Can use sections") {}
  // clang-format off
  DiskType solution[kBoardHeight][kBoardWidth] = { 
    {DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty}, 
    {DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty}, 
    {DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty}, 
    {DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty}, 
    {DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty}, 
    {DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty, DiskType::kEmpty} 
  };
  // clang-format on
  Board student;  // NOLINT
  InitBoard(student);
  REQUIRE(AreEqual(solution, student.board));
}

TEST_CASE("Dropping pieces", "[DropDiskToBoard]") {
  const std::vector<DiskType> order = {
      DiskType::kPlayer1, DiskType::kPlayer2, DiskType::kPlayer1,
      DiskType::kPlayer1, DiskType::kPlayer2, DiskType::kPlayer2,
      DiskType::kPlayer1, DiskType::kPlayer1, DiskType::kPlayer1};
  SECTION(
      "Dropped pieces should stack, starting from the bottom (index 0) of the "
      "board") {
    for (int x = 0; x < Board::kBoardWidth; x++) {
      Board b;
      InitBoard(b);
      for (int i = 0; i < 4; i++) {
        DropDiskToBoard(b, order[i], x);
      }
      for (int y = 0; y < 4; y++) {
        REQUIRE(b.board[y][x] == order[y]);
      }
    }
  }
  SECTION("Dropped pieces cannot be dropped out of bounds") {
    std::vector<int> invalid_columns = {-10,
                                        -2,
                                        -1,
                                        Board::kBoardWidth,
                                        Board::kBoardWidth + 1,
                                        Board::kBoardWidth + 10};

    for (int invalid_column : invalid_columns) {
      try {
        Board b;
        InitBoard(b);
        DropDiskToBoard(b, DiskType::kPlayer1, invalid_column);
        REQUIRE(false);
      } catch (const std::runtime_error& e) {
      } catch (...) {
        REQUIRE(false);
      }
      try {
        Board b;
        InitBoard(b);
        DropDiskToBoard(b, DiskType::kPlayer2, invalid_column);
        REQUIRE(false);
      } catch (const std::runtime_error& e) {
      } catch (...) {
        REQUIRE(false);
      }
    }
  }
  SECTION("Dropped pieces cannot be dropped in a full column") {
    for (int x = 0; x < Board::kBoardWidth; x++) {
      Board b;
      InitBoard(b);
      for (int i = 0; i < Board::kBoardHeight; i++) {
        DropDiskToBoard(b, order[i], x);
      }
      try {
        DropDiskToBoard(b, DiskType::kPlayer1, x);
        REQUIRE(false);
      } catch (const std::runtime_error& e) {
      } catch (...) {
        REQUIRE(false);
      }
      try {
        DropDiskToBoard(b, DiskType::kPlayer2, x);
        REQUIRE(false);
      } catch (const std::runtime_error& e) {
      } catch (...) {
        REQUIRE(false);
      }
    }
  }
}

TEST_CASE("Board contains a winning sequence", "[CheckForWinner]") {
  Board b;
  InitBoard(b);
  unsigned int length = 4;
  SECTION("Horizontal") {
    // Copy-pasted code from CheckHorizontalDirection lmao
    for (unsigned int x = 0; x < Board::kBoardWidth - length + 1; x++) {
      for (unsigned int y = 0; y < Board::kBoardHeight; y++) {
        for (unsigned int i = 0; i < length; i++) {
          b.board[y][x + i] = DiskType::kPlayer1;
        }
        REQUIRE(CheckForWinner(b, DiskType::kPlayer1));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y][x + i] = DiskType::kPlayer2;
        }
        REQUIRE(CheckForWinner(b, DiskType::kPlayer2));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y][x + i] = DiskType::kEmpty;
        }
      }
    }
  }
  SECTION("Vertical") {
    for (unsigned int x = 0; x < Board::kBoardWidth; x++) {
      for (unsigned int y = 0; y < Board::kBoardHeight - length + 1; y++) {
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x] = DiskType::kPlayer1;
        }
        REQUIRE(CheckForWinner(b, DiskType::kPlayer1));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x] = DiskType::kPlayer2;
        }
        REQUIRE(CheckForWinner(b, DiskType::kPlayer2));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x] = DiskType::kEmpty;
        }
      }
    }
  }
  SECTION("Left Diagonal") {
    for (unsigned int x = 0; x < Board::kBoardWidth - length + 1; x++) {
      for (unsigned int y = (length - 1); y < Board::kBoardHeight; y++) {
        for (unsigned int i = 0; i < length; i++) {
          b.board[y - i][x + i] = DiskType::kPlayer1;
        }
        REQUIRE(CheckForWinner(b, DiskType::kPlayer1));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y - i][x + i] = DiskType::kPlayer2;
        }
        REQUIRE(CheckForWinner(b, DiskType::kPlayer2));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y - i][x + i] = DiskType::kEmpty;
        }
      }
    }
  }
  SECTION("Right Diagonal") {
    for (unsigned int x = 0; x < Board::kBoardWidth - length + 1; x++) {
      for (unsigned int y = 0; y < Board::kBoardHeight - length + 1; y++) {
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x + i] = DiskType::kPlayer1;
        }
        REQUIRE(CheckForWinner(b, DiskType::kPlayer1));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x + i] = DiskType::kPlayer2;
        }
        REQUIRE(CheckForWinner(b, DiskType::kPlayer2));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x + i] = DiskType::kEmpty;
        }
      }
    }
  }
}

TEST_CASE("Board does not contain a winning sequence", "[CheckForWinner]") {
  Board b;
  InitBoard(b);
  unsigned int length = 4;
  std::vector<std::vector<DiskType>> orderings = {
      {DiskType::kPlayer1, DiskType::kPlayer1, DiskType::kPlayer1,
       DiskType::kPlayer2},
      {DiskType::kPlayer2, DiskType::kPlayer1, DiskType::kPlayer1,
       DiskType::kPlayer1},
      {DiskType::kPlayer2, DiskType::kPlayer2, DiskType::kPlayer2,
       DiskType::kPlayer1},
      {DiskType::kPlayer1, DiskType::kPlayer2, DiskType::kPlayer2,
       DiskType::kPlayer2},
      {DiskType::kPlayer1, DiskType::kPlayer2, DiskType::kPlayer2,
       DiskType::kPlayer1},
      {DiskType::kPlayer2, DiskType::kPlayer1, DiskType::kPlayer1,
       DiskType::kPlayer2}};
  SECTION("Horizontal") {
    // Copy-pasted code from CheckHorizontalDirection lmao
    for (unsigned int x = 0; x < Board::kBoardWidth - length + 1; x++) {
      for (unsigned int y = 0; y < Board::kBoardHeight; y++) {
        for (unsigned int order_number = 0; order_number < orderings.size();
             order_number++) {
          for (unsigned int i = 0; i < length; i++) {
            b.board[y][x + i] = orderings[order_number][i];
          }
          REQUIRE_FALSE(CheckForWinner(b, DiskType::kPlayer1));
          REQUIRE_FALSE(CheckForWinner(b, DiskType::kPlayer2));
          for (unsigned int i = 0; i < length; i++) {
            b.board[y][x + i] = DiskType::kEmpty;
          }
        }
      }
    }
  }
  SECTION("Vertical") {
    for (unsigned int x = 0; x < Board::kBoardWidth; x++) {
      for (unsigned int y = 0; y < Board::kBoardHeight - length + 1; y++) {
        for (unsigned int order_number = 0; order_number < orderings.size();
             order_number++) {
          for (unsigned int i = 0; i < length; i++) {
            b.board[y + 1][x] = orderings[order_number][i];
          }
          REQUIRE_FALSE(CheckForWinner(b, DiskType::kPlayer1));
          REQUIRE_FALSE(CheckForWinner(b, DiskType::kPlayer2));
          for (unsigned int i = 0; i < length; i++) {
            b.board[y + 1][x] = DiskType::kEmpty;
          }
        }
      }
    }
  }
  SECTION("Left Diagonal") {
    for (unsigned int x = 0; x < Board::kBoardWidth - length + 1; x++) {
      for (unsigned int y = (length - 1); y < Board::kBoardHeight; y++) {
        for (unsigned int order_number = 0; order_number < orderings.size();
             order_number++) {
          for (unsigned int i = 0; i < length; i++) {
            b.board[y - i][x + i] = orderings[order_number][i];
          }
          REQUIRE_FALSE(CheckForWinner(b, DiskType::kPlayer1));
          REQUIRE_FALSE(CheckForWinner(b, DiskType::kPlayer2));
          for (unsigned int i = 0; i < length; i++) {
            b.board[y - i][x + i] = DiskType::kEmpty;
          }
        }
      }
    }
  }
  SECTION("Right Diagonal") {
    for (unsigned int x = 0; x < Board::kBoardWidth - length + 1; x++) {
      for (unsigned int y = 0; y < Board::kBoardHeight - length + 1; y++) {
        for (unsigned int order_number = 0; order_number < orderings.size();
             order_number++) {
          for (unsigned int i = 0; i < length; i++) {
            b.board[y + i][x + i] = orderings[order_number][i];
          }
          REQUIRE_FALSE(CheckForWinner(b, DiskType::kPlayer1));
          REQUIRE_FALSE(CheckForWinner(b, DiskType::kPlayer2));
          for (unsigned int i = 0; i < length; i++) {
            b.board[y + i][x + i] = DiskType::kEmpty;
          }
        }
      }
    }
  }
}

TEST_CASE("SearchForWinner finds a winner in the winning direction",
          "[SearchForWinner]") {
  Board b;
  InitBoard(b);
  unsigned int length = 4;
  SECTION("Horizontal") {
    // Copy-pasted code from CheckHorizontalDirection lmao
    for (unsigned int x = 0; x < Board::kBoardWidth - length + 1; x++) {
      for (unsigned int y = 0; y < Board::kBoardHeight; y++) {
        for (unsigned int i = 0; i < length; i++) {
          b.board[y][x + i] = DiskType::kPlayer1;
        }
        REQUIRE(SearchForWinner(b, DiskType::kPlayer1,
                                WinningDirection::kHorizontal));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y][x + i] = DiskType::kPlayer2;
        }
        REQUIRE(SearchForWinner(b, DiskType::kPlayer2,
                                WinningDirection::kHorizontal));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y][x + i] = DiskType::kEmpty;
        }
      }
    }
  }
  SECTION("Vertical") {
    for (unsigned int x = 0; x < Board::kBoardWidth; x++) {
      for (unsigned int y = 0; y < Board::kBoardHeight - length + 1; y++) {
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x] = DiskType::kPlayer1;
        }
        REQUIRE(SearchForWinner(b, DiskType::kPlayer1,
                                WinningDirection::kVertical));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x] = DiskType::kPlayer2;
        }
        REQUIRE(SearchForWinner(b, DiskType::kPlayer2,
                                WinningDirection::kVertical));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x] = DiskType::kEmpty;
        }
      }
    }
  }
  SECTION("Left Diagonal") {
    for (unsigned int x = 0; x < Board::kBoardWidth - length + 1; x++) {
      for (unsigned int y = (length - 1); y < Board::kBoardHeight; y++) {
        for (unsigned int i = 0; i < length; i++) {
          b.board[y - i][x + i] = DiskType::kPlayer1;
        }
        REQUIRE(SearchForWinner(b, DiskType::kPlayer1,
                                WinningDirection::kLeftDiag));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y - i][x + i] = DiskType::kPlayer2;
        }
        REQUIRE(SearchForWinner(b, DiskType::kPlayer2,
                                WinningDirection::kLeftDiag));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y - i][x + i] = DiskType::kEmpty;
        }
      }
    }
  }
  SECTION("Right Diagonal") {
    for (unsigned int x = 0; x < Board::kBoardWidth - length + 1; x++) {
      for (unsigned int y = 0; y < Board::kBoardHeight - length + 1; y++) {
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x + i] = DiskType::kPlayer1;
        }
        REQUIRE(SearchForWinner(b, DiskType::kPlayer1,
                                WinningDirection::kRightDiag));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x + i] = DiskType::kPlayer2;
        }
        REQUIRE(SearchForWinner(b, DiskType::kPlayer2,
                                WinningDirection::kRightDiag));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x + i] = DiskType::kEmpty;
        }
      }
    }
  }
}

TEST_CASE("SearchForWinner doesn't find a winner in the wrong direction",
          "[SearchForWinner]") {
  Board b;
  InitBoard(b);
  unsigned int length = 4;
  SECTION("Horizontal") {
    // Copy-pasted code from CheckHorizontalDirection lmao
    for (unsigned int x = 0; x < Board::kBoardWidth - length + 1; x++) {
      for (unsigned int y = 0; y < Board::kBoardHeight; y++) {
        for (unsigned int i = 0; i < length; i++) {
          b.board[y][x + i] = DiskType::kPlayer1;
        }
        REQUIRE(SearchForWinner(b, DiskType::kPlayer1,
                                WinningDirection::kHorizontal));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer1,
                                      WinningDirection::kLeftDiag));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer1,
                                      WinningDirection::kRightDiag));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer1,
                                      WinningDirection::kVertical));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y][x + i] = DiskType::kPlayer2;
        }
        REQUIRE(SearchForWinner(b, DiskType::kPlayer2,
                                WinningDirection::kHorizontal));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer2,
                                      WinningDirection::kLeftDiag));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer2,
                                      WinningDirection::kRightDiag));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer2,
                                      WinningDirection::kVertical));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y][x + i] = DiskType::kEmpty;
        }
      }
    }
  }
  SECTION("Vertical") {
    for (unsigned int x = 0; x < Board::kBoardWidth; x++) {
      for (unsigned int y = 0; y < Board::kBoardHeight - length + 1; y++) {
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x] = DiskType::kPlayer1;
        }
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer1,
                                      WinningDirection::kHorizontal));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer1,
                                      WinningDirection::kLeftDiag));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer1,
                                      WinningDirection::kRightDiag));
        REQUIRE(SearchForWinner(b, DiskType::kPlayer1,
                                WinningDirection::kVertical));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x] = DiskType::kPlayer2;
        }
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer2,
                                      WinningDirection::kHorizontal));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer2,
                                      WinningDirection::kLeftDiag));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer2,
                                      WinningDirection::kRightDiag));
        REQUIRE(SearchForWinner(b, DiskType::kPlayer2,
                                WinningDirection::kVertical));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x] = DiskType::kEmpty;
        }
      }
    }
  }
  SECTION("Left Diagonal") {
    for (unsigned int x = 0; x < Board::kBoardWidth - length + 1; x++) {
      for (unsigned int y = (length - 1); y < Board::kBoardHeight; y++) {
        for (unsigned int i = 0; i < length; i++) {
          b.board[y - i][x + i] = DiskType::kPlayer1;
        }
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer1,
                                      WinningDirection::kHorizontal));
        REQUIRE(SearchForWinner(b, DiskType::kPlayer1,
                                WinningDirection::kLeftDiag));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer1,
                                      WinningDirection::kRightDiag));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer1,
                                      WinningDirection::kVertical));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y - i][x + i] = DiskType::kPlayer2;
        }
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer2,
                                      WinningDirection::kHorizontal));
        REQUIRE(SearchForWinner(b, DiskType::kPlayer2,
                                WinningDirection::kLeftDiag));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer2,
                                      WinningDirection::kRightDiag));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer2,
                                      WinningDirection::kVertical));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y - i][x + i] = DiskType::kEmpty;
        }
      }
    }
  }
  SECTION("Right Diagonal") {
    for (unsigned int x = 0; x < Board::kBoardWidth - length + 1; x++) {
      for (unsigned int y = 0; y < Board::kBoardHeight - length + 1; y++) {
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x + i] = DiskType::kPlayer1;
        }
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer1,
                                      WinningDirection::kHorizontal));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer1,
                                      WinningDirection::kLeftDiag));
        REQUIRE(SearchForWinner(b, DiskType::kPlayer1,
                                WinningDirection::kRightDiag));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer1,
                                      WinningDirection::kVertical));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x + i] = DiskType::kPlayer2;
        }
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer2,
                                      WinningDirection::kHorizontal));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer2,
                                      WinningDirection::kLeftDiag));
        REQUIRE(SearchForWinner(b, DiskType::kPlayer2,
                                WinningDirection::kRightDiag));
        REQUIRE_FALSE(SearchForWinner(b, DiskType::kPlayer2,
                                      WinningDirection::kVertical));
        for (unsigned int i = 0; i < length; i++) {
          b.board[y + i][x + i] = DiskType::kEmpty;
        }
      }
    }
  }
}

TEST_CASE("You cannot put a piece out of bounds", "[BoardLocationInBounds]") {
  std::vector<int> invalid_x_positions = {-10,
                                          -2,
                                          -1,
                                          Board::kBoardWidth,
                                          Board::kBoardWidth + 1,
                                          Board::kBoardWidth + 10};

  std::vector<int> invalid_y_positions = {-10,
                                          -2,
                                          -1,
                                          Board::kBoardHeight,
                                          Board::kBoardHeight + 1,
                                          Board::kBoardHeight + 10};

  for (int invalid_y_position : invalid_y_positions) {
    for (int invalid_x_position : invalid_x_positions) {
      REQUIRE_FALSE(
          BoardLocationInBounds(invalid_y_position, invalid_x_position));
    }
  }
}

TEST_CASE("Flailing test cases", "[StupidTests]") {
  Board b;
  InitBoard(b);
  // R
  // R
  // R
  // R R R R

  DropDiskToBoard(b, DiskType::kPlayer1, 0);
  DropDiskToBoard(b, DiskType::kPlayer1, 0);
  DropDiskToBoard(b, DiskType::kPlayer1, 0);
  DropDiskToBoard(b, DiskType::kPlayer1, 0);

  DropDiskToBoard(b, DiskType::kPlayer1, 1);
  DropDiskToBoard(b, DiskType::kPlayer1, 2);
  DropDiskToBoard(b, DiskType::kPlayer1, 3);
  REQUIRE(
      SearchForWinner(b, DiskType::kPlayer1, WinningDirection::kHorizontal));
  REQUIRE(SearchForWinner(b, DiskType::kPlayer1, WinningDirection::kVertical));
  REQUIRE(CheckForWinner(b, DiskType::kPlayer1));

  InitBoard(b);

  // R R R
  // R R R
  // R R R

  DropDiskToBoard(b, DiskType::kPlayer1, 0);
  DropDiskToBoard(b, DiskType::kPlayer1, 0);
  DropDiskToBoard(b, DiskType::kPlayer1, 0);

  DropDiskToBoard(b, DiskType::kPlayer1, 1);
  DropDiskToBoard(b, DiskType::kPlayer1, 1);
  DropDiskToBoard(b, DiskType::kPlayer1, 1);

  DropDiskToBoard(b, DiskType::kPlayer1, 2);
  DropDiskToBoard(b, DiskType::kPlayer1, 2);
  DropDiskToBoard(b, DiskType::kPlayer1, 2);

  REQUIRE_FALSE(CheckForWinner(b, DiskType::kPlayer1));

  InitBoard(b);

  //       B
  //     B R
  //   B R R
  // B R R R

  DropDiskToBoard(b, DiskType::kPlayer1, 0);

  DropDiskToBoard(b, DiskType::kPlayer2, 1);
  DropDiskToBoard(b, DiskType::kPlayer1, 1);

  DropDiskToBoard(b, DiskType::kPlayer2, 2);
  DropDiskToBoard(b, DiskType::kPlayer2, 2);
  DropDiskToBoard(b, DiskType::kPlayer1, 2);

  DropDiskToBoard(b, DiskType::kPlayer2, 3);
  DropDiskToBoard(b, DiskType::kPlayer2, 3);
  DropDiskToBoard(b, DiskType::kPlayer2, 3);
  DropDiskToBoard(b, DiskType::kPlayer1, 3);

  REQUIRE_FALSE(CheckForWinner(b, DiskType::kPlayer2));
  REQUIRE(CheckForWinner(b, DiskType::kPlayer1));

  InitBoard(b);

  // B
  // R B
  // R R B
  // R R R B

  DropDiskToBoard(b, DiskType::kPlayer2, 3);

  DropDiskToBoard(b, DiskType::kPlayer1, 2);
  DropDiskToBoard(b, DiskType::kPlayer2, 2);

  DropDiskToBoard(b, DiskType::kPlayer1, 1);
  DropDiskToBoard(b, DiskType::kPlayer1, 1);
  DropDiskToBoard(b, DiskType::kPlayer2, 1);

  DropDiskToBoard(b, DiskType::kPlayer1, 0);
  DropDiskToBoard(b, DiskType::kPlayer1, 0);
  DropDiskToBoard(b, DiskType::kPlayer1, 0);
  DropDiskToBoard(b, DiskType::kPlayer2, 0);

  REQUIRE_FALSE(CheckForWinner(b, DiskType::kPlayer1));
  REQUIRE(CheckForWinner(b, DiskType::kPlayer2));

  InitBoard(b);

  // R R R
  // R R R
  // R R R

  DropDiskToBoard(b, DiskType::kPlayer1, 0);
  DropDiskToBoard(b, DiskType::kPlayer1, 0);
  DropDiskToBoard(b, DiskType::kPlayer1, 0);

  DropDiskToBoard(b, DiskType::kPlayer1, 1);
  DropDiskToBoard(b, DiskType::kPlayer1, 1);
  DropDiskToBoard(b, DiskType::kPlayer1, 1);

  DropDiskToBoard(b, DiskType::kPlayer1, 2);
  DropDiskToBoard(b, DiskType::kPlayer1, 2);
  DropDiskToBoard(b, DiskType::kPlayer1, 2);

  REQUIRE_FALSE(CheckForWinner(b, DiskType::kPlayer1));
}

/////////////////////////////////////////////////////////////////////////////////////////////
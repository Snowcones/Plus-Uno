#include "board.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

bool Board::is_goal(int goal) const {
  if (values[0] > 1 && values[0] != goal)
    return false;
  for (int i = 1; i < KBOARD_SIZE; i++) {
    if (values[i] != goal)
      return false;
  }
  return true;
}

bool Board::is_valid_position() const {
  if (values[1] < 1) {
    return false;
  }
  return true;
}

bool Board::is_sorted() const {
  for (int i = 1; i < KBOARD_SIZE; ++i) {
    if (values[i - 1] > values[i]) {
      return false;
    }
  }
  return true;
}

// Maintains the sorted order of the board
void Board::move() { assert(is_sorted()); }

void Board::sort() { std::sort(values.begin(), values.end()); }

void Board::print() const {
  for (int i = 0; i < 9; i++) {
    std::cout << values[i] << " ";
  }
  std::cout << std::endl;
}

int Board::operator[](int index) const { return values[index]; }

int &Board::operator[](int index) { return values[index]; }

bool Board::operator==(const Board& board) const {
  for(int i=0; i<KBOARD_SIZE; ++i) {
    if((*this)[i] != board[i]) {
      return false;
    }
  }
  return true;
}

int Board::find_first(int val) const {
  int index = std::find(values.begin(), values.end(), val) - values.begin();
  assert(0 <= index);
  assert(index < KBOARD_SIZE);
  return index;
}

int Board::find_last(int val) const {
  int index = std::find(values.begin(), values.end(), val) - values.begin();
  if (index < KBOARD_SIZE - 1 && (*this)[index + 1] == val) {
    index = index + 1;
  }
  assert(0 <= index);
  assert(index < KBOARD_SIZE);
  return index;
}

Board Board::undo(int* new_i1, int* new_i2, int i1, int i2) const {
  Board undone = *this;
  int v1 = (*this)[i1];
  int v2 = (*this)[i2];
  int new_v1 = v1-1;
  int new_v2 = v2-v1+1;
  undone[i1] = new_v1;
  undone[i2] = new_v2;
  undone.sort();
  *new_i1 = undone.find_first(new_v1);
  *new_i2 = undone.find_last(new_v2);
  return undone;
}

Board Board::undo_move(int* undone_index_1, int *undone_index_2, int index_1, int index_2) const {
  assert(index_1 != index_2);
  return undo(undone_index_1, undone_index_2, index_1, index_2);
}

std::pair<Board, Board>
Board::undo_one_two_case(int *first_index_1, int *first_index_2,
                         int *second_index_1, int *second_index_2, int index_1,
                         int index_2) const {
  assert((*this)[index_1] == 1);
  assert((*this)[index_2] == 2);
  assert(index_1 != index_2);

  Board first_undone = undo(first_index_1, first_index_2, index_1, index_2);
  Board second_undone = undo(second_index_1, second_index_2, index_2, index_1);
  return {first_undone, second_undone};
}

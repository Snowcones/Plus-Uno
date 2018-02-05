#ifndef BOARD_HPP
#define BOARD_HPP

#define KBOARD_SIZE 9
#include <array>

class Board {
  std::array<int, KBOARD_SIZE> values;

private:
  Board undo(int* new_i1, int* new_i2, int i1, int i2) const;

  // These two finds are necessary to create undone moves that don't use the
  // same tile twice.
  int find_first(int val) const;
  int find_last(int val) const;

public:
  Board(std::array<int, KBOARD_SIZE> values) : values(values){};
  bool is_goal(int goal) const;
  bool is_valid_position() const;
  bool is_sorted() const;
  // Maintains the sorted order of the board
  void move();
  void sort();
  void print() const;
  int operator[](int index) const;
  int &operator[](int index);
  bool operator==(const Board& board) const;

  // Returns a board with the move on index_1 and index_2 undone. Also returns
  // the indices of the undone board that are used to move to the current board.
  Board undo_move(int *undone_index_1, int *undone_index_2, int index_1,
                  int index_2) const;

  // Returns the two boards representing an undone move indices to values 1
  // and 2. Asserts that the values at these indices truly are 1 and 2. Returns
  // the indices of the undone board that are used to move to the current board.
  std::pair<Board, Board>
  undo_one_two_case(int *first_index_1, int *first_index_2, int *second_index_1,
                    int *second_index_2, int index_1, int index_2) const;
};

#endif /* BOARD_HPP */

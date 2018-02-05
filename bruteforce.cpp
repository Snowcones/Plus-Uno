#include "board.hpp"

#include <algorithm>
#include <cassert>
#include <ctime>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>

#define KMAX_VAL 64000

using Move = std::pair<int, int>;
using Info = std::pair<Move, size_t>;
using SearchNode = std::pair<Board, Move>;

struct FNV64Bit {
  inline std::size_t operator()(const Board &k) const {
    const size_t offsetBasis = 14695981039346656037llu;
    const size_t fnvPrime = (1ll << 40) + (1ll << 8) + 0xb3;
    std::size_t h = offsetBasis;
    for (int i = 0; i < KBOARD_SIZE; i++) {
      h = h ^ k[i];
      h = h * fnvPrime;
    }
    return h;
  }
};

void markPath(SearchNode s,
              std::unordered_map<Board, Info, FNV64Bit> &explored) {
  return;
  Board current_board = s.first;
  Move m = s.second;
  if (explored.count(current_board) == 0)
    return;
  Info &storedRes = explored[current_board];
  if (storedRes.second != (size_t)-1) {
    Info &storedRes = explored[current_board];
    //        std::cout << "Starting mark path" << std::endl;
    //        std::cout << "On: ";
    //        printBoard(cb);
    //        std::cout << "With second: " << storedRes.second << std::endl;

    storedRes.second = -1;
    if (m == Move(-1, -1))
      return;

    int i = m.first;
    int j = m.second;

    current_board[j] = current_board[i] + current_board[j];
    current_board[i] += 1;

    current_board.sort();
    Move next_move = storedRes.first;
    SearchNode next_node = SearchNode(current_board, next_move);
    markPath(next_node, explored);
    return;
  } else {
    return;
  }
}

void printRoute(SearchNode &s,
                const std::unordered_map<Board, Info, FNV64Bit> &explored) {
  Board current_board = s.first;
  Move packed_move = s.second;
  if (packed_move == Move(-1, -1)) {
    std::cout << "Board at: ";
    current_board.print();
    return;
  } else {
    std::cout << "Board at: ";
    current_board.print();

    size_t i = packed_move.first;
    size_t j = packed_move.second;

    current_board[j] = current_board[i] + current_board[j];
    current_board[i] += 1;
    current_board.sort();
    Move next_move = explored.at(current_board).first;

    SearchNode nextToPrint = SearchNode(current_board, next_move);
    printRoute(nextToPrint, explored);
    return;
  }
}

bool run(std::unordered_map<Board, Info, FNV64Bit> &explored,
         std::vector<SearchNode> &toExplore, int goal) {
  int numEx = 0;
  while (toExplore.size() != 0) {

    SearchNode edge = toExplore.back();
    Board curr = edge.first;
    Move m = edge.second;
    toExplore.pop_back();

    if (explored.count(curr) != 0) {
      if (explored[curr].second == (size_t)-1) {
        std::cout << "Early exit" << std::endl;
        // std::cout << "At ";
        // printBoard(curr);
        explored[curr] = Info(m, goal);
        markPath(edge, explored);
        printRoute(edge, explored);
        return true;
      } else {
        //std::cout << "Search collision?" << std::endl;
        if (explored[curr].second == (size_t)goal)
          continue;
      }
    }

    if (curr.is_goal(1)) {
      explored[curr] = Info(m, goal);
      markPath(edge, explored);
      printRoute(edge, explored);
      return true;
    }

    numEx++;

    if (numEx % 100000 == 0) {
      std::cout << "Explored: " << numEx << std::endl;
      std::cout << "ToExplore size: " << toExplore.size() << std::endl;
      std::cout << "Explored size: " << explored.size() << std::endl;
    }

    for (int i = 0; i < KBOARD_SIZE; i++) {
      if(curr[i] == 0) continue;
      if (i > 0 && curr[i - 1] == curr[i])
        continue;

      for (int j = i + 1; j < KBOARD_SIZE; j++) {
        if (j > i + 1) {
          if (curr[j] == curr[j-1]) continue;
        }
        if (curr[i] == 1 && curr[j] == 2) {
          assert(curr.is_sorted());

          int first_index_1;
          int first_index_2;
          int second_index_1;
          int second_index_2;
          std::pair<Board, Board> undone_boards =
              curr.undo_one_two_case(&first_index_1, &first_index_2,
                                     &second_index_1, &second_index_2, i, j);
          Board &zero_one_board = undone_boards.first;
          Board &zero_two_board = undone_boards.second;
          Move first_move(first_index_1, first_index_2);
          Move second_move(second_index_1, second_index_2);

          SearchNode zero_one_node =
              SearchNode(undone_boards.first, first_move);
          SearchNode zero_two_node =
              SearchNode(undone_boards.second, second_move);

          if (zero_one_board.is_valid_position()) {
            toExplore.push_back(zero_one_node);
          }
          if (zero_two_board.is_valid_position()) {
            toExplore.push_back(zero_two_node);
          }
        } else {
          assert(curr.is_sorted());

          int undone_index_1;
          int undone_index_2;
          Board undone = curr.undo_move(&undone_index_1, &undone_index_2, i, j);
          Move move(undone_index_1, undone_index_2);

          SearchNode smallSearch = SearchNode(undone, move);
          if (undone.is_valid_position())
            toExplore.push_back(smallSearch);
        }
      }
    }

    explored[curr] = Info(m, goal);
  }
  return false;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::unordered_map<Board, Info, FNV64Bit> explored;
  explored.max_load_factor(.5);
  std::vector<SearchNode> toExplore;

  clock_t begin = clock();

  for (int goal = 20000; goal <= 20000; goal++) {
    Board initial({1, goal - 1, goal, goal, goal, goal, goal, goal, goal});
    SearchNode start = SearchNode(initial, {-1, -1});
    explored.clear();
    toExplore.clear();
    toExplore.push_back(start);
    std::cout << "Starting level: " << goal << std::endl;
    std::cout << "Solution found for level: " << run(explored, toExplore, goal)
              << std::endl;
    std::cout << std::endl;
  }

  clock_t end = clock();
  double time = double(end - begin) / CLOCKS_PER_SEC;
  std::cout << "Ran in: " << time << "s" << std::endl;
}

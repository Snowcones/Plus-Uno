#include <iostream>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <cassert>
#include <thread>
#include <algorithm>

#define B_SIZE 9
#define MAX_VAL 32000
using Board = std::array<int, B_SIZE>;
using Info = std::pair<size_t, size_t>;
using SearchNode = std::pair<Board, size_t>;

struct FNV64Bit {
  inline std::size_t operator()(const Board& k) const {
      const size_t offsetBasis = 14695981039346656037llu;
      const size_t fnvPrime = (1ll << 40) + (1ll << 8) + 0xb3;
      std::size_t h = offsetBasis;
      for(int i=0; i<B_SIZE; i++) {
          h = h ^ k[i];
          h = h * fnvPrime;
      }
      return h;
  }
};

inline bool boardIsGoal(const Board& b, int goal) {
    //if(b[0] > 1) return false;
    for(int i=1; i<B_SIZE; i++) {
        if(b[i] != goal) return false;
    }
    return true;
}

inline bool boardIsCand(const Board& b) {
    if(b[1] < 1) return false;
/*    if(b[6] == 1) {
        int n = b[7];
        int y = b[8];
        return ((n*(n-1))/2 + 1) == y;
    }*/
    return true;
}


inline void singleSort(Board& b, int i, int j, int val) {

}

inline void sort(Board& b, int index_1, int index_2, int val) {
    // Decrement lowest board spot with val i
    while(index_1 > 0 && b.at(index_1-1) == b.at(index_1)) index_1--;
    b.at(index_1)--;

    // Move all spots val < b[k] < b[index_2] up one
    // Set first spot to val
    int s=0;
    while(s < 9 && b[s] < val) s++;
    for(int id=index_2; id!=s; id--) {
        b.at(id) = b.at(id-1);
    }
    b.at(s) = val;
}

void printBoard(Board& b) {
    for(int i=0; i<9; i++) {
        std::cout << b[i] << " ";
    }
    std::cout << std::endl;
}

void markPath(SearchNode s, std::unordered_map<Board, Info, FNV64Bit>& explored) {
    return;
    Board cb = s.first;
    size_t m = s.second;
    if (explored.count(cb) == 0) return;
    Info& storedRes = explored[cb];
    if(storedRes.second != -1) {
        Info& storedRes = explored[cb];
//        std::cout << "Starting mark path" << std::endl;
//        std::cout << "On: ";
//        printBoard(cb);
//        std::cout << "With second: " << storedRes.second << std::endl;

        storedRes.second = -1;
        if(m == (size_t)-1) return;

        size_t i = m % MAX_VAL;
        size_t j = m / MAX_VAL;

        cb[j] = cb[i] + cb[j];
        cb[i] += 1;

        std::sort(cb.begin(), cb.end());
        size_t nm = storedRes.first;
        SearchNode nextToSearch = SearchNode(cb, nm);
        markPath(nextToSearch, explored);
        return;
    } else {
        return;
    }
}

void printRoute(SearchNode& s, std::unordered_map<Board, Info, FNV64Bit>& explored) {
    Board cb = s.first;
    size_t m = s.second;
    if(m == (size_t)-1) return;
    else {
        std::cout << "Board at: ";
        printBoard(cb);

        size_t i = m % MAX_VAL;
        size_t j = m / MAX_VAL;

        cb[j] = cb[i] + cb[j];
        cb[i] += 1;

        std::sort(cb.begin(), cb.end());
        size_t nm = explored.at(cb).first;
        SearchNode nextToPrint = SearchNode(cb, nm);
        printRoute(nextToPrint, explored);
        return;
    }
}

bool run(std::unordered_map<Board, Info, FNV64Bit>& explored, std::vector<SearchNode>& toExplore, int goal) {
    int numEx = 0;
    while(toExplore.size()!=0) {

        SearchNode edge = toExplore.back();
        Board curr = edge.first;
        size_t m = edge.second;
        toExplore.pop_back();

        if(explored.count(curr) != 0) {
            if(explored[curr].second == -1) {
                //std::cout << "Early exit" << std::endl;
                //std::cout << "At ";
                //printBoard(curr);
                explored[curr] = Info(m, goal);
                markPath(edge, explored);
                printRoute(edge, explored);
                return true;
            } else {
                //std::cout << "Unkown board condition" << std::endl;
                //std::cout << "At ";
                //printBoard(curr);
                if(explored[curr].second == goal) continue;
            }
        }

        if(boardIsGoal(curr, 1)) {
            explored[curr] = Info(m, goal);
            markPath(edge, explored);
            printRoute(edge, explored);
            return true;
        }

        numEx++;

        if(numEx % 1000000 == 0) {
            std::cout << "Explored: " << numEx << std::endl;
        }

        for(int i=0; i<B_SIZE; i++) {
            for(int j=i+1; j<B_SIZE; j++) {
            //for(int j=B_SIZE-1; j>=i+1; j--) {
                if(curr[i] >= 1) {
                    if(curr[i] == 1 && curr[j] == 2) {
                        Board small = curr;
                        Board big = curr;

                        small[i] = 0;
                        small[j] = 1;

                        big[i] = 0;
                        big[j] = 2;

                        std::sort(small.begin(), small.end());
                        std::sort(big.begin(), big.end());

                        size_t si = std::find(small.begin(), small.end(), 1) - small.begin();
                        size_t sj = std::find(small.begin(), small.end(), 0) - small.begin();
                        size_t sm = si + sj * MAX_VAL;

                        size_t bi = std::find(big.begin(), big.end(), 0) - big.begin();
                        size_t bj = std::find(big.begin(), big.end(), 2) - big.begin();
                        size_t bm = bi + bj * MAX_VAL;

                        SearchNode smallSearch = SearchNode(small, sm);
                        SearchNode bigSearch = SearchNode(big, bm);

                        if(boardIsCand(small)) toExplore.push_back(smallSearch);
                        if(boardIsCand(big)) toExplore.push_back(bigSearch);
                    } else {
                        Board small = curr;

                        sort(small, i, j, curr[j] - curr[i] + 1);

                        size_t si = std::find(small.begin(), small.end(), curr[i]-1) - small.begin();
                        size_t sj = std::find(small.begin(), small.end(), curr[j]-curr[i]+1) - small.begin();
                        if(si == sj) sj++;
                        size_t sm = si + sj * MAX_VAL;

                        SearchNode smallSearch = SearchNode(small, sm);
                        if(boardIsCand(small)) toExplore.push_back(smallSearch);
                    }
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

    for(int goal = 3; goal <= 700; goal++) {
        Board initial = {1, goal-1, goal, goal, goal, goal, goal, goal, goal};
        SearchNode start = SearchNode(initial, -1);
        explored.clear();
        toExplore.clear();
        toExplore.push_back(start);
        std::cout << "Starting level: " << goal << std::endl;
        std::cout << "Solution found for level: " << run(explored, toExplore, goal) << std::endl;
        std::cout << std::endl;
    }

    clock_t end = clock();
    double time = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << "Ran in: " << time << "s" << std::endl;
}

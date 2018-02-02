#include <iostream>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <cassert>
#include <thread>
#include <algorithm>

#include <x86intrin.h>

#define B_SIZE 9
#define MAX_VAL 32000
using board = std::array<int, B_SIZE>;
using info = std::pair<size_t, size_t>;
using toSearch = std::pair<board, size_t>;

struct perfHash
{
    inline std::size_t operator()(const board k) const {
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

inline bool boardIsGoal(const board& b, int goal) {
    if(b[0] > 1) return false;
    for(int i=1; i<B_SIZE; i++) {
        if(b[i] != goal) return false;
    }
    return true;
}

inline bool boardIsCand(const board& b) {
    if(b[1] < 1) return false;
    if(b[6] == 1) {
        int n = b[7];
        int y = b[8];
        if ( y >= 3 && (n*(n-1)/2 + 1) <= y) {
            return false;
        }
    }
/*    if(b[6] == 1) {
        int n = b[7];
        int y = b[8];
        return ((n*(n-1))/2 + 1) == y;
    }*/
    return true;
}


inline void sort(board& b, int i, int j, int v) {
    b[i]--;

    // Move all spots v < b[k] < b[j] up one
    // Set first spot to v
    int s=0;
    while(s < B_SIZE && b[s] < v) s++;
    for(int id=j; id!=s; id--) {
        b[id] = b[id-1];
    }
    b[s] = v;
}

void printBoard(board& b) {
    for(int i=0; i<B_SIZE; i++) {
        std::cout << b[i] << " ";
    }
    std::cout << std::endl;
}

void markPath(toSearch s, std::unordered_map<board, info, perfHash>& explored) {
    return;
    board cb = s.first;
    size_t m = s.second;
    if (explored.count(cb) == 0) return;
    info& storedRes = explored[cb];
    if(storedRes.second != -1) {
        info& storedRes = explored[cb];
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
        toSearch nextToSearch = toSearch(cb, nm);
        markPath(nextToSearch, explored);
        return;
    } else {
        return;
    }
}

void printRoute(toSearch& s, std::unordered_map<board, info, perfHash>& explored) {
    board cb = s.first;
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
        size_t nm = explored[cb].first;
        toSearch nextToPrint = toSearch(cb, nm);
        printRoute(nextToPrint, explored);
        return;
    }
}

bool run(std::unordered_map<board, info, perfHash>& explored, std::vector<toSearch>& toExplore, int goal) {
    int numEx = 0;
    while(toExplore.size()!=0) {

        toSearch edge = toExplore.back();
        board curr = edge.first;
        size_t m = edge.second;
        toExplore.pop_back();

        if(explored.count(curr) != 0) {
            if(explored[curr].second == -1) {
                //std::cout << "Early exit" << std::endl;
                //std::cout << "At ";
                //printBoard(curr);
                explored[curr] = info(m, goal);
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
            explored[curr] = info(m, goal);
            markPath(edge, explored);
            printRoute(edge, explored);
            return true;
        }

        numEx++;

        if(numEx % 1000000 == 0) {
            std::cout << "Explored: " << numEx << std::endl;
        }

        bool found = false;

        int lastU = -1;
        int lastV = -1;

        for(int i=0; i<B_SIZE; i++) {
            if(curr[i] == lastU) continue;
            else lastU = curr[i];

            lastV = -1;
            for(int j=i+1; j<B_SIZE; j++) {
                if(curr[j] == lastV) continue;
                else lastV = curr[j];

                if(curr[i] >= 1) {
                    if(curr[i] == 1 && curr[j] == 2) {
                        board small = curr;
                        board big = curr;

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

                        toSearch smallSearch = toSearch(small, sm);
                        toSearch bigSearch = toSearch(big, bm);

                        if(boardIsCand(small)) {
                            toExplore.push_back(smallSearch);
                            found = true;
                        }
                        if(boardIsCand(big)) {
                            toExplore.push_back(bigSearch);
                            found = true;
                        }
                    } else {
                        board small = curr;

                        sort(small, i, j, curr[j] - curr[i] + 1);

                        size_t si = std::find(small.begin(), small.end(), curr[i]-1) - small.begin();
                        size_t sj = std::find(small.begin(), small.end(), curr[j]-curr[i]+1) - small.begin();
                        if(si == sj) sj++;
                        size_t sm = si + sj * MAX_VAL;

                        toSearch smallSearch = toSearch(small, sm);
                        if(boardIsCand(small)) {
                            toExplore.push_back(smallSearch);
                            found = true;
                        }
                    }
                }
            }
        }
        if(!found) {
            //std::cout << "Dead end for board: ";
            //printBoard(curr);
            //std::cout << "With path" << std::endl;
            //printRoute(edge, explored);
            //std::cout << std::endl << std::endl;
        }


        explored[curr] = info(m, goal);
    }
    return false;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::unordered_map<board, info, perfHash> explored;
    explored.max_load_factor(.5);
    std::vector<toSearch> toExplore;

    clock_t begin = clock();

    for(int goal = 2; goal <= 3000; goal++) {
        board initial;
        initial.fill(goal);
        toSearch start = toSearch(initial, -1);
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

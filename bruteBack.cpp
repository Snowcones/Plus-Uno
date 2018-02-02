#include <iostream>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <cassert>
#include <thread>

#define B_SIZE 9
using board = std::array<int, B_SIZE>;

struct perfHash
{
    inline std::size_t operator()(const board k) const {
/*        std::size_t h = 0;
        h |= k[0] & 0x1;
        for(int i=1; i<B_SIZE; i++) {
            h |= (k[i] & 0x3f) << (6*(i-1) + 1);
        }
*/
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
    return true;
}

inline void sort(board& b) {
/*
[[0,1],[3,4],[6,7]]
[[1,2],[4,5],[7,8]]
[[0,1],[3,4],[6,7],[2,5]]
[[0,3],[1,4],[5,8]]
[[3,6],[4,7],[2,5]]
[[0,3],[1,4],[5,7],[2,6]]
[[1,3],[4,6]]
[[2,4],[5,6]]
[[2,3]]
*/
    #define cswap(x,y) if (y < x) { std::swap(x,y); }
    cswap(b[0], b[1]);
    cswap(b[3], b[4]);
    cswap(b[6], b[7]);
    cswap(b[1], b[2]);
    cswap(b[4], b[5]);
    cswap(b[7], b[8]);
    cswap(b[0], b[1]);
    cswap(b[3], b[4]);
    cswap(b[6], b[7]);
    cswap(b[2], b[5]);
    cswap(b[0], b[3]);
    cswap(b[1], b[4]);
    cswap(b[5], b[8]);
    cswap(b[3], b[6]);
    cswap(b[4], b[7]);
    cswap(b[2], b[5]);
    cswap(b[0], b[3]);
    cswap(b[1], b[4]);
    cswap(b[5], b[7]);
    cswap(b[2], b[6]);
    cswap(b[1], b[3]);
    cswap(b[4], b[6]);
    cswap(b[2], b[4]);
    cswap(b[5], b[6]);
    cswap(b[2], b[3]);
    #undef cswap
}

inline void sort2(board& b, int i, int j, int v) {
    // Decrement lowest board spot with val i
    while(i > 0 && b.at(i-1) == b.at(i)) i--;
    b.at(i)--;

    // Move all spots v < b[k] < b[j] up one
    // Set first spot to v
    int s=0;
    while(s < 9 && b[s] < v) s++;
    for(int id=j; id!=s; id--) {
        b.at(id) = b.at(id-1);
    }
    b.at(s) = v;
}

bool run(std::unordered_map<board, int, perfHash>& explored, std::vector<board>& toExplore, int goal) {
    int numEx = 0;
    while(toExplore.size()!=0) {

        board curr = toExplore.back();
        toExplore.pop_back();

        if(explored.count(curr) != 0) continue;
        if(boardIsGoal(curr, 1)) return true;

        numEx++;

        if(numEx % 1000000 == 0) {
            std::cout << "Explored: " << numEx << std::endl;
        }

        for(int i=0; i<B_SIZE; i++) {
            for(int j=i+1; j<B_SIZE; j++) {
                if(curr[i] >= 1) {
                    if(curr[i] == 1 && curr[j] == 2) {
                        board small = curr;
                        board big = curr;

                        small[i] = 0;
                        small[j] = 1;

                        big[i] = 0;
                        big[j] = 2;

                        sort(small);
                        sort(big);

                        if(boardIsCand(small)) toExplore.push_back(small);
                        if(boardIsCand(big)) toExplore.push_back(big);
                    } else {
                        //board small = curr;
                        board cc = curr;

                        //small[i] -= 1;
                        //small[j] = curr[j] - small[i];

                        sort2(cc, i, j, curr[j] - curr[i] + 1);
                        //sort(small);
                        //assert(cc == small);

                        if(boardIsCand(cc)) toExplore.push_back(cc);
                    }
                }
            }
        }

        explored[curr] = 0;
    }
    return false;
}

int main() {
    std::unordered_map<board, int, perfHash> explored;
    explored.max_load_factor(.5);
    std::vector<board> toExplore;

    clock_t begin = clock();

    for(int goal = 1; goal < 400; goal++) {
        board initial = {goal, goal, goal, goal, goal, goal, goal, goal, goal};
        toExplore.clear();
        explored.clear();
        toExplore.push_back(initial);
        std::cout << "Starting level: " << goal << std::endl;
        std::cout << "Solution found for level: " << run(explored, toExplore, goal) << std::endl;
        std::cout << std::endl;
    }

    clock_t end = clock();
    double time = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << "Ran in: " << time << "s" << std::endl;
}

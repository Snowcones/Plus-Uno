#include <iostream>
#include <unordered_map>
#include <vector>
#include <ctime>
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
    if(b[1] == goal - 1) return false;
    for(int i=2; i<B_SIZE; i++) {
        if(b[i] != goal) return false;
    }
    return true;
}

inline bool boardIsForwCand(const board& b) {
    if(b[0] > 1) return false;
    return true;
}

inline bool boardIsBackCand(const board& b) {
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

inline void forwStep(const board& curr, int i, int j, std::vector<board>& toExForw, int goal) {
    if(curr[i] + curr[j] <= goal) {
        board small = curr;
        board big = curr;

        small[j] = curr[i] + curr[j];
        small[i] += 1;

        big[j] = curr[i] + curr[j];
        big[i] = curr[j] + 1;

        sort(small);
        sort(big);

        if(boardIsForwCand(small)) toExForw.push_back(small);
        if(boardIsForwCand(big)) toExForw.push_back(big);
    }
}

inline void backStep(const board& curr, int i, int j, std::vector<board>& toExBack, int goal) {
    if(curr[i] > 0) {
        if(curr[i] == 1 && curr[j] == 2) {
            board small = curr;
            board big = curr;

            small[i] = 0;
            small[j] = 1;

            big[i] = 0;
            big[j] = 2;

            sort(small);
            sort(big);

            if(boardIsForwCand(small)) toExBack.push_back(small);
            if(boardIsForwCand(big)) toExBack.push_back(big);
        } else {
            board small = curr;

            small[i] -= 1;
            small[j] = curr[j] - small[i];

            sort(small);

            if(boardIsBackCand(small)) toExBack.push_back(small);
        }
    }
}

bool run(std::unordered_map<board, int, perfHash>& exForw,
         std::unordered_map<board, int, perfHash>& exBack,
         std::vector<board>& toExForw,
         std::vector<board>& toExBack,
         int goal) {
    int numEx = 0;
    while(toExForw.size()!=0 && toExBack.size()!=0) {

        board curr = toExForw.back();
        toExForw.pop_back();

        if(exForw[curr] == goal) continue;
        if(exBack[curr] == goal) return true;

        numEx++;

        if(numEx % 1000000 == 0) {
            std::cout << "Explored: " << numEx << std::endl;
        }

        for(int i=0; i<B_SIZE; i++) {
            for(int j=i+1; j<B_SIZE; j++) {
                forwStep(curr, i, j, toExForw, goal);
            }
        }
        exForw[curr] = goal;

        curr = toExBack.back();
        toExBack.pop_back();

        if(exBack[curr] != 0) continue;
        if(exForw[curr] != 0) return true;

        for(int i=0; i<B_SIZE; i++) {
            for(int j=i+1; j<B_SIZE; j++) {
                backStep(curr, i, j, toExBack, goal);
            }
        }
        exBack[curr] = goal;
    }
    return false;
}

int main() {
    std::unordered_map<board, int, perfHash> exForw;
    std::unordered_map<board, int, perfHash> exBack;
    exForw.max_load_factor(.5);
    exBack.max_load_factor(.5);
    std::vector<board> toExForw;
    std::vector<board> toExBack;
    board initial = {0, 1, 1, 1, 1, 1, 1, 1, 1};

    clock_t begin = clock();
    for(int goal = 1; goal < 300; goal++) {
        board last = {goal, goal, goal, goal, goal, goal, goal, goal, goal};

        toExForw.clear();
        toExForw.push_back(initial);
        toExBack.clear();
        toExBack.push_back(last);
        std::cout << "Starting level: " << goal << std::endl;
        std::cout << "Solution found for level: " << run(exForw, exBack, toExForw, toExBack, goal) << std::endl;
        std::cout << std::endl;
        if(goal == 500) {
            long long v = 0;
            long long c = 0;
            for(int i=0; i<exForw.bucket_count(); i++) {
                if(exForw.bucket_size(i) == 0) continue;
//                std::cout << "Bucket " << i << " has " << explored.bucket_size(i) << " elements" << std::endl;
                v += exForw.bucket_size(i);
                c++;
            }
            std::cout << "Non-zero avr: " << (double)v / c << std::endl;
        }
    }

    clock_t end = clock();
    double time = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << "Ran in: " << time << "s" << std::endl;
}

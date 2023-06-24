//
// Created by Stanley Zhong on 6/23/23.
//

#include "Const.h"
#include "Move.h"
#include "util.h"
#include "micro.cpp"
#include <algorithm>
#include <random>
#include <vector>

using namespace std;

mt19937 rng(8);

bool canAttack(const Child& us) {
    if (us.dazed > 0) return false;
    return us.holding == HOLD_S1 || us.holding == HOLD_S2 || us.holding == HOLD_S3;
}

bool snowmanPhase() {
    return false;
}

bool pickUpSnow(vector <vector<Position>>& field, const Child& us, complex<int>& returnPos) {
    cerr << "trying to pickUpSnow" << endl;
    if (us.standing || !(us.holding == HOLD_EMPTY || us.holding == HOLD_P1 || us.holding == HOLD_P2)) return false;

    for (int x = us.x - 1; x <= us.x + 1; x++) {
        for (int y = us.y - 1; y <= us.y + 1; y++) {
            if (inBounds(x, y) && field[x][y].height > 0 && field[x][y].ground != GROUND_TREE && !field[x][y].pickupTaken && field[x][y].childTeam == -1) {
                returnPos = complex<int>(x, y);
                field[x][y].pickupTaken = true;
                return true;
            }
        }
    }
    return false;
}

// assumes that (targetX, targetY) is within movement range
bool canMoveTo(vector<vector<Position>>& field, const Child& us, int targetX, int targetY, bool mark = false) {
    if (!inBounds(targetX, targetY)) return false;
    const int n = max(abs(targetX - us.x), abs(targetY - us.y));
    for (int step = 1; step <= n; step++) {  // seems that C++ round already rounds away from 0
        int curX = us.x + round((double)(step * (targetX - us.x)) / n);
        int curY = us.y + round((double)(step * (targetY - us.y)) / n);

        if (field[curX][curY].ground == GROUND_TREE || (!mark && field[curX][curY].movementTaken)) return false;
        if (mark) field[curX][curY].movementTaken = true;
        if (curX == targetX && curY == targetY) return true;
    }
    return false;
}

const int chX[12] = { -1,  0,  1,  0, -2, -1,  0,  1,  2,  1,  0, -1 };
const int chY[12] = {  0, -1,  0,  1,  0, -1, -2, -1,  0,  1,  2,  1 };
int moves[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
bool moveRandomly(vector<vector<Position>>& field, const Child& us, complex<int>& returnPos) {
    cerr << "moveRandomly: " << us.x << ' ' << us.y << endl;
    /*
     * Assign each of the 12 moves an index, as shown below
     * ..6..
     * .517.
     * 40.28
     * .b39.
     * ..a..
     */
    int n = us.standing ? 12 : 4;
    sort(moves, moves+12);
    shuffle(moves, moves+n, rng);

    for (int i = 0; i < n; i++) {
        int x = us.x + chX[moves[i]], y = us.y + chY[moves[i]];
        if (canMoveTo(field, us, x, y)) {
            assert(canMoveTo(field, us, x, y, true));
            field[us.x][us.y].movementTaken = true;
            returnPos = complex<int>(x, y);
            return true;
        }
    }
    return false;
}

Move pick_move(int score[], vector <vector<Position>>& field, const vector <Child>& ourTeam,
               const vector <Child>& theirTeam, const Child& us, complex<int>& returnPos) {
    if (canAttack(us)) {
        cerr << "trying attacking" << endl;
        vector<int> attackScore(theirTeam.size());
        for (int i = 0; i < theirTeam.size(); i++) {
            attackScore[i] = attackability(field, us, theirTeam[i]);
            cerr << "i,attackScore: " << i << ' ' << attackScore[i] << endl;
        }
        int attackIdx = max_element(attackScore.begin(), attackScore.end()) - attackScore.begin();
        cerr << "attackIdx=" << attackIdx << endl;
        if (attackScore[attackIdx] > 0) {  // attack!
            returnPos = targetToAttack(field, us, theirTeam[attackIdx]);
            cerr << "attack target: " << returnPos.real() << ' ' << returnPos.imag() << endl;
            return Move::THROW;
        }
        cerr << "chose not to attack" << endl;
    }

    if (snowmanPhase()) {
        // TODO: build snowmen and prep for end of game
    }

    if (us.holding == HOLD_P1) {
        return Move::CRUSH;
    }
    if (us.holding == HOLD_EMPTY) {
        if (us.standing) {
            return Move::CROUCH;
        }
        if (pickUpSnow(field, us, returnPos)) {
            return Move::PICKUP;
        }
    }

    if (moveRandomly(field, us, returnPos)) {  // crawl somewhere
        return Move::CRAWL;
    }

    return Move::IDLE;
}
//
// Created by Stanley Zhong on 6/24/23.
//

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Const.h"
#include <algorithm>
#include <random>


mt19937 rng(8);


// assumes that (targetX, targetY) is within movement range
bool canMoveTo(vector<vector<Position>>& field, const Child& us, int targetX, int targetY, bool mark = false) {
    if (!inBounds(targetX, targetY)) return false;
    const int n = max(abs(targetX - us.x), abs(targetY - us.y));
    for (int step = 1; step <= n; step++) {  // seems that C++ round already rounds away from 0
        int curX = us.x + round((double)(step * (targetX - us.x)) / n);
        int curY = us.y + round((double)(step * (targetY - us.y)) / n);

        if (field[curX][curY].ground == GROUND_TREE || field[curX][curY].childTeam != -1 || field[curX][curY].height >= 6 || (!mark && field[curX][curY].movementTaken)) return false;
        if (mark) field[curX][curY].movementTaken = true;
        if (curX == targetX && curY == targetY) return true;
    }
    return false;
}

/*
 * Assign each of the 12 moves an index, as shown below
 * ..6..
 * .517.
 * 40.28
 * .b39.
 * ..a..
 */
const int chX[12] = { -1,  0,  1,  0, -2, -1,  0,  1,  2,  1,  0, -1 };
const int chY[12] = {  0, -1,  0,  1,  0, -1, -2, -1,  0,  1,  2,  1 };
int moves[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
bool moveRandomly(vector<vector<Position>>& field, const Child& us, complex<int>& returnPos) {
    sort(moves, moves+12);

    int n = us.standing ? 12 : 4;
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

bool moveToTarget(vector<vector<Position>>& field, const Child& us, const complex<int>& targetPos, complex<int>& returnPos) {
    int n = us.standing ? 12 : 4;

    int idx = -1;
    double minDist = abs(complex<double>{us.x - targetPos.real(), us.y - targetPos.imag()});
    for (int i = 0; i < n; i++) {
        int x = us.x + chX[i], y = us.y + chY[i];
        if (canMoveTo(field, us, x, y)) {
            complex<double> movement(x - targetPos.real(), y - targetPos.imag());
            double dist = abs(movement);
            if (minDist > dist) {
                minDist = dist;
                idx = i;
            }
        }
    }
    if (idx != -1) {
        int x = us.x + chX[idx], y = us.y + chY[idx];
        assert(canMoveTo(field, us, x, y, true));
        returnPos = complex<int>(x, y);
        return true;
    }
    return false;
}

bool moveAwayFrom(vector<vector<Position>>& field, const Child& us, const complex<int>& awayPos, complex<int>& returnPos) {
    sort(moves, moves+12);

    int n = us.standing ? 12 : 4;

    int idx = -1;
    double maxDist = 0;
    for (int i = 0; i < n; i++) {
        int x = us.x + chX[i], y = us.y + chY[i];
        if (canMoveTo(field, us, x, y)) {
            complex<int> result(x, y);
            double dist = abs(awayPos - result);
            if (maxDist <= dist) {
                maxDist = dist;
                idx = i;
            }
        }
    }
    if (idx != -1) {
        int x = us.x + chX[idx], y = us.y + chY[idx];
        assert(canMoveTo(field, us, x, y, true));
        returnPos = complex<int>(x, y);
        return true;
    }
    return false;
}

#endif //MOVEMENT_H
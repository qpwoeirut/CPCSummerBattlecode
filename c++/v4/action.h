//
// Created by Stanley Zhong on 6/24/23.
//

#ifndef ACTION_H
#define ACTION_H

#include "Const.h"
#include "util.h"


bool pickUpSnow(vector <vector<Position>>& field, const Child& us, int toPickUp, complex<int>& returnPos) {
    if (us.standing || !(us.holding == HOLD_EMPTY || us.holding == HOLD_P1 || us.holding == HOLD_P2 || us.holding == HOLD_S1 || us.holding == HOLD_S2)) return false;

    for (int x = us.x - 1; x <= us.x + 1; x++) {
        for (int y = us.y - 1; y <= us.y + 1; y++) {
            if (inBounds(x, y) && field[x][y].height > 0 && field[x][y].ground == toPickUp && !field[x][y].pickupTaken && field[x][y].childTeam == -1) {
                returnPos = complex<int>(x, y);
                field[x][y].pickupTaken = true;
                return true;
            }
        }
    }
    return false;
}

bool dropItemTargeted(const vector<vector<Position>>& field, const Child& us, const complex<int>& targetPos, complex<int>& returnPos) {
    const int x = targetPos.real(), y = targetPos.imag();
    if (inBounds(x, y) && (x != us.x || y != us.y) && abs(x - us.x) <= 1 && abs(y - us.y) <= 1 && field[x][y].ground != GROUND_TREE && field[x][y].height + us.holdingSize() <= 9 && field[x][y].childTeam == -1) {
        returnPos = targetPos;
        return true;
    }
    return false;
}

bool dropItemAvoid(const vector<vector<Position>>& field, const Child& us, const complex<int>& avoid, complex<int>& returnPos) {
    for (int x = us.x - 1; x <= us.x + 1; x++) {
        for (int y = us.y - 1; y <= us.y + 1; y++) {
            complex<int> targetPos(x, y);
            // prioritize dropping on empty ground
            if (field[x][y].ground == GROUND_EMPTY && dropItemTargeted(field, us, targetPos, returnPos)) {
                return true;
            }
        }
    }
    for (int x = us.x - 1; x <= us.x + 1; x++) {
        for (int y = us.y - 1; y <= us.y + 1; y++) {
            complex<int> targetPos(x, y);
            if (dropItemTargeted(field, us, targetPos, returnPos)) {
                return true;
            }
        }
    }
    return false;
}

bool dropItem(const vector<vector<Position>>& field, const Child& us, complex<int>& returnPos) {
    complex<int> invalidPos(-1, -1);
    return dropItemAvoid(field, us, invalidPos, returnPos);
}

#endif //ACTION_H

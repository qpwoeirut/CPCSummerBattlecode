//
// Created by Stanley Zhong on 6/24/23.
//

#ifndef UTIL_H
#define UTIL_H

#include "Const.h"
#include <vector>

using namespace std;

struct Position {
    int height;
    int ground;
    int childTeam;
    bool pickupTaken;
    bool movementTaken;
};


bool inBounds(const int x, const int y) {
    return 0 <= x && x < SIZE && 0 <= y && y < SIZE;
}

// dist = max(|x1 - x2|, |y1 - y2|)
int nearestTeammate(const Child& us, const vector<Child>& ourTeam, int& minDist) {
    int idx = -1;
    minDist = INIT;
    for (int i = 0; i < ourTeam.size(); i++) {
        if (ourTeam[i].x == us.x && ourTeam[i].y == us.y) continue;
        int dist = max(abs(ourTeam[i].x - us.x), abs(ourTeam[i].y - us.y));
        if (minDist > dist) {
            minDist = dist;
            idx = i;
        }
    }
    return idx;
}

int countAdjacent(const vector<vector<Position>>& field, const Child& us, int value) {
    int ct = 0;
    for (int x = us.x - 1; x <= us.x + 1; x++) {
        for (int y = us.y - 1; y <= us.y + 1; y++) {
            if (x == us.x && y == us.y) continue;
            if (inBounds(x, y) && field[x][y].ground == value) ct++;
        }
    }
    return ct;
}

#endif //UTIL_H

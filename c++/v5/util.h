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

bool canAttack(const Child& us) {
    if (us.dazed > 0) return false;
    return us.holding == HOLD_S1 || us.holding == HOLD_S2 || us.holding == HOLD_S3;
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

int nearestPosition(const vector<vector<Position>>& field, const Child& us, const vector<complex<int>>& positions, int avoid, double& minDist) {
    int idx = -1;
    minDist = INIT;
    complex<int> startPos(us.x, us.y);
    for (int i = 0; i < positions.size(); i++) {
        if (field[positions[i].real()][positions[i].imag()].ground == avoid) continue;
        double dist = abs(positions[i] - startPos);
        if (minDist > dist) {
            minDist = dist;
            idx = i;
        }
    }
    return idx;
}

int nearestPosition(const complex<int> startPos, const vector<pair<int, complex<int>>>& positions, double& minDist) {
    int idx = -1;
    minDist = INIT;
    for (int i = 0; i < positions.size(); i++) {
        double dist = abs(positions[i].second - startPos);
        if (minDist > dist) {
            minDist = dist;
            idx = i;
        }
    }
    return idx;
}

complex<int> nearestAvailable(const vector<vector<Position>>& field, const complex<int>& position) {
    double bestDist = INIT;
    complex<int> bestPos(-1, -1);

    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            if (field[x][y].ground != GROUND_TREE) {
                double dist = abs(position - complex<int>(x, y));
                if (bestDist > dist) {
                    bestDist = dist;
                    bestPos = complex<int>(x, y);
                }
            }
        }
    }
    return bestPos;
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

bool findAdjacent(const vector<vector<Position>>& field, const Child& us, int value, complex<int>& position) {
    for (int x = us.x - 1; x <= us.x + 1; x++) {
        for (int y = us.y - 1; y <= us.y + 1; y++) {
            if (inBounds(x, y) && (x != us.x || y != us.y) && field[x][y].ground == value) {
                position = complex<int>(x, y);
                return true;
            }
        }
    }
    return false;
}

double distanceBetween(const Child& us, const Child& them) {
    complex<double> usPos(us.x, us.y), themPos(them.x, them.y);
    return abs(usPos - themPos);
}

double distanceBetween(const Child& us, const complex<int>& position) {
    complex<double> usPos(us.x, us.y), otherPos(position.real(), position.imag());
    return abs(usPos - otherPos);
}

vector<complex<int>> positionsOf(const vector<vector<Position>>& field, int groundValue) {
    vector<complex<int>> positions;
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            if (field[x][y].ground == groundValue) {
                positions.emplace_back(x, y);
            }
        }
    }
    return positions;
}

// check if we've been hit recently
bool theyCanSeeUs(const vector<vector<Position>>& field, const Child& us, const vector<Child>& theirTeam) {
    vector<complex<int>> theirSnowmen = positionsOf(field, GROUND_SMB);
    if (us.dazed > 1) return true;
    for (const Child& them: theirTeam) {
        if (distanceBetween(us, them) <= 8) return true;
    }
    for (const complex<int>& snowman: theirSnowmen) {
        if (distanceBetween(us, snowman) <= 8) return true;
    }
    return false;
}

#endif //UTIL_H

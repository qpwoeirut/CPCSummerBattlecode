//
// Created by Stanley Zhong on 6/24/23.
//

#ifndef UTIL_H
#define UTIL_H


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

#endif //UTIL_H

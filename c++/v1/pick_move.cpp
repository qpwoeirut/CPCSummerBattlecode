//
// Created by Stanley Zhong on 6/23/23.
//

#include "Const.h"
#include "Move.h"
#include "util.h"
#include "micro.cpp"
#include "movement.cpp"
#include <algorithm>
#include <random>
#include <vector>

using namespace std;

bool canAttack(const Child& us) {
    if (us.dazed > 0) return false;
    return us.holding == HOLD_S1 || us.holding == HOLD_S2 || us.holding == HOLD_S3;
}

bool snowmanPhase() {
    return false;
}

bool pickUpSnow(vector <vector<Position>>& field, const Child& us, int toPickUp, complex<int>& returnPos) {
    cerr << "trying to pickUpSnow" << endl;
    if (us.standing || !(us.holding == HOLD_EMPTY || us.holding == HOLD_S1 || us.holding == HOLD_S2)) return false;

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

Move prepareToAttack(vector<vector<Position>>& field, const Child& us, const complex<int>& targetPos, complex<int>& returnPos) {
    if (us.holding == HOLD_P1) return Move::CRUSH;
    if (us.holding == HOLD_EMPTY) {  // need to get a snowball to throw
        if (us.standing) return Move::CROUCH;
        if (pickUpSnow(field, us, GROUND_EMPTY, returnPos)) return Move::PICKUP;
        if (moveRandomly(field, us, returnPos)) return Move::CRAWL;  // in case there's no snow left to pick up
    }

    if (!us.standing) return Move::STAND;  // increase mobility
    if (moveToTarget(field, us, targetPos, returnPos)) return Move::RUN;
    return Move::IDLE;
}

bool dropItem(vector<vector<Position>>& field, const Child& us, complex<int>& returnPos) {
    for (int x = us.x - 1; x <= us.x + 1; x++) {
        for (int y = us.y - 1; y <= us.y + 1; y++) {
            if (inBounds(x, y) && field[x][y].ground == GROUND_EMPTY && field[x][y].height + us.holdingSize() <= 9 && field[x][y].childTeam == -1) {
                returnPos = complex<int>(x, y);
                return true;
            }
        }
    }
    return false;
}

Move stockpileSnowballs(vector<vector<Position>>& field, const vector<Child>& ourTeam, const Child& us, complex<int>& returnPos) {
    int dist;
    int teammateIdx = nearestTeammate(us, ourTeam, dist);
    if (dist <= 2) {
        cerr << "nearest teammate dist: " << dist << endl;
        complex<int> teammatePos(ourTeam[teammateIdx].x, ourTeam[teammateIdx].y);
        if (moveAwayFrom(field, us, teammatePos, returnPos)) return us.standing ? Move::RUN : Move::CRAWL;
    }
    if (us.holding == HOLD_S3) {
        cerr << "moving toward center" << endl;
        if (!us.standing) return Move::STAND;
        if (moveToTarget(field, us, complex<int>(SIZE / 2, SIZE / 2), returnPos)) return Move::RUN;
        return Move::IDLE;
    }

    if (us.standing) return Move::CROUCH;
    if (us.holding == HOLD_P1) return Move::CRUSH;

    // create snowballs until we can pick up 3 at once
    int heldSnowballs = us.holding == HOLD_S1 ? 1 : (us.holding == HOLD_S2 ? 2 : 0);
    int snowballsNearby = countAdjacent(field, us, GROUND_S) + heldSnowballs;
    cerr << "snowballsNearby=" << snowballsNearby << endl;
    if (snowballsNearby >= 3) {  // 3 snowballs available, go pick them up
        if (pickUpSnow(field, us, GROUND_S, returnPos)) return Move::PICKUP;
    } else if (us.holding != HOLD_EMPTY) {  // need to make more snowballs, so free up our hands
        if (dropItem(field, us, returnPos)) return Move::DROP;
    }
    if (us.holding == HOLD_EMPTY) {
        if (pickUpSnow(field, us, GROUND_EMPTY, returnPos)) return Move::PICKUP;
    }

    return Move::IDLE;  // TODO handle this
}

Move pick_move(int score[], vector <vector<Position>>& field, const vector <Child>& ourTeam,
               const vector <Child>& theirTeam, const vector<pair<int, complex<int>>>& theirLastPosition,
               const Child& us, complex<int>& returnPos) {
    if (us.dazed) return Move::IDLE;

    complex<int> targetPos;
    int targetIdx = pickTarget(ourTeam, theirTeam, theirLastPosition, targetPos);  // check if there's a threat/target
    cerr << "targetIdx: " << targetIdx << endl;
    if (targetIdx != -1) {
        if (canAttack(us)) {
            cerr << "trying attacking" << endl;
            vector<int> attackScore(theirTeam.size());
            for (int i = 0; i < theirTeam.size(); i++) {
                attackScore[i] = attackability(field, us, theirTeam[i]);
                cerr << "i,attackScore: " << i << ' ' << attackScore[i] << endl;
            }
            int attackIdx = max_element(attackScore.begin(), attackScore.end()) - attackScore.begin();
            if (attackScore[targetIdx] > 0) attackIdx = targetIdx;  // override
            cerr << "attackIdx=" << attackIdx << endl;

            if (attackScore[attackIdx] > 0) {  // attack!
                returnPos = targetToAttack(field, us, theirTeam[attackIdx]);
                cerr << "attack target: " << returnPos.real() << ' ' << returnPos.imag() << endl;
                return Move::THROW;
            }
            cerr << "chose not to attack" << endl;
        }
        Move move = prepareToAttack(field, us, targetPos, returnPos);
        if (move != Move::IDLE) return move;
    }

    return stockpileSnowballs(field, ourTeam, us, returnPos);

//    if (snowmanPhase()) {
//        // TODO: build snowmen and prep for end of game
//    }
}
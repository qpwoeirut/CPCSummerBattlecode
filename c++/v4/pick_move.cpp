//
// Created by Stanley Zhong on 6/23/23.
//

#include "Const.h"
#include "Move.h"
#include "util.h"
#include "action.h"
#include "micro.cpp"
#include "movement.h"
#include "snowman.cpp"
#include <algorithm>
#include <random>
#include <vector>

using namespace std;

Move prepareToAttack(vector<vector<Position>>& field, const Child& us, const complex<int>& targetPos, complex<int>& returnPos) {
    switch(us.holding) {
        case HOLD_EMPTY:
            if (us.standing) return Move::CROUCH;
            if (pickUpSnow(field, us, GROUND_S, returnPos)) return Move::PICKUP;
            if (pickUpSnow(field, us, GROUND_EMPTY, returnPos)) return Move::PICKUP;
            if (moveRandomly(field, us, returnPos)) return Move::CRAWL;  // in case there's no snow left to pick up
            break;
        case HOLD_P1:
            return Move::CRUSH;
        case HOLD_P2:
        case HOLD_P3:
        case HOLD_M:
        case HOLD_L:
            if (dropItem(field, us, returnPos)) return Move::DROP;
    }

    if (!us.standing) return Move::STAND;  // increase mobility
    if (moveToTarget(field, us, targetPos, returnPos)) return movement(us);
    if (moveRandomly(field, us, returnPos)) return movement(us);
    return Move::IDLE;
}

Move stockpileSnowballs(vector<vector<Position>>& field, const vector<Child>& ourTeam, const Child& us, complex<int>& returnPos) {
    if (us.holding == HOLD_S3) {
        if (!us.standing) return Move::STAND;
        if (moveToTarget(field, us, complex<int>(SIZE / 2, SIZE / 2), returnPos)) return movement(us);
        return Move::IDLE;
    }

    int dist;
    int teammateIdx = nearestTeammate(us, ourTeam, dist);
    if (dist <= 2) {
        complex<int> teammatePos(ourTeam[teammateIdx].x, ourTeam[teammateIdx].y);
        if (moveAwayFrom(field, us, teammatePos, returnPos)) return movement(us);
    }

    if (us.standing) return Move::CROUCH;
    if (us.holding == HOLD_P1) return Move::CRUSH;

    // create snowballs until we can pick up 3 at once
    int heldSnowballs = us.holding == HOLD_S1 ? 1 : (us.holding == HOLD_S2 ? 2 : 0);
    int snowballsNearby = countAdjacent(field, us, GROUND_S) + heldSnowballs;
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

Move pick_move(int turnNum, int score[], vector <vector<Position>>& field, const vector <Child>& ourTeam,
               vector <Child>& theirTeam, const vector<pair<int, complex<int>>>& theirLastPosition,
               int currentChildIdx, complex<int>& returnPos) {
    const Child& us = ourTeam[currentChildIdx];

    if (us.dazed) return Move::IDLE;

    complex<int> snowmanPos;
    SnowmanStage snowmanStage = nearbySnowmanStage(field, us, snowmanPos);

    if (attack(field, us, theirTeam, theirLastPosition, returnPos)) {
        return Move::THROW;
    }
    if (snowmanStage == SnowmanStage::BASE_AND_BODY) {  // finish snowman
        return buildSnowman(field, currentChildIdx, ourTeam, returnPos);
    }

    complex<int> targetPos;
    if (opponentsInRange(ourTeam, theirLastPosition, targetPos)) {
        Move move = prepareToAttack(field, us, targetPos, returnPos);
        if (move != Move::IDLE) return move;
    }

    // build snowmen at end of game, in case we're facing campers
    if (turnNum <= 15 || turnNum >= ROUNDS - 80 || snowmanStage != NONE) {
        Move move = buildSnowman(field, currentChildIdx, ourTeam, returnPos);
        if (move != Move::IDLE) return move;
    }

    return stockpileSnowballs(field, ourTeam, us, returnPos);
}
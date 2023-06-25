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

const complex<int> CHILD_POST[4][4] = {
        {
                complex<int> (6, 12),
                complex<int> (7, 10),
                complex<int> (10, 7),
                complex<int> (12, 6)
        },
        {
                complex<int> (13, 6),
                complex<int> (20, 8),
                complex<int> (18, 10),
                complex<int> (21, 7)
        },
        {
                complex<int> (16, 13),
                complex<int> (18, 15),
                complex<int> (20, 17),
                complex<int> (22, 19)
        },
        {
                complex<int> (10, 15),
                complex<int> (15, 10),
                complex<int> (20, 15),
                complex<int> (15, 20)
        }
};

Move prepareToAttack(vector<vector<Position>>& field, const Child& us, complex<int>& returnPos) {
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
    return Move::IDLE;
}

Move stockpileSnowballs(vector<vector<Position>>& field, const vector<Child>& ourTeam, const Child& us, complex<int>& returnPos) {
    if (us.holding == HOLD_S3) {
        if (!us.standing) return Move::STAND;
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

    if (attack(field, us, theirTeam, theirLastPosition, returnPos)) {
        return Move::THROW;
    }

    complex<int> snowmanPos;
    SnowmanStage snowmanStage = nearbySnowmanStage(field, us, snowmanPos);
    if (turnNum <= 20 || snowmanStage == SnowmanStage::BASE_AND_BODY) {  // finish snowman
        return buildSnowman(field, currentChildIdx, ourTeam, returnPos);
    }

    if (attackSnowman(field, us, returnPos)) {
        return Move::THROW;
    }

    complex<int> targetPos;
    const double threat = threatScore(ourTeam, theirLastPosition, targetPos);
    if (threat == 0 && turnNum >= 50) {
        Move move = buildSnowman(field, currentChildIdx, ourTeam, returnPos);
        if (move != Move::IDLE) return move;
    }

    if (threat >= 100) {
        Move move = prepareToAttack(field, us, returnPos);
        if (move != Move::IDLE) return move;
    }
    if (turnNum <= 80) {
        if (theyCanSeeUs(field, us, theirTeam)) {
            if (moveToTarget(field, us, complex<int>(1, 1), returnPos)) return movement(us);
        } else {
            if (moveToTarget(field, us, CHILD_POST[0][currentChildIdx], returnPos)) return movement(us);
        }
    }
    if (80 < turnNum && turnNum <= 105) {
        if (currentChildIdx == 3) {
            Move move = buildSnowman(field, currentChildIdx, ourTeam, returnPos);
            if (move != Move::IDLE) return move;
        }
        if (moveToTarget(field, us, CHILD_POST[1][currentChildIdx], returnPos)) return movement(us);
    }
    if (105 < turnNum && turnNum <= 130) {
        if (currentChildIdx == 2) {
            Move move = buildSnowman(field, currentChildIdx, ourTeam, returnPos);
            if (move != Move::IDLE) return move;
        }
        if (moveToTarget(field, us, CHILD_POST[2][currentChildIdx], returnPos)) return movement(us);
    }
    if (155 < turnNum && turnNum <= 180) {
        if (currentChildIdx == 1) {
            Move move = buildSnowman(field, currentChildIdx, ourTeam, returnPos);
            if (move != Move::IDLE) return move;
        }
        if (moveToTarget(field, us, CHILD_POST[3][currentChildIdx], returnPos)) return movement(us);
    }

    Move move = stockpileSnowballs(field, ourTeam, us, returnPos);
    if (move != Move::IDLE) return move;

    returnPos = targetPos;
    return Move::CATCH;
}
//
// Created by Stanley Zhong on 6/24/23.
//


#include "Move.h"
#include "util.h"
#include "action.h"
#include "movement.h"


enum SnowmanStage { NONE, BASE, BASE_AND_BODY };


SnowmanStage nearbySnowmanStage(const vector<vector<Position>>& field, const Child& us, complex<int>& snowmanPos) {
    if (findAdjacent(field, us, GROUND_LM, snowmanPos) > 0) return BASE_AND_BODY;
    if (findAdjacent(field, us, GROUND_L, snowmanPos) > 0) return BASE;
    snowmanPos = complex<int>(us.x + 1, us.y + 1);
    return NONE;
}

Move buildPart(vector<vector<Position>>& field, const Child& us, const complex<int>& snowmanPos, int sizeToDrop, int sizeToCrush, complex<int>& returnPos) {
    if (us.standing) return Move::CROUCH;
    if (us.holding == sizeToDrop) {
        returnPos = snowmanPos;
        return Move::DROP;
    } else if (us.holding == sizeToCrush) {
        return Move::CRUSH;
    } else if (us.holding == HOLD_S1 || us.holding == HOLD_S2 || us.holding == HOLD_S3 || us.holding == HOLD_M || us.holding == HOLD_L || (us.holding == HOLD_P3 && (sizeToCrush == HOLD_P2 || sizeToCrush == HOLD_P1)) || (us.holding == HOLD_P2 && sizeToCrush == HOLD_P1)) {
        if (dropItem(field, us, snowmanPos, returnPos)) return Move::DROP;
    } else {
        if (pickUpSnow(field, us, GROUND_EMPTY, returnPos)) return Move::PICKUP;
    }
    if (moveRandomly(field, us, returnPos)) return Move::CRAWL;
    return Move::IDLE;
}

Move buildSnowman(vector<vector<Position>>& field, const Child& us, const vector<Child>& ourTeam, complex<int>& returnPos) {
    int dist;
    int teammateIdx = nearestTeammate(us, ourTeam, dist);
    if (dist <= 2) {
        complex<int> teammatePos(ourTeam[teammateIdx].x, ourTeam[teammateIdx].y);
        if (moveAwayFrom(field, us, teammatePos, returnPos)) return us.standing ? Move::RUN : Move::CRAWL;
    }

    complex<int> snowmanPos;
    const SnowmanStage stage = nearbySnowmanStage(field, us, snowmanPos);
    switch(stage) {
        case NONE:
            return buildPart(field, us, snowmanPos, HOLD_L, HOLD_P3, returnPos);
        case BASE:
            return buildPart(field, us, snowmanPos, HOLD_M, HOLD_P2, returnPos);
        case BASE_AND_BODY:
            // technically HOLD_S2 and HOLD_S3 work too for sizeToDrop, but it's unlikely those will come up
            return buildPart(field, us, snowmanPos, HOLD_S1, HOLD_P1, returnPos);
    }
    if (moveRandomly(field, us, returnPos)) return us.standing ? Move::RUN : Move::CRAWL;
    return Move::IDLE;
}
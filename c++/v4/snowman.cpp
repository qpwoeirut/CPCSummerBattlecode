//
// Created by Stanley Zhong on 6/24/23.
//


#include "Move.h"
#include "util.h"
#include "action.h"
#include "movement.h"


const vector<complex<int>> SNOWMAN_POSITIONS[] = {
        vector<complex<int>>{
                complex<int>(6, 14),
                complex<int>(6, 23),
        },
        vector<complex<int>>{
                complex<int>(5, 5),
                complex<int>(17, 24)
        },
        vector<complex<int>>{
                complex<int>(9, 9),
                complex<int>(24, 17)
        },
        vector<complex<int>>{
                complex<int>(14, 6),
                complex<int>(23, 6)
        }
};


enum SnowmanStage { NONE, BASE, BASE_AND_BODY, BUILT };


SnowmanStage nearbySnowmanStage(const vector<vector<Position>>& field, const Child& us, complex<int>& snowmanPos) {
    if (findAdjacent(field, us, GROUND_LM, snowmanPos) > 0) return BASE_AND_BODY;
    if (findAdjacent(field, us, GROUND_L, snowmanPos) > 0) return BASE;
    snowmanPos = complex<int>(us.x + 1, us.y + 1);
    return NONE;
}


SnowmanStage snowmanStageAt(const vector<vector<Position>>& field, const complex<int>& snowmanPos) {
    switch(field[snowmanPos.real()][snowmanPos.imag()].ground) {
        case GROUND_SMR: return BUILT;
        case GROUND_LM: return BASE_AND_BODY;
        case GROUND_L: return BASE;
        default: return NONE;
    }
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

Move buildSnowman(vector<vector<Position>>& field, int currentChildIdx, const vector<Child>& ourTeam, complex<int>& returnPos) {
    const Child& us = ourTeam[currentChildIdx];

    double dist;
    int positionIdx = nearestPosition(field, us, SNOWMAN_POSITIONS[currentChildIdx], GROUND_SMR, dist);
    if (positionIdx == -1) {
        cerr << "ERROR: unable to find snowman position for child " << currentChildIdx << endl;
        return Move::IDLE;
    }
    // find nearestAvailable in case there's a tree in the way
    complex<int> snowmanPos = nearestAvailable(field, SNOWMAN_POSITIONS[currentChildIdx][positionIdx]);
    if (dist >= 1.42) {  // adjacent positions are all sqrt(2) or closer
        if (!us.standing && dist >= 6) return Move::STAND;
        if (moveToTarget(field, us, snowmanPos, returnPos)) return movement(us);
    }

    const SnowmanStage stage = snowmanStageAt(field, snowmanPos);
    switch(stage) {
        case NONE:
            return buildPart(field, us, snowmanPos, HOLD_L, HOLD_P3, returnPos);
        case BASE:
            return buildPart(field, us, snowmanPos, HOLD_M, HOLD_P2, returnPos);
        case BASE_AND_BODY:
            // technically HOLD_S2 and HOLD_S3 work too for sizeToDrop, but it's unlikely those will come up
            return buildPart(field, us, snowmanPos, HOLD_S1, HOLD_P1, returnPos);
        case BUILT:
            cerr << "ERROR: found a built snowman for child " << currentChildIdx << " at " << snowmanPos.real() << ',' << snowmanPos.imag() << endl;
            break;
    }
    if (moveRandomly(field, us, returnPos)) return movement(us);
    return Move::IDLE;
}
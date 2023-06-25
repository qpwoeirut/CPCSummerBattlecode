//
// Created by Stanley Zhong on 6/23/23.
//

#include "Const.h"
#include "util.h"
#include <algorithm>
#include <vector>
#include <complex>

using namespace std;


const int ATTACK_RANGE = 24;
const double EPS = 1e-12;

bool willHitTarget(const vector <vector<Position>>& field, const Child& us, const Child& them, int x, int y) {
    int start_height = us.standing ? 9 : 6;

    complex<double> start(us.x, us.y);
    complex<double> target(x, y);
    if (abs(start - target) > ATTACK_RANGE + EPS) return false;

    const int n = max(abs(x - us.x), abs(y - us.y));
    for (int step = 1; step <= n; step++) {  // seems that C++ round already rounds away from 0
        int curX = us.x + round((double)(step * (x - us.x)) / n);
        int curY = us.y + round((double)(step * (y - us.y)) / n);
        int curH = start_height - round((double)(9 * step) / n);

        if (inBounds(curX, curY) && (curH <= field[curX][curY].height || field[curX][curY].ground == GROUND_TREE)) {
            return false;
        }
        if (curX == them.x && curY == them.y) {
            // TODO: if a child crouches in snow of height 6, will they get hit? this assumes they don't
            return them.standing || curH <= 6;
        }
        if (inBounds(curX, curY) && field[curX][curY].childTeam != -1) {  // avoid hitting a child other than the target
            return false;
        }
    }
    return false;
}

bool willHitTarget(const vector <vector<Position>>& field, const Child& us, const complex<int>& toAttack, const int height, int x, int y) {
    int start_height = us.standing ? 9 : 6;

    complex<double> start(us.x, us.y);
    complex<double> target(x, y);
    if (abs(start - target) > ATTACK_RANGE + EPS) return false;

    const int n = max(abs(x - us.x), abs(y - us.y));
    for (int step = 1; step <= n; step++) {  // seems that C++ round already rounds away from 0
        int curX = us.x + round((double)(step * (x - us.x)) / n);
        int curY = us.y + round((double)(step * (y - us.y)) / n);
        int curH = start_height - round((double)(9 * step) / n);

        if (curX == toAttack.real() && curY == toAttack.imag()) {
            return curH == height;
        }
        if (inBounds(curX, curY) && (curH <= field[curX][curY].height || field[curX][curY].ground == GROUND_TREE)) {
            return false;
        }
        if (inBounds(curX, curY) && field[curX][curY].childTeam != -1) {  // avoid hitting a child
            return false;
        }
    }
    return false;
}

// if we want to attack "them", what location should we throw the snowball at?
// tries all reachable locations because reversing the snowball path is annoying and runs into edge cases
complex<int> targetToAttack(const vector <vector<Position>>& field, const Child& us, const Child& them) {
    double bestScore = 0;
    complex<int> target(INIT, INIT);  // can't use -1 since (-1, -1) is a valid target
    for (int x = -ATTACK_RANGE; x < SIZE + ATTACK_RANGE; x++) {
        for (int y = -ATTACK_RANGE; y < SIZE + ATTACK_RANGE; y++) {
            if (willHitTarget(field, us, them, x, y)) {
                double score = abs(complex<double>(us.x - x, us.y - y));  // further distance -> faster throw
                if (bestScore < score) {
                    bestScore = score;
                    target = complex<int>(x, y);
                }
            }
        }
    }
    return target;
}

complex<int> targetToAttack(const vector <vector<Position>>& field, const Child& us, const complex<int>& toAttack, const int height) {
    double bestScore = 0;
    complex<int> target(INIT, INIT);  // can't use -1 since (-1, -1) is a valid target
    for (int x = -ATTACK_RANGE; x < SIZE + ATTACK_RANGE; x++) {
        for (int y = -ATTACK_RANGE; y < SIZE + ATTACK_RANGE; y++) {
            if (willHitTarget(field, us, toAttack, height, x, y)) {
                double score = abs(complex<double>(us.x - x, us.y - y));  // further distance -> faster throw
                if (bestScore < score) {
                    bestScore = score;
                    target = complex<int>(x, y);
                }
            }
        }
    }
    return target;
}

// returns how easy it is to attack this target
// score is based on how close by the target is, how high the target is, and obstacles in the way
// uses stance (stand/crouch) provided in the "us" Child
int attackability(const vector<vector<Position>>& field, const Child& us, int themIdx, const vector<Child>& theirTeam, const vector<pair<int, complex<int>>>& theirLastPosition) {
    if (theirTeam[themIdx].x == UNKNOWN || theirTeam[themIdx].y == UNKNOWN) return 0;

    complex<int> target = targetToAttack(field, us, theirTeam[themIdx]);
    if (target.real() == INIT || target.imag() == INIT) return 0;

    complex<int> usPos(us.x, us.y), themPos(theirTeam[themIdx].x, theirTeam[themIdx].y);

    int distScore = round(1000 / (abs(usPos - themPos) + 1));  // prioritize closer targets
    int dazedScore = theirTeam[themIdx].dazed > 0 ? 100 : 0;  // dazed players can't dodge
    int decayScore = (theirLastPosition[themIdx].first * theirLastPosition[themIdx].first) / 4;

    // spread out shots to maximize time opponents are dazed
    if (theirTeam[themIdx].targetClaimed) {
        dazedScore = 0;
        distScore /= 4;
    }

    int score = distScore + dazedScore - decayScore;
    return score;
}

double threatScore(const vector<Child>& ourTeam, const vector<pair<int, complex<int>>>& theirLastPosition, complex<int>& targetPos) {
    double maxScore = 0;
    for (const Child& us: ourTeam) {
        const complex<int> usPos(us.x, us.y);
        for (int i = 0; i < theirLastPosition.size(); i++) {
            double dist = abs(theirLastPosition[i].second - usPos) + 1;
            double score = (1000 / dist) - (theirLastPosition[i].first * theirLastPosition[i].first) / 4;
            if (maxScore < score) {
                maxScore = score;
                targetPos = theirLastPosition[i].second;
            }
        }
    }
    return maxScore;
}

bool attack(const vector<vector<Position>>& field, const Child& us, vector<Child>& theirTeam, const vector<pair<int, complex<int>>>& theirLastPosition, complex<int>& returnPos) {
    if (canAttack(us)) {
        vector<int> attackScore(theirTeam.size());
        for (int i = 0; i < theirTeam.size(); i++) {
            attackScore[i] = attackability(field, us, i, theirTeam, theirLastPosition);
        }
        int attackIdx = max_element(attackScore.begin(), attackScore.end()) - attackScore.begin();
        if (attackScore[attackIdx] > 0) {  // attack!
            returnPos = targetToAttack(field, us, theirTeam[attackIdx]);
            theirTeam[attackIdx].targetClaimed = true;
            return true;
        }
    }
    return false;
}

bool attackSnowman(const vector<vector<Position>>& field, const Child& us, complex<int>& returnPos) {
    if (canAttack(us)) {
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                if (field[x][y].ground == GROUND_SMB) {
                    complex<int> target = targetToAttack(field, us, complex<int>(x, y), field[x][y].height);
                    if (target.real() != INIT && target.imag() != INIT) {
                        returnPos = target;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
//
// Created by Stanley Zhong on 6/23/23.
//

#include "Const.h"
#include "util.h"
#include <vector>
#include <complex>

using namespace std;


const int ATTACK_RANGE = 24;
const double EPS = 1e-12;
const int INIT = 1001001001;  // can't use -1 since (-1, -1) is a valid target

bool willHitTarget(const vector <vector<Position>>& field, const Child& us, const Child& them, int x, int y) {
    int start_height = us.standing ? 9 : 6;

    complex<double> start(us.x, us.y);
    complex<double> target(x, y);
    if (abs(start - target) > 24 + EPS) return false;

    const int n = max(abs(x - us.x), abs(y - us.y));
    for (int step = 1; step <= n; step++) {  // seems that C++ round already rounds away from 0
        int curX = us.x + round((double)(step * (x - us.x)) / n);
        int curY = us.y + round((double)(step * (y - us.y)) / n);
        int curH = start_height - round((double)(9 * step) / n);

        if (inBounds(curX, curY) && (curH <= field[curX][curY].height || field[curX][curY].ground == GROUND_TREE)) return false;
        if (curX == them.x && curY == them.y) {
            // TODO: if a child crouches in snow of height 6, will they get hit? this assumes they don't
            return them.standing || curH <= 6;
        }
        // TODO does friendly fire exist? or do the snowballs pass through your teammates? this assumes they do
    }
    return false;
}

// if we want to attack "them", what location should we throw the snowball at?
// tries all reachable locations because reversing the snowball path is annoying and runs into edge cases
complex<int> targetToAttack(const vector <vector<Position>>& field, const Child& us, const Child& them) {
    cerr << "targetToAttack: " << us.x << ',' << us.y << ' ' << them.x << ',' << them.y << endl;
    for (int x = -ATTACK_RANGE; x < SIZE + ATTACK_RANGE; x++) {
        for (int y = -ATTACK_RANGE; y < SIZE + ATTACK_RANGE; y++) {
            if (willHitTarget(field, us, them, x, y)) {
                return complex<int>(x, y);
            }
        }
    }
    return complex<int>(INIT, INIT);
}

// returns how easy it is to attack this target, on a scale from 0 (impossible) to 10 (very easy)
// score is based on how close by the target is, how high the target is, and obstacles in the way
// uses stance (stand/crouch) provided in the "us" Child
int attackability(const vector <vector<Position>>& field, const Child& us, const Child& them) {
    if (them.x == UNKNOWN || them.y == UNKNOWN) return false;

    complex<int> target = targetToAttack(field, us, them);
    if (target.real() == INIT || target.imag() == INIT) return 0;

    complex<int> usPos(us.x, us.y), themPos(them.x, them.y);

    int distanceScore = round(20 / (abs(usPos - themPos) + 1));  // prioritize closer targets
    int dazedBonus = (them.dazed + 1) / 2;
    return distanceScore + dazedBonus;
}

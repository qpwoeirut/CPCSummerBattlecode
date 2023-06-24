//
// Created by Stanley Zhong on 6/23/23.
//

#include <iostream>
#include <cassert>
#include <string>
#include <complex>

#ifndef MOVE_H
#define MOVE_H

enum Move { IDLE, CROUCH, STAND, RUN, CRAWL, PICKUP, DROP, CRUSH, THROW, CATCH };

std::string move_to_str(const Move move) {
    switch(move) {
        case IDLE: return "idle";
        case CROUCH: return "crouch";
        case STAND: return "stand";
        case RUN: return "run";
        case CRAWL: return "crawl";
        case PICKUP: return "pickup";
        case DROP: return "drop";
        case CRUSH: return "crush";
        case THROW: return "throw";
        case CATCH: return "catch";
    }
    throw "Enum did not match any options; this should never happen";
}

Move movement(const Child& us) {
    return us.standing ? Move::RUN : Move::CRAWL;
}

void print_move(const Move move) {
    assert(move == IDLE || move == CROUCH || move == STAND || move == CRUSH);
    std::cout << move_to_str(move) << std::endl;
}
void print_move(const Move move, const std::complex<int>& pos) {
    if (move == IDLE || move == CROUCH || move == STAND || move == CRUSH) {
        print_move(move);
    } else {
        std::cout << move_to_str(move) << ' ' << pos.real() << ' ' << pos.imag() << std::endl;
    }
}

#endif //MOVE_H

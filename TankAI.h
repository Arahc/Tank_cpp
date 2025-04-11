/*
 * @brief enemy tank strategy
 * @file TankAI.h
 * Decide how to move for the enemy tanks
 */

#pragma once
#include "Math.h"
#include "_Object.h"

bool randTankMove(Tank &tk) {
    // ! return true if the tank actually willing to move
    Vector dir = randDir4(1);
    if (dir == _vecZERO)
        return false;
    tk.dir = dir;
    return true;
}

bool randTankAttack(const Tank &tk, int numer = 1, int denom = 2) {
    // ! return true if the tank actually willing to attack
    if (randProb(numer, denom)) {
        tankAttack(tk);
        return true;
    }
    return false;
}

bool littelCleverTankMove(Tank &tk, const Vector &tar) {
    // ! return true if the tank actually willing to move
    // tar: target
    // 10% not to move, 90% move
    // 33.33% to move to the target in a direct direction, 66.67% to move randomly
    if (randProb(1, 10))
        return false;
    if (randProb(1, 3))
        return randTankMove(tk);
    Vector dir = tar - tk.pos;
    if (abs(dir.x) <= 1 && !(abs(dir.y) <= 1))
        dir.x = 0;
    if (abs(dir.y) <= 1 && !(abs(dir.x) <= 1))
        dir.y = 0;
    dir.x = sign(dir.x);
    dir.y = sign(dir.y);
    // ! dir != (0, 0), if tank move failed, assert this
    if (dir.x != 0 && dir.y != 0) {
        if (randProb(1, 2))
            dir.x = 0;
        else
            dir.y = 0;
    }
    tk.dir = dir;
    return true;
}

bool littleCleverTankAttack(const Tank &tk, const Vector &tar) {
    // ! return true if the tank actually willing to attack
    // tar: target
    // If the target can be attacked, 100% to attack
    // Otherwise, 30% to attack, 70% not
    Vector dir = tar - tk.pos;
    if (abs(dir.x) <= 1 && !(abs(dir.y) <= 1))
        dir.x = 0;
    if (abs(dir.y) <= 1 && !(abs(dir.x) <= 1))
        dir.y = 0;
    dir.x = sign(dir.x);
    dir.y = sign(dir.y);
    if (dir == tk.dir){
        tankAttack(tk);
        return true;
    }
    else
        return randTankAttack(tk, 3, 10);
}
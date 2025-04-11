/*
 * @brief game data for tanks
 * @file _Data.h
 * The tank atkCD, moveCD, HP, ATK are all set in the config file
 */

#pragma once
#include "Memory.h"
#include "_Config.h"
#include "_Object.h"

class Data : memRegist {
  public:
    bool isPlayer;              // true: player, false: enemy
    int atkCD, moveCD, HP, ATK; // CD will not change (data), Cnt will change (calculate if CD done)
    Data(bool _isPlayer, int _atkCD, int _moveCD, int _HP, int _ATK) {
        isPlayer = _isPlayer;
        atkCD = _atkCD;
        moveCD = _moveCD;
        HP = _HP;
        ATK = _ATK;
    }
    Data(Tank &tk) {
        isPlayer = tk.isPlayer;
        atkCD = tk.atkCD;
        moveCD = tk.moveCD;
        HP = tk.HP;
        ATK = tk.ATK;
    }
    ~Data() {}
};

static memList<Data> LIST_DATA;
extern memList<Tank> LIST_TANK;

void initData() {
    LIST_DATA.clear();
    LIST_DATA.emplace(1, config.atkCD[1], config.moveCD[1], config.HP[1], config.ATK[1]);
    for (int i = 0; i < config.nEnemy; ++i)
        LIST_DATA.emplace(0, config.atkCD[0], config.moveCD[0], config.HP[0], config.ATK[0]);
}
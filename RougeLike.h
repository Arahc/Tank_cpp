/*
 * @brief rouge-like level update (enhance both player and enemy)
 * @file RougeLike.h
 * Use BUFFs to enhance the player and enemy tanks
 * When completing a level, choose 1 pair of BUFFs in some pairs
 * A pair of BUFFs is a player's buff and a enemy's buff
 * The higher the level, the more extreme the buff (both player and enemy) is
 ? Will `r` reset the level count ? Maybe I will not do this, since the random map may cause suffering starting point
 ! A good way is: use `r` at the beginning will not. Use `r` when the game is already start will do
 */

///
#pragma once
#include "Print.h"
#include "SysPort.h"
#include "_Data.h"

struct Buff {
    enum class Type { buffSPEED = 0, buffATKCD = 1, buffHP = 2, buffATK = 3 };
    Type type;
    int val; // the value of the buff (e.g. type=buffSPEED, val=2 -> speed+=2)
    // Let val may be 0, it will be abstract, but fun
    // ! when val = -1, it means that the value is random for the tanks, respectively
    Buff(Type _type = Type::buffSPEED, int _val = 0) : type(_type), val(_val) {}
    ~Buff() {}
};

#define BTP Buff::Type

Buff randBuff() {
    Buff buf;
    buf.type = static_cast<BTP>(randIntEx(0, 3, -1)); // HP should be a rare buff, ATK shoule be more rare
    if (randProb(1, 3))
        buf.val = -1;
    else {
        if (buf.type == BTP::buffATK)
            buf.val = randInt(0, 1);
        else if (buf.type == BTP::buffHP)
            buf.val = randInt(0, 2);
        else
            buf.val = randInt(0, 3);
    }
    return buf;
}
Buff randBuffEx(int k) {
    Buff buf;
    buf.type = static_cast<BTP>(randIntEx(0, 3, -1));
    if (randProb(1, 3))
        buf.val = -1;
    else {
        if (buf.type == BTP::buffATK)
            buf.val = randIntEx(0, 1, k);
        else if (buf.type == BTP::buffHP)
            buf.val = randIntEx(0, 2, k);
        else
            buf.val = randIntEx(0, 3, k);
    }
    return buf;
}

extern memList<Data> LIST_DATA;

void applyBuff(Buff buf, bool isPlayer) {
    if (buf.val == -1) {
        for (auto &dt : LIST_DATA)
            if (dt.isPlayer == isPlayer) {
                if (buf.type == BTP::buffSPEED)
                    dt.moveCD = max(config.moveCD_lim[isPlayer], dt.moveCD - randInt(0, 3));
                else if (buf.type == BTP::buffATKCD)
                    dt.atkCD = max(config.atkCD_lim[isPlayer], dt.atkCD - randInt(0, 3));
                else if (buf.type == BTP::buffHP)
                    dt.HP = min(config.HP_lim[isPlayer], dt.HP + randInt(0, 1));
                else if (buf.type == BTP::buffATK)
                    dt.ATK = min(config.ATK_lim[isPlayer], dt.ATK + randInt(0, 1));
            }
        return;
    } else {
        for (auto &dt : LIST_DATA)
            if (dt.isPlayer == isPlayer) {
                if (buf.type == BTP::buffSPEED)
                    dt.moveCD = max(config.moveCD_lim[isPlayer], dt.moveCD - buf.val);
                else if (buf.type == BTP::buffATKCD)
                    dt.atkCD = max(config.atkCD_lim[isPlayer], dt.atkCD - buf.val);
                else if (buf.type == BTP::buffHP)
                    dt.HP = min(config.HP_lim[isPlayer], dt.HP + buf.val);
                else if (buf.type == BTP::buffATK)
                    dt.ATK = min(config.ATK_lim[isPlayer], dt.ATK + buf.val);
            }
    }
}

void printBufInfo(Buff buf) {
    printf("Add ");
    if (buf.val == -1)
        printf("[RANDOM] ");
    else
        printf("%d ", buf.val);
    if (buf.type == BTP::buffSPEED)
        printf("speed");
    else if (buf.type == BTP::buffATKCD)
        printf("attack speed");
    else if (buf.type == BTP::buffHP)
        printf("HP");
    else if (buf.type == BTP::buffATK)
        printf("ATK");
}

#undef BTP

bool buffSelect(int level) {
    // ! return false if the player is willing to force quit
    clearScreen();
    printf("Select a pair of BUFFs, you will apply your BUFF, but enemy will also apply their BUFF.\n\n");
    /* 4 pairs are supposed to be shown
     * To make it more interesting, hide some buff's detailed info
     *  - both player and enemy's buff info will show
     *  - only show info of player's buff
     *  - only show info of enemy's buff
     *  - show nothing
     */
    Buff buf[4][2]; // buf[i][j]: i -> the i-th buf; j = 0/1 -> Player/Enemy
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 2; ++j)
            buf[i][j] = randBuffEx(level / 2 - 3);

    const int midPos = 40;

    printf("     | Your BUFF");
    moveCursorCol(midPos);
    printf("| Enemy BUFF\n");

    printf("-----|---------------------------------|---------------------------------\n");

    // bufA: both player and enemy's buff info will show
    printf("a(1) | ");
    printBufInfo(buf[0][1]);
    moveCursorCol(midPos);
    printf("| ");
    printBufInfo(buf[0][0]);
    printf("\n");

    // bufB: only show info of player's buff
    printf("b(2) | ");
    printBufInfo(buf[1][1]);
    moveCursorCol(midPos);
    printf("| ");
    printf("[HIDDEN BUFF]");
    printf("\n");

    // bufC: only show info of enemy's buff
    printf("c(3) | ");
    printf("[HIDDEN BUFF]");
    moveCursorCol(midPos);
    printf("| ");
    printBufInfo(buf[2][0]);
    printf("\n");

    // bufD: show nothing
    printf("d(4) | ");
    printf("[HIDDEN BUFF]");
    moveCursorCol(midPos);
    printf("| ");
    printf("[HIDDEN BUFF]");
    printf("\n\n");

    printf("Notice that there is a limit of each data (check the _Config.h code if you really want to).\n   -> "
           "Balance "
           "the buffs is important.\n\n");

    printf("Press 'a'(or `1`),'b'(or `2`),'c'(or `3`),'d'(or `4`) to choose one. 'q' or 'Esc' to quit.\n");

    while (1) {
        if (kbhit()) {
            int ch = getch();
            if (ch >= 'A' && ch <= 'Z')
                ch = ch - 'A' + 'a';
            if (ch == 'q' || ch == 27)
                return false;
            if ((ch < 'a' || ch > 'd') && (ch < '1' || ch > '4'))
                continue;
            int tp = (ch >= 'a' && ch <= 'd') ? ch - 'a' : ch - '1';
            applyBuff(buf[tp][0], 0);
            applyBuff(buf[tp][1], 1);
            break;
        }
    }
    return true;
}
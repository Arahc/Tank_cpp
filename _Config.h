/*
 * @brief game config
 * @file _Config.h
 * game config header file
 ! game config should be set before the game starts
 */

#pragma once

struct Config {
    int fps;
    int mapWidth, mapHeight;
    int nEnemy, nSolid, nDirt;

    int atkCD[2], moveCD[2], HP[2], ATK[2]; // 0: enemy; 1: player

    int nEnemy_lim;
    int atkCD_lim[2], moveCD_lim[2], HP_lim[2], ATK_lim[2]; // limit of the data
};

static Config config;

void setConfig() {
    // reset the config to deafualt

    // basic setting
    config.fps = 60;       // FPS
    config.mapWidth = 56;  // map size
    config.mapHeight = 24; // map size
    config.nSolid = 5;     // number of solid, solid is unbreakable wall
    config.nDirt = 6;      // number of dirt, dirt is breakable wall

    // gamerule setting (initial data)
    config.nEnemy = 2;     // number of enemy tanks
    config.atkCD[0] = 25;  // enemy attack CD. CD = the frame number between two operation
    config.atkCD[1] = 25;  // player attack CD.
    config.moveCD[0] = 20; // enemy move CD.
    config.moveCD[1] = 20; // player move CD.
    config.HP[0] = 1;      // enemy HP, when HP <= 0, die. HP will show on the center of the tank, as a base36 number.
    config.HP[1] = 2;      // player HP
    config.ATK[0] = 1;     // enemy attack power. When attacking a tank, the tank's HP -= the attacker's ATK.
    config.ATK[1] = 1;     // player attack power

    // game data limit
    config.nEnemy_lim = 15;
    config.atkCD_lim[0] = 2;
    config.atkCD_lim[1] = 2;
    config.moveCD_lim[0] = 2;
    config.moveCD_lim[1] = 1;
    config.HP_lim[0] = 20;
    config.HP_lim[1] = 35;
    config.ATK_lim[0] = 15;
    config.ATK_lim[1] = 15;
}

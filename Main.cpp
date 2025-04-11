/*
 * @brief main = game prelude
 * @file Main.cpp
 * set the config
 */

#include "Game.h"
#include "_Config.h"

int main(/* int argc, char *argv[] */) {
    srand(time(NULL));
    setConfig();
    bufferInit(config.mapHeight, config.mapWidth);
    hideCursor();
    levelInit(1);
    gameRun();
    return 0;
}

/*
 * @brief main controller
 * @file Game.h
 * main controller for the project
 */

#pragma once
#include "Print.h"
#include "RougeLike.h"
#include "SysPort.h"
#include "TankAI.h"
#include <setjmp.h>

// game initiallize and support functions

extern memList<Tank> LIST_TANK;
extern memList<Bullet> LIST_BULLET;
extern memList<Wall> LIST_WALL;
extern memList<Data> LIST_DATA;

extern Color colTank[2];
// colTank[1] = colPlayer, colTank[0] = colEnemy

static int gameLevel = 1;
// add 1 each time win a game

static bool haveStarted = false;
// If the game have not started (restart at the beginning when checking the map), don't reset level
// If the game have started (restart when pause), reset the level

void levelInit(bool isLevel1) {
    if (isLevel1) {
        gameLevel = 1;
        colTank[0] = randColorfulCol();
        colTank[1] = randColorfulCol();
        while (colTank[0].isSimilar(colTank[1]))
            colTank[1] = randColorfulCol();
        initData();
    }
    haveStarted = false;
    LIST_TANK.clear();
    LIST_BULLET.clear();
    LIST_WALL.clear();

    // set the tank data
    Vector pos(0, 0); // a tmp position for pos decision

    for (const auto &dt : LIST_DATA) {
        pos = randVec(2, config.mapWidth - 1, 2, config.mapHeight - 1);
        while (!isAreaEmpty(Rect(pos - Vector(1, 1), pos + Vector(1, 1))))
            pos = randVec(2, config.mapWidth - 1, 2, config.mapHeight - 1);
        createTank(pos, _vecUP, dt.isPlayer, dt.atkCD, dt.moveCD, dt.HP, dt.ATK);
    }
    // set the wall data
    for (int i = 0; i < config.nSolid; ++i) {
        pos = randVec(2, config.mapWidth - 1, 2, config.mapHeight - 1);
        while (!isAreaEmpty(Rect(pos - Vector(1, 1), pos + Vector(1, 1))))
            pos = randVec(2, config.mapWidth - 1, 2, config.mapHeight - 1);
        for (int x = -1; x <= 1; ++x)
            for (int y = -1; y <= 1; ++y)
                createWall(pos + Vector(x, y), _colLightGray, 0);
    }
    for (int i = 0; i < config.nDirt; ++i) {
        pos = randVec(2, config.mapWidth - 1, 2, config.mapHeight - 1);
        while (!isAreaEmpty(Rect(pos - Vector(1, 1), pos + Vector(1, 1))))
            pos = randVec(2, config.mapWidth - 1, 2, config.mapHeight - 1);
        for (int x = -1; x <= 1; ++x)
            for (int y = -1; y <= 1; ++y)
                createWall(pos + Vector(x, y), _colDarkGray, 1);
    }

    // init the map
    mapInit();
}

extern Buffer mapBuf;

void ForceQuit() {
    LIST_TANK.clear();
    LIST_BULLET.clear();
    LIST_WALL.clear();
    LIST_DATA.clear();
    clearScreen();
    showCursor();
    exit(0);
}

jmp_buf startGame; // a position direct to gameRun

void gameEnd(bool isWin, bool isForceQuit, bool isForceRestart) {
    /* free the memory
     *  - tanks
     *  - bullets
     *  - walls
     */
    resetColor();
    clearScreen();
    if (isForceQuit)
        ForceQuit();
    if (isForceRestart) {
        levelInit(haveStarted);
        longjmp(startGame, 1);
    }
    puts(isWin ? "Win!" : "Lose...");
    puts("Press `r` or `c` to continue, `q` or `Esc` to quit");
    while (1)
        if (kbhit()) {
            int ch = getch();
            if (ch >= 'A' && ch <= 'Z')
                ch = ch - 'A' + 'a';
            if (ch == 'q' || ch == 27)
                ForceQuit();
            if (ch == 'r' || ch == 'c') {
                if (isWin) {
                    ++gameLevel;
                    if (!buffSelect(gameLevel))
                        ForceQuit();
                    // Every 3 levels, add a new enemy tank
                    if (gameLevel > 1 && gameLevel % 3 == 1 &&
                        (int)LIST_TANK.size() - 1 < config.nEnemy_lim) // size = nEnemy + 1, -1 for player tank
                        LIST_DATA.emplace(0, config.atkCD[0], config.moveCD[0], config.HP[0], config.ATK[0]);
                    levelInit(0);
                } else
                    levelInit(1);
                longjmp(startGame, 1);
            }
        }
}

// gamemode and pausemode set

static bool isPause = false;

void enterPauseMode() {
    isPause = true;
    resetColor();
    clearRow(config.mapHeight + 3);
    clearRow(config.mapHeight + 2);
    printf("`q`,`Esc`-> quit    `r`-> start new    `c`-> continue\n");
    printf("[NOTE] `r` restart the game with a new map, not the map you are playing!\n");
}

void enterGameMode() {
    isPause = false;
    haveStarted = true;
    resetColor();
    clearRow(config.mapHeight + 3);
    clearRow(config.mapHeight + 2);
    printf("`wasd`-> move    `j`-> attack    `:`-> pause    `Esc`-> quit\n");
    printf("[NOTE] `:q` = quit. But not `:wq`, because saving game is not supported :(\n");
}

// support functions

void handleInput(int key) {
    /* when not pause:
     * `WASD` player tank move
     * `J` player tank attack
     * `:` pause
     * `Esc` force quit
     * when pause:
     * `c` continue
     * `r` renew
     * `q`,`Esc` quit
     ! return false if the key is not valid (in CD or not a key above)
     */
    if (!isPause) {
        Tank *tk = nullptr;
        if (key == 'w') {
            for (auto &tk : LIST_TANK)
                if (tk.isPlayer) {
                    if (tk.moveCnt > 0)
                        return;
                    tk.dir = _vecUP;
                    tk.moveCnt = tk.moveCD;
                }
        } else if (key == 's') {
            for (auto &tk : LIST_TANK)
                if (tk.isPlayer) {
                    if (tk.moveCnt > 0)
                        return;
                    tk.dir = _vecDOWN;
                    tk.moveCnt = tk.moveCD;
                }
        } else if (key == 'a') {
            for (auto &tk : LIST_TANK)
                if (tk.isPlayer) {
                    if (tk.moveCnt > 0)
                        return;
                    tk.dir = _vecLEFT;
                    tk.moveCnt = tk.moveCD;
                }
        } else if (key == 'd') {
            for (auto &tk : LIST_TANK)
                if (tk.isPlayer) {
                    if (tk.moveCnt > 0)
                        return;
                    tk.dir = _vecRIGHT;
                    tk.moveCnt = tk.moveCD;
                }
        } else if (key == 'j') {
            for (auto &tk : LIST_TANK)
                if (tk.isPlayer) {
                    if (tk.atkCnt > 0)
                        return;
                    tankAttack(tk);
                    tk.atkCnt = tk.atkCD;
                }
        } else if (key == ':')
            enterPauseMode();
        else if (key == 27) 
            gameEnd(0, 1, 0);
    } else {
        if (key == 'c') 
            enterGameMode();
        else if (key == 'r')
            gameEnd(0, 0, 1);
        else if (key == 'q' || key == 27) 
            gameEnd(0, 1, 0);
    }
}

bool handleBulletHit(const Bullet &bl) {
    // return true if the bullet actually hit sth
    // DONE avoid the bullet of flying out of the map!
    if (bl.pos.x < 1 || bl.pos.x > config.mapWidth || bl.pos.y < 1 || bl.pos.y > config.mapHeight)
        return true;
    for (auto it = LIST_WALL.begin(); it != LIST_WALL.end(); ++it)
        if (it->isCollide(bl)) {
            if (it->breakable) {
                imgDelete(*it);
                LIST_WALL.erase(it);
            }
            return true;
        }
    for (auto it = LIST_TANK.begin(); it != LIST_TANK.end(); ++it)
        if (it->isCollide(bl)) {
            if (it->isPlayer != bl.isPlayer) {
                it->HP -= bl.ATK;
                if (it->HP <= 0) {
                    imgDelete(*it);
                    LIST_TANK.erase(it);
                } else
                    modifyChar(it->pos.y, it->pos.x, (it->HP <= 9 ? '0' + it->HP : 'A' + it->HP - 10),
                               colTank[it->isPlayer]); // modify the HP show on the tank
            }
            return true;
        }
    return false;
}

// main function

void updateGame() {
    /* update the game state
     *  - handle the input
     *  - move everything: bullet, tank (move enemy tank only when enemyDo=true)
     *    - check happennings
     *      - bullet hit wall
     *      - bullet hit tank
     *      - game End
     */
    // refresh the CD
    for (auto &tk : LIST_TANK) {
        if (tk.moveCnt > 0)
            --tk.moveCnt;
        if (tk.atkCnt > 0)
            --tk.atkCnt;
    }

    // handle the input (player do)
    if (kbhit()) {
        int ch = getch();
        if (ch >= 'A' && ch <= 'Z')
            ch = ch - 'A' + 'a';
        handleInput(ch);
    }
    if (isPause)
        return;

    clearMapObjects();

    // enemy do
    // To avoid the tank move too fast, DO NOT move per frame, that is why `enemyDo` is needed
    Vector pos(0, 0);
    for (const auto &tk : LIST_TANK)
        if (tk.isPlayer) {
            pos = tk.pos;
            break;
        }
    for (auto &tk : LIST_TANK)
        if (!tk.isPlayer) {
            if (tk.moveCnt == 0)
                if (littelCleverTankMove(tk, pos))
                    tk.moveCnt = tk.moveCD;
            if (tk.atkCnt == 0)
                if (littleCleverTankAttack(tk, pos))
                    tk.atkCnt = tk.atkCD;
        }

    // move the bullet
    for (auto it = LIST_BULLET.begin(); it != LIST_BULLET.end();) {
        bulletMove(*it);
        bool hit = handleBulletHit(*it);
        if (hit)
            it = LIST_BULLET.erase(it);
        else
            ++it;
    }
    // move tanks
    for (auto &tk : LIST_TANK)
        if (tk.moveCnt == tk.moveCD && canTankMove(tk))
            tankMove(tk);
    // check the game ends
    bool isLose = true, isWin = true;
    for (const auto &tk : LIST_TANK) {
        if (!isLose && !isWin)
            break;
        if (tk.isPlayer)
            isLose = false;
        if (!tk.isPlayer)
            isWin = false;
    }
    if (isLose)
        gameEnd(0, 0, 0);
    if (isWin)
        gameEnd(1, 0, 0);

    drawObjects();
}

void gameRun() {
    setjmp(startGame);
    enterPauseMode();
    double frameTime = 1000.0 / config.fps;
    sysTimer lst, now;
    timerFreqInit(&lst);
    while (1) {
        timerCntGet(&lst);
        updateGame();
        swapBuffer();
        for (timerCntGet(&now); getTime(&lst, &now) * 1000.0 < frameTime - 0.5; timerCntGet(&now))
            Daze();
    }
    ForceQuit(); // ! In theory, this won't run
}

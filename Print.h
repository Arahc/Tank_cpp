/*
 * @brief print to terminal
 * @file Print.h
 * a header file for printing to the terminal
 * the print function is defined here
 * Use double buffer to draw the map
 * Mostly copy from `Renderer.h`
 *   - Set two maps, last and current
       // past, present, future, beyond, eternal [doge]
 *   - Swap the two maps and only redraw the changed cell
 */

#pragma once
#include "Memory.h"
#include "_Color.h"
#include "_Object.h"
#include <stdio.h>

// buffer class.

struct MapCell {
    char c;
    Color col;
    MapCell(char _c = ' ', Color _col = _colWhite) : c(_c), col(_col) {}
    ~MapCell() {}
    bool operator==(const MapCell &cell) const {
        return c == cell.c && col == cell.col;
    }
};

const MapCell _blankCell(' ', _colWhite);

struct Buffer {
    MapCell *lst, *cur;
    int width, height;
    Buffer() {}
    ~Buffer() {
        delete[] lst;
        delete[] cur;
    }
};

static Buffer mapBuf;

// basic command of terminal

void setColor(Color col) {
    printf("\033[38;2;%d;%d;%dm", col.r, col.g, col.b);
}

void setBgColor(Color col) {
    printf("\033[48;2;%d;%d;%dm", col.r, col.g, col.b);
}

void resetColor() {
    printf("\033[0m");
}

void clearScreen() {
    printf("\033[2J\033[1;1f");
}

void hideCursor() {
    printf("\033[?25l");
}

void showCursor() {
    printf("\033[?25h");
}

void moveCursor(int r, int c) {
    printf("\033[%d;%df", r + 1, c + 1);
}

void moveCursorCol(int c) {
    printf("\033[%dG", c);
}

void clearRow(int r) {
    moveCursor(r, 0);
    printf("\033[2K\r");
}

// main function

#define getID(r, c) (r) * mapBuf.width + c

void modifyChar(int r, int c, char ch, Color col) {
    mapBuf.cur[getID(r, c * 2)] = MapCell(ch, col);
}
void modifyChar(int r, int c, const MapCell &cel) {
    mapBuf.cur[getID(r, c * 2)] = cel;
}

void setAreaBlank(Rect area) {
    for (int i = area.LU.y; i <= area.RD.y; ++i)
        for (int j = area.LU.x; j <= area.RD.x; ++j)
            modifyChar(i, j, ' ', _colWhite);
}

void imgDelete(const Object &obj) {
    // delete the image of the object
    // this function will be called when the object is deleted
    // ! this function will not free the object, just delete the image
    setAreaBlank(obj.getHitbox());
}

extern memList<Tank> LIST_TANK;
extern memList<Bullet> LIST_BULLET;
extern memList<Wall> LIST_WALL;

void clearMapObjects() {
    // clear all objects that may move
    // Tank and Bullet
    // Wall and Dirt will not move, no need to clear
    for (const auto &tk : LIST_TANK)
        imgDelete(tk);
    for (const auto &bl : LIST_BULLET)
        imgDelete(bl);
}

static Color colTank[2];
// colTank[1] = colPlayer, colTank[0] = colEnemy;

void drawTank(const Tank &tk) {
    // draw a tank, this will cover the original char
    int r = tk.pos.y, c = tk.pos.x;
    Color col = colTank[tk.isPlayer];
    modifyChar(r, c, (tk.HP <= 9 ? '0' + tk.HP : 'A' + tk.HP - 10), col); // the center shows HP

    const MapCell tankEdge('@', col);

    modifyChar(r - 1, c - 1, tankEdge);
    modifyChar(r - 1, c + 1, tankEdge);
    modifyChar(r + 1, c - 1, tankEdge);
    modifyChar(r + 1, c + 1, tankEdge);

    if (tk.dir == _vecUP) {
        modifyChar(r - 1, c, '|', col);
        modifyChar(r + 1, c, 'X', col);
        modifyChar(r, c - 1, tankEdge);
        modifyChar(r, c + 1, tankEdge);
    } else if (tk.dir == _vecDOWN) {
        modifyChar(r - 1, c, 'X', col);
        modifyChar(r + 1, c, '|', col);
        modifyChar(r, c - 1, tankEdge);
        modifyChar(r, c + 1, tankEdge);
    } else if (tk.dir == _vecLEFT) {
        modifyChar(r - 1, c, tankEdge);
        modifyChar(r + 1, c, tankEdge);
        modifyChar(r, c - 1, '-', col);
        modifyChar(r, c + 1, 'X', col);
    } else if (tk.dir == _vecRIGHT) {
        modifyChar(r - 1, c, tankEdge);
        modifyChar(r + 1, c, tankEdge);
        modifyChar(r, c - 1, 'X', col);
        modifyChar(r, c + 1, '-', col);
    }
}

void drawBullet(const Bullet &bl) {
    modifyChar(bl.pos.y, bl.pos.x, 'o', colTank[bl.isPlayer]);
}

void drawObjects() {
    // draw all objects that has been cleared
    // Tank and Bullet
    for (const auto &tk : LIST_TANK)
        drawTank(tk);
    for (const auto &bl : LIST_BULLET)
        drawBullet(bl);
}

void swapBuffer() {
    for (int i = 0, id = 0; i < mapBuf.height; ++i)
        for (int j = 0; j < mapBuf.width; ++j, ++id) {
            // id = the id of position (i, j);
            if (mapBuf.lst[id] == mapBuf.cur[id])
                continue;
            moveCursor(i, j);
            setColor(mapBuf.cur[id].col);
            putchar(mapBuf.cur[id].c);
            mapBuf.lst[id] = mapBuf.cur[id];
        }
}

// init

void setBufferBlank() {
    int r = mapBuf.height, c = mapBuf.width;
    for (int i = 0, id = 0; i < r; ++i)
        for (int j = 0; j < c; ++j, ++id) {
            // id = the id of position (i, j);
            mapBuf.lst[id].col = mapBuf.cur[id].col = _colWhite;
            mapBuf.lst[id].c = ' ';
            mapBuf.cur[id].c = " %"[((i == 0 || i == r - 1) && !(j & 1)) || j == 0 || j == c - 1];
        }
}

void bufferInit(int r, int c) {
    // ! this function will be called exactly once when the whole game starts
    r = r + 2, c = (c + 1) * 2 + 1;
    mapBuf.width = c;
    mapBuf.height = r;
    mapBuf.lst = new MapCell[r * c];
    mapBuf.cur = new MapCell[r * c];
}

void mapInit() {
    // ! this function will be called each time a level start
    clearScreen();
    setBufferBlank();
    for (const auto &tk : LIST_TANK)
        drawTank(tk);
    for (const auto &wl : LIST_WALL)
        modifyChar(wl.pos.y, wl.pos.x, "%#"[wl.breakable], wl.col);
    swapBuffer();
}
/*
 * @brief color class
 * @file _Color.h
 * color class header file
 */

#pragma once
#include "Math.h"

struct Color {
    int r, g, b;
    Color(int _r = 0, int _g = 0, int _b = 0) : r(_r), g(_g), b(_b) {}
    ~Color() {}
    bool operator==(const Color &col) const {
        return r == col.r && g == col.g && b == col.b;
    }
    bool isSimilar(const Color &col) {
        // ! 24678 is just a little big number. If the function doesn't work, change it to a bigger number.
        return (r - col.r) * (r - col.r) + (g - col.g) * (g - col.g) + (b - col.b) * (b - col.b) < 24678;
    }
};

const Color _colWhite(255, 255, 255);
const Color _colLightGray(192, 192, 192);
const Color _colGray(128, 128, 128);
const Color _colDarkGray(64, 64, 64);
const Color _colBlack(0, 0, 0);
const Color _colRed(255, 0, 0);
const Color _colGreen(0, 255, 0);
const Color _colBlue(0, 0, 255);

bool isColorless(Color &col) {
    return sqr(col.r - col.g) + sqr(col.g - col.b) + sqr(col.b - col.r) < 24678 || col.isSimilar(_colWhite) ||
           col.isSimilar(_colLightGray) || col.isSimilar(_colGray) || col.isSimilar(_colDarkGray) ||
           col.isSimilar(_colBlack);
}

Color randColor() {
    return Color(randInt(0, 255), randInt(0, 255), randInt(0, 255));
}

Color randColorfulCol() {
    Color c = randColor();
    while (isColorless(c))
        c = randColor();
    return c;
}

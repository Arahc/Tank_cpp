/*
 * @brief simple math calculations
 * @file Math.h
 * random
 * vector class
 !   createVector() create a vector ==pointer==
 !   makeVector() create a vector ==object==
 */

#pragma once
#include <stdlib.h>

// basic math func

#define sign(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))
#define abs(x) ((x) > 0 ? (x) : -(x))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))
#define sqr(x) ((x) * (x))

// random

int randInt(int l, int r) {
    return rand() % (r - l + 1) + l;
}

int randIntEx(int l, int r, int K) {
    /* A random number in [l,r]
     * For possitive K, The larger K is, the more likely the number is to be close to r.
     * For negative K, The smaller K is, the more likely the number is to be close to l.
     ! The time complexity is O(K). Do not use super big K.
     */
    if (K > 0)
        return randIntEx(randInt(l, r), r, K - 1);
    if (K < 0)
        return randIntEx(l, randInt(l, r), K + 1);
    return randInt(l, r);
}

bool randProb(int n,int d){ // numerator, denominator
    // ! n/d probability to return true
    return randInt(1, d) <= n;
}

// Vector class

struct Vector {
    int x, y;
    Vector(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    ~Vector() {}
    Vector operator+(const Vector &b) const {
        return Vector(x + b.x, y + b.y);
    }
    Vector operator-(const Vector &b) const {
        return Vector(x - b.x, y - b.y);
    }
    Vector operator-() const {
        return Vector(-x, -y);
    }
    int operator*(const Vector &b) const {
        return x * b.x + y * b.y;
    }
    Vector operator^(const Vector &b) const {
        return Vector(x * b.y - y * b.x, y * b.x - x * b.y);
    }
    Vector operator*(int k) const {
        return Vector(x * k, y * k);
    }
    Vector operator/(int k) const {
        return Vector(x / k, y / k);
    }
    bool operator==(const Vector &b) const {
        return x == b.x && y == b.y;
    }
    bool operator!=(const Vector &b) const {
        return x != b.x || y != b.y;
    }
    Vector operator+=(const Vector &b) {
        x += b.x;
        y += b.y;
        return *this;
    }
    Vector operator-=(const Vector &b) {
        x -= b.x;
        y -= b.y;
        return *this;
    }
    Vector operator*=(int k) {
        x *= k;
        y *= k;
        return *this;
    }
    Vector operator^=(const Vector &b) {
        int tmp = x;
        x = x * b.y - y * b.x;
        y = y * b.x - tmp * b.y;
        return *this;
    }
    Vector operator/=(int k) {
        x /= k;
        y /= k;
        return *this;
    }
};

// constants

const Vector _vecZERO(0, 0);
const Vector _vecUP(0, -1);
const Vector _vecDOWN(0, 1);
const Vector _vecLEFT(-1, 0);
const Vector _vecRIGHT(1, 0);
const Vector _vecLEFTUP(-1, -1);
const Vector _vecLEFTDOWN(-1, 1);
const Vector _vecRIGHTUP(1, -1);
const Vector _vecRIGHTDOWN(1, 1);

const Vector _invalidPos(-1, -1);

Vector randVec(int l1, int r1, int l2, int r2) {
    return Vector(randInt(l1, r1), randInt(l2, r2));
}

Vector randDir4(bool canStill) {
    int tp = randInt(1 - canStill, 4);
    if (tp == 1)
        return _vecUP;
    if (tp == 2)
        return _vecDOWN;
    if (tp == 3)
        return _vecLEFT;
    if (tp == 4)
        return _vecRIGHT;
    return _vecZERO;
}

Vector randDir8(bool canStill) {
    int tp = randInt(1 - canStill, 8);
    if (tp == 1)
        return _vecUP;
    if (tp == 2)
        return _vecDOWN;
    if (tp == 3)
        return _vecLEFT;
    if (tp == 4)
        return _vecRIGHT;
    if (tp == 5)
        return _vecLEFTUP;
    if (tp == 6)
        return _vecLEFTDOWN;
    if (tp == 7)
        return _vecRIGHTUP;
    if (tp == 8)
        return _vecRIGHTDOWN;
    return _vecZERO;
}

// Rectangle class

struct Rect {
    Vector LU, RD; // left-top, right-bottom
    Rect(Vector _LU, Vector _RD) : LU(_LU), RD(_RD) {}
    Rect(int _Lux, int _Luy, int _Rdx, int _Rdy) : LU(_Lux, _Luy), RD(_Rdx, _Rdy) {}
    ~Rect() {}

    bool isInclude(Vector pos) {
        return pos.x >= LU.x && pos.x <= RD.x && pos.y >= LU.y && pos.y <= RD.y;
    }
    bool isInclude(Rect rec) {
        return LU.x <= rec.LU.x && RD.x >= rec.RD.x && LU.y <= rec.LU.y && RD.y >= rec.RD.y;
    }
    bool isIntersect(Rect rec) {
        return LU.x <= rec.RD.x && RD.x >= rec.LU.x && LU.y <= rec.RD.y && RD.y >= rec.LU.y;
    }
};

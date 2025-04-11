/*
 * @brief tank, bullet, wall, dirt -> the objects
 * @file _Object.h
 * tank class header file
 * bullet class header file
 * wall class header file
 */

#pragma once
#include "Math.h"
#include "Memory.h"
#include "_Config.h"

class Object : memRegist {
  public:
    enum class Type { objTANK, objBULLET, objWALL };
    Type type;
    virtual ~Object() = default;
    virtual Rect getHitbox() const = 0;
    bool isCollide(Rect rec) {
        return getHitbox().isIntersect(rec);
    }
    bool isCollide(const Object &obj) {
        return getHitbox().isIntersect(obj.getHitbox());
    }
};

class Tank : public Object {
  public:
    Vector pos, dir;
    bool isPlayer;
    int atkCD, moveCD, atkCnt, moveCnt; // CD will not change (data), Cnt will change (calculate if CD done)
    int HP, ATK;
    Tank() {
        type = Type::objTANK;
    }
    Rect getHitbox() const override {
        return Rect(pos - Vector(1, 1), pos + Vector(1, 1));
    }
};

class Bullet : public Object {
  public:
    Vector pos, dir;
    bool isPlayer;
    int ATK;
    Bullet() {
        type = Type::objBULLET;
    }
    Rect getHitbox() const override {
        return Rect(pos, pos);
    }
};

class Wall : public Object {
  public:
    Vector pos;
    Color col;
    bool breakable; // ! Solid: false, Dirt: true
    Wall() {
        type = Type::objWALL;
    }
    Rect getHitbox() const override {
        return Rect(pos, pos);
    }
};

class PlaceHolder : public Object { // only a place holder for collision detection
  public:
    Vector pos;
    PlaceHolder() {
        type = Type::objWALL;
    }
    Rect getHitbox() const override {
        return Rect(pos, pos);
    }
};

// memory control

static memList<Tank> LIST_TANK;
static memList<Bullet> LIST_BULLET;
static memList<Wall> LIST_WALL;

Tank *createTank(Vector pos, Vector dir, bool isPlayer, int atkCD, int moveCD, int HP, int ATK) {
    Tank *tk = LIST_TANK.emplace();
    tk->pos = pos;
    tk->dir = dir;
    tk->isPlayer = isPlayer;
    tk->atkCD = atkCD;
    tk->moveCD = moveCD;
    tk->atkCnt = tk->moveCnt = 0;
    tk->HP = HP;
    tk->ATK = ATK;
    return tk;
}

void freeTank(Tank *tk) {
    LIST_TANK.memDelete(tk);
}

Bullet *createBullet(Vector pos, Vector dir, bool isPlayer, int ATK) {
    Bullet *bl = LIST_BULLET.emplace();
    bl->pos = pos;
    bl->dir = dir;
    bl->isPlayer = isPlayer;
    bl->ATK = ATK;
    return bl;
}

void freeBullet(Bullet *bl) {
    LIST_BULLET.memDelete(bl);
}

Wall *createWall(Vector pos, Color col, bool breakable) {
    Wall *wl = LIST_WALL.emplace();
    wl->pos = pos;
    wl->col = col;
    wl->breakable = breakable;
    return wl;
}

void freeWall(Wall *wl) {
    LIST_WALL.memDelete(wl);
}

// tank operation

void tankMove(Tank &tk) {
    tk.pos += tk.dir;
}

void tankTurn(Tank &tk, Vector dir) {
    tk.dir = dir;
}

void bulletMove(Bullet &bl) {
    bl.pos += bl.dir;
}

Bullet *tankAttack(const Tank &tk) {
    // ! remeber to check the attack CD at first
    // ! the bullet will be created immediately at the gun of the tank, not the center or the front.
    return createBullet(tk.pos + tk.dir, tk.dir, tk.isPlayer, tk.ATK);
}

bool isAreaEmpty(const Rect area) {
    for(auto &tk: LIST_TANK)
        if(tk.isCollide(area))
            return false;
    for(auto &bl: LIST_BULLET)
        if(bl.isCollide(area))
            return false;
    for(auto &wl: LIST_WALL)
        if(wl.isCollide(area))
            return false;
    return true;
}

bool canTankMove(const Tank &Tk) {
    // ! remember to check fly out of the map
    Rect area = Rect(Tk.pos + Tk.dir - Vector(1, 1), Tk.pos + Tk.dir + Vector(1, 1));
    if(area.LU.x < 1 || area.RD.x > config.mapWidth || area.LU.y < 1 || area.RD.y > config.mapHeight)
        return false;
    for(auto &tk: LIST_TANK)
        if(tk.isCollide(area) && tk.pos != Tk.pos) // avoid collision with itself
            return false;
    for(auto &bl: LIST_BULLET)
        if(bl.isCollide(area))
            return false;
    for(auto &wl: LIST_WALL)
        if(wl.isCollide(area))
            return false;
    return true;
}

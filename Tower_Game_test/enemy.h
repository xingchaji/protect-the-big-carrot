#pragma once

#include <graphics.h>
#include <vector>
#include <cstddef>

class Operator;

class Enemy
{
protected:
    float x;
    float y;
    float speed;

    int width;
    int height;

    int hp;
    int maxHp;

    int attackDamage;
    DWORD attackInterval;
    DWORD attackTimer;

    std::vector<POINT> path;
    std::size_t targetIndex;

    bool reachedGoal;
    Operator* blocker;

    void drawHealthBar() const;

public:
    Enemy(
        const std::vector<POINT>& path,
        float speed,
        int width,
        int height,
        int maxHp,
        int attackDamage,
        DWORD attackInterval);

    virtual ~Enemy() = default;

    void update(DWORD deltaTime);
    void reset();
    void takeDamage(int damage);

    bool isDead() const;
    bool hasReachedGoal() const;

    float getX() const;
    float getY() const;

    bool isBlocked() const;
    Operator* getBlocker() const;
    void setBlocker(Operator* newBlocker);
    void clearBlocker();

    virtual void draw() const = 0;
};

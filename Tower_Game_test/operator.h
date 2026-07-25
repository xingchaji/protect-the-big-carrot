#pragma once

#include <windows.h>
#include <vector>

class Enemy;
class Bullet;

class Operator
{
protected:
    int row;
    int col;
    int cost;
    int hp;
    int maxHp;

    int getCenterX() const;
    int getCenterY() const;
    void drawHealthBar() const;

public:
    Operator(int row, int col, int cost, int maxHp);
    virtual ~Operator() = default;

    int getRow() const;
    int getCol() const;
    int getCost() const;
    int getHp() const;
    int getMaxHp() const;

    void takeDamage(int damage);
    bool isDead() const;

    virtual void update(
        DWORD deltaTime,
        const std::vector<Enemy*>& enemies,
        std::vector<Bullet*>& bullets) = 0;

    virtual void draw() const = 0;
};

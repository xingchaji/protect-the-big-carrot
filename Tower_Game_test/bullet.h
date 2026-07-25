#pragma once

#include <graphics.h>

class Enemy;

class Bullet
{
private:
    float x;
    float y;
    float speed;

    int damage;
    Enemy* target;
    bool valid;

public:
    Bullet(
        float startX,
        float startY,
        Enemy* target,
        int damage,
        float speed = 420.0f);

    void update(DWORD deltaTime);
    void draw() const;

    bool isValid() const;
    Enemy* getTarget() const;
};

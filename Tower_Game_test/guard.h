#pragma once

#include "operator.h"
#include "sprite_animation.h"

class Guard : public Operator
{
private:
    int attackDamage;

    DWORD attackInterval;
    DWORD attackTimer;

    float attackRange;
    DWORD attackEffectTimer;

    float blockRange;
    bool blocking;

    SpriteAnimation idleAnimation;
    SpriteAnimation attackAnimation;

    Enemy* findBlockedEnemy(
        const std::vector<Enemy*>& enemies);

    Enemy* findBlockCandidate(
        const std::vector<Enemy*>& enemies);

    Enemy* findTarget(
        const std::vector<Enemy*>& enemies);

public:
    static constexpr int COST = 10;

    Guard(int row, int col);

    void update(
        DWORD deltaTime,
        const std::vector<Enemy*>& enemies,
        std::vector<Bullet*>& bullets) override;

    void draw() const override;
};

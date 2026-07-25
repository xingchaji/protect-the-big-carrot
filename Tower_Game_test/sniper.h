#pragma once

#include "operator.h"
#include "sprite_animation.h"

class Sniper : public Operator
{
private:
    int attackDamage;

    DWORD attackInterval;
    DWORD attackTimer;

    float attackRange;
    DWORD attackEffectTimer;

    SpriteAnimation idleAnimation;
    SpriteAnimation attackAnimation;

    Enemy* findTarget(
        const std::vector<Enemy*>& enemies) const;

public:
    static constexpr int COST = 12;

    Sniper(int row, int col);

    void update(
        DWORD deltaTime,
        const std::vector<Enemy*>& enemies,
        std::vector<Bullet*>& bullets) override;

    void draw() const override;
};

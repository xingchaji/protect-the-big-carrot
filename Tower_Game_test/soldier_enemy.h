#pragma once

#include "enemy.h"

class SoldierEnemy : public Enemy
{
public:
    explicit SoldierEnemy(const std::vector<POINT>& path);

    void draw() const override;
};

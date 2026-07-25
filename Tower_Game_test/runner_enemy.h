#pragma once

#include "enemy.h"

class RunnerEnemy : public Enemy
{
public:
    explicit RunnerEnemy(const std::vector<POINT>& path);

    void draw() const override;
};

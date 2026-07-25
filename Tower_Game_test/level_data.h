#pragma once

#include <tchar.h>
#include <vector>

#include "enemy_type.h"

struct GridPosition
{
    int row;
    int col;
};

struct LevelData
{
    const TCHAR* name;
    std::vector<std::vector<GridPosition>> paths;
    std::vector<std::vector<EnemyType>> waves;
    int initialBaseHp;
    int initialDeployPoints;
};

const LevelData& getLevelData(int levelIndex);
int getLevelCount();

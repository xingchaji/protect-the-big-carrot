#include "level_data.h"

namespace
{
    const std::vector<LevelData> LEVELS =
    {
        {
            _T("第一关：三路汇合"),
            {
                {{1, 14}, {1, 5}, {2, 5}, {2, 2}, {4, 2}, {4, 0}},
                {{4, 14}, {4, 2}, {4, 0}},
                {{7, 14}, {7, 5}, {6, 5}, {6, 2}, {4, 2}, {4, 0}}
            },
            {
                {EnemyType::Soldier, EnemyType::Soldier, EnemyType::Runner, EnemyType::Soldier},
                {EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner},
                {EnemyType::Soldier, EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner, EnemyType::Runner, EnemyType::Soldier}
            },
            8,
            25
        },
        {
            _T("第二关：交错战线"),
            {
                {{1, 14}, {1, 10}, {2, 10}, {2, 7}, {3, 7}, {3, 4}, {4, 4}, {4, 0}},
                {{4, 14}, {4, 11}, {5, 11}, {5, 8}, {4, 8}, {4, 4}, {4, 0}},
                {{7, 14}, {7, 12}, {6, 12}, {6, 6}, {5, 6}, {5, 4}, {4, 4}, {4, 0}}
            },
            {
                {EnemyType::Soldier, EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner, EnemyType::Soldier},
                {EnemyType::Runner, EnemyType::Runner, EnemyType::Soldier, EnemyType::Soldier, EnemyType::Runner},
                {EnemyType::Soldier, EnemyType::Runner, EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner, EnemyType::Soldier},
                {EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner, EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner}
            },
            8,
            28
        },
        {
            _T("第三关：曲折防线"),
            {
                {{1, 14}, {1, 12}, {2, 12}, {2, 9}, {3, 9}, {3, 6}, {2, 6}, {2, 3}, {4, 3}, {4, 0}},
                {{4, 14}, {4, 11}, {5, 11}, {5, 8}, {4, 8}, {4, 3}, {4, 0}},
                {{7, 14}, {7, 13}, {6, 13}, {6, 10}, {7, 10}, {7, 7}, {6, 7}, {6, 5}, {5, 5}, {5, 3}, {4, 3}, {4, 0}}
            },
            {
                {EnemyType::Soldier, EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner, EnemyType::Soldier},
                {EnemyType::Runner, EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner},
                {EnemyType::Soldier, EnemyType::Runner, EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner, EnemyType::Soldier},
                {EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner, EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner, EnemyType::Soldier},
                {EnemyType::Runner, EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner, EnemyType::Soldier, EnemyType::Runner, EnemyType::Runner}
            },
            10,
            30
        }
    };
}

const LevelData& getLevelData(int levelIndex)
{
    if (levelIndex < 0 || levelIndex >= static_cast<int>(LEVELS.size()))
    {
        return LEVELS[0];
    }

    return LEVELS[levelIndex];
}

int getLevelCount()
{
    return static_cast<int>(LEVELS.size());
}

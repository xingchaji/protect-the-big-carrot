#pragma once

#include <windows.h>
#include <vector>
#include <cstddef>

#include "enemy_type.h"

class WaveManager
{
private:
    std::vector<std::vector<EnemyType>> waves;

    std::size_t currentWaveIndex;
    std::size_t nextEnemyIndex;

    DWORD spawnTimer;
    DWORD waveWaitTimer;

    DWORD spawnInterval;
    DWORD waveInterval;

    bool waitingForNextWave;
    bool finished;

public:
    explicit WaveManager(int levelIndex = 0);

    void reset();

    bool update(
        DWORD deltaTime,
        bool battlefieldClear,
        EnemyType& enemyType);

    int getCurrentWaveNumber() const;
    int getTotalWaveCount() const;
    int getSpawnedInCurrentWave() const;
    int getCurrentWaveEnemyCount() const;

    bool isWaitingForNextWave() const;
    int getSecondsUntilNextWave() const;
    bool isFinished() const;
};

#include "wave_manager.h"

#include "level_data.h"

WaveManager::WaveManager(int levelIndex)
    : waves(getLevelData(levelIndex).waves),
      currentWaveIndex(0),
      nextEnemyIndex(0),
      spawnTimer(0),
      waveWaitTimer(0),
      spawnInterval(1200),
      waveInterval(2500),
      waitingForNextWave(false),
      finished(false)
{
    reset();
}

void WaveManager::reset()
{
    currentWaveIndex = 0;
    nextEnemyIndex = 0;

    spawnTimer = spawnInterval;
    waveWaitTimer = 0;

    waitingForNextWave = false;
    finished = waves.empty();
}

bool WaveManager::update(
    DWORD deltaTime,
    bool battlefieldClear,
    EnemyType& enemyType)
{
    if (finished)
    {
        return false;
    }

    if (waitingForNextWave)
    {
        waveWaitTimer += deltaTime;

        if (waveWaitTimer >= waveInterval)
        {
            currentWaveIndex++;
            nextEnemyIndex = 0;
            spawnTimer = spawnInterval;
            waveWaitTimer = 0;
            waitingForNextWave = false;
        }

        return false;
    }

    const std::vector<EnemyType>& currentWave =
        waves[currentWaveIndex];

    if (nextEnemyIndex < currentWave.size())
    {
        spawnTimer += deltaTime;

        if (spawnTimer >= spawnInterval)
        {
            spawnTimer -= spawnInterval;
            enemyType = currentWave[nextEnemyIndex];
            nextEnemyIndex++;
            return true;
        }

        return false;
    }

    if (battlefieldClear)
    {
        if (currentWaveIndex + 1 >= waves.size())
        {
            finished = true;
        }
        else
        {
            waitingForNextWave = true;
            waveWaitTimer = 0;
        }
    }

    return false;
}

int WaveManager::getCurrentWaveNumber() const
{
    if (waves.empty())
    {
        return 0;
    }

    return static_cast<int>(currentWaveIndex + 1);
}

int WaveManager::getTotalWaveCount() const
{
    return static_cast<int>(waves.size());
}

int WaveManager::getSpawnedInCurrentWave() const
{
    return static_cast<int>(nextEnemyIndex);
}

int WaveManager::getCurrentWaveEnemyCount() const
{
    if (waves.empty())
    {
        return 0;
    }

    return static_cast<int>(waves[currentWaveIndex].size());
}

bool WaveManager::isWaitingForNextWave() const
{
    return waitingForNextWave;
}

int WaveManager::getSecondsUntilNextWave() const
{
    if (!waitingForNextWave)
    {
        return 0;
    }

    const DWORD remainingTime =
        waveInterval > waveWaitTimer
            ? waveInterval - waveWaitTimer
            : 0;

    return static_cast<int>((remainingTime + 999) / 1000);
}

bool WaveManager::isFinished() const
{
    return finished;
}

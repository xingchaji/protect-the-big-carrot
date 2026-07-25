#pragma once

#include <cstddef>
#include <vector>

#include "scene.h"
#include "game_map.h"
#include "wave_manager.h"

class Enemy;
class Operator;
class Bullet;

class GameScene : public Scene
{
private:
    enum class SelectedOperator
    {
        None,
        Guard,
        Sniper
    };

    int currentLevel;
    GameMap gameMap;
    WaveManager waveManager;

    std::vector<std::vector<POINT>> enemyPaths;
    std::size_t nextEnemyPathIndex;

    std::vector<Enemy*> enemies;
    std::vector<Operator*> operators;
    std::vector<Bullet*> bullets;

    int hoverRow;
    int hoverCol;
    int selectedRow;
    int selectedCol;

    int baseHp;
    int defeatedCount;

    int deployPoints;
    DWORD deployPointTimer;

    SelectedOperator selectedOperator;
    Operator* selectedDeployedOperator;
    const TCHAR* actionMessage;

    std::vector<std::vector<POINT>> createEnemyPaths() const;
    Enemy* createEnemy(
        EnemyType type,
        const std::vector<POINT>& path) const;

    void clearEnemies();
    void clearOperators();
    void clearBullets();

    void removeBulletsTargeting(Enemy* target);
    void removeInvalidBullets();
    void releaseEnemiesBlockedBy(Operator* op);
    void removeDeadOperators();

    Operator* findOperatorAt(int row, int col) const;
    bool retreatSelectedOperator();

    bool tryDeployGuard(int row, int col);
    bool tryDeploySniper(int row, int col);

    void resetBattle();

    bool pointInsideRectangle(
        int x,
        int y,
        int left,
        int top,
        int right,
        int bottom) const;

    const TCHAR* tileTypeToText(TileType type) const;

    void drawGuardCard() const;
    void drawSniperCard() const;
    void drawRetreatButton() const;
    void drawOperatorPanel() const;
    void drawInterface() const;

public:
    explicit GameScene(int levelIndex = 0);
    ~GameScene() override;

    void handleInput(const ExMessage& msg) override;
    void update(DWORD deltaTime) override;
    void draw() const override;
};

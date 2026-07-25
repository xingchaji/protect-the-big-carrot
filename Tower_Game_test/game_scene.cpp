#include "game_scene.h"

#include <tchar.h>
#include <cstddef>

#include "config.h"

#include "enemy.h"
#include "soldier_enemy.h"
#include "runner_enemy.h"

#include "operator.h"
#include "guard.h"
#include "sniper.h"

#include "bullet.h"
#include "level_data.h"

namespace
{
    constexpr int MAX_DEPLOY_POINTS = 99;
    constexpr DWORD DEPLOY_POINT_INTERVAL = 1000;

    constexpr int GUARD_CARD_LEFT = 25;
    constexpr int GUARD_CARD_TOP = 150;
    constexpr int GUARD_CARD_RIGHT = 215;
    constexpr int GUARD_CARD_BOTTOM = 245;

    constexpr int SNIPER_CARD_LEFT = 25;
    constexpr int SNIPER_CARD_TOP = 265;
    constexpr int SNIPER_CARD_RIGHT = 215;
    constexpr int SNIPER_CARD_BOTTOM = 360;

    constexpr int RETREAT_LEFT = 25;
    constexpr int RETREAT_TOP = 390;
    constexpr int RETREAT_RIGHT = 215;
    constexpr int RETREAT_BOTTOM = 455;

    constexpr int RIGHT_PANEL_LEFT = 1190;
    constexpr int RIGHT_PANEL_RIGHT = 1424;

    void drawTacticalBackground()
    {
        setfillcolor(RGB(13, 18, 23));
        solidrectangle(
            0,
            0,
            GameConfig::WINDOW_WIDTH,
            GameConfig::WINDOW_HEIGHT);

        setlinecolor(RGB(22, 31, 38));
        setlinestyle(PS_SOLID, 1);

        for (int y = 112; y < GameConfig::WINDOW_HEIGHT; y += 34)
        {
            line(0, y, GameConfig::WINDOW_WIDTH, y);
        }

        for (int x = -160; x < GameConfig::WINDOW_WIDTH; x += 210)
        {
            line(x, GameConfig::WINDOW_HEIGHT,
                 x + 360, 100);
        }

        setfillcolor(RGB(20, 27, 33));
        solidrectangle(0, 0, GameConfig::WINDOW_WIDTH, 100);

        setfillcolor(RGB(17, 24, 30));
        solidrectangle(0, 812, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);

        setfillcolor(RGB(57, 201, 220));
        solidrectangle(0, 98, 330, 101);
        solidrectangle(1110, 98, GameConfig::WINDOW_WIDTH, 101);

        setfillcolor(RGB(237, 177, 58));
        solidrectangle(330, 98, 470, 101);
    }

    void drawPanel(
        int left,
        int top,
        int right,
        int bottom,
        COLORREF accent)
    {
        POINT points[6] =
        {
            { left + 13, top },
            { right, top },
            { right, bottom - 13 },
            { right - 13, bottom },
            { left, bottom },
            { left, top + 13 }
        };

        setfillcolor(RGB(27, 35, 42));
        solidpolygon(points, 6);

        setlinecolor(RGB(75, 91, 101));
        setlinestyle(PS_SOLID, 2);
        polygon(points, 6);

        setfillcolor(accent);
        solidrectangle(left + 13, top, left + 80, top + 3);
        solidrectangle(left, top + 13, left + 3, top + 70);
    }

    void drawMetricBox(
        int left,
        int top,
        int right,
        int bottom,
        const TCHAR* label,
        const TCHAR* value,
        COLORREF accent)
    {
        setfillcolor(RGB(31, 41, 48));
        solidrectangle(left, top, right, bottom);

        setlinecolor(RGB(72, 90, 101));
        setlinestyle(PS_SOLID, 1);
        rectangle(left, top, right, bottom);

        setfillcolor(accent);
        solidrectangle(left, top, left + 4, bottom);

        setbkmode(TRANSPARENT);
        settextcolor(RGB(139, 157, 166));
        settextstyle(14, 0, _T("微软雅黑"));
        outtextxy(left + 13, top + 7, label);

        settextcolor(WHITE);
        settextstyle(25, 0, _T("Consolas"));
        outtextxy(right - 13 - textwidth(value), top + 22, value);
    }

    int normalizeLevelIndex(int levelIndex)
    {
        return levelIndex >= 0 && levelIndex < getLevelCount()
            ? levelIndex
            : 0;
    }
}

GameScene::GameScene(int levelIndex)
    : currentLevel(normalizeLevelIndex(levelIndex)),
      gameMap(currentLevel),
      waveManager(currentLevel),
      nextEnemyPathIndex(0),
      hoverRow(-1),
      hoverCol(-1),
      selectedRow(-1),
      selectedCol(-1),
      baseHp(getLevelData(currentLevel).initialBaseHp),
      defeatedCount(0),
      deployPoints(getLevelData(currentLevel).initialDeployPoints),
      deployPointTimer(0),
      selectedOperator(SelectedOperator::None),
      selectedDeployedOperator(nullptr),
      actionMessage(_T("请选择近卫或狙击"))
{
    enemyPaths = createEnemyPaths();
}

GameScene::~GameScene()
{
    clearBullets();
    clearEnemies();
    clearOperators();
}

std::vector<std::vector<POINT>> GameScene::createEnemyPaths() const
{
    std::vector<std::vector<POINT>> paths;
    const LevelData& level = getLevelData(currentLevel);

    for (const std::vector<GridPosition>& gridPath : level.paths)
    {
        std::vector<POINT> path;

        for (const GridPosition& position : gridPath)
        {
            path.push_back(
                gameMap.getTileCenter(position.row, position.col));
        }

        paths.push_back(path);
    }

    return paths;
}

Enemy* GameScene::createEnemy(
    EnemyType type,
    const std::vector<POINT>& path) const
{
    switch (type)
    {
    case EnemyType::Soldier:
        return new SoldierEnemy(path);

    case EnemyType::Runner:
        return new RunnerEnemy(path);
    }

    return nullptr;
}

void GameScene::clearEnemies()
{
    for (Enemy* enemy : enemies)
    {
        delete enemy;
    }

    enemies.clear();
}

void GameScene::clearOperators()
{
    for (Operator* op : operators)
    {
        delete op;
    }

    operators.clear();
    selectedDeployedOperator = nullptr;
}

void GameScene::clearBullets()
{
    for (Bullet* bullet : bullets)
    {
        delete bullet;
    }

    bullets.clear();
}

void GameScene::removeBulletsTargeting(Enemy* target)
{
    for (std::size_t i = 0; i < bullets.size();)
    {
        if (bullets[i]->getTarget() == target)
        {
            delete bullets[i];
            bullets.erase(bullets.begin() + i);
        }
        else
        {
            i++;
        }
    }
}

void GameScene::removeInvalidBullets()
{
    for (std::size_t i = 0; i < bullets.size();)
    {
        if (!bullets[i]->isValid())
        {
            delete bullets[i];
            bullets.erase(bullets.begin() + i);
        }
        else
        {
            i++;
        }
    }
}

void GameScene::releaseEnemiesBlockedBy(Operator* op)
{
    for (Enemy* enemy : enemies)
    {
        if (enemy != nullptr && enemy->getBlocker() == op)
        {
            enemy->clearBlocker();
        }
    }
}

void GameScene::removeDeadOperators()
{
    for (std::size_t i = 0; i < operators.size();)
    {
        Operator* op = operators[i];

        if (!op->isDead())
        {
            i++;
            continue;
        }

        if (selectedDeployedOperator == op)
        {
            selectedDeployedOperator = nullptr;
            selectedRow = -1;
            selectedCol = -1;
        }

        releaseEnemiesBlockedBy(op);
        gameMap.setOccupied(op->getRow(), op->getCol(), false);

        delete op;
        operators.erase(operators.begin() + i);
        actionMessage = _T("一名干员生命归零，已退场");
    }
}

Operator* GameScene::findOperatorAt(int row, int col) const
{
    for (Operator* op : operators)
    {
        if (op != nullptr && op->getRow() == row && op->getCol() == col)
        {
            return op;
        }
    }

    return nullptr;
}

bool GameScene::retreatSelectedOperator()
{
    if (selectedDeployedOperator == nullptr)
    {
        return false;
    }

    for (std::size_t i = 0; i < operators.size(); i++)
    {
        if (operators[i] != selectedDeployedOperator)
        {
            continue;
        }

        Operator* op = operators[i];
        releaseEnemiesBlockedBy(op);
        gameMap.setOccupied(op->getRow(), op->getCol(), false);

        delete op;
        operators.erase(operators.begin() + i);

        selectedDeployedOperator = nullptr;
        selectedRow = -1;
        selectedCol = -1;
        selectedOperator = SelectedOperator::None;
        actionMessage = _T("干员已主动撤退，本次不返还费用");
        return true;
    }

    selectedDeployedOperator = nullptr;
    return false;
}

bool GameScene::tryDeployGuard(int row, int col)
{
    if (deployPoints < Guard::COST ||
        !gameMap.canDeployGround(row, col))
    {
        return false;
    }

    operators.push_back(new Guard(row, col));
    gameMap.setOccupied(row, col, true);
    deployPoints -= Guard::COST;

    return true;
}

bool GameScene::tryDeploySniper(int row, int col)
{
    if (deployPoints < Sniper::COST ||
        !gameMap.canDeployHighGround(row, col))
    {
        return false;
    }

    operators.push_back(new Sniper(row, col));
    gameMap.setOccupied(row, col, true);
    deployPoints -= Sniper::COST;

    return true;
}

void GameScene::resetBattle()
{
    clearBullets();
    clearEnemies();
    clearOperators();

    gameMap.clearOccupancy();
    waveManager.reset();
    nextEnemyPathIndex = 0;

    hoverRow = -1;
    hoverCol = -1;
    selectedRow = -1;
    selectedCol = -1;

    const LevelData& level = getLevelData(currentLevel);
    baseHp = level.initialBaseHp;
    defeatedCount = 0;

    deployPoints = level.initialDeployPoints;
    deployPointTimer = 0;

    selectedOperator = SelectedOperator::None;
    selectedDeployedOperator = nullptr;
    actionMessage = _T("游戏已重新开始");
}

bool GameScene::pointInsideRectangle(
    int x,
    int y,
    int left,
    int top,
    int right,
    int bottom) const
{
    return x >= left && x <= right &&
           y >= top && y <= bottom;
}

const TCHAR* GameScene::tileTypeToText(TileType type) const
{
    switch (type)
    {
    case TileType::Ground:
        return _T("平地");

    case TileType::HighGround:
        return _T("高台");

    case TileType::Blocked:
        return _T("禁入区域");

    case TileType::Entrance:
        return _T("敌人起点");

    case TileType::Base:
        return _T("我方终点");
    }

    return _T("未知");
}

void GameScene::handleInput(const ExMessage& msg)
{
    if (msg.message == WM_MOUSEMOVE)
    {
        if (!gameMap.screenToGrid(
            msg.x,
            msg.y,
            hoverRow,
            hoverCol))
        {
            hoverRow = -1;
            hoverCol = -1;
        }
    }
    else if (msg.message == WM_LBUTTONDOWN)
    {
        if (pointInsideRectangle(
            msg.x,
            msg.y,
            GUARD_CARD_LEFT,
            GUARD_CARD_TOP,
            GUARD_CARD_RIGHT,
            GUARD_CARD_BOTTOM))
        {
            selectedDeployedOperator = nullptr;

            if (deployPoints < Guard::COST)
            {
                selectedOperator = SelectedOperator::None;
                actionMessage = _T("部署费用不足，无法选择近卫");
            }
            else
            {
                selectedOperator = SelectedOperator::Guard;
                actionMessage = _T("已选择近卫，请点击平地");
            }

            return;
        }

        if (pointInsideRectangle(
            msg.x,
            msg.y,
            SNIPER_CARD_LEFT,
            SNIPER_CARD_TOP,
            SNIPER_CARD_RIGHT,
            SNIPER_CARD_BOTTOM))
        {
            selectedDeployedOperator = nullptr;

            if (deployPoints < Sniper::COST)
            {
                selectedOperator = SelectedOperator::None;
                actionMessage = _T("部署费用不足，无法选择狙击");
            }
            else
            {
                selectedOperator = SelectedOperator::Sniper;
                actionMessage = _T("已选择狙击，请点击高台");
            }

            return;
        }

        if (pointInsideRectangle(
            msg.x,
            msg.y,
            RETREAT_LEFT,
            RETREAT_TOP,
            RETREAT_RIGHT,
            RETREAT_BOTTOM))
        {
            if (!retreatSelectedOperator())
            {
                actionMessage = _T("请先点击地图上的一名干员");
            }

            return;
        }

        int clickedRow;
        int clickedCol;

        if (!gameMap.screenToGrid(
            msg.x,
            msg.y,
            clickedRow,
            clickedCol))
        {
            return;
        }

        selectedRow = clickedRow;
        selectedCol = clickedCol;

        if (selectedOperator == SelectedOperator::None)
        {
            selectedDeployedOperator =
                findOperatorAt(clickedRow, clickedCol);

            actionMessage = selectedDeployedOperator != nullptr
                ? _T("已选中干员，可点击撤退或按Delete")
                : _T("请先选择一种干员");
            return;
        }

        if (gameMap.isOccupied(clickedRow, clickedCol))
        {
            actionMessage = _T("这个格子已经有干员");
            return;
        }

        bool deployed = false;

        if (selectedOperator == SelectedOperator::Guard)
        {
            deployed = tryDeployGuard(clickedRow, clickedCol);

            actionMessage =
                deployed
                    ? _T("近卫部署成功，消耗10点费用")
                    : _T("近卫只能部署在平地");
        }
        else if (selectedOperator == SelectedOperator::Sniper)
        {
            deployed = tryDeploySniper(clickedRow, clickedCol);

            actionMessage =
                deployed
                    ? _T("狙击部署成功，消耗12点费用")
                    : _T("狙击只能部署在高台");
        }

        if (deployed)
        {
            selectedOperator = SelectedOperator::None;
            selectedDeployedOperator = nullptr;
        }
    }
    else if (msg.message == WM_KEYDOWN)
    {
        if (msg.vkcode == VK_ESCAPE)
        {
            request.target = SceneType::Menu;
        }
        else if (msg.vkcode == 'R')
        {
            resetBattle();
        }
        else if (msg.vkcode == VK_DELETE)
        {
            if (!retreatSelectedOperator())
            {
                actionMessage = _T("请先点击地图上的一名干员");
            }
        }
    }
    else if (msg.message == WM_CLOSE)
    {
        request.target = SceneType::Exit;
    }
}

void GameScene::update(DWORD deltaTime)
{
    EnemyType typeToSpawn;

    const bool shouldSpawn = waveManager.update(
        deltaTime,
        enemies.empty(),
        typeToSpawn);

    if (shouldSpawn)
    {
        Enemy* newEnemy = nullptr;

        if (!enemyPaths.empty())
        {
            newEnemy = createEnemy(
                typeToSpawn,
                enemyPaths[nextEnemyPathIndex]);

            nextEnemyPathIndex =
                (nextEnemyPathIndex + 1) % enemyPaths.size();
        }

        if (newEnemy != nullptr)
        {
            enemies.push_back(newEnemy);
        }
    }

    if (deployPoints < MAX_DEPLOY_POINTS)
    {
        deployPointTimer += deltaTime;

        while (deployPointTimer >= DEPLOY_POINT_INTERVAL &&
               deployPoints < MAX_DEPLOY_POINTS)
        {
            deployPoints++;
            deployPointTimer -= DEPLOY_POINT_INTERVAL;
        }
    }
    else
    {
        deployPointTimer = 0;
    }

    for (std::size_t i = 0; i < enemies.size();)
    {
        Enemy* enemy = enemies[i];
        enemy->update(deltaTime);

        if (enemy->hasReachedGoal())
        {
            baseHp--;

            removeBulletsTargeting(enemy);

            delete enemy;
            enemies.erase(enemies.begin() + i);

            if (baseHp <= 0)
            {
                baseHp = 0;
                request.target = SceneType::Result;
                request.victory = false;
                request.levelIndex = currentLevel;
                return;
            }
        }
        else
        {
            i++;
        }
    }

    removeDeadOperators();

    for (Operator* op : operators)
    {
        op->update(deltaTime, enemies, bullets);
    }

    for (Bullet* bullet : bullets)
    {
        bullet->update(deltaTime);
    }

    removeInvalidBullets();

    for (std::size_t i = 0; i < enemies.size();)
    {
        Enemy* enemy = enemies[i];

        if (enemy->isDead())
        {
            removeBulletsTargeting(enemy);

            delete enemy;
            enemies.erase(enemies.begin() + i);

            defeatedCount++;
            actionMessage = _T("一名敌人已被击败");
        }
        else
        {
            i++;
        }
    }

    if (waveManager.isFinished() && enemies.empty())
    {
        clearBullets();

        request.target = SceneType::Result;
        request.victory = true;
        request.levelIndex = currentLevel;
    }
}

void GameScene::drawGuardCard() const
{
    const bool selected =
        selectedOperator == SelectedOperator::Guard;

    const bool affordable =
        deployPoints >= Guard::COST;

    const COLORREF accent = affordable
        ? RGB(71, 202, 174)
        : RGB(78, 84, 87);

    setfillcolor(
        !affordable
            ? RGB(31, 35, 38)
            : selected
                  ? RGB(42, 72, 70)
                  : RGB(38, 47, 52));
    solidrectangle(
        GUARD_CARD_LEFT,
        GUARD_CARD_TOP,
        GUARD_CARD_RIGHT,
        GUARD_CARD_BOTTOM);

    setlinecolor(selected && affordable
        ? RGB(255, 210, 75)
        : RGB(81, 96, 104));
    setlinestyle(PS_SOLID, selected && affordable ? 3 : 1);
    rectangle(
        GUARD_CARD_LEFT,
        GUARD_CARD_TOP,
        GUARD_CARD_RIGHT,
        GUARD_CARD_BOTTOM);

    setfillcolor(accent);
    solidrectangle(
        GUARD_CARD_LEFT,
        GUARD_CARD_TOP,
        GUARD_CARD_LEFT + 5,
        GUARD_CARD_BOTTOM);

    POINT shield[5] =
    {
        { GUARD_CARD_LEFT + 22, GUARD_CARD_TOP + 22 },
        { GUARD_CARD_LEFT + 58, GUARD_CARD_TOP + 22 },
        { GUARD_CARD_LEFT + 58, GUARD_CARD_TOP + 49 },
        { GUARD_CARD_LEFT + 40, GUARD_CARD_TOP + 67 },
        { GUARD_CARD_LEFT + 22, GUARD_CARD_TOP + 49 }
    };

    setfillcolor(affordable
        ? RGB(49, 131, 119)
        : RGB(64, 68, 70));
    solidpolygon(shield, 5);
    setlinecolor(accent);
    setlinestyle(PS_SOLID, 2);
    polygon(shield, 5);

    setlinecolor(RGB(220, 247, 240));
    line(GUARD_CARD_LEFT + 31, GUARD_CARD_TOP + 43,
         GUARD_CARD_LEFT + 49, GUARD_CARD_TOP + 43);

    setbkmode(TRANSPARENT);
    settextcolor(
        affordable ? WHITE : RGB(125, 125, 125));

    settextstyle(25, 0, _T("微软雅黑"));
    outtextxy(
        GUARD_CARD_LEFT + 72,
        GUARD_CARD_TOP + 12,
        _T("近卫"));

    TCHAR costText[30];

    _stprintf_s(
        costText,
        _countof(costText),
        _T("费用：%d"),
        Guard::COST);

    setfillcolor(affordable
        ? RGB(229, 175, 52)
        : RGB(68, 72, 74));
    solidrectangle(
        GUARD_CARD_RIGHT - 63,
        GUARD_CARD_TOP + 9,
        GUARD_CARD_RIGHT - 8,
        GUARD_CARD_TOP + 34);

    settextcolor(affordable ? RGB(25, 28, 30) : RGB(135, 135, 135));
    settextstyle(14, 0, _T("微软雅黑"));
    outtextxy(GUARD_CARD_RIGHT - 59, GUARD_CARD_TOP + 13, costText);

    settextcolor(affordable ? RGB(196, 211, 216) : RGB(112, 116, 118));
    settextstyle(15, 0, _T("微软雅黑"));
    outtextxy(
        GUARD_CARD_LEFT + 72,
        GUARD_CARD_TOP + 55,
        _T("平地  阻挡 1"));
}

void GameScene::drawSniperCard() const
{
    const bool selected =
        selectedOperator == SelectedOperator::Sniper;

    const bool affordable =
        deployPoints >= Sniper::COST;

    const COLORREF accent = affordable
        ? RGB(120, 151, 255)
        : RGB(78, 84, 87);

    setfillcolor(
        !affordable
            ? RGB(31, 35, 38)
            : selected
                  ? RGB(50, 54, 91)
                  : RGB(38, 47, 52));
    solidrectangle(
        SNIPER_CARD_LEFT,
        SNIPER_CARD_TOP,
        SNIPER_CARD_RIGHT,
        SNIPER_CARD_BOTTOM);

    setlinecolor(selected && affordable
        ? RGB(255, 210, 75)
        : RGB(81, 96, 104));
    setlinestyle(PS_SOLID, selected && affordable ? 3 : 1);
    rectangle(
        SNIPER_CARD_LEFT,
        SNIPER_CARD_TOP,
        SNIPER_CARD_RIGHT,
        SNIPER_CARD_BOTTOM);

    setfillcolor(accent);
    solidrectangle(
        SNIPER_CARD_LEFT,
        SNIPER_CARD_TOP,
        SNIPER_CARD_LEFT + 5,
        SNIPER_CARD_BOTTOM);

    const int iconX = SNIPER_CARD_LEFT + 40;
    const int iconY = SNIPER_CARD_TOP + 44;

    setlinecolor(accent);
    setlinestyle(PS_SOLID, 2);
    circle(iconX, iconY, 20);
    circle(iconX, iconY, 8);
    line(iconX - 27, iconY, iconX + 27, iconY);
    line(iconX, iconY - 27, iconX, iconY + 27);
    setfillcolor(affordable
        ? RGB(214, 226, 255)
        : RGB(100, 104, 106));
    solidcircle(iconX, iconY, 3);

    setbkmode(TRANSPARENT);
    settextcolor(
        affordable ? WHITE : RGB(125, 125, 125));

    settextstyle(25, 0, _T("微软雅黑"));
    outtextxy(
        SNIPER_CARD_LEFT + 72,
        SNIPER_CARD_TOP + 12,
        _T("狙击"));

    TCHAR costText[30];

    _stprintf_s(
        costText,
        _countof(costText),
        _T("费用：%d"),
        Sniper::COST);

    setfillcolor(affordable
        ? RGB(229, 175, 52)
        : RGB(68, 72, 74));
    solidrectangle(
        SNIPER_CARD_RIGHT - 63,
        SNIPER_CARD_TOP + 9,
        SNIPER_CARD_RIGHT - 8,
        SNIPER_CARD_TOP + 34);

    settextcolor(affordable ? RGB(25, 28, 30) : RGB(135, 135, 135));
    settextstyle(14, 0, _T("微软雅黑"));
    outtextxy(SNIPER_CARD_RIGHT - 59, SNIPER_CARD_TOP + 13, costText);

    settextcolor(affordable ? RGB(196, 211, 216) : RGB(112, 116, 118));
    settextstyle(15, 0, _T("微软雅黑"));
    outtextxy(
        SNIPER_CARD_LEFT + 72,
        SNIPER_CARD_TOP + 55,
        _T("高台  远程"));
}

void GameScene::drawRetreatButton() const
{
    const bool available = selectedDeployedOperator != nullptr;

    setfillcolor(available
        ? RGB(91, 43, 43)
        : RGB(34, 38, 41));
    setlinecolor(available
        ? RGB(255, 135, 115)
        : RGB(90, 90, 90));
    setlinestyle(PS_SOLID, available ? 3 : 2);

    solidrectangle(
        RETREAT_LEFT,
        RETREAT_TOP,
        RETREAT_RIGHT,
        RETREAT_BOTTOM);

    rectangle(
        RETREAT_LEFT,
        RETREAT_TOP,
        RETREAT_RIGHT,
        RETREAT_BOTTOM);

    if (available)
    {
        setfillcolor(RGB(235, 91, 70));
        solidrectangle(
            RETREAT_LEFT,
            RETREAT_TOP,
            RETREAT_LEFT + 5,
            RETREAT_BOTTOM);
    }

    const TCHAR text[] = _T("撤退  [Delete]");
    setbkmode(TRANSPARENT);
    settextcolor(available ? WHITE : RGB(120, 120, 120));
    settextstyle(21, 0, _T("微软雅黑"));

    outtextxy(
        RETREAT_LEFT +
            (RETREAT_RIGHT - RETREAT_LEFT - textwidth(text)) / 2,
        RETREAT_TOP + 19,
        text);
}

void GameScene::drawOperatorPanel() const
{
    drawPanel(15, 105, 245, 520, RGB(57, 201, 220));

    setbkmode(TRANSPARENT);
    settextcolor(RGB(223, 234, 238));
    settextstyle(22, 0, _T("微软雅黑"));

    outtextxy(25, 115, _T("干员部署 / OPS"));

    settextcolor(RGB(93, 215, 232));
    settextstyle(12, 0, _T("Consolas"));
    outtextxy(25, 137, _T("SELECT UNIT AND DEPLOY"));

    drawGuardCard();
    drawSniperCard();
    drawRetreatButton();

    setlinecolor(RGB(66, 82, 91));
    setlinestyle(PS_SOLID, 1);
    line(25, 473, 215, 473);
}

void GameScene::drawInterface() const
{
    setbkmode(TRANSPARENT);
    settextcolor(RGB(238, 244, 246));

    settextstyle(31, 0, _T("微软雅黑"));
    outtextxy(25, 15, _T("保卫大萝卜"));

    settextcolor(RGB(57, 201, 220));
    settextstyle(13, 0, _T("Consolas"));
    outtextxy(28, 56, _T("TACTICAL DEFENSE SYSTEM // ONLINE"));

    const TCHAR* levelName = getLevelData(currentLevel).name;
    settextcolor(RGB(219, 229, 233));
    settextstyle(28, 0, _T("微软雅黑"));
    outtextxy(
        (GameConfig::WINDOW_WIDTH - textwidth(levelName)) / 2,
        17,
        levelName);

    TCHAR levelIndexText[40];
    _stprintf_s(
        levelIndexText,
        _countof(levelIndexText),
        _T("OPERATION  %02d / %02d"),
        currentLevel + 1,
        getLevelCount());

    settextcolor(RGB(130, 148, 157));
    settextstyle(14, 0, _T("Consolas"));
    outtextxy(
        (GameConfig::WINDOW_WIDTH - textwidth(levelIndexText)) / 2,
        57,
        levelIndexText);

    drawPanel(
        RIGHT_PANEL_LEFT,
        150,
        RIGHT_PANEL_RIGHT,
        520,
        RGB(237, 177, 58));

    settextcolor(RGB(223, 234, 238));
    settextstyle(21, 0, _T("微软雅黑"));
    outtextxy(RIGHT_PANEL_LEFT + 15, 162, _T("战场状态 / STATUS"));

    TCHAR baseText[16];
    TCHAR deployText[16];
    TCHAR waveTextValue[32];
    TCHAR enemyText[16];
    TCHAR defeatedText[16];

    _stprintf_s(baseText, _countof(baseText), _T("%02d"), baseHp);
    _stprintf_s(deployText, _countof(deployText), _T("%02d"), deployPoints);
    _stprintf_s(
        waveTextValue,
        _countof(waveTextValue),
        _T("%d / %d"),
        waveManager.getCurrentWaveNumber(),
        waveManager.getTotalWaveCount());
    _stprintf_s(
        enemyText,
        _countof(enemyText),
        _T("%02d"),
        static_cast<int>(enemies.size()));
    _stprintf_s(
        defeatedText,
        _countof(defeatedText),
        _T("%02d"),
        defeatedCount);

    drawMetricBox(
        RIGHT_PANEL_LEFT + 14, 200,
        RIGHT_PANEL_RIGHT - 14, 258,
        _T("基地耐久 / BASE"),
        baseText,
        baseHp > 2 ? RGB(76, 211, 139) : RGB(239, 91, 80));

    drawMetricBox(
        RIGHT_PANEL_LEFT + 14, 269,
        RIGHT_PANEL_RIGHT - 14, 327,
        _T("部署费用 / DP"),
        deployText,
        RGB(237, 177, 58));

    drawMetricBox(
        RIGHT_PANEL_LEFT + 14, 338,
        RIGHT_PANEL_RIGHT - 14, 396,
        _T("当前波次 / WAVE"),
        waveTextValue,
        RGB(93, 215, 232));

    drawMetricBox(
        RIGHT_PANEL_LEFT + 14, 407,
        RIGHT_PANEL_LEFT + 105, 485,
        _T("场上"),
        enemyText,
        RGB(193, 103, 89));

    drawMetricBox(
        RIGHT_PANEL_LEFT + 116, 407,
        RIGHT_PANEL_RIGHT - 14, 485,
        _T("击破"),
        defeatedText,
        RGB(132, 183, 107));

    if (waveManager.isWaitingForNextWave())
    {
        TCHAR waveText[100];

        _stprintf_s(
            waveText,
            _countof(waveText),
            _T("下一波将在%d秒后开始"),
            waveManager.getSecondsUntilNextWave());

        setfillcolor(RGB(25, 54, 62));
        solidrectangle(555, 105, 885, 141);
        setlinecolor(RGB(93, 215, 232));
        rectangle(555, 105, 885, 141);

        settextcolor(RGB(151, 238, 248));
        settextstyle(18, 0, _T("微软雅黑"));

        const int x =
            (GameConfig::WINDOW_WIDTH - textwidth(waveText)) / 2;

        outtextxy(x, 112, waveText);
    }

    settextcolor(RGB(242, 205, 115));
    settextstyle(14, 0, _T("微软雅黑"));
    outtextxy(25, 488, actionMessage);

    settextcolor(RGB(172, 190, 198));
    settextstyle(16, 0, _T("微软雅黑"));
    outtextxy(25, 831, _T("操作：点击卡片部署  |  Delete撤退  |  R重置  |  Esc返回菜单"));

    if (selectedRow != -1 && selectedCol != -1)
    {
        TCHAR tileText[180];

        _stprintf_s(
            tileText,
            _countof(tileText),
            _T("当前选择：第%d行，第%d列，类型：%s"),
            selectedRow + 1,
            selectedCol + 1,
            tileTypeToText(
                gameMap.getTileType(
                    selectedRow,
                    selectedCol)));

        settextcolor(RGB(213, 225, 230));
        settextstyle(17, 0, _T("微软雅黑"));
        outtextxy(25, 863, tileText);
    }
}

void GameScene::draw() const
{
    drawTacticalBackground();

    gameMap.draw();

    for (const Enemy* enemy : enemies)
    {
        enemy->draw();
    }

    for (const Operator* op : operators)
    {
        op->draw();
    }

    for (const Bullet* bullet : bullets)
    {
        bullet->draw();
    }

    gameMap.drawSelectedTile(selectedRow, selectedCol);
    gameMap.drawHoverTile(hoverRow, hoverCol);

    drawOperatorPanel();
    drawInterface();
}

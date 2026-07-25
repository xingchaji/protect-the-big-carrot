#pragma once

#include <graphics.h>

#include "config.h"
#include "tile.h"

class GameMap
{
private:
    Tile tiles[GameConfig::MAP_ROWS][GameConfig::MAP_COLS];

public:
    explicit GameMap(int levelIndex = 0);

    void draw() const;

    bool isValidGrid(int row, int col) const;

    bool screenToGrid(
        int x,
        int y,
        int& row,
        int& col) const;

    TileType getTileType(int row, int col) const;
    POINT getTileCenter(int row, int col) const;

    bool canDeployGround(int row, int col) const;
    bool canDeployHighGround(int row, int col) const;

    bool isOccupied(int row, int col) const;
    bool setOccupied(int row, int col, bool value);
    void clearOccupancy();

    void drawSelectedTile(int row, int col) const;
    void drawHoverTile(int row, int col) const;
};

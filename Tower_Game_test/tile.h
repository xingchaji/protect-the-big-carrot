#pragma once

#include <graphics.h>

enum class TileType
{
    Ground,
    HighGround,
    Blocked,
    Entrance,
    Base
};

class Tile
{
private:
    int row;
    int col;
    TileType type;
    bool occupied;

public:
    Tile();
    Tile(int row, int col, TileType type);

    void draw() const;

    TileType getType() const;
    bool isOccupied() const;
    void setOccupied(bool value);

    bool canDeployGround() const;
    bool canDeployHighGround() const;
};

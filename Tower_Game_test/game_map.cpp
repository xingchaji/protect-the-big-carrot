#include "game_map.h"

#include <cstddef>
#include <tchar.h>

#include "level_data.h"

GameMap::GameMap(int levelIndex)
{
    TileType layout[GameConfig::MAP_ROWS][GameConfig::MAP_COLS];

    for (int row = 0; row < GameConfig::MAP_ROWS; row++)
    {
        for (int col = 0; col < GameConfig::MAP_COLS; col++)
        {
            const bool border =
                row == 0 || row == GameConfig::MAP_ROWS - 1 ||
                col == 0 || col == GameConfig::MAP_COLS - 1;

            layout[row][col] = border
                ? TileType::Blocked
                : TileType::HighGround;
        }
    }

    const LevelData& level = getLevelData(levelIndex);

    for (const std::vector<GridPosition>& path : level.paths)
    {
        for (std::size_t i = 1; i < path.size(); i++)
        {
            const GridPosition& from = path[i - 1];
            const GridPosition& to = path[i];

            if (from.row == to.row)
            {
                const int left = from.col < to.col
                    ? from.col
                    : to.col;
                const int right = from.col > to.col
                    ? from.col
                    : to.col;

                for (int col = left; col <= right; col++)
                {
                    layout[from.row][col] = TileType::Ground;
                }
            }
            else if (from.col == to.col)
            {
                const int top = from.row < to.row
                    ? from.row
                    : to.row;
                const int bottom = from.row > to.row
                    ? from.row
                    : to.row;

                for (int row = top; row <= bottom; row++)
                {
                    layout[row][from.col] = TileType::Ground;
                }
            }
        }
    }

    for (const std::vector<GridPosition>& path : level.paths)
    {
        if (!path.empty())
        {
            layout[path.front().row][path.front().col] = TileType::Entrance;
            layout[path.back().row][path.back().col] = TileType::Base;
        }
    }

    for (int row = 0; row < GameConfig::MAP_ROWS; row++)
    {
        for (int col = 0; col < GameConfig::MAP_COLS; col++)
        {
            tiles[row][col] = Tile(row, col, layout[row][col]);
        }
    }
}

void GameMap::draw() const
{
    const int mapRight =
        GameConfig::MAP_LEFT +
        GameConfig::MAP_COLS * GameConfig::TILE_SIZE;

    const int mapBottom =
        GameConfig::MAP_TOP +
        GameConfig::MAP_ROWS * GameConfig::TILE_SIZE;

    // 棋盘阴影与外框，突出战术平台从背景中抬起的层次。
    setfillcolor(RGB(8, 12, 16));
    solidrectangle(
        GameConfig::MAP_LEFT - 15,
        GameConfig::MAP_TOP + 12,
        mapRight + 15,
        mapBottom + 18);

    setfillcolor(RGB(31, 40, 47));
    solidrectangle(
        GameConfig::MAP_LEFT - 12,
        GameConfig::MAP_TOP - 12,
        mapRight + 12,
        mapBottom + 12);

    setlinecolor(RGB(91, 111, 122));
    setlinestyle(PS_SOLID, 2);
    rectangle(
        GameConfig::MAP_LEFT - 12,
        GameConfig::MAP_TOP - 12,
        mapRight + 12,
        mapBottom + 12);

    setlinecolor(RGB(61, 201, 220));
    setlinestyle(PS_SOLID, 3);
    line(GameConfig::MAP_LEFT - 12, GameConfig::MAP_TOP - 12,
         GameConfig::MAP_LEFT + 115, GameConfig::MAP_TOP - 12);
    line(mapRight - 115, mapBottom + 12,
         mapRight + 12, mapBottom + 12);

    for (int row = 0; row < GameConfig::MAP_ROWS; row++)
    {
        for (int col = 0; col < GameConfig::MAP_COLS; col++)
        {
            tiles[row][col].draw();
        }
    }

    // 坐标刻度让地图更像战术终端，而不是普通二维数组。
    setbkmode(TRANSPARENT);
    settextcolor(RGB(106, 127, 137));
    settextstyle(12, 0, _T("Consolas"));

    for (int col = 0; col < GameConfig::MAP_COLS; col++)
    {
        TCHAR columnText[3];
        columnText[0] = static_cast<TCHAR>(_T('A') + col);
        columnText[1] = _T('\0');

        const int x =
            GameConfig::MAP_LEFT +
            col * GameConfig::TILE_SIZE +
            GameConfig::TILE_SIZE / 2 -
            textwidth(columnText) / 2;

        outtextxy(x, GameConfig::MAP_TOP - 29, columnText);
    }

    for (int row = 0; row < GameConfig::MAP_ROWS; row++)
    {
        TCHAR rowText[4];
        _stprintf_s(rowText, _countof(rowText), _T("%02d"), row + 1);

        const int y =
            GameConfig::MAP_TOP +
            row * GameConfig::TILE_SIZE +
            GameConfig::TILE_SIZE / 2 -
            textheight(rowText) / 2;

        outtextxy(GameConfig::MAP_LEFT - 37, y, rowText);
    }
}

bool GameMap::isValidGrid(int row, int col) const
{
    return row >= 0 && row < GameConfig::MAP_ROWS &&
           col >= 0 && col < GameConfig::MAP_COLS;
}

bool GameMap::screenToGrid(int x, int y, int& row, int& col) const
{
    const int localX = x - GameConfig::MAP_LEFT;
    const int localY = y - GameConfig::MAP_TOP;

    if (localX < 0 || localY < 0)
    {
        return false;
    }

    col = localX / GameConfig::TILE_SIZE;
    row = localY / GameConfig::TILE_SIZE;
    return isValidGrid(row, col);
}

TileType GameMap::getTileType(int row, int col) const
{
    return isValidGrid(row, col)
        ? tiles[row][col].getType()
        : TileType::Blocked;
}

POINT GameMap::getTileCenter(int row, int col) const
{
    POINT point;
    point.x = GameConfig::MAP_LEFT + col * GameConfig::TILE_SIZE + GameConfig::TILE_SIZE / 2;
    point.y = GameConfig::MAP_TOP + row * GameConfig::TILE_SIZE + GameConfig::TILE_SIZE / 2;
    return point;
}

bool GameMap::canDeployGround(int row, int col) const
{
    return isValidGrid(row, col) && tiles[row][col].canDeployGround();
}

bool GameMap::canDeployHighGround(int row, int col) const
{
    return isValidGrid(row, col) && tiles[row][col].canDeployHighGround();
}

bool GameMap::isOccupied(int row, int col) const
{
    return isValidGrid(row, col) && tiles[row][col].isOccupied();
}

bool GameMap::setOccupied(int row, int col, bool value)
{
    if (!isValidGrid(row, col))
    {
        return false;
    }

    tiles[row][col].setOccupied(value);
    return true;
}

void GameMap::clearOccupancy()
{
    for (int row = 0; row < GameConfig::MAP_ROWS; row++)
    {
        for (int col = 0; col < GameConfig::MAP_COLS; col++)
        {
            tiles[row][col].setOccupied(false);
        }
    }
}

void GameMap::drawSelectedTile(int row, int col) const
{
    if (!isValidGrid(row, col))
    {
        return;
    }

    const int left = GameConfig::MAP_LEFT + col * GameConfig::TILE_SIZE;
    const int top = GameConfig::MAP_TOP + row * GameConfig::TILE_SIZE;

    const int right = left + GameConfig::TILE_SIZE;
    const int bottom = top + GameConfig::TILE_SIZE;
    const int length = 15;

    setlinecolor(RGB(255, 211, 82));
    setlinestyle(PS_SOLID, 4);

    line(left + 4, top + 4, left + 4 + length, top + 4);
    line(left + 4, top + 4, left + 4, top + 4 + length);
    line(right - 4, top + 4, right - 4 - length, top + 4);
    line(right - 4, top + 4, right - 4, top + 4 + length);
    line(left + 4, bottom - 4, left + 4 + length, bottom - 4);
    line(left + 4, bottom - 4, left + 4, bottom - 4 - length);
    line(right - 4, bottom - 4, right - 4 - length, bottom - 4);
    line(right - 4, bottom - 4, right - 4, bottom - 4 - length);
}

void GameMap::drawHoverTile(int row, int col) const
{
    if (!isValidGrid(row, col))
    {
        return;
    }

    const int left = GameConfig::MAP_LEFT + col * GameConfig::TILE_SIZE;
    const int top = GameConfig::MAP_TOP + row * GameConfig::TILE_SIZE;

    const int centerX = left + GameConfig::TILE_SIZE / 2;
    const int centerY = top + GameConfig::TILE_SIZE / 2;

    setlinecolor(RGB(93, 215, 232));
    setlinestyle(PS_DOT, 1);
    rectangle(left + 7, top + 7,
              left + GameConfig::TILE_SIZE - 7,
              top + GameConfig::TILE_SIZE - 7);

    setlinestyle(PS_SOLID, 2);
    line(centerX - 7, centerY, centerX + 7, centerY);
    line(centerX, centerY - 7, centerX, centerY + 7);
}

#include "tile.h"

#include "config.h"

namespace
{
    void drawCornerBolt(int x, int y)
    {
        setfillcolor(RGB(118, 130, 136));
        setlinecolor(RGB(35, 42, 46));
        fillcircle(x, y, 2);
    }

    void drawRouteMark(int left, int top, int right, int bottom)
    {
        const int centerX = (left + right) / 2;
        const int centerY = (top + bottom) / 2;

        setlinecolor(RGB(132, 151, 145));
        setlinestyle(PS_SOLID, 2);
        line(centerX - 13, centerY, centerX + 13, centerY);
        line(centerX + 6, centerY - 6, centerX + 13, centerY);
        line(centerX + 6, centerY + 6, centerX + 13, centerY);

        setfillcolor(RGB(73, 88, 83));
        solidcircle(centerX - 17, centerY, 2);
    }
}

Tile::Tile()
    : row(0),
      col(0),
      type(TileType::Blocked),
      occupied(false)
{
}

Tile::Tile(int row, int col, TileType type)
    : row(row),
      col(col),
      type(type),
      occupied(false)
{
}

void Tile::draw() const
{
    const int left =
        GameConfig::MAP_LEFT + col * GameConfig::TILE_SIZE;

    const int top =
        GameConfig::MAP_TOP + row * GameConfig::TILE_SIZE;

    const int right = left + GameConfig::TILE_SIZE;
    const int bottom = top + GameConfig::TILE_SIZE;

    // 所有地块先绘制深色底座，让棋盘拥有统一的机械拼装感。
    setfillcolor(RGB(24, 30, 34));
    solidrectangle(left, top, right, bottom);

    if (type == TileType::Ground)
    {
        setfillcolor(RGB(78, 92, 86));
        solidrectangle(left + 2, top + 2, right - 2, bottom - 2);

        setlinecolor(RGB(112, 130, 122));
        line(left + 3, top + 3, right - 3, top + 3);
        setlinecolor(RGB(52, 63, 59));
        line(left + 3, bottom - 3, right - 3, bottom - 3);

        drawRouteMark(left, top, right, bottom);
    }
    else if (type == TileType::HighGround)
    {
        // 高台使用亮顶面和深色侧面表现高度差。
        setfillcolor(RGB(41, 49, 62));
        solidrectangle(left + 2, top + 7, right - 2, bottom - 2);

        setfillcolor(RGB(91, 104, 126));
        solidrectangle(left + 3, top + 3, right - 3, bottom - 9);

        setlinecolor(RGB(137, 151, 176));
        line(left + 4, top + 4, right - 4, top + 4);
        line(left + 4, top + 4, left + 4, bottom - 10);

        setlinecolor(RGB(55, 64, 79));
        line(left + 5, bottom - 9, right - 5, bottom - 9);

        drawCornerBolt(left + 10, top + 11);
        drawCornerBolt(right - 10, top + 11);
        drawCornerBolt(left + 10, bottom - 17);
        drawCornerBolt(right - 10, bottom - 17);

        setlinecolor(RGB(115, 130, 154));
        rectangle(left + 17, top + 17, right - 17, bottom - 23);
    }
    else if (type == TileType::Blocked)
    {
        setfillcolor(RGB(35, 42, 47));
        solidrectangle(left + 2, top + 2, right - 2, bottom - 2);

        setlinecolor(RGB(48, 57, 62));
        setlinestyle(PS_SOLID, 1);

        for (int offset = -40; offset <= 80; offset += 14)
        {
            int x1 = left + offset;
            int y1 = bottom - 3;
            int x2 = x1 + 42;
            int y2 = top + 3;

            if (x1 < left + 3)
            {
                y1 -= left + 3 - x1;
                x1 = left + 3;
            }

            if (x2 > right - 3)
            {
                y2 += x2 - (right - 3);
                x2 = right - 3;
            }

            line(x1, y1, x2, y2);
        }

        setlinecolor(RGB(73, 82, 87));
        rectangle(left + 6, top + 6, right - 6, bottom - 6);
    }
    else if (type == TileType::Entrance)
    {
        setfillcolor(RGB(91, 46, 37));
        solidrectangle(left + 2, top + 2, right - 2, bottom - 2);

        setfillcolor(RGB(207, 91, 54));
        solidrectangle(left + 7, top + 7, right - 7, bottom - 7);

        setlinecolor(RGB(255, 194, 88));
        setlinestyle(PS_SOLID, 3);
        for (int x = left + 9; x < right - 12; x += 14)
        {
            line(x, bottom - 9, x + 11, top + 9);
        }

        setfillcolor(RGB(93, 38, 31));
        solidrectangle(left + 12, top + 18, right - 12, bottom - 18);

        setbkmode(TRANSPARENT);
        settextcolor(WHITE);
        settextstyle(15, 0, _T("微软雅黑"));
        outtextxy(left + 15, top + 21, _T("入口"));
    }
    else if (type == TileType::Base)
    {
        setfillcolor(RGB(34, 67, 91));
        solidrectangle(left + 2, top + 2, right - 2, bottom - 2);

        setfillcolor(RGB(48, 128, 166));
        solidrectangle(left + 6, top + 6, right - 6, bottom - 6);

        const int centerX = (left + right) / 2;
        const int centerY = (top + bottom) / 2;

        setlinecolor(RGB(151, 232, 255));
        setlinestyle(PS_SOLID, 2);
        circle(centerX, centerY, 18);
        circle(centerX, centerY, 11);
        line(centerX - 22, centerY, centerX + 22, centerY);
        line(centerX, centerY - 22, centerX, centerY + 22);

        setfillcolor(RGB(211, 249, 255));
        solidcircle(centerX, centerY, 4);
    }

    setlinecolor(RGB(18, 23, 27));
    setlinestyle(PS_SOLID, 1);
    rectangle(left, top, right, bottom);

    if (occupied)
    {
        setfillcolor(RGB(242, 187, 67));
        solidrectangle(right - 11, top + 4, right - 4, top + 11);
    }
}

TileType Tile::getType() const
{
    return type;
}

bool Tile::isOccupied() const
{
    return occupied;
}

void Tile::setOccupied(bool value)
{
    occupied = value;
}

bool Tile::canDeployGround() const
{
    return type == TileType::Ground && !occupied;
}

bool Tile::canDeployHighGround() const
{
    return type == TileType::HighGround && !occupied;
}

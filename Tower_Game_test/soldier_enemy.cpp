#include "soldier_enemy.h"

SoldierEnemy::SoldierEnemy(const std::vector<POINT>& path)
    : Enemy(path, 55.0f, 42, 42, 140, 32, 1000)
{
}

void SoldierEnemy::draw() const
{
    const int drawX = static_cast<int>(x);
    const int drawY = static_cast<int>(y);

    // 装甲兵：阴影、护盾、装甲躯干和头部标识。
    setfillcolor(RGB(12, 16, 19));
    solidellipse(drawX - 25, drawY + 13, drawX + 25, drawY + 24);

    POINT body[6] =
    {
        { drawX - 15, drawY - 15 },
        { drawX + 13, drawY - 15 },
        { drawX + 20, drawY - 5 },
        { drawX + 16, drawY + 17 },
        { drawX - 13, drawY + 17 },
        { drawX - 19, drawY - 3 }
    };

    setfillcolor(RGB(158, 57, 55));
    solidpolygon(body, 6);
    setlinecolor(RGB(239, 113, 92));
    setlinestyle(PS_SOLID, 2);
    polygon(body, 6);

    setfillcolor(RGB(83, 39, 40));
    solidrectangle(drawX - 26, drawY - 10, drawX - 15, drawY + 14);
    setlinecolor(RGB(223, 115, 83));
    rectangle(drawX - 26, drawY - 10, drawX - 15, drawY + 14);

    setfillcolor(RGB(47, 51, 55));
    solidcircle(drawX, drawY - 15, 8);
    setlinecolor(RGB(226, 95, 74));
    circle(drawX, drawY - 15, 8);

    setfillcolor(RGB(255, 184, 79));
    solidrectangle(drawX - 4, drawY - 17, drawX + 5, drawY - 14);

    setlinecolor(RGB(246, 196, 118));
    line(drawX - 7, drawY + 2, drawX + 9, drawY + 2);

    drawHealthBar();
}

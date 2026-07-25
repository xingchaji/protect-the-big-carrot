#include "runner_enemy.h"

RunnerEnemy::RunnerEnemy(const std::vector<POINT>& path)
    : Enemy(path, 90.0f, 30, 30, 80, 18, 750)
{
}

void RunnerEnemy::draw() const
{
    const int drawX = static_cast<int>(x);
    const int drawY = static_cast<int>(y);

    // 高速单位使用运动残影和尖锐轮廓体现速度感。
    setlinecolor(RGB(126, 76, 30));
    setlinestyle(PS_SOLID, 2);
    line(drawX - 27, drawY - 7, drawX - 15, drawY - 7);
    line(drawX - 31, drawY, drawX - 17, drawY);
    line(drawX - 25, drawY + 7, drawX - 13, drawY + 7);

    setfillcolor(RGB(15, 18, 20));
    solidellipse(drawX - 18, drawY + 9, drawX + 19, drawY + 18);

    POINT body[4] =
    {
        { drawX, drawY - 18 },
        { drawX + 18, drawY },
        { drawX, drawY + 18 },
        { drawX - 18, drawY }
    };

    setfillcolor(RGB(221, 132, 42));
    solidpolygon(body, 4);
    setlinecolor(RGB(255, 201, 96));
    setlinestyle(PS_SOLID, 2);
    polygon(body, 4);

    setfillcolor(RGB(68, 48, 32));
    solidcircle(drawX, drawY, 8);

    setfillcolor(RGB(238, 245, 238));
    solidcircle(drawX + 2, drawY - 1, 3);

    drawHealthBar();
}

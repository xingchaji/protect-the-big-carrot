#include "operator.h"

#include "config.h"

#include <graphics.h>

Operator::Operator(int row, int col, int cost, int maxHp)
    : row(row),
      col(col),
      cost(cost),
      hp(maxHp),
      maxHp(maxHp)
{
}

int Operator::getRow() const
{
    return row;
}

int Operator::getCol() const
{
    return col;
}

int Operator::getCost() const
{
    return cost;
}

int Operator::getHp() const
{
    return hp;
}

int Operator::getMaxHp() const
{
    return maxHp;
}

void Operator::takeDamage(int damage)
{
    if (damage <= 0 || isDead())
    {
        return;
    }

    hp -= damage;

    if (hp < 0)
    {
        hp = 0;
    }
}

bool Operator::isDead() const
{
    return hp <= 0;
}

int Operator::getCenterX() const
{
    return GameConfig::MAP_LEFT +
           col * GameConfig::TILE_SIZE +
           GameConfig::TILE_SIZE / 2;
}

int Operator::getCenterY() const
{
    return GameConfig::MAP_TOP +
           row * GameConfig::TILE_SIZE +
           GameConfig::TILE_SIZE / 2;
}

void Operator::drawHealthBar() const
{
    const int centerX = getCenterX();
    const int centerY = getCenterY();
    const int barWidth = 50;
    const int barHeight = 6;
    const int left = centerX - barWidth / 2;
    const int top = centerY - 67;

    setfillcolor(RGB(15, 20, 23));
    solidrectangle(
        left - 2,
        top - 2,
        left + barWidth + 2,
        top + barHeight + 2);

    setfillcolor(RGB(54, 61, 64));
    solidrectangle(left, top, left + barWidth, top + barHeight);

    const int hpWidth = maxHp > 0 ? barWidth * hp / maxHp : 0;

    if (hpWidth > 0)
    {
        setfillcolor(hp * 3 > maxHp
            ? RGB(80, 210, 95)
            : RGB(245, 95, 75));
        solidrectangle(left, top, left + hpWidth, top + barHeight);
    }

    setlinecolor(RGB(134, 147, 151));
    rectangle(left, top, left + barWidth, top + barHeight);
}

#include "enemy.h"

#include <cmath>

#include "operator.h"

Enemy::Enemy(
    const std::vector<POINT>& path,
    float speed,
    int width,
    int height,
    int maxHp,
    int attackDamage,
    DWORD attackInterval)
    : x(0.0f),
      y(0.0f),
      speed(speed),
      width(width),
      height(height),
      hp(maxHp),
      maxHp(maxHp),
      attackDamage(attackDamage),
      attackInterval(attackInterval),
      attackTimer(0),
      path(path),
      targetIndex(0),
      reachedGoal(false),
      blocker(nullptr)
{
    reset();
}

void Enemy::reset()
{
    hp = maxHp;
    blocker = nullptr;
    attackTimer = 0;

    if (path.empty())
    {
        x = 0.0f;
        y = 0.0f;
        targetIndex = 0;
        reachedGoal = true;
        return;
    }

    x = static_cast<float>(path[0].x);
    y = static_cast<float>(path[0].y);

    if (path.size() >= 2)
    {
        targetIndex = 1;
        reachedGoal = false;
    }
    else
    {
        targetIndex = 0;
        reachedGoal = true;
    }
}

void Enemy::update(DWORD deltaTime)
{
    if (isDead() || reachedGoal || path.empty())
    {
        return;
    }

    if (isBlocked())
    {
        if (blocker->isDead())
        {
            clearBlocker();
            attackTimer = 0;
            return;
        }

        attackTimer += deltaTime;

        if (attackTimer >= attackInterval)
        {
            blocker->takeDamage(attackDamage);
            attackTimer -= attackInterval;

            if (blocker->isDead())
            {
                clearBlocker();
                attackTimer = 0;
            }
        }

        return;
    }

    attackTimer = 0;

    const POINT& target = path[targetIndex];

    const float dx = static_cast<float>(target.x) - x;
    const float dy = static_cast<float>(target.y) - y;

    const float distance = std::sqrt(dx * dx + dy * dy);
    const float moveDistance = speed * deltaTime / 1000.0f;

    if (distance <= moveDistance)
    {
        x = static_cast<float>(target.x);
        y = static_cast<float>(target.y);

        targetIndex++;

        if (targetIndex >= path.size())
        {
            reachedGoal = true;
        }

        return;
    }

    if (distance > 0.0f)
    {
        x += dx / distance * moveDistance;
        y += dy / distance * moveDistance;
    }
}

void Enemy::takeDamage(int damage)
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

bool Enemy::isDead() const
{
    return hp <= 0;
}

bool Enemy::hasReachedGoal() const
{
    return reachedGoal;
}

float Enemy::getX() const
{
    return x;
}

float Enemy::getY() const
{
    return y;
}

bool Enemy::isBlocked() const
{
    return blocker != nullptr;
}

Operator* Enemy::getBlocker() const
{
    return blocker;
}

void Enemy::setBlocker(Operator* newBlocker)
{
    blocker = newBlocker;
}

void Enemy::clearBlocker()
{
    blocker = nullptr;
}

void Enemy::drawHealthBar() const
{
    const int centerX = static_cast<int>(x);
    const int centerY = static_cast<int>(y);

    const int barWidth = width + 12;
    const int barHeight = 6;

    const int left = centerX - barWidth / 2;
    const int top = centerY - height / 2 - 13;

    setfillcolor(RGB(15, 20, 23));
    solidrectangle(
        left - 2,
        top - 2,
        left + barWidth + 2,
        top + barHeight + 2);

    setfillcolor(RGB(54, 61, 64));
    solidrectangle(left, top, left + barWidth, top + barHeight);

    int hpWidth = 0;

    if (maxHp > 0)
    {
        hpWidth = barWidth * hp / maxHp;
    }

    if (hpWidth > 0)
    {
        setfillcolor(hp * 3 > maxHp
            ? RGB(78, 215, 126)
            : RGB(238, 86, 72));
        solidrectangle(
            left,
            top,
            left + hpWidth,
            top + barHeight);
    }

    setlinecolor(RGB(134, 147, 151));
    rectangle(
        left,
        top,
        left + barWidth,
        top + barHeight);

    if (isBlocked())
    {
        setfillcolor(YELLOW);
        solidcircle(centerX, top - 5, 3);
    }
}

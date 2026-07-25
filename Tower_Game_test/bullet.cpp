#include "bullet.h"

#include <cmath>

#include "enemy.h"

Bullet::Bullet(
    float startX,
    float startY,
    Enemy* target,
    int damage,
    float speed)
    : x(startX),
      y(startY),
      speed(speed),
      damage(damage),
      target(target),
      valid(target != nullptr)
{
}

void Bullet::update(DWORD deltaTime)
{
    if (!valid)
    {
        return;
    }

    if (target == nullptr ||
        target->isDead() ||
        target->hasReachedGoal())
    {
        valid = false;
        return;
    }

    const float dx = target->getX() - x;
    const float dy = target->getY() - y;

    const float distance = std::sqrt(dx * dx + dy * dy);
    const float moveDistance = speed * deltaTime / 1000.0f;

    if (distance <= moveDistance || distance <= 6.0f)
    {
        target->takeDamage(damage);
        valid = false;
        return;
    }

    if (distance > 0.0f)
    {
        x += dx / distance * moveDistance;
        y += dy / distance * moveDistance;
    }
}

void Bullet::draw() const
{
    if (!valid)
    {
        return;
    }

    const int drawX = static_cast<int>(x);
    const int drawY = static_cast<int>(y);

    float directionX = 1.0f;
    float directionY = 0.0f;

    if (target != nullptr)
    {
        const float dx = target->getX() - x;
        const float dy = target->getY() - y;
        const float distance = std::sqrt(dx * dx + dy * dy);

        if (distance > 0.0f)
        {
            directionX = dx / distance;
            directionY = dy / distance;
        }
    }

    setlinecolor(RGB(54, 125, 163));
    setlinestyle(PS_SOLID, 3);
    line(
        drawX - static_cast<int>(directionX * 14.0f),
        drawY - static_cast<int>(directionY * 14.0f),
        drawX,
        drawY);

    setfillcolor(RGB(73, 177, 222));
    solidcircle(drawX, drawY, 7);

    setfillcolor(RGB(221, 249, 255));
    solidcircle(drawX, drawY, 3);
}

bool Bullet::isValid() const
{
    return valid;
}

Enemy* Bullet::getTarget() const
{
    return target;
}

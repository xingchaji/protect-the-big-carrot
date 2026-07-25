#include "sniper.h"

#include <graphics.h>

#include "enemy.h"
#include "bullet.h"

constexpr int Sniper::COST;

Sniper::Sniper(int row, int col)
    : Operator(row, col, COST, 350),
    attackDamage(35),
    attackInterval(850),
    attackTimer(0),
    attackRange(260.0f),
    attackEffectTimer(0)
{
    idleAnimation.load(
        _T("assets\\sniper_idle"),
        _T("sniper_idle"),
        32,
        166,
        true);

    attackAnimation.load(
        _T("assets\\sniper_attack"),
        _T("sniper_attack"),
        17,
        50,
        false);
}

Enemy* Sniper::findTarget(
    const std::vector<Enemy*>& enemies) const
{
    Enemy* nearestEnemy = nullptr;
    float nearestDistanceSquared = attackRange * attackRange;

    const float centerX = static_cast<float>(getCenterX());
    const float centerY = static_cast<float>(getCenterY());

    for (Enemy* enemy : enemies)
    {
        if (enemy == nullptr ||
            enemy->isDead() ||
            enemy->hasReachedGoal())
        {
            continue;
        }

        const float dx = enemy->getX() - centerX;
        const float dy = enemy->getY() - centerY;
        const float distanceSquared = dx * dx + dy * dy;

        if (distanceSquared <= nearestDistanceSquared)
        {
            nearestDistanceSquared = distanceSquared;
            nearestEnemy = enemy;
        }
    }

    return nearestEnemy;
}

void Sniper::update(
    DWORD deltaTime,
    const std::vector<Enemy*>& enemies,
    std::vector<Bullet*>& bullets)
{
    idleAnimation.update(deltaTime);

    if (attackEffectTimer > 0)
    {
        attackAnimation.update(deltaTime);
    }

    if (attackEffectTimer > deltaTime)
    {
        attackEffectTimer -= deltaTime;
    }
    else
    {
        attackEffectTimer = 0;
    }

    if (attackTimer < attackInterval)
    {
        attackTimer += deltaTime;

        if (attackTimer > attackInterval)
        {
            attackTimer = attackInterval;
        }
    }

    if (attackTimer < attackInterval)
    {
        return;
    }

    Enemy* target = findTarget(enemies);

    if (target == nullptr)
    {
        attackTimer = attackInterval;
        return;
    }

    bullets.push_back(
        new Bullet(
            static_cast<float>(getCenterX()),
            static_cast<float>(getCenterY()),
            target,
            attackDamage,
            420.0f));

    attackTimer = 0;
    attackEffectTimer = attackInterval;
    attackAnimation.restart();
}

void Sniper::draw() const
{
    const int centerX = getCenterX();
    const int centerY = getCenterY();

    setfillcolor(RGB(17, 24, 28));
    solidellipse(centerX - 27, centerY + 15, centerX + 27, centerY + 27);
    setlinecolor(RGB(116, 146, 245));
    setlinestyle(PS_SOLID, 2);
    ellipse(centerX - 25, centerY + 11, centerX + 25, centerY + 24);

    const SpriteAnimation& animation =
        attackEffectTimer > 0
        ? attackAnimation
        : idleAnimation;

    if (animation.hasFrames())
    {
        animation.draw(
            centerX - 48,
            centerY - 68);
    }
    else
    {
        constexpr int RADIUS = 22;

        setfillcolor(RGB(100, 105, 205));
        setlinecolor(RGB(45, 45, 110));
        setlinestyle(PS_SOLID, 3);

        fillcircle(
            centerX,
            centerY,
            RADIUS);
    }

    drawHealthBar();
}

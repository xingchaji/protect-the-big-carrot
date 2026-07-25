#include "guard.h"

#include <graphics.h>

#include "enemy.h"

constexpr int Guard::COST;

Guard::Guard(int row, int col)
    : Operator(row, col, COST, 650),
    attackDamage(45),
    attackInterval(700),
    attackTimer(0),
    attackRange(105.0f),
    attackEffectTimer(0),
    blockRange(34.0f),
    blocking(false)
{
    idleAnimation.load(
        _T("assets\\guard_idle"),
        _T("guard_idle"),
        24,
        165,
        true);

    attackAnimation.load(
        _T("assets\\guard_attack"),
        _T("guard_attack"),
        10,
        70,
        false);
}

Enemy* Guard::findBlockedEnemy(
    const std::vector<Enemy*>& enemies)
{
    for (Enemy* enemy : enemies)
    {
        if (enemy == nullptr ||
            enemy->isDead() ||
            enemy->hasReachedGoal())
        {
            continue;
        }

        if (enemy->getBlocker() == this)
        {
            return enemy;
        }
    }

    return nullptr;
}

Enemy* Guard::findBlockCandidate(
    const std::vector<Enemy*>& enemies)
{
    Enemy* nearestEnemy = nullptr;
    float nearestDistanceSquared = blockRange * blockRange;

    const float centerX = static_cast<float>(getCenterX());
    const float centerY = static_cast<float>(getCenterY());

    for (Enemy* enemy : enemies)
    {
        if (enemy == nullptr ||
            enemy->isDead() ||
            enemy->hasReachedGoal() ||
            enemy->isBlocked())
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

Enemy* Guard::findTarget(
    const std::vector<Enemy*>& enemies)
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

void Guard::update(
    DWORD deltaTime,
    const std::vector<Enemy*>& enemies,
    std::vector<Bullet*>& bullets)
{
    (void)bullets;

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

    Enemy* blockedEnemy = findBlockedEnemy(enemies);

    if (blockedEnemy == nullptr)
    {
        Enemy* candidate = findBlockCandidate(enemies);

        if (candidate != nullptr)
        {
            candidate->setBlocker(this);
            blockedEnemy = candidate;
        }
    }

    blocking = blockedEnemy != nullptr;

    Enemy* target = blockedEnemy;

    if (target == nullptr)
    {
        target = findTarget(enemies);
    }

    if (attackTimer < attackInterval)
    {
        return;
    }

    if (target == nullptr)
    {
        attackTimer = attackInterval;
        return;
    }

    target->takeDamage(attackDamage);

    attackTimer = 0;
    attackEffectTimer = attackInterval;
    attackAnimation.restart();
}

void Guard::draw() const
{
    const int centerX = getCenterX();
    const int centerY = getCenterY();

    setfillcolor(RGB(17, 24, 28));
    solidellipse(centerX - 27, centerY + 15, centerX + 27, centerY + 27);
    setlinecolor(RGB(65, 196, 170));
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
        constexpr int HALF_SIZE = 23;

        setfillcolor(RGB(45, 155, 145));
        setlinecolor(RGB(20, 80, 75));
        setlinestyle(PS_SOLID, 3);

        fillroundrect(
            centerX - HALF_SIZE,
            centerY - HALF_SIZE,
            centerX + HALF_SIZE,
            centerY + HALF_SIZE,
            10,
            10);
    }

    if (blocking)
    {
        setlinecolor(RGB(255, 90, 70));
        setlinestyle(PS_SOLID, 3);

        circle(
            centerX,
            centerY,
            27);
    }

    drawHealthBar();
}

#include "sprite_animation.h"

#include <tchar.h>

SpriteAnimation::SpriteAnimation()
    : currentFrame(0),
    frameDuration(100),
    frameTimer(0),
    loop(true),
    finished(false)
{
}

SpriteAnimation::~SpriteAnimation()
{
    clear();
}

void SpriteAnimation::clear()
{
    for (IMAGE* image : colorFrames)
    {
        delete image;
    }

    for (IMAGE* image : maskFrames)
    {
        delete image;
    }

    colorFrames.clear();
    maskFrames.clear();

    currentFrame = 0;
    frameTimer = 0;
    finished = false;
}

void SpriteAnimation::load(
    const TCHAR* folder,
    const TCHAR* prefix,
    int frameCount,
    DWORD newFrameDuration,
    bool shouldLoop)
{
    clear();

    frameDuration =
        newFrameDuration > 0
        ? newFrameDuration
        : 1;

    loop = shouldLoop;

    for (int i = 0; i < frameCount; i++)
    {
        TCHAR colorPath[MAX_PATH];
        TCHAR maskPath[MAX_PATH];

        _stprintf_s(
            colorPath,
            _countof(colorPath),
            _T("%s\\%s_%02d.png"),
            folder,
            prefix,
            i);

        _stprintf_s(
            maskPath,
            _countof(maskPath),
            _T("%s\\%s_%02d_mask.png"),
            folder,
            prefix,
            i);

        IMAGE* colorImage = new IMAGE();
        IMAGE* maskImage = new IMAGE();

        loadimage(colorImage, colorPath);
        loadimage(maskImage, maskPath);

        colorFrames.push_back(colorImage);
        maskFrames.push_back(maskImage);
    }
}

void SpriteAnimation::update(DWORD deltaTime)
{
    if (!hasFrames() || finished)
    {
        return;
    }

    frameTimer += deltaTime;

    while (frameTimer >= frameDuration)
    {
        frameTimer -= frameDuration;

        if (currentFrame + 1 <
            static_cast<int>(colorFrames.size()))
        {
            currentFrame++;
        }
        else if (loop)
        {
            currentFrame = 0;
        }
        else
        {
            currentFrame =
                static_cast<int>(colorFrames.size()) - 1;

            finished = true;
            break;
        }
    }
}

void SpriteAnimation::restart()
{
    currentFrame = 0;
    frameTimer = 0;
    finished = false;
}

bool SpriteAnimation::hasFrames() const
{
    return !colorFrames.empty() &&
        colorFrames.size() == maskFrames.size();
}

bool SpriteAnimation::isFinished() const
{
    return finished;
}

void SpriteAnimation::draw(int x, int y) const
{
    if (!hasFrames())
    {
        return;
    }

    // 第一步：使用黑白遮罩清除角色区域
    putimage(
        x,
        y,
        maskFrames[currentFrame],
        SRCAND);

    // 第二步：绘制角色彩色图片
    putimage(
        x,
        y,
        colorFrames[currentFrame],
        SRCPAINT);
}
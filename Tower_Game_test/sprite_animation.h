#pragma once

#include <graphics.h>
#include <windows.h>
#include <vector>

class SpriteAnimation
{
private:
    std::vector<IMAGE*> colorFrames;
    std::vector<IMAGE*> maskFrames;

    int currentFrame;
    DWORD frameDuration;
    DWORD frameTimer;

    bool loop;
    bool finished;

    void clear();

public:
    SpriteAnimation();
    ~SpriteAnimation();

    SpriteAnimation(const SpriteAnimation&) = delete;
    SpriteAnimation& operator=(const SpriteAnimation&) = delete;

    void load(
        const TCHAR* folder,
        const TCHAR* prefix,
        int frameCount,
        DWORD frameDuration,
        bool loop);

    void update(DWORD deltaTime);
    void restart();

    bool hasFrames() const;
    bool isFinished() const;

    void draw(int x, int y) const;
};
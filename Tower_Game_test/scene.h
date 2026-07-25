#pragma once

#include <graphics.h>
#include <windows.h>

enum class SceneType
{
    None,
    Menu,
    Game,
    Result,
    Exit
};

struct SceneRequest
{
    SceneType target = SceneType::None;
    bool victory = false;
    int levelIndex = 0;
};

class Scene
{
protected:
    SceneRequest request;

public:
    virtual ~Scene() = default;

    virtual void onEnter() {}
    virtual void onExit() {}

    virtual void handleInput(const ExMessage& msg) = 0;
    virtual void update(DWORD deltaTime) = 0;
    virtual void draw() const = 0;

    bool hasRequest() const
    {
        return request.target != SceneType::None;
    }

    SceneRequest getRequest() const
    {
        return request;
    }

    void clearRequest()
    {
        request.target = SceneType::None;
        request.victory = false;
        request.levelIndex = 0;
    }
};

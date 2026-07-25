#pragma once

#include "scene.h"

class SceneManager
{
private:
    Scene* currentScene;
    bool running;

    void switchScene(
        SceneType target,
        bool victory = false,
        int levelIndex = 0);
    void processRequest();

public:
    SceneManager();
    ~SceneManager();

    void start(SceneType firstScene);
    void handleInput(const ExMessage& msg);
    void update(DWORD deltaTime);
    void draw() const;

    bool isRunning() const;
};

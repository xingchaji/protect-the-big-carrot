#pragma once

#include "scene.h"

class ResultScene : public Scene
{
private:
    bool victory;
    int levelIndex;
    bool hoverPrimary;
    bool hoverMenu;
    bool hoverExit;

    bool hasNextLevel() const;
    void requestPrimaryAction();

public:
    ResultScene(bool victory, int levelIndex);

    void handleInput(const ExMessage& msg) override;
    void update(DWORD deltaTime) override;
    void draw() const override;
};

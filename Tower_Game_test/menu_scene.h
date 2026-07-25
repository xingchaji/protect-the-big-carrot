#pragma once

#include "scene.h"

class MenuScene : public Scene
{
private:
    bool hoverStart;
    bool hoverExit;

public:
    MenuScene();

    void handleInput(const ExMessage& msg) override;
    void update(DWORD deltaTime) override;
    void draw() const override;
};

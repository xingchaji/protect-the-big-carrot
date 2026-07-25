#include <graphics.h>
#include <windows.h>
#include <chrono>

#include "config.h"
#include "scene_manager.h"

int main()
{
    ExMessage msg;

    HWND window = initgraph(
        GameConfig::WINDOW_WIDTH,
        GameConfig::WINDOW_HEIGHT);

    SetWindowText(
        window,
        _T("±£Œ¿¥Û¬‹≤∑"));

    setbkcolor(RGB(12, 17, 22));

    SceneManager sceneManager;
    sceneManager.start(SceneType::Menu);

    BeginBatchDraw();

    using Clock = std::chrono::steady_clock;

    auto lastUpdateTime = Clock::now();

    while (sceneManager.isRunning())
    {
        const auto frameStartTime = Clock::now();

        long long deltaMilliseconds =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                frameStartTime - lastUpdateTime)
            .count();

        lastUpdateTime = frameStartTime;

        if (deltaMilliseconds > 100)
        {
            deltaMilliseconds = 100;
        }

        if (deltaMilliseconds < 0)
        {
            deltaMilliseconds = 0;
        }

        const DWORD deltaTime =
            static_cast<DWORD>(deltaMilliseconds);

        while (peekmessage(&msg))
        {
            sceneManager.handleInput(msg);
        }

        sceneManager.update(deltaTime);

        cleardevice();
        sceneManager.draw();
        FlushBatchDraw();

        const auto frameEndTime = Clock::now();

        const long long elapsedMilliseconds =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                frameEndTime - frameStartTime)
            .count();

        const long long frameDuration =
            1000 / GameConfig::FPS;

        if (elapsedMilliseconds < frameDuration)
        {
            Sleep(
                static_cast<DWORD>(
                    frameDuration - elapsedMilliseconds));
        }
    }

    EndBatchDraw();
    closegraph();

    return 0;
}

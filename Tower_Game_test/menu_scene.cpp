#include "menu_scene.h"

#include <tchar.h>

#include "config.h"

namespace
{
    constexpr int START_LEFT = GameConfig::WINDOW_WIDTH / 2 - 175;
    constexpr int START_TOP = 360;
    constexpr int START_RIGHT = GameConfig::WINDOW_WIDTH / 2 + 175;
    constexpr int START_BOTTOM = 435;

    constexpr int EXIT_LEFT = GameConfig::WINDOW_WIDTH / 2 - 175;
    constexpr int EXIT_TOP = 465;
    constexpr int EXIT_RIGHT = GameConfig::WINDOW_WIDTH / 2 + 175;
    constexpr int EXIT_BOTTOM = 540;

    bool pointInside(
        int x,
        int y,
        int left,
        int top,
        int right,
        int bottom)
    {
        return x >= left && x <= right &&
               y >= top && y <= bottom;
    }

    void drawButton(
        int left,
        int top,
        int right,
        int bottom,
        const TCHAR* text,
        bool hovered)
    {
        POINT panel[6] =
        {
            { left + 16, top },
            { right, top },
            { right, bottom - 16 },
            { right - 16, bottom },
            { left, bottom },
            { left, top + 16 }
        };

        setfillcolor(
            hovered ? RGB(42, 73, 78)
                    : RGB(31, 40, 47));
        solidpolygon(panel, 6);

        setlinecolor(
            hovered ? RGB(88, 226, 235)
                    : RGB(89, 106, 116));
        setlinestyle(PS_SOLID, hovered ? 3 : 1);
        polygon(panel, 6);

        setfillcolor(
            hovered ? RGB(88, 226, 235)
                    : RGB(218, 166, 57));
        solidrectangle(left, top + 16, left + 5, bottom);

        if (hovered)
        {
            setlinecolor(RGB(255, 218, 93));
            line(right - 62, bottom - 7, right - 16, bottom - 7);
        }

        setbkmode(TRANSPARENT);
        settextcolor(WHITE);
        settextstyle(28, 0, _T("微软雅黑"));

        const int textX =
            left + (right - left - textwidth(text)) / 2;

        const int textY =
            top + (bottom - top - textheight(text)) / 2;

        outtextxy(textX, textY, text);
    }
}

MenuScene::MenuScene()
    : hoverStart(false),
      hoverExit(false)
{
}

void MenuScene::handleInput(const ExMessage& msg)
{
    if (msg.message == WM_MOUSEMOVE)
    {
        hoverStart = pointInside(
            msg.x,
            msg.y,
            START_LEFT,
            START_TOP,
            START_RIGHT,
            START_BOTTOM);

        hoverExit = pointInside(
            msg.x,
            msg.y,
            EXIT_LEFT,
            EXIT_TOP,
            EXIT_RIGHT,
            EXIT_BOTTOM);
    }
    else if (msg.message == WM_LBUTTONDOWN)
    {
        if (pointInside(
            msg.x,
            msg.y,
            START_LEFT,
            START_TOP,
            START_RIGHT,
            START_BOTTOM))
        {
            request.target = SceneType::Game;
            request.levelIndex = 0;
        }
        else if (pointInside(
            msg.x,
            msg.y,
            EXIT_LEFT,
            EXIT_TOP,
            EXIT_RIGHT,
            EXIT_BOTTOM))
        {
            request.target = SceneType::Exit;
        }
    }
    else if (msg.message == WM_KEYDOWN)
    {
        if (msg.vkcode == VK_RETURN)
        {
            request.target = SceneType::Game;
            request.levelIndex = 0;
        }
        else if (msg.vkcode == VK_ESCAPE)
        {
            request.target = SceneType::Exit;
        }
    }
    else if (msg.message == WM_CLOSE)
    {
        request.target = SceneType::Exit;
    }
}

void MenuScene::update(DWORD deltaTime)
{
    (void)deltaTime;
}

void MenuScene::draw() const
{
    setfillcolor(RGB(12, 17, 22));
    solidrectangle(
        0,
        0,
        GameConfig::WINDOW_WIDTH,
        GameConfig::WINDOW_HEIGHT);

    setlinecolor(RGB(22, 31, 38));
    setlinestyle(PS_SOLID, 1);

    for (int y = 0; y < GameConfig::WINDOW_HEIGHT; y += 36)
    {
        line(0, y, GameConfig::WINDOW_WIDTH, y);
    }

    for (int x = -500; x < GameConfig::WINDOW_WIDTH; x += 190)
    {
        line(x, GameConfig::WINDOW_HEIGHT,
             x + 520, 0);
    }

    setfillcolor(RGB(20, 28, 34));
    solidrectangle(0, 90, GameConfig::WINDOW_WIDTH, 300);

    setfillcolor(RGB(25, 35, 42));
    solidrectangle(0, 305, GameConfig::WINDOW_WIDTH, 310);

    setfillcolor(RGB(55, 198, 217));
    solidrectangle(0, 305, 465, 310);

    setfillcolor(RGB(224, 168, 54));
    solidrectangle(465, 305, 610, 310);

    // 两侧的工业警示纹理形成视觉边界。
    setlinecolor(RGB(99, 77, 38));
    setlinestyle(PS_SOLID, 5);
    for (int y = 345; y < 690; y += 28)
    {
        line(75, y, 105, y - 22);
        line(GameConfig::WINDOW_WIDTH - 105, y,
             GameConfig::WINDOW_WIDTH - 75, y - 22);
    }

    setbkmode(TRANSPARENT);

    const TCHAR title[] = _T("保卫大萝卜");

    settextcolor(RGB(238, 244, 246));
    settextstyle(64, 0, _T("微软雅黑"));

    const int titleX =
        (GameConfig::WINDOW_WIDTH - textwidth(title)) / 2;

    outtextxy(titleX, 125, title);

    const TCHAR operationCode[] = _T("TACTICAL DEFENSE // OPERATION R-01");
    settextcolor(RGB(80, 216, 229));
    settextstyle(16, 0, _T("Consolas"));
    outtextxy(
        (GameConfig::WINDOW_WIDTH - textwidth(operationCode)) / 2,
        205,
        operationCode);

    const TCHAR subtitle[] =
        _T("工业战术防御模拟系统");

    settextcolor(RGB(156, 173, 181));
    settextstyle(21, 0, _T("微软雅黑"));

    const int subtitleX =
        (GameConfig::WINDOW_WIDTH - textwidth(subtitle)) / 2;

    outtextxy(subtitleX, 245, subtitle);

    drawButton(
        START_LEFT,
        START_TOP,
        START_RIGHT,
        START_BOTTOM,
        _T("开始游戏"),
        hoverStart);

    drawButton(
        EXIT_LEFT,
        EXIT_TOP,
        EXIT_RIGHT,
        EXIT_BOTTOM,
        _T("退出游戏"),
        hoverExit);

    const TCHAR tip[] =
        _T("按 Enter 开始，按 Esc 退出");

    settextcolor(RGB(126, 143, 151));
    settextstyle(17, 0, _T("微软雅黑"));

    const int tipX =
        (GameConfig::WINDOW_WIDTH - textwidth(tip)) / 2;

    outtextxy(tipX, 690, tip);

    settextcolor(RGB(72, 91, 101));
    settextstyle(13, 0, _T("Consolas"));
    outtextxy(28, 856, _T("SYSTEM READY    EASYX RENDERER    1440 x 900    FPS 60"));

    setlinecolor(RGB(61, 79, 88));
    setlinestyle(PS_SOLID, 1);
    line(28, 842, GameConfig::WINDOW_WIDTH - 28, 842);
}

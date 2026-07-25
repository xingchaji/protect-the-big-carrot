#include "result_scene.h"

#include <tchar.h>

#include "config.h"
#include "level_data.h"

namespace
{
    constexpr int BUTTON_WIDTH = 370;
    constexpr int BUTTON_HEIGHT = 65;
    constexpr int BUTTON_LEFT =
        (GameConfig::WINDOW_WIDTH - BUTTON_WIDTH) / 2;
    constexpr int BUTTON_RIGHT = BUTTON_LEFT + BUTTON_WIDTH;

    constexpr int PRIMARY_TOP = 390;
    constexpr int PRIMARY_BOTTOM = PRIMARY_TOP + BUTTON_HEIGHT;
    constexpr int MENU_TOP = 480;
    constexpr int MENU_BOTTOM = MENU_TOP + BUTTON_HEIGHT;
    constexpr int EXIT_TOP = 570;
    constexpr int EXIT_BOTTOM = EXIT_TOP + BUTTON_HEIGHT;

    bool pointInside(int x, int y, int left, int top, int right, int bottom)
    {
        return x >= left && x <= right && y >= top && y <= bottom;
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
            { left + 14, top },
            { right, top },
            { right, bottom - 14 },
            { right - 14, bottom },
            { left, bottom },
            { left, top + 14 }
        };

        setfillcolor(hovered ? RGB(43, 73, 78) : RGB(31, 40, 47));
        solidpolygon(panel, 6);
        setlinecolor(hovered ? RGB(88, 226, 235) : RGB(89, 106, 116));
        setlinestyle(PS_SOLID, hovered ? 3 : 1);
        polygon(panel, 6);

        setfillcolor(hovered ? RGB(88, 226, 235) : RGB(218, 166, 57));
        solidrectangle(left, top + 14, left + 5, bottom);

        setbkmode(TRANSPARENT);
        settextcolor(WHITE);
        settextstyle(26, 0, _T("微软雅黑"));

        outtextxy(
            left + (right - left - textwidth(text)) / 2,
            top + (bottom - top - textheight(text)) / 2,
            text);
    }
}

ResultScene::ResultScene(bool victory, int levelIndex)
    : victory(victory),
      levelIndex(levelIndex),
      hoverPrimary(false),
      hoverMenu(false),
      hoverExit(false)
{
}

bool ResultScene::hasNextLevel() const
{
    return victory && levelIndex + 1 < getLevelCount();
}

void ResultScene::requestPrimaryAction()
{
    request.target = SceneType::Game;
    request.levelIndex = hasNextLevel()
        ? levelIndex + 1
        : levelIndex;
}

void ResultScene::handleInput(const ExMessage& msg)
{
    if (msg.message == WM_MOUSEMOVE)
    {
        hoverPrimary = pointInside(
            msg.x, msg.y,
            BUTTON_LEFT, PRIMARY_TOP,
            BUTTON_RIGHT, PRIMARY_BOTTOM);

        hoverMenu = pointInside(
            msg.x, msg.y,
            BUTTON_LEFT, MENU_TOP,
            BUTTON_RIGHT, MENU_BOTTOM);

        hoverExit = pointInside(
            msg.x, msg.y,
            BUTTON_LEFT, EXIT_TOP,
            BUTTON_RIGHT, EXIT_BOTTOM);
    }
    else if (msg.message == WM_LBUTTONDOWN)
    {
        if (pointInside(msg.x, msg.y,
                        BUTTON_LEFT, PRIMARY_TOP,
                        BUTTON_RIGHT, PRIMARY_BOTTOM))
        {
            requestPrimaryAction();
        }
        else if (pointInside(msg.x, msg.y,
                             BUTTON_LEFT, MENU_TOP,
                             BUTTON_RIGHT, MENU_BOTTOM))
        {
            request.target = SceneType::Menu;
        }
        else if (pointInside(msg.x, msg.y,
                             BUTTON_LEFT, EXIT_TOP,
                             BUTTON_RIGHT, EXIT_BOTTOM))
        {
            request.target = SceneType::Exit;
        }
    }
    else if (msg.message == WM_KEYDOWN)
    {
        if (msg.vkcode == 'R' || msg.vkcode == VK_RETURN)
        {
            requestPrimaryAction();
        }
        else if (msg.vkcode == 'M' || msg.vkcode == VK_ESCAPE)
        {
            request.target = SceneType::Menu;
        }
    }
    else if (msg.message == WM_CLOSE)
    {
        request.target = SceneType::Exit;
    }
}

void ResultScene::update(DWORD deltaTime)
{
    (void)deltaTime;
}

void ResultScene::draw() const
{
    setfillcolor(RGB(12, 17, 22));
    solidrectangle(0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);

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

    const bool allCleared =
        victory && levelIndex + 1 >= getLevelCount();

    const TCHAR* title = allCleared
        ? _T("全部关卡通关")
        : victory
            ? _T("作战胜利")
            : _T("作战失败");

    const TCHAR* description = allCleared
        ? _T("三条战线全部守住，恭喜完成挑战")
        : victory
            ? _T("本关敌人已经全部清理")
            : _T("基地生命已经归零");

    const COLORREF resultColor = victory
        ? RGB(79, 220, 146)
        : RGB(239, 91, 80);

    POINT resultPanel[6] =
    {
        { 390, 82 },
        { 1060, 82 },
        { 1060, 322 },
        { 1038, 344 },
        { 368, 344 },
        { 368, 104 }
    };

    setfillcolor(RGB(24, 33, 40));
    solidpolygon(resultPanel, 6);
    setlinecolor(RGB(78, 97, 108));
    setlinestyle(PS_SOLID, 2);
    polygon(resultPanel, 6);

    setfillcolor(resultColor);
    solidrectangle(390, 82, 585, 87);
    solidrectangle(368, 104, 373, 225);

    setbkmode(TRANSPARENT);
    settextcolor(resultColor);
    settextstyle(56, 0, _T("微软雅黑"));
    outtextxy((GameConfig::WINDOW_WIDTH - textwidth(title)) / 2, 122, title);

    const TCHAR* resultCode = victory
        ? _T("MISSION ACCOMPLISHED")
        : _T("MISSION FAILED");

    settextcolor(resultColor);
    settextstyle(15, 0, _T("Consolas"));
    outtextxy(
        (GameConfig::WINDOW_WIDTH - textwidth(resultCode)) / 2,
        198,
        resultCode);

    settextcolor(RGB(185, 195, 205));
    settextstyle(25, 0, _T("微软雅黑"));
    outtextxy(
        (GameConfig::WINDOW_WIDTH - textwidth(description)) / 2,
        238,
        description);

    const TCHAR* levelName = getLevelData(levelIndex).name;
    settextcolor(RGB(112, 212, 229));
    settextstyle(21, 0, _T("微软雅黑"));
    outtextxy(
        (GameConfig::WINDOW_WIDTH - textwidth(levelName)) / 2,
        292,
        levelName);

    const TCHAR* primaryText = hasNextLevel()
        ? _T("进入下一关")
        : victory
            ? _T("再次挑战本关")
            : _T("重新挑战");

    drawButton(BUTTON_LEFT, PRIMARY_TOP, BUTTON_RIGHT, PRIMARY_BOTTOM,
               primaryText, hoverPrimary);
    drawButton(BUTTON_LEFT, MENU_TOP, BUTTON_RIGHT, MENU_BOTTOM,
               _T("返回菜单"), hoverMenu);
    drawButton(BUTTON_LEFT, EXIT_TOP, BUTTON_RIGHT, EXIT_BOTTOM,
               _T("退出游戏"), hoverExit);

    settextcolor(RGB(78, 96, 105));
    settextstyle(13, 0, _T("Consolas"));
    outtextxy(28, 856, _T("COMBAT RECORD SAVED    PRESS ENTER TO CONTINUE"));

    setlinecolor(RGB(61, 79, 88));
    line(28, 842, GameConfig::WINDOW_WIDTH - 28, 842);
}

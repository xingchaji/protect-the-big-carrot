#pragma once

namespace GameConfig
{
    constexpr int WINDOW_WIDTH = 1440;
    constexpr int WINDOW_HEIGHT = 900;
    constexpr int FPS = 60;

    constexpr int MAP_ROWS = 9;
    constexpr int MAP_COLS = 15;
    constexpr int TILE_SIZE = 60;

    constexpr int MAP_LEFT =
        (WINDOW_WIDTH - MAP_COLS * TILE_SIZE) / 2;

    constexpr int MAP_TOP = 150;
}

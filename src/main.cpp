#include "raylib.h"
#include "spdlog/spdlog.h"
#include "world/World.hpp"

int main(void) {
    spdlog::set_level(spdlog::level::trace);

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Splash!");

    World world{static_cast<float>(GetScreenWidth()),
                static_cast<float>(GetScreenHeight())};

    while (!WindowShouldClose()) {
        auto dt = GetFrameTime();
        world.update(dt);

        BeginDrawing();
        ClearBackground(BLACK);
        world.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

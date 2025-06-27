#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

static constexpr int SCREEN_WIDTH = 1280;
static constexpr int SCREEN_HEIGHT = 720;
static constexpr const char* WINDOW_TITLE = "3D Model Viewer";

std::vector<std::string> get_all_glb_files_in_dir(const std::string& directory) {
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".glb") {
                files.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
    }
    return files;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

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
static constexpr const char* ASSETS_DIR_PATH = "assets";

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

    // Discover all GLB files in the 'assets' directory
    std::vector<std::string> all_glb_files = get_all_glb_files_in_dir(ASSETS_DIR_PATH);

    // GLB file picker GUI state
    struct {
        bool is_dialog_visible{true};
        int selected_file_index{0};
    } file_picker_gui_state;

    // 3D model state
    struct {
        Model loaded_model{0};
        bool is_model_loaded{false};
        ModelAnimation* animations{nullptr};
        int anim_count{0};
        int current_anim{0};
        float anim_frame_counter{0.0f};
        float anim_speed{1.0f};
        bool anim_paused{false};
    } model_state;

    // 3D model rendering state
    struct {
        float scale{1.0f};
        Vector3 position{0.0f, 0.0f, 0.0f};
        Vector3 rotation{0.0f, 0.0f, 0.0f};
        bool show_wireframe{false};
        bool show_bounding_box{false};
    } model_render_state;

    while (!WindowShouldClose()) {
        // Toggle showing file picker GUI with SPACE
        if (IsKeyPressed(KEY_SPACE)) {
            file_picker_gui_state.is_dialog_visible = !file_picker_gui_state.is_dialog_visible;
        }

        BeginDrawing();
            ClearBackground(DARKGRAY);
            
            // Draw the file picker GUI when visible
            if (file_picker_gui_state.is_dialog_visible) {
                const auto dialog_bounds = Rectangle{
                    .width = 400,
                    .height = 500,
                    .x = (SCREEN_WIDTH - 400) / 2.0f,
                    .y = (SCREEN_HEIGHT - 500) / 2.0f
                };

                // Draw semi-transparent overlay
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.5f));

                GuiPanel(dialog_bounds, "Select a GLB model file to preview");

                if (all_glb_files.empty()) {
                    // Draw empty-state if no GLB files
                    GuiLabel(Rectangle(dialog_bounds.x + 10.0f, dialog_bounds.y + 30.0, dialog_bounds.width - 40.0f, 30.0f), "No GLB files found");
                } else {
                    // Draw the selectable GLB file list
                    for (int i = 0; i < all_glb_files.size(); i++) {
                        const auto file_button_bounds = Rectangle{
                            .width = dialog_bounds.width - 20.0f,
                            .height = 30.0f,
                            .x = dialog_bounds.x + 10.0f,
                            .y = dialog_bounds.y + 40.0f + i * 40.0f
                        };

                        if (GuiButton(file_button_bounds, all_glb_files[i].c_str())) {
                            // Unload previous model and animations if necessary
                            if (model_state.is_model_loaded) {
                                UnloadModel(model_state.loaded_model);
                                UnloadModelAnimations(model_state.animations, model_state.anim_count);
                                model_state.animations = nullptr;
                                model_state.anim_count = 0;
                            }

                            // Load the selected model
                            const std::string model_file_path = std::string(ASSETS_DIR_PATH) + '/' + all_glb_files[i];
                            model_state.loaded_model = LoadModel(model_file_path.c_str());
                            model_state.is_model_loaded = true;

                            // Load the selected model animations
                            model_state.animations = LoadModelAnimations(model_file_path.c_str(), &model_state.anim_count);

                            // Reset the rendering state
                            model_render_state.scale = 1.0f;
                            model_render_state.position = Vector3Zero();
                            model_render_state.rotation = Vector3Zero();
                            model_render_state.show_wireframe = false;
                            model_render_state.show_bounding_box = false;

                            file_picker_gui_state.selected_file_index = i;
                            file_picker_gui_state.is_dialog_visible = false;
                        }
                    }
                }

                // Draw refresh button to check for new GLB files
                if (GuiButton(Rectangle(dialog_bounds.x + 10.0f, dialog_bounds.y + dialog_bounds.height - 40.0f, 80.0f, 30.0f), "Refresh")) {
                    all_glb_files = get_all_glb_files_in_dir(ASSETS_DIR_PATH);
                }
            }

        EndDrawing();
    }

    // Cleanup loaded model if necessary
    if (model_state.is_model_loaded) {
        UnloadModel(model_state.loaded_model);
        UnloadModelAnimations(model_state.animations, model_state.anim_count);
        model_state.animations = nullptr;
        model_state.anim_count = 0;
    }

    CloseWindow();
    return 0;
}

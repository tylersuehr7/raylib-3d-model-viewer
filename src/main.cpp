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
static constexpr int FPS = 120;
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
    SetTargetFPS(FPS);

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
        float rotation_correction_angle{0.0f};
        bool show_wireframe{false};
        bool show_bounding_box{false};
    } model_render_state;

    // Camera and view settings
    Camera3D camera{
        .position = Vector3(60.0f, 60.0f, 60.0f),
        .target = Vector3(0.0f, 0.0f, 0.0f),
        .up = Vector3(0.0f, 1.0f, 0.0f),
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE
    };

    while (!WindowShouldClose()) {
        // Toggle showing file picker GUI with SPACE
        if (IsKeyPressed(KEY_SPACE)) {
            file_picker_gui_state.is_dialog_visible = !file_picker_gui_state.is_dialog_visible;
        }

        if (model_state.is_model_loaded && !file_picker_gui_state.is_dialog_visible) {
            UpdateCamera(&camera, CAMERA_ORBITAL);

            // Update model animation
            if (model_state.animations && model_state.anim_count > 0 && !model_state.anim_paused) {
                model_state.anim_frame_counter += GetFrameTime() * model_state.anim_speed * 60.0f;
                if (model_state.anim_frame_counter >= model_state.animations[model_state.current_anim].frameCount) {
                    model_state.anim_frame_counter = 0.0f;
                }
                UpdateModelAnimation(model_state.loaded_model, model_state.animations[model_state.current_anim], int(model_state.anim_frame_counter));
            }

            // Toggle wireframe mode
            if (IsKeyPressed(KEY_W)) {
                model_render_state.show_wireframe = !model_render_state.show_wireframe;
            }
            
            // Toggle bounding box
            if (IsKeyPressed(KEY_B)) {
                model_render_state.show_bounding_box = !model_render_state.show_bounding_box;
            }

            // Toggle animation paused / resumed
            if (IsKeyPressed(KEY_P) && model_state.animations && model_state.anim_count > 0) {
                model_state.anim_paused = !model_state.anim_paused;
            }

            // Switch animations
            if (IsKeyPressed(KEY_COMMA) && model_state.animations && model_state.anim_count > 0) {
                model_state.current_anim--;
                if (model_state.current_anim < 0) {
                    model_state.current_anim = model_state.anim_count - 1;
                }
                model_state.anim_frame_counter = 0.0f;
            }
            if (IsKeyPressed(KEY_PERIOD) && model_state.animations && model_state.anim_count > 0) {
                model_state.current_anim++;
                if (model_state.current_anim >= int(model_state.anim_count)) {
                    model_state.current_anim = 0;
                }
                model_state.anim_frame_counter = 0.0f;
            }

            // Animation speed control
            if (IsKeyDown(KEY_MINUS)) model_state.anim_speed -= 0.01f;
            if (IsKeyDown(KEY_EQUAL)) model_state.anim_speed += 0.01f;
            if (model_state.anim_speed < 0.1f) model_state.anim_speed = 0.1f;
            if (model_state.anim_speed > 3.0f) model_state.anim_speed = 3.0f;

            // Model rotation with keys
            if (IsKeyDown(KEY_LEFT)) model_render_state.rotation.y -= 2.0f * GetFrameTime();
            if (IsKeyDown(KEY_RIGHT)) model_render_state.rotation.y += 2.0f * GetFrameTime();
            if (IsKeyDown(KEY_UP)) model_render_state.rotation.x -= 2.0f * GetFrameTime();
            if (IsKeyDown(KEY_DOWN)) model_render_state.rotation.x += 2.0f * GetFrameTime();

            // Model scale control
            if (IsKeyDown(KEY_Q)) model_render_state.scale -= 0.01f;
            if (IsKeyDown(KEY_E)) model_render_state.scale += 0.01f;

            // Model rotation correction control
            if (IsKeyPressed(KEY_R)) model_render_state.rotation_correction_angle += 90.0f;
            if (model_render_state.rotation_correction_angle > 360.0) model_render_state.rotation_correction_angle = 0.0f;  
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            // Render the selected 3D model and animations
            if (model_state.is_model_loaded && !file_picker_gui_state.is_dialog_visible) {
                BeginMode3D(camera);
                    // Draw debug grid
                    DrawGrid(100, 1.0f);
                    DrawLine3D({0,0,0}, {5,0,0}, RED);    // X axis
                    DrawLine3D({0,0,0}, {0,5,0}, GREEN);  // Y axis
                    DrawLine3D({0,0,0}, {0,0,5}, BLUE);   // Z axis

                    // Apply rotation angle correction to the model
                    Matrix transform = MatrixIdentity();
                    transform = MatrixMultiply(transform, MatrixRotateX(model_render_state.rotation_correction_angle));
                    model_state.loaded_model.transform = transform;

                    if (model_render_state.show_wireframe) {
                        DrawModelWires(model_state.loaded_model, Vector3Zero(), model_render_state.scale, WHITE);
                    } else {
                        DrawModel(model_state.loaded_model, Vector3Zero(), model_render_state.scale, WHITE);
                    }

                    if (model_render_state.show_bounding_box) {
                        BoundingBox bbox = GetModelBoundingBox(model_state.loaded_model);
                        DrawBoundingBox(bbox, GREEN);
                    }

                EndMode3D();

                // UI overlay
                DrawText("Press SPACE to open file selection", 10, 10, 14, DARKGRAY);
                DrawText("Use mouse to orbit camera, mouse wheel to zoom", 10, 40, 14, DARKGRAY);
                DrawText("Arrow keys to rotate model, W for wireframe, B for bounding box", 10, 70, 14, DARKGRAY);
                if (model_state.animations && model_state.anim_count > 0) {
                    // Animation UI overlay
                    DrawText(TextFormat("Animation: %d/%d %s", model_state.current_anim + 1, model_state.anim_count, 
                        model_state.anim_paused ? "[PAUSED]" : ""), 10, 100, 14, DARKGRAY);
                    DrawText("P to pause, < > to change animation, -/+ to adjust speed", 10, 130, 14, DARKGRAY);
                    DrawText(TextFormat("Speed: %.2fx", model_state.anim_speed), 10, 160, 14, DARKGRAY);
                    
                    // Animation timeline
                    DrawRectangle(10, SCREEN_HEIGHT - 40, SCREEN_WIDTH - 20, 20, LIGHTGRAY);
                    DrawRectangle(10, SCREEN_HEIGHT - 40, 
                        (int)((model_state.anim_frame_counter / model_state.animations[model_state.current_anim].frameCount) * (SCREEN_WIDTH - 20)), 
                        20, MAROON);
                }
            }
            
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

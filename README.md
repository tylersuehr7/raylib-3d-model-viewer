# 3D Model Viewer

A simple 3D model viewer for GLB files built with raylib and raygui.

## Features

- File browser to select GLB models from the assets directory
- 3D model viewing with camera orbit controls
- Model transformation controls:
  - Scale with mouse wheel
  - Rotate with arrow keys
- Display options:
  - Wireframe mode (W key)
  - Bounding box display (B key)
- Simple UI with file selection dialog

## Controls

- **Space**: Toggle file selection dialog
- **Mouse**: Orbit camera around model
- **Mouse Wheel**: Zoom in/out (scale model)
- **Arrow Keys**: Rotate model
- **W**: Toggle wireframe mode
- **B**: Toggle bounding box display

## Building

This project uses CMake for building:

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make

# Run the application
./game
```

## Adding Models

Place your GLB model files in the `assets` directory. The application will detect them automatically.

You can find sample GLB files at:
- [Khronos glTF Sample Models](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0)
- [Sketchfab](https://sketchfab.com/) (Many models can be downloaded in GLB format)

## Dependencies

- [raylib](https://www.raylib.com/) - A simple and easy-to-use library for game development
- [raygui](https://github.com/raysan5/raygui) - A simple and easy-to-use immediate-mode GUI for raylib

## License

This project is licensed under the MIT License - see the LICENSE file for details.

> Copyright © 2025 Tyler R. Suehr

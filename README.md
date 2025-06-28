# 3D Model Viewer

Helps you visualize GLB 3D models and animations with raylib for game development.

## Features

- File browser to select GLB models from the assets directory
- 3D model viewing with camera orbit controls
- Model transformation controls:
  - Scale down / up model ('q' key / 'e' key)
  - Adjusts upright angle ('r' key)
- Model animation controls:
  - Switch between animations (',' key / '.' key)
  - Increase / decrease animation speed ('+' key / '-' key)
- Display options:
  - Wireframe mode ('w' key)
  - Bounding box display mode ('b' key)
  - Toggle file picker ('space' key)

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

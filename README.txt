================================================================================
  OpenGL Terrain Roaming System
  Based on Modern OpenGL (Core Profile 4.5+)
================================================================================

OVERVIEW
--------
This is a real-time large-scale natural scene roaming system built with OpenGL.
It features:
- Height-map based terrain generation with multi-texture blending
- Dynamic water with reflection, refraction, and Fresnel effect
- Skybox environment rendering
- ImGui-based editor interface
- Scene settings save/load functionality

SYSTEM REQUIREMENTS
-------------------
- Windows 10/11 (64-bit)
- OpenGL 4.5 compatible graphics card (GTX 750 or better recommended)
- At least 2GB VRAM recommended

CONTROLS
--------
- WASD          Move camera
- Mouse         Look around
- Scroll        Zoom in/out
- SHIFT         Sprint (3x speed)
- SPACE         Toggle mouse cursor
- F1            Toggle wireframe mode
- ESC           Exit application

QUICK START
-----------
1. Make sure the "assets" and "shaders" folders are in the same directory as
   RoamingSystem.exe
2. Run RoamingSystem.exe
3. The application will load with default settings

ADDING RESOURCES
----------------
Heightmap:
  Place your heightmap image in: assets/heightmaps/heightmap.png
  - Grayscale PNG (8-bit or 16-bit)
  - Recommended size: 512x512

Terrain Textures:
  Place textures in: assets/textures/terrain/
  - Ground037_1K-PNG_Color.png (grass)
  - Rocks001_1K-PNG_Color.png (rock)
  - Snow010A_1K-PNG_Color.png (snow)

Water Textures:
  Place textures in: assets/textures/water/
  - dudv.png (distortion map)
  - normal.png (normal map)

Skybox:
  Place cubemap faces in: assets/skybox/
  Naming convention (either works):
  - posx.jpg, negx.jpg, posy.jpg, negy.jpg, posz.jpg, negz.jpg
  - OR: right.jpg, left.jpg, top.jpg, bottom.jpg, front.jpg, back.jpg

EDITOR INTERFACE
----------------
Performance Panel:
  Shows FPS, frame time, vertex/triangle counts, GPU info

Scene Editor:
  - Camera: Position, ground walk mode, movement speed
  - Terrain: Texture settings, height blending parameters
  - Water: Height, wave parameters, appearance
  - Lighting: Light direction, sky color

Settings:
  Click "Save Settings" to save current parameters to settings.ini
  Settings are automatically loaded on startup

TROUBLESHOOTING
---------------
- Black screen: Check that shaders folder is present
- No terrain: Add a heightmap to assets/heightmaps/
- No skybox: Add cubemap textures to assets/skybox/
- Low FPS: Reduce terrain texture resolution or disable water

LICENSE
-------
This project is developed as a graduation project.
For educational and demonstration purposes.

CREDITS
-------
Libraries used:
- GLFW (Window management)
- GLAD (OpenGL loader)
- GLM (OpenGL Mathematics)
- stb_image (Image loading)
- Dear ImGui (User interface)

================================================================================
                    Developed with Modern C++ and OpenGL
================================================================================

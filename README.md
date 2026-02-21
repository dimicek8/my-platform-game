# Pure C Win32 Platformer 🍄

A custom 2D platformer game built entirely from scratch using pure C and the Windows API (Win32). 

This project was created as a learning exercise to understand how game engines work under the hood. It does **not** use any external game libraries (like SDL, SFML, Raylib, or Unity). Everything from memory allocation to pixel rendering and physics is written manually.

## 🚀 Features

* **Custom Software Rendering:** Direct memory manipulation using a Back Buffer and `StretchDIBits` to render pixels to the screen.
* **Custom Physics Engine:** Gravity, velocity, and robust AABB (Axis-Aligned Bounding Box) collision detection.
* **Scrolling Camera:** A dynamic viewport that follows the player through a world larger than the application window.
* **Hardcoded Pixel Art:** Sprite rendering implementation using character arrays to draw shapes without loading external image files.
* **Game Logic:** Triggers (coin collection), win-state detection (flagpole), and a continuous non-blocking game loop.
* **UI Overlay:** Real-time FPS counter and score display using Windows GDI (`TextOut`).

## 🎮 Controls

* **Left Arrow (`<-`)**: Move Left
* **Right Arrow (`->`)**: Move Right
* **Up Arrow (`^`)**: Jump

## 🛠️ How to Build and Run

This project is built for Windows. You can compile it using **Visual Studio** or the MSVC command line.

### Using Visual Studio:
1. Create an empty C++ project in Visual Studio.
2. Add a new `main.c` file and paste the source code.
3. **Important:** Right-click your project in the Solution Explorer -> `Properties` -> `Linker` -> `System`. Change the **SubSystem** to `Windows (/SUBSYSTEM:WINDOWS)`.
4. Compile and run (F5).

### Using MSVC Command Line:
```bat
cl main.c user32.lib gdi32.lib /link /SUBSYSTEM:WINDOWS
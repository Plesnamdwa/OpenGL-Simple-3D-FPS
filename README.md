# 3D Horror FPS — OpenGL & GLFW

A survival horror first-person shooter (FPS) written in C++ using **OpenGL (fixed-function pipeline)** and **GLFW**. In "Outbreak at SPPG Warehouse," you must navigate a dark, procedurally generated warehouse, survive against zombie workers, and find three colored keys (Red, Yellow, Green) to unlock the final vault door and escape. Every visual element, UI component, and sound effect is generated procedurally at runtime, requiring **no external image, model, or audio assets**.

> **Platform:** Windows only (audio uses `PlaySoundA` from the Win32 API).

---

## Features

### 3D Graphics & Rendering
- **Hierarchical transformations** — Utilizes `glPushMatrix` and `glPopMatrix` for complex object rendering:
  - *Zombies*: Animated leg swings, arm sways, and a toppling death animation.
  - *Items*: Rotating floating keys and physics-based falling padlocks when unlocked.
  - *First-Person Weapon*: Smooth interpolation for aiming down sights (ADS), recoil animations, and reload movements.
- **Dynamic Lighting & Fog** — Uses OpenGL lighting to create a spooky atmosphere:
  - *Flashlight*: A spot light (`GL_LIGHT0`) attached to the player's camera with specific cutoff and attenuation.
  - *Muzzle Flash*: A brief point light (`GL_LIGHT1`) that illuminates the environment when firing the weapon.
  - *Fog*: Linear depth fog (`GL_FOG`) that obscures distant areas and limits visibility.
- **Hitscan Raycasting** — Mathematical projection of the camera's forward vector to detect precise collisions with zombie hitboxes for the shooting mechanics.
- **Custom Environment** — A fully modeled warehouse environment with walls, concrete pillars, wooden crates, and a heavily fortified vault door.

### Interaction & UI
- **Custom 2D Overlay HUD** — Rendered using orthographic projection (`glOrtho`) on top of the 3D scene:
  - Health bar, ammo counter, and an interactive inventory slot system for the three colored keys.
  - Damage vignette effect that flashes red when hit by enemies.
  - Dynamic crosshair that tightens when aiming down sights.
- **Vector Stroke Font** — A complete alphanumeric font renderer built entirely out of `GL_LINES`, requiring no external font libraries like FreeType.
- **State Management** — Handles interaction prompts, interaction distances, reload timers, and win/loss states.

### Procedural Audio
- Synthesized sound effects generated mathematically via sine waves, noise functions, and ADSR envelopes.
- Generates in-memory WAV buffers for weapon shots, reloading, zombie groans, flesh hits, padlock unlocking, and footsteps.
- Played asynchronously via the Windows multimedia API (`PlaySoundA`).

---

## Dependencies

| Dependency | Purpose |
|---|---|
| **C++11** (or newer) compiler | Language features (`auto`, range-based `for`, etc.) |
| **[GLFW 3](https://www.glfw.org/)** | Window creation, OpenGL context, input callbacks (mouse/keyboard) |
| **OpenGL** (`opengl32`) | Fixed-function 3D rendering (ships with Windows) |
| **Windows API** (`windows.h`, `mmsystem.h`, linked with `winmm`) | Audio playback via `PlaySoundA` |

GLEW, GLM, and GLUT are **not** required.

Recommended toolchains: **MinGW-w64 (MSYS2)** or **Visual Studio (MSVC)**.

---

## How to Build

### Option A — MinGW-w64 via MSYS2 (recommended)

1. Install [MSYS2](https://www.msys2.org/), then open the **MSYS2 MinGW 64-bit** terminal.
2. Install the compiler and GLFW:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-glfw
   ```
3. Compile the game:
   ```bash
   g++ horrorFPS.cpp -o horrorFPS.exe -std=c++11 -lglfw3 -lopengl32 -lgdi32 -lwinmm
   ```
4. Run:
   ```bash
   ./horrorFPS.exe
   ```

### Option B — Visual Studio (MSVC)

1. Download the GLFW pre-compiled binaries (`lib-vc20xx` folder) from the [GLFW website](https://www.glfw.org/).
2. Open a **Developer Command Prompt**:
   ```bat
   cl /EHsc /std:c++14 horrorFPS.cpp /I"path\to\glfw\include" ^
      /link /LIBPATH:"path\to\glfw\lib-vc2022" ^
      glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib winmm.lib
   ```
3. Run `horrorFPS.exe`.

> **Note:** If compiling with MinGW using manually downloaded GLFW binaries, you may need to copy `glfw3.dll` into the same directory as your executable.

---

## Controls

### Keyboard

| Key | Action |
|---|---|
| `W`, `A`, `S`, `D` | Move Forward, Left, Backward, Right |
| `Left Shift` | Sprint (increases movement speed, lowers accuracy) |
| `R` | Reload weapon / Restart game (on Death or Win screen) |
| `E` | Interact (unlock padlocks when holding the correct key) |
| `F` | Toggle Flashlight on / off |
| `Esc` | Close the game |

### Mouse

| Action | Effect |
|---|---|
| **Mouse Movement** | Look around (controls yaw and pitch) |
| **Left Click** | Shoot weapon |
| **Right Click (Hold)** | Aim Down Sights (ADS) - slows movement but increases damage and accuracy |

---

## Notes

- Keys are automatically picked up by walking close to them. 
- You must interact with the vault padlocks using the `E` key when standing directly in front of them with the required key in your inventory.
- The game loop relies on delta-time (`dt`), meaning movement speed and animations will remain consistent regardless of your monitor's refresh rate.

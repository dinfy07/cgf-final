# Solar System Explorer

Computer Graphics Fundamentals, SE-2426. Instructor: Nazira Shaimerdenova.

Repository: https://github.com/dinfy07/cgf-final
Application source verified for this submission: e8ef457b505cbcb12bfebd6f2b028551b6804871.

## Team and contributions

The roles below follow the published commit history. They describe implemented areas, not percentages of individual effort.

| Member | Implemented areas | Representative commits |
| --- | --- | --- |
| Saken Akhmediyar | Scene animation and Earth-Moon hierarchy; final tour, screenshot export, timing CSV and core tests | 2c5b200, 85fc9bd |
| Yeshimkul Beybit | Shader loading and render pipeline; lighting and Earth day/night; rings, asteroid instancing and orbit paths; integration checks | 566050f, 4a4f4b8, 242a366 |
| Davlyudov Aron | OpenGL window and main loop; sphere geometry; texture and sky rendering integration | 089206f, 0de81e0, b1e6481 |
| Zhandos Elzhan | Orbit and free-flight camera; ImGui controls, labels and FPS display | b21fd53, d04fbe6 |

## Build and tests

Use a C++17 compiler, CMake 3.20 or newer, and the same OpenGL development setup as the course assignments. A working OpenGL driver is required. GLFW and GLM are detected from installed packages; otherwise CMake fetches GLFW 3.3.10 and GLM 1.0.1, which requires Internet access. Dear ImGui, header-only GLAD and stb_image are included.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

On macOS and Linux, run `./build/solar_explorer`. With a Visual Studio multi-configuration build on Windows, run `build\Release\solar_explorer.exe`.

macOS requests an OpenGL 4.1 core context. Linux and Windows request 3.3 core. Shaders use GLSL 330. The application locates assets using the source directory embedded at build time; keep that directory available, or pass `--assets-root /path/to/source-root` when moving the executable.

## Controls

| Input | Action |
| --- | --- |
| 1-8 | Follow a planet, Mercury through Neptune |
| 9 or M | Follow the Moon |
| 0 | System overview |
| Right mouse drag or arrow keys | Rotate the orbit camera |
| Mouse wheel | Zoom |
| F | Switch between orbit camera and free flight |
| W/S, A/D, Q/E in free flight | Forward/back, sideways, down/up |
| Space | Pause or resume time |
| + / - | Increase or decrease simulation speed |
| R | Reset scene and camera |
| N / L | Toggle Earth night lights / camera light |
| G / B / O / V | Toggle rings / asteroid belt / orbit paths / wireframe |
| H | Hide or show the control panel |
| Escape | Exit |

The ImGui panel also selects destinations, controls speed and switches rendering features. When the panel captures input, its controls take priority over scene shortcuts. Click the scene to return keyboard focus to camera control.

## Features

Sun, eight textured planets and Moon; Earth-Moon parent-child hierarchy; explicit model/view/projection matrices; per-fragment Blinn-Phong lighting with Sun and switchable camera lights; mipmapped planetary maps and star cubemap; orbit and free-flight navigation; live FPS; ImGui controls and labels.

Stretch effects are Earth day/night texture blending, transparent Saturn rings, and 2200 asteroids in one instanced draw. Sizes and distances are compressed, orbits are circular and coplanar, and the scene is not an astronomical ephemeris. Eclipse shadows, atmospheric scattering and surface landing are not implemented.

## Tour, screenshots and timing

```sh
./build/solar_explorer --tour --seconds 200 --width 1280 --height 720 --capture captures --stats frame-times.csv
```

The tour changes chapter every 20 seconds. Screenshot export writes genuine RGB PPM images once per chapter, eight seconds after its start. The CSV contains elapsed seconds, wall-clock frame interval in milliseconds, framebuffer dimensions and selected body. It does not contain isolated GPU timings. Screen recordings are separate from these still images.

Other options: `--hidden`, `--no-vsync`, `--assets-root DIR`, `--help`. Do not use software-rendered Linux FPS as a hardware performance score.

## Verification and limitations

The final application built and its core tests passed on macOS with Apple M4. The native tour and its frame timings were checked separately from Linux compatibility tests. Detailed submission evidence is provided with the report. Windows execution and performance on the TA machine require verification on those devices. Run the same clean-clone build before defense.

## AI Use Note

We used OpenAI ChatGPT and Codex as supporting tools, especially to investigate compiler and runtime errors, understand documentation and compare possible fixes. They also assisted with code scaffolding and revisions, test design, and preparation of documentation and presentation material. Accepted changes were checked through builds, automated tests and native application runs. Reported performance values come from measured frame timings, not AI estimates. Each team member remains responsible for reviewing, understanding and explaining the submitted work.

## Credits

See CREDITS.md for third-party libraries, texture attribution, references and licenses. Texture checksums and original URLs are in assets/manifest.json.

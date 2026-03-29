# PHM Engine — Project Structure Explainer

For someone coming from Python, JavaScript, or another language and opening a C++ project for the first time.

---

## The big picture

In most scripting languages you just run a file directly. In C++ you can't — the source code must be **compiled** into a binary first. This project uses three tools to manage that process:

| Tool | What it does | Analogy |
|---|---|---|
| **CMake** | Describes *how* to build the project | `package.json` scripts / `Makefile` |
| **Ninja** | Actually runs the compiler commands fast | `make` (but faster) |
| **FetchContent** | Downloads dependencies at configure time | `npm install` / `pip install` |

You never call the compiler (`clang++`) directly. You tell CMake what you want built, CMake generates Ninja build files, and Ninja calls the compiler for you.

---

## Directory map

```
hailmary/
│
├── CMakeLists.txt              ← root build definition (start here)
├── vcpkg.json                  ← dependency list (reference only — see below)
│
├── cmake/                      ← CMake helper scripts
│   ├── Deps.cmake              ← downloads all third-party libraries
│   └── MetalCpp.cmake          ← finds Apple's metal-cpp, defines shader compiler
│
├── src/                        ← all your C++ source code
│   ├── main.cpp                ← program entry point
│   ├── core/                   ← Module 1: memory allocators, ECS
│   ├── physics/                ← Module 2: relativity, orbits, rocket equation
│   ├── gpu/
│   │   ├── metal/              ← Module 3: Metal compute kernels (.metal shaders)
│   │   └── render/             ← Module 4: Metal render pipeline, GLTF loader
│   ├── ui/                     ← Module 5: Dear ImGui dashboard
│   └── webgpu/                 ← Module 6: WebGPU port (.wgsl shaders)
│
├── tests/                      ← unit tests (Google Test)
├── benchmarks/                 ← micro-benchmarks (Google Benchmark)
│
├── assets/models/              ← GLB 3D model files (not in git — download separately)
├── vendor/metal-cpp/           ← Apple's Metal C++ wrapper (not in git — run scripts/fetch_metal_cpp.sh)
├── scripts/                    ← shell helper scripts
├── docs/                       ← documentation (you are here)
│
├── .gitignore                  ← files git should never track
├── .clang-format               ← auto-formatter rules
└── .clang-tidy                 ← static analyser rules
```

---

## Configuration files explained

### `CMakeLists.txt` — the build definition

This is the most important file in the project. Every directory that contains source code has one. They form a tree:

```
CMakeLists.txt          ← root: project name, C++ standard, options, includes Deps.cmake
└── src/CMakeLists.txt  ← defines the phm_engine executable and which sub-libraries link into it
    ├── core/CMakeLists.txt     ← defines the phm_core library (header-only)
    ├── physics/CMakeLists.txt  ← defines the phm_physics library (header-only)
    ├── gpu/CMakeLists.txt      ← conditionally adds metal/ and render/
    │   ├── metal/CMakeLists.txt
    │   └── render/CMakeLists.txt
    ├── ui/CMakeLists.txt
    └── webgpu/CMakeLists.txt
tests/CMakeLists.txt      ← defines the phm_tests executable
benchmarks/CMakeLists.txt ← defines the phm_benchmarks executable
```

**Key concepts inside a CMakeLists.txt:**

```cmake
# Declare a library (a compiled unit other targets can link to)
add_library(phm_physics INTERFACE)
#           ^ name       ^ INTERFACE = header-only, no .cpp files to compile

# Declare an executable (a program with a main())
add_executable(phm_engine main.cpp)

# Say "this target needs these other libraries"
target_link_libraries(phm_engine PRIVATE phm_physics nlohmann_json::nlohmann_json)

# Say "when compiling this target, look for headers in this directory"
target_include_directories(phm_physics INTERFACE "${CMAKE_SOURCE_DIR}/src")
```

**The `INTERFACE` / `PUBLIC` / `PRIVATE` distinction:**

| Keyword | Meaning |
|---|---|
| `PRIVATE` | Only this target uses it |
| `PUBLIC` | This target uses it, and so does anything that links to this target |
| `INTERFACE` | Only downstream targets use it (the target itself doesn't compile anything) |

**Build options (top of root `CMakeLists.txt`):**

```cmake
option(PHM_BUILD_METAL      "..." OFF)   # enable Metal GPU modules
option(PHM_BUILD_WEBGPU     "..." OFF)   # enable WebGPU module
option(PHM_BUILD_TESTS      "..." ON)
option(PHM_BUILD_BENCHMARKS "..." ON)
```

Pass these on the command line: `cmake -B build -DPHM_BUILD_METAL=ON`

---

### `cmake/Deps.cmake` — dependency downloader

Uses CMake's built-in `FetchContent` module, which is the C++ equivalent of `npm install`. It downloads source code from GitHub at configure time, then compiles it alongside your code.

```cmake
FetchContent_Declare(
    eigen                                    # ← internal name
    GIT_REPOSITORY https://github.com/libeigen/eigen.git
    GIT_TAG        3.4.0                     # ← pinned version (like package-lock.json)
    GIT_SHALLOW    TRUE                      # ← only download this commit, not full history
)
FetchContent_MakeAvailable(eigen)            # ← actually download + configure it
```

After `MakeAvailable`, the library is available as a CMake target (e.g. `Eigen3::Eigen`) and you link to it like any other library.

**Libraries fetched:**

| Name | What it is | Used in |
|---|---|---|
| `eigen` | Linear algebra — vectors, matrices, quaternions | render, camera |
| `nlohmann_json` | JSON parser/serializer | config files |
| `tinygltf` | Loads `.glb` 3D model files | GLTFLoader |
| `imgui` | Immediate-mode GUI (the dashboard) | ui/ |
| `googletest` | Unit testing framework | tests/ |
| `benchmark` | Micro-benchmark framework | benchmarks/ |

---

### `cmake/MetalCpp.cmake` — Apple GPU setup

Does two things:

1. **Finds metal-cpp** — looks for `vendor/metal-cpp/Metal/Metal.hpp`. If not found, disables the Metal build and prints a message telling you to run the download script.

2. **Defines `phm_add_metallib()`** — a CMake function that compiles `.metal` shader files into a `.metallib` binary (the GPU program). Calls `xcrun metal` and `xcrun metallib` under the hood.

```cmake
# Example use (in gpu/metal/CMakeLists.txt, uncomment in Phase 4):
phm_add_metallib(phm_compute_metallib "phm_compute.metallib"
    ParticleKernel.metal
    SpatialHash.metal
    EnergyAbsorption.metal
)
```

---

### `vcpkg.json` — alternative package manager (reference only)

[vcpkg](https://vcpkg.io) is Microsoft's C++ package manager, similar to Homebrew but for libraries. This file lists the same dependencies as `Deps.cmake`.

**This project currently uses `Deps.cmake` (FetchContent), not vcpkg.** The `vcpkg.json` is here as a reference — if you later want to switch to vcpkg (which is common in larger teams), the dependency list is already written out.

You would only activate vcpkg by passing `-DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake` to CMake.

---

### `.gitignore` — what git should never track

Notable entries:

```gitignore
build/              ← compiled output — never commit build artifacts
vendor/metal-cpp/   ← downloaded separately, too large for git
assets/models/*.glb ← 3D model files, too large for git (use Git LFS or download script)
*.metallib          ← compiled GPU shaders — regenerated at build time
```

---

### `.clang-format` — automatic code formatter

Defines the code style. Run it with:

```bash
clang-format -i src/**/*.hpp src/**/*.cpp
```

Or configure your editor to run it on save. The settings are based on the Google C++ style guide but with 4-space indentation instead of 2.

Key rules in use:

```yaml
IndentWidth: 4           # 4 spaces (not tabs)
ColumnLimit: 100         # max line length
PointerAlignment: Left   # int* ptr  (not  int *ptr)
```

---

### `.clang-tidy` — static analyser

Scans your code for bugs, style violations, and dangerous patterns *without compiling*. It is stricter than the compiler.

Run it with:

```bash
clang-tidy src/physics/Relativity.hpp -- -std=c++20 -I src
```

Or let CMake run it automatically by adding `-DCMAKE_CXX_CLANG_TIDY=clang-tidy` at configure time.

The checks enabled in this project include `modernize-*` (flags old C++ patterns), `performance-*` (flags inefficient code), and `cppcoreguidelines-*` (flags unsafe patterns like raw pointers without ownership).

---

### `scripts/fetch_metal_cpp.sh` — metal-cpp downloader

Apple distributes their C++ Metal wrapper as a zip download, not through any package manager. This script downloads and unpacks it into `vendor/metal-cpp/`.

Run it once before enabling the Metal build:

```bash
bash scripts/fetch_metal_cpp.sh
cmake -B build -DPHM_BUILD_METAL=ON
```

---

## How a build works, step by step

```
1. cmake -B build -G Ninja -DPHM_BUILD_METAL=OFF
   │
   ├── Reads all CMakeLists.txt files recursively
   ├── Runs FetchContent: downloads eigen, imgui, googletest, etc. into build/_deps/
   ├── Generates build/build.ninja (the actual build instructions)
   └── Prints "Configuring done"

2. cmake --build build
   │
   ├── Ninja reads build.ninja
   ├── Calls clang++ for each .cpp file → produces .o object files
   ├── Links object files + downloaded libraries → produces executables
   └── Outputs: build/bin/phm_engine, build/bin/phm_tests, build/bin/phm_benchmarks

3. ctest --test-dir build
   │
   └── Runs phm_tests, collects pass/fail per test case
```

The `build/` directory is completely disposable — delete it and re-run step 1 to start fresh.

---

## Header files vs source files

In C++, code is split into two file types:

| Extension | Purpose | Analogy |
|---|---|---|
| `.hpp` | **Header** — declares types, functions, classes. Included by other files via `#include`. | Like a Python module's public `__init__.py` interface |
| `.cpp` | **Source** — implements functions declared in the header. Compiled separately. | Like the actual function bodies in a Python module |

**This project uses a lot of header-only code** (especially in `core/` and `physics/`). That means the function bodies live in `.hpp` files, marked `inline`. This is common for template-heavy or small math functions. The trade-off is slightly longer compile times in exchange for no separate `.cpp` to manage.

The only `.cpp` file in `src/` today is `GLTFLoader.cpp`, because it owns the `tinygltf` implementation (a large header that can only be included in one translation unit).

---

## The `#pragma once` at the top of every header

Prevents the same header from being included twice in the same compilation unit. Without it, you'd get "duplicate definition" errors. It's the modern, simpler alternative to the old `#ifndef GUARD` pattern.

---

## Library types: `INTERFACE` vs `STATIC` vs `SHARED`

| Type | Compiled? | Linked into binary? | When to use |
|---|---|---|---|
| `INTERFACE` | No (header-only) | No | Pure header libraries (core/, physics/) |
| `STATIC` | Yes → `.a` archive | Yes, at link time, embedded | Most libraries in this project |
| `SHARED` | Yes → `.dylib` on macOS | Yes, at runtime, separate file | System libraries, plugins |

`phm_core` and `phm_physics` are `INTERFACE` because they have no `.cpp` files — all code is `inline` in headers. `imgui` is `STATIC` because it has real `.cpp` files that must be compiled once.

# PHM Engine — Tech Stack

---

## Language

| Technology | Version | Role |
|---|---|---|
| C++ | C++20 | Entire codebase |
| Metal Shading Language | MSL 3.0 | GPU compute & render shaders |
| WGSL | 1.0 | WebGPU compute shaders |
| CMake | 3.27+ | Build system |
| vcpkg | latest | Package management |

**C++20 features actively used:**
- Concepts — constrain template parameters on allocators and physics types
- Ranges — particle filtering, scene iteration
- `std::jthread` — thread pool with automatic cancellation
- `std::span` — non-owning views over GPU buffers
- Designated initializers — readable particle and physics config structs
- Modules (optional, if compiler support is stable) — isolate GPU backend

---

## GPU Compute

### Primary: metal-cpp (macOS / Apple Silicon)
- **What:** Apple's official header-only C++ wrapper for the Metal API (released 2022)
- **Why:** Native GPU compute on Mac. No Objective-C required. Full access to compute pipelines, command queues, shared memory, SIMD-groups
- **Replaces:** CUDA (same concepts — threadgroups = warps, threadgroup memory = shared memory, command queues = CUDA streams)
- **Source:** https://developer.apple.com/metal/cpp/

### Secondary: Dawn (WebGPU)
- **What:** Google Chrome's implementation of WebGPU, usable as a standalone C++ library
- **Why:** Write WebGPU compute shaders in WGSL that run in-browser. Same algorithms as Metal kernels, different API. Directly relevant to LLM browser inference (llama.cpp WebGPU backend)
- **Install:** `vcpkg install dawn`

### Future (when NVIDIA access available): CUDA
- The threadgroup/shared-memory mental model from Metal maps directly
- Spatial hashing kernel → same pattern as CUDA neighbor search
- All Metal compute knowledge transfers

---

## Rendering

| Technology | Role |
|---|---|
| metal-cpp render pipeline | 3D scene rendering (stars, planets, ship, particles) |
| Metal Performance Shaders (MPS) | Acceleration structures if hardware RT is added |
| Dear ImGui (docking branch) | Physics dashboard, parameter sliders, debug overlays |
| ImGui Metal backend | Connects ImGui to Metal render pass |

**Rendering approach:** Forward rendering, HDR framebuffer, tone mapping for star luminosity. Point-sprite instancing for Astrophage particle cloud. No game engine — raw Metal pipeline, fully hand-rolled.

---

## Math & Scientific Computing

| Library | Role |
|---|---|
| Eigen 3.4 | Linear algebra (vectors, matrices, quaternions for camera) |
| GLM (optional) | GLSL-compatible math types if preferred over Eigen |
| Custom `physics/` module | All relativistic and orbital mechanics (hand-written, no dependency) |

**No physics engine library (Bullet, PhysX, etc.)** — the physics here is bespoke relativistic/astrophysics math that no general engine provides. Writing it from scratch is the point.

---

## 3D Asset Loading (glTF / GLB)

| Library | Role |
|---|---|
| tinygltf | Header-only glTF 2.0 / GLB loader. Bundles stb_image for texture decoding |
| stb_image | Bundled with tinygltf — decodes PNG/JPG textures from GLB blobs |

**Why tinygltf:** Single header, zero external dependencies, actively maintained, handles the full glTF 2.0 spec including PBR materials, node hierarchies, and binary GLB format. Used in production engines (Mozilla Hubs, Filament samples).

**Asset pipeline:**
- Geometry (vertices, normals, UVs, indices) → `MTLBuffer` via `MTLResourceStorageModeShared` (unified memory on Apple Silicon — zero copy)
- Textures (base color, normal, metallic-roughness, emissive) → `MTLTexture` (RGBA8 sRGB)
- Node transforms (TRS decomposition) → flattened `simd::float4x4` hierarchy

**Asset sources:**
- Planets + Sun: NASA 3D Resources (public domain) — https://nasa3d.arc.nasa.gov/models
- Spacecraft: Sketchfab free models (CC license) — https://sketchfab.com/features/free-3d-models
- Tau Ceti: modified Sun GLB (rescale + recolor)

**Install:** `vcpkg install tinygltf` or drop `tiny_gltf.h` into `vendor/`

---

## Browser / WebAssembly

| Technology | Role |
|---|---|
| Emscripten | Compiles C++ core to WebAssembly |
| Dawn (WebGPU) | GPU compute in browser via WebGPU API |
| WGSL | Shader language for WebGPU |
| HTML/CSS (minimal) | Canvas host for WebGPU surface |

**Compilation target:** `--target wasm32-unknown-emscripten` with `-sUSE_WEBGPU=1`

---

## Build & Tooling

| Tool | Role |
|---|---|
| CMake 3.27+ | Build system, multi-target (native + wasm) |
| vcpkg | Dependency management (manifest mode) |
| Ninja | Fast build backend |
| Xcode GPU Frame Capture | Metal kernel profiling (occupancy, memory bandwidth) |
| Google Benchmark | Micro-benchmarks for allocators and kernels |
| Google Test | Unit tests for physics functions |
| clang-format | Code style enforcement |
| clang-tidy | Static analysis |

---

## Project Structure

```
hailmary/
├── CMakeLists.txt
├── vcpkg.json                    # dependency manifest
├── vcpkg-configuration.json
│
├── docs/                         # ← you are here
│   ├── architecture.md
│   ├── tech-stack.md
│   ├── plan.md
│   └── physics.md
│
├── src/
│   ├── main.cpp                  # app entry point
│   │
│   ├── core/                     # Module 1: Memory Architecture
│   │   ├── ArenaAllocator.hpp
│   │   ├── SlabAllocator.hpp
│   │   ├── ObjectPool.hpp
│   │   └── ECS.hpp               # Entity Component System (SoA)
│   │
│   ├── physics/                  # Module 2: Relativistic Physics
│   │   ├── Relativity.hpp        # Lorentz, time dilation, energy
│   │   ├── Brachistochrone.hpp   # Constant-g flip trajectory
│   │   ├── RocketEquation.hpp    # Relativistic Tsiolkovsky + fuel mass ratio
│   │   ├── OrbitalMechanics.hpp  # Kepler, RK4 integrator
│   │   └── EnergyComparison.hpp  # Chemical/Fission/Fusion/Astrophage
│   │
│   ├── gpu/
│   │   ├── metal/                # Module 3: Metal Compute
│   │   │   ├── MetalDevice.hpp   # MTLDevice + command queue setup
│   │   │   ├── ParticleKernel.metal
│   │   │   ├── SpatialHash.metal
│   │   │   └── EnergyAbsorption.metal
│   │   │
│   │   └── render/               # Module 4: Metal Render Pipeline
│   │       ├── Renderer.hpp
│   │       ├── GLTFLoader.hpp        # glTF/GLB → MTLBuffer + MTLTexture
│   │       ├── GLTFLoader.cpp
│   │       ├── Mesh.hpp              # MTLBuffer refs + draw params
│   │       ├── Material.hpp          # MTLTexture refs + PBR factors
│   │       ├── RenderObject.hpp      # mesh + material + world transform
│   │       ├── StarSystem.hpp
│   │       ├── TrajectoryArc.hpp
│   │       ├── Shaders.metal         # Cook-Torrance PBR shaders
│   │       └── Camera.hpp
│   │
│   ├── ui/                       # Module 5: ImGui Dashboard
│   │   ├── Dashboard.hpp
│   │   ├── EnergyPanel.hpp
│   │   └── PhysicsPanel.hpp
│   │
│   └── webgpu/                   # Module 6: WebGPU Port
│       ├── WebGPUDevice.hpp
│       ├── ParticleKernel.wgsl
│       └── SpatialHash.wgsl
│
├── assets/
│   └── models/                   # GLB files (not checked into git — too large)
│       ├── sun.glb               # NASA public domain
│       ├── earth.glb
│       ├── mars.glb
│       ├── jupiter.glb
│       ├── saturn.glb            # includes ring mesh
│       ├── tau_ceti.glb          # modified sun
│       └── hail_mary_ship.glb
│
├── tests/
│   ├── test_relativity.cpp       # validates against known calculator outputs
│   ├── test_brachistochrone.cpp
│   ├── test_allocators.cpp
│   └── test_orbital.cpp
│
└── benchmarks/
    ├── bench_allocators.cpp
    └── bench_particles.cpp
```

---

## Dependency Matrix

| Module | Dependencies |
|---|---|
| core/ | C++20 stdlib only |
| physics/ | C++20 stdlib, `<cmath>` only |
| gpu/metal/ | metal-cpp (header-only, Apple SDK) |
| gpu/render/ | metal-cpp, Eigen |
| ui/ | Dear ImGui, ImGui Metal backend |
| webgpu/ | Dawn, Emscripten (for WASM build) |
| tests/ | Google Test |
| benchmarks/ | Google Benchmark |

**Key principle:** `core/` and `physics/` have zero external dependencies. They are fully portable, testable on any machine, and the first thing you build.

---

## vcpkg.json (Starter Manifest)

```json
{
  "name": "phm-engine",
  "version": "0.1.0",
  "dependencies": [
    "eigen3",
    "imgui",
    "gtest",
    "benchmark",
    "nlohmann-json",
    "tinygltf"
  ]
}
```

metal-cpp and Dawn are added separately:
- metal-cpp: downloaded directly from Apple (header-only, drop into `vendor/`)
- Dawn: added via CMake FetchContent or vcpkg when WebGPU module begins

---

## Compiler Requirements

| Platform | Compiler | Min Version |
|---|---|---|
| macOS (primary) | Apple Clang | 15.0 (Xcode 15+) |
| Linux (CI) | GCC or Clang | GCC 13 / Clang 16 |
| WASM | emcc (Emscripten) | 3.1.50+ |

**C++ standard:** `-std=c++20` everywhere, no exceptions.

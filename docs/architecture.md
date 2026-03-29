# PHM Engine — Architecture Overview

**Project Hail Mary Simulation Engine**
A GPU-accelerated scientific simulation of the Hail Mary's journey from Earth to Tau Ceti, showcasing C++20, Metal compute, WebGPU, relativistic physics, and Astrophage particle dynamics.

---

## The One-Line Pitch

> "A real-time 3D simulation of the Hail Mary's journey from Earth to Tau Ceti — accurate orbital mechanics, special relativity, and a live energy dashboard proving why Astrophage changes everything."

---

## System Architecture

```
 assets/models/*.glb (NASA + Sketchfab)
       │
       ▼
┌─────────────────────────────────────────────────────────────┐
│                    Asset Pipeline                            │
│   tinygltf → GLTFLoader → MTLBuffer (mesh) + MTLTexture     │
│   (geometry, PBR textures, node transforms — zero-copy on   │
│    Apple Silicon via unified memory / StorageModeShared)     │
└──────────────────────────┬──────────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────────┐
│                     Application Layer                        │
│              Dear ImGui — Debug UI & Dashboard               │
│   (Physics dashboard, energy bars, time dilation display)    │
└──────────────────────────┬──────────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────────┐
│                     Rendering Layer                          │
│              Metal Render Pipeline (macOS)                   │
│   Cook-Torrance PBR shader reads GLB textures + geometry     │
│   (Star systems, ship trajectory arc, particle rendering)    │
└──────────┬───────────────────────────────────┬──────────────┘
           │                                   │
┌──────────▼──────────┐             ┌──────────▼──────────────┐
│   GPU Compute        │             │   CPU Simulation         │
│   Metal Compute      │             │   C++20 Core             │
│   Shaders (.metal)   │             │                          │
│                      │             │  • N-body RK4 integrator │
│  • Astrophage        │             │  • Brachistochrone       │
│    particle update   │◄───────────►│    trajectory solver     │
│  • Spatial hashing   │             │  • Relativistic math     │
│  • Energy absorption │             │  • ECS & scene graph     │
│  • N-body gravity    │             │  • Custom allocators     │
│    (large N)         │             │  • Config & I/O          │
└──────────────────────┘             └──────────────────────────┘
           │
┌──────────▼──────────────────────────────────────────────────┐
│                   WebGPU Module (Separate)                   │
│              Dawn C++ library + WGSL shaders                 │
│   Same algorithms ported — runs in Chrome, zero install      │
│   (This is the LLM/browser inference portfolio signal)       │
└─────────────────────────────────────────────────────────────┘
```

---

## Module Breakdown

### Module 1 — Memory Architecture (`src/core/`)
The foundation everything else sits on. No GPU code, pure C++20.

**What it implements:**
- Arena allocator — linear bump allocator for per-frame scratch memory
- Slab allocator — fixed-size block allocator for particle data
- Object pool — reusable entity handles for scene objects
- Structure-of-Arrays (SoA) Entity Component System (ECS)
- Lock-free work-stealing thread pool (std::jthread)

**Why it matters to employers:**
This is the same memory model used by PyTorch, JAX, and game engines. SoA layout is why GPU memory access is fast — understanding this at implementation level signals genuine depth.

**Deliverable:** CLI benchmark showing allocator throughput vs. std::allocator.

---

### Module 2 — Relativistic Physics Engine (`src/physics/`)
The scientific computing core. CPU-side, mathematically rigorous.

**What it implements:**
- Brachistochrone trajectory solver (constant-g flip trajectory)
- Lorentz factor and time dilation (ship time vs. Earth time)
- Relativistic Tsiolkovsky rocket equation (fuel mass ratio)
- Energy density comparison (Chemical / Fission / Fusion / Astrophage)
- 4th-order Runge-Kutta (RK4) integrator for orbital mechanics
- Kepler orbit solver for planetary positions

**Deliverable:** CLI that prints a full journey summary for Earth → Tau Ceti and matches the overvieweffekt.com calculators exactly.

---

### Module 3 — Metal Compute Kernel (`src/gpu/metal/`)
The GPU compute layer. Astrophage particles on the GPU.

**What it implements:**
- MTLDevice, MTLCommandQueue, MTLComputePipelineState setup in C++ (metal-cpp)
- Particle position/velocity update kernel (thread-per-particle)
- Spatial grid hashing kernel for neighbor search (coalesced memory access)
- Photon absorption kernel (energy accumulation per particle)
- Particle reproduction/death based on energy threshold

**Key learning:** Memory coalescing, threadgroup shared memory, occupancy analysis with Xcode GPU Frame Capture. These techniques map 1:1 to CUDA for future NVIDIA work.

**Deliverable:** 2 million Astrophage particles updated at 60fps on Apple Silicon.

---

### Module 4 — Metal Render Pipeline (`src/gpu/render/`)
Visualization layer, also via metal-cpp. Uses real 3D GLB models for all scene objects.

**What it implements:**
- `GLTFLoader` — parses `.glb` files via tinygltf, uploads to `MTLBuffer`/`MTLTexture`
- `Material` — holds PBR texture references (base color, normal, metallic-roughness, emissive)
- `Mesh` — holds vertex/index `MTLBuffer` + draw parameters
- `RenderObject` — combines mesh + material + world transform from physics engine
- Cook-Torrance BRDF fragment shader (GGX distribution, Smith geometry, Schlick Fresnel)
- Normal mapping from glTF normal textures
- Star emission override — stars skip BRDF, use emissive factor × luminosity from physics
- Alpha blending pipeline state for Saturn rings and ship engine glow
- MTLRenderPipelineState for point-sprite Astrophage particle rendering
- Trajectory arc as a vertex buffer (updated each frame from Brachistochrone solver)
- HDR framebuffer + Reinhard tone mapping + gamma correction
- Camera: arcball orbit controls, zoom, pan

**Asset sources:**
- Planets + Sun: NASA 3D Resources (public domain GLBs)
- Hail Mary ship: Sketchfab (CC license fan models or custom Blender export)
- Tau Ceti: Sun GLB rescaled to 0.783 solar radii, tinted K-type orange

**Deliverable:** Two star systems (Sol + Tau Ceti) rendered with real GLB models and PBR materials. Ship GLB travels the brachistochrone arc. Engine glow brightness tied to physics fuel burn rate.

---

### Module 5 — Dear ImGui Dashboard (`src/ui/`)
The "mission control" panel that tells the scientific story.

**What it displays:**
- Live physics: velocity (as % of c), Lorentz factor (γ), ship time, Earth time
- Trajectory: distance covered, distance remaining, midpoint flip indicator
- Energy comparison bars (Chemical / Fission / Fusion / Astrophage)
- Astrophage fuel tank: particle count, energy stored, depletion over time
- Configurable: acceleration (g), ship mass (tonnes), destination distance (ly)

**Deliverable:** Full interactive dashboard with sliders driving live simulation.

---

### Module 6 — WebGPU Browser Demo (`src/webgpu/`)
Portfolio accessibility layer. Same algorithms, different GPU API.

**What it implements:**
- Dawn (Chrome's WebGPU implementation) as a C++ library
- WGSL compute shaders for particle update (ported from .metal)
- Emscripten compilation to WebAssembly for browser deployment
- Explicit staging buffer management (vs Metal's unified memory)
- Subgroup operations (WebGPU equivalent of Metal SIMD-groups)

**Why it matters:** WebGPU is the backend for llama.cpp browser inference, MediaPipe, and on-device ML. Having written both Metal and WebGPU versions of the same kernel, and explaining the architectural differences, puts you in a very small group of candidates.

**Deliverable:** Simulation running at acceptable framerate in Chrome with zero installation.

---

## Data Flow

```
Config (JSON)
    │
    ▼
Physics Engine (CPU)
    │  computes trajectory, positions, energy values
    ▼
ECS Scene Graph (CPU, SoA layout)
    │  particle positions, planet positions, ship state
    ├──► Metal Compute (GPU)
    │        updates Astrophage particles
    │        writes back to shared MTLBuffer
    │
    ▼
Metal Render Pipeline (GPU)
    │  reads scene graph, draws everything
    ▼
Dear ImGui (CPU overlay)
    │  reads physics engine output, draws dashboard
    ▼
Screen
```

---

## Key Design Principles

**1. Physics first, visuals second.**
Every number on screen comes from a validated physics function. The simulation is correct before it is pretty.

**2. Separation of compute and render.**
Metal compute kernels never touch render state. Render shaders never run physics. Clean boundaries make each module independently testable.

**3. CPU is the orchestrator, GPU is the worker.**
High-level logic (trajectory planning, config, UI) lives on CPU. Embarrassingly parallel work (particle updates, N-body at large N) lives on GPU.

**4. WebGPU is a first-class citizen, not an afterthought.**
The WGSL shaders are maintained in parallel with .metal shaders. The algorithms are identical; only the API differs. This proves GPU knowledge is transferable, not platform-specific.

**5. Benchmarks are part of the project.**
Every module ships with a Google Benchmark target. Allocator throughput, particle update fps, kernel occupancy — all measured and documented.

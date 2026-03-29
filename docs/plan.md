# PHM Engine — Build Plan

The project is built in six phases. Each phase is independently shippable and demo-able. Never move to the next phase until the current one has a working, testable deliverable.

---

## The Guiding Rule

> **Physics first. Visuals second. GPU third.**
> The simulation must be *correct* before it is *fast*, and *fast* before it is *pretty*.

---

## Phase 0 — Scaffold (Week 1)
**Goal:** Empty project compiles cleanly. All tooling works.

Tasks:
- [ ] Init git repo, write `.gitignore`
- [ ] Write `CMakeLists.txt` with two targets: `phm_engine` (app) and `phm_tests` (tests)
- [ ] Add `vcpkg.json` with Eigen, ImGui, GTest, Google Benchmark
- [ ] Confirm `clang-format` and `clang-tidy` configs run cleanly
- [ ] Write a `main.cpp` that prints "PHM Engine v0.1" and exits 0
- [ ] CI: GitHub Actions workflow that builds and runs tests on push

**Deliverable:** `cmake --build . && ./phm_engine` prints version. Tests pass (empty suite).

---

## Phase 1 — Physics Engine (Weeks 2–3)
**Goal:** Every number on the future dashboard has a validated source.

Tasks:
- [ ] Implement `physics/Relativity.hpp`:
  - Lorentz factor `γ(v)`
  - Relativistic kinetic energy `E = (γ-1)mc²`
  - Time dilation: ship time vs Earth time
- [ ] Implement `physics/Brachistochrone.hpp`:
  - Ship time `τ = (2c/a) · acosh(ad/2c² + 1)`
  - Earth time `t = (2c/a) · sinh(aτ/2c)`
  - Peak velocity `v_max = c · tanh(aτ/2c)`
- [ ] Implement `physics/RocketEquation.hpp`:
  - Relativistic mass ratio: `m₀/mf = exp(Δv / v_exhaust)`
  - Fuel fraction for each energy source
- [ ] Implement `physics/EnergyComparison.hpp`:
  - Energy density table: Chemical, Fission, Fusion, Astrophage
  - Required fuel mass for Earth → Tau Ceti per source
- [ ] Implement `physics/OrbitalMechanics.hpp`:
  - Kepler orbit positions for all 8 solar planets
  - RK4 integrator for N-body (small N, CPU)
- [ ] Write unit tests for all physics functions
  - Cross-validate against overvieweffekt.com calculator outputs manually
- [ ] CLI output: print full journey summary for Earth → Tau Ceti

**Deliverable:**
```
═══ PHM Journey: Earth → Tau Ceti ═══════════════════
Distance:          12.0 light-years
Acceleration:      1.00 g (9.81 m/s²)

Ship time:         3.58 years
Earth time:        13.94 years
Time dilation:     ×3.89 (γ at peak)
Peak velocity:     0.9946c

Ship mass:         1,000 tonnes
─── Fuel Required by Source ──────────────────────────
Chemical (H₂/O₂): IMPOSSIBLE (fuel > observable universe mass)
Fission (U-235):   IMPOSSIBLE (1.7 × 10²⁴ kg required)
Fusion (D-T):      IMPOSSIBLE (4.1 × 10²³ kg required)
Astrophage:        820 kg          ← MISSION CAPABLE
══════════════════════════════════════════════════════
```

---

## Phase 2 — Memory Architecture (Week 4)
**Goal:** Replace all `new`/`delete` with custom allocators. Benchmark the difference.

Tasks:
- [ ] Implement `core/ArenaAllocator.hpp` (linear bump, reset per frame)
- [ ] Implement `core/SlabAllocator.hpp` (fixed-size blocks for particle data)
- [ ] Implement `core/ObjectPool.hpp` (reusable handles for scene entities)
- [ ] Implement `core/ECS.hpp` — SoA Entity Component System:
  - `PositionComponent[]`, `VelocityComponent[]`, `EnergyComponent[]`
  - Archetype-based storage, cache-line aligned
- [ ] Write benchmarks comparing custom allocators vs std::allocator
- [ ] Migrate particle data structures to SoA ECS

**Deliverable:** Benchmark output showing allocator throughput. ECS holds 2M particles in memory without fragmentation.

---

## Phase 3 — Metal Render Pipeline (Weeks 5–6)
**Goal:** Two star systems visible on screen. Ship arc between them.

Tasks:
- [ ] Integrate metal-cpp (drop into `vendor/`, link in CMake)
- [ ] Set up `MTLDevice`, `MTLCommandQueue`, `CAMetalLayer`
- [ ] Write `gpu/render/Shaders.metal`: vertex + fragment shader for point sprites
- [ ] Render Sol as a yellow point light, Tau Ceti as an orange-red point light
- [ ] Render 8 solar planets in correct orbital positions (from Phase 1 Kepler solver)
- [ ] Render ship trajectory arc as a vertex buffer (computed from Brachistochrone)
- [ ] Implement arcball camera (orbit, zoom, pan) with mouse/trackpad input
- [ ] Integrate Dear ImGui with Metal backend
- [ ] Connect Phase 1 physics output to ImGui dashboard (read-only display first)

**Deliverable:** Window showing two star systems in 3D. Camera is controllable. Physics dashboard shows live numbers. Time slider scrubs the ship position along the arc.

---

## Phase 4 — Metal Compute: Astrophage Particles (Weeks 7–9)
**Goal:** The Astrophage fuel is a visible, simulated particle cloud that depletes during the journey.

Tasks:
- [ ] Set up `MTLComputePipelineState` and compute command encoder
- [ ] Write `gpu/metal/ParticleKernel.metal`:
  - Thread-per-particle position/velocity update
  - Threadgroup shared memory for local accumulation
  - Coalesced memory access pattern (SoA layout from Phase 2 ECS)
- [ ] Write `gpu/metal/SpatialHash.metal`:
  - Uniform grid spatial hash for neighbor search
  - Atomic operations for cell population counts
- [ ] Write `gpu/metal/EnergyAbsorption.metal`:
  - Photon absorption per particle (energy += incident_photons × absorption_rate)
  - Reproduction rule: if energy > threshold, spawn child particle
  - Death rule: if energy < minimum, flag for removal
- [ ] Profile with Xcode GPU Frame Capture:
  - Measure occupancy, memory bandwidth, ALU utilization
  - Optimize until 2M particles run at ≥30fps
- [ ] Render Astrophage cloud as glowing point sprites (color = energy level)
- [ ] Connect fuel tank depletion to ship energy consumption from Phase 1

**Deliverable:** 2M Astrophage particles simulated on GPU, visibly depleting as the ship burns fuel. Xcode profiling screenshot documenting GPU occupancy.

---

## Phase 5 — Interactive Dashboard (Week 10)
**Goal:** Full mission control UI. Sliders drive the simulation live.

Tasks:
- [ ] Configurable parameters via ImGui sliders:
  - Ship mass (100–10,000 tonnes)
  - Acceleration (0.1g–2.0g)
  - Destination (dropdown: Alpha Centauri, Tau Ceti, Barnard's Star, custom ly)
  - Energy source (dropdown updates fuel mass calculation live)
- [ ] Energy comparison panel:
  - Horizontal progress bars, color-coded (red = impossible, green = feasible)
  - Logarithmic scale for the absurd range between Chemical and Astrophage
- [ ] Time control:
  - Play/pause/scrub slider
  - Speed multiplier (1× = real years, up to 1000× compressed)
- [ ] Export: save journey summary as JSON

**Deliverable:** Fully interactive simulation. Changing the ship mass or acceleration live-updates all physics values, particle count, and energy bars.

---

## Phase 6 — WebGPU Browser Port (Weeks 11–13)
**Goal:** Simulation runs in Chrome. Zero installation for recruiters.

Tasks:
- [ ] Add Dawn to CMake as FetchContent dependency
- [ ] Port `ParticleKernel.metal` → `ParticleKernel.wgsl`
  - Document API differences: `device` memory space, `workgroup` memory, `subgroupBallot`
- [ ] Port `SpatialHash.metal` → `SpatialHash.wgsl`
- [ ] Set up Emscripten CMake toolchain file
- [ ] Compile `src/core/` and `src/physics/` to WASM (no GPU code, pure C++)
- [ ] Connect WASM physics output to WebGPU render surface
- [ ] Deploy to GitHub Pages
- [ ] Write a comparison doc: Metal vs WebGPU API differences for same kernel

**Deliverable:** Working URL. Physics dashboard in browser. Note in README: "The WGSL shaders in this module use the same memory tiling strategy as llama.cpp's WebGPU attention kernel."

---

## Milestone Summary

| Phase | Weeks | Key Deliverable | Employer Signal |
|---|---|---|---|
| 0 — Scaffold | 1 | Project builds, CI green | Software engineering hygiene |
| 1 — Physics | 2–3 | Validated journey summary CLI | Scientific computing, numerical methods |
| 2 — Memory | 4 | Benchmarked custom allocators + ECS | Systems C++, memory architecture |
| 3 — Render | 5–6 | Two star systems on screen | GPU rendering, Metal pipeline |
| 4 — Compute | 7–9 | 2M Astrophage particles at 60fps | GPU compute, Metal kernels, profiling |
| 5 — Dashboard | 10 | Full interactive UI | Polish, UX, scientific visualization |
| 6 — WebGPU | 11–13 | Runs in Chrome | WebGPU, WASM, browser ML relevance |

---

## Interview Talking Points (Per Phase)

**Phase 1:** "I implemented the relativistic Tsiolkovsky equation from scratch and cross-validated it against three independent calculators. The fuel mass difference between fusion and Astrophage is 17 orders of magnitude."

**Phase 2:** "I replaced the standard allocator with a slab allocator for particle data. The SoA layout gives the GPU linear memory access — the same reason PyTorch uses contiguous tensors."

**Phase 4:** "The spatial hash kernel uses threadgroup shared memory to reduce global memory accesses by ~8×. That's the same optimization technique used in the FlashAttention CUDA kernel."

**Phase 6:** "The WebGPU shaders use workgroup memory tiling identically to how llama.cpp implements its WebGPU attention pass. The APIs differ but the optimization principle is the same."

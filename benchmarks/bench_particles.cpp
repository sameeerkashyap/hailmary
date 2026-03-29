#include <benchmark/benchmark.h>
#include "core/ECS.hpp"

using namespace phm::core;

// Benchmark SoA particle update throughput (CPU baseline before GPU offload).
static void BM_SoAParticleUpdate(benchmark::State& state) {
    const std::size_t N = state.range(0);
    ParticleWorld world;
    world.resize(N);

    // Initialize with dummy data
    for (std::size_t i = 0; i < N; ++i) {
        world.positions[i]  = { float(i), 0, 0, 0 };
        world.velocities[i] = { 1.0f, 0, 0, 0 };
        world.energies[i]   = { 1000.0f, 0.01f, 0, 0 };
        world.lifetimes[i]  = { 0, true };
    }

    const float dt = 1.0f / 60.0f;

    for (auto _ : state) {
        for (std::size_t i = 0; i < N; ++i) {
            world.positions[i].x += world.velocities[i].vx * dt;
            world.positions[i].y += world.velocities[i].vy * dt;
            world.positions[i].z += world.velocities[i].vz * dt;
            world.energies[i].energy -= world.energies[i].absorption * dt;
            ++world.lifetimes[i].age_ticks;
        }
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * N);
}
BENCHMARK(BM_SoAParticleUpdate)
    ->Arg(100'000)
    ->Arg(1'000'000)
    ->Arg(2'000'000);

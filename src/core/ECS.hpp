#pragma once
// Module 1 — Memory Architecture (Phase 2)
// Structure-of-Arrays ECS for Astrophage particle data.
//
// Requirements:
//   - All component arrays are parallel: particle i lives at index i in every array
//   - SoA layout (not AoS) for sequential GPU/SIMD memory access
//   - Align hot components to cache-line boundaries (16 bytes min)
//   - ParticleWorld owns the arrays; resize() allocates all at once

#include <cstdint>
#include <vector>

namespace phm::core {

// TODO: define PositionComponent  — x, y, z + padding, aligned to 16 bytes
// TODO: define VelocityComponent  — vx, vy, vz + padding, aligned to 16 bytes
// TODO: define EnergyComponent    — energy, absorption_rate + padding
// TODO: define LifetimeComponent  — age_ticks (uint32), alive (bool)

struct PositionComponent;
struct VelocityComponent;
struct EnergyComponent;
struct LifetimeComponent;

// SoA world: all arrays are parallel, indexed by particle id.
struct ParticleWorld {
    std::vector<PositionComponent>  positions;
    std::vector<VelocityComponent>  velocities;
    std::vector<EnergyComponent>    energies;
    std::vector<LifetimeComponent>  lifetimes;

    // TODO: implement resize(), size(), clear()
    void resize(std::size_t n);
    [[nodiscard]] std::size_t size() const noexcept;
    void clear();
};

} // namespace phm::core

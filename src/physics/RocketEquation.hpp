#pragma once
// Module 2 — Relativistic Physics Engine (Phase 1)
// Relativistic Tsiolkovsky rocket equation and fuel mass ratios.
//
// Classic (non-relativistic): m0/mf = exp(Δv / ve)
//
// Full relativistic formula (for exhaust velocities approaching c):
//   m0/mf = ((1 + Δv/c) / (1 - Δv/c)) ^ (c / (2·ve))
//
// The 17-orders-of-magnitude gap between chemical and Astrophage fuel requirements
// is the central numerical fact this module must produce.

#include "Relativity.hpp"
#include <string_view>

namespace phm::physics {

// Effective exhaust velocities (m/s) per propulsion type.
// TODO: fill in physically correct values for each source.
namespace ExhaustVelocity {
    inline constexpr double Chemical   = 0.0;  // TODO: H2/O2 bipropellant (~4,500 m/s)
    inline constexpr double Fission    = 0.0;  // TODO: nuclear thermal (~1e7 m/s)
    inline constexpr double Fusion     = 0.0;  // TODO: D-T inertial confinement (~1e8 m/s)
    inline constexpr double Astrophage = 0.0;  // TODO: photon drive (= C)
}

struct FuelResult {
    double mass_ratio;    // m0 / mf (initial mass / dry mass)
    double fuel_mass_kg;  // fuel required for given dry_mass_kg
    bool   feasible;      // false if fuel_mass_kg > observable universe mass (~1e53 kg)
};

// TODO: implement mass_ratio_relativistic(delta_v, exhaust_vel)
//   Apply the full relativistic rocket equation.
//   delta_v and exhaust_vel in m/s.
[[nodiscard]] inline double mass_ratio_relativistic(double delta_v, double exhaust_vel);

// TODO: implement fuel_required(dry_mass_kg, delta_v, exhaust_vel)
//   Compute fuel mass and feasibility for a given dry mass and Δv.
[[nodiscard]] inline FuelResult fuel_required(double dry_mass_kg,
                                               double delta_v,
                                               double exhaust_vel);

} // namespace phm::physics

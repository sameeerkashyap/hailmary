#pragma once
// Module 2 — Relativistic Physics Engine (Phase 1)
// Lorentz factor, time dilation, relativistic kinetic energy.
//
// All functions are pure math — no state, no side effects.
// Cross-validate outputs against: https://www.omnicalculator.com/physics/time-dilation

#include <cmath>

namespace phm::physics {

// Speed of light in m/s (exact by SI definition)
inline constexpr double C  = 2.99792458e8;
inline constexpr double C2 = C * C;

// TODO: implement lorentz_factor(v)
//   γ(v) = 1 / sqrt(1 - v²/c²)
//   v is in m/s, result is dimensionless, always >= 1
[[nodiscard]] inline double lorentz_factor(double v);

// TODO: implement ship_time_from_earth_time(earth_time_s, v)
//   Ship proper time: Δτ = Δt / γ(v)
[[nodiscard]] inline double ship_time_from_earth_time(double earth_time_s, double v);

// TODO: implement relativistic_ke(mass_kg, v)
//   Relativistic kinetic energy: E_k = (γ - 1) m c²
[[nodiscard]] inline double relativistic_ke(double mass_kg, double v);

// TODO: implement velocity_add(u, v)
//   Relativistic velocity addition (same axis):
//   w = (u + v) / (1 + uv/c²)
//   Result must be < c for any u, v < c
[[nodiscard]] inline double velocity_add(double u, double v);

} // namespace phm::physics

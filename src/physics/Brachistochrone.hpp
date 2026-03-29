#pragma once
// Module 2 — Relativistic Physics Engine (Phase 1)
// Brachistochrone (constant proper-acceleration flip) trajectory.
//
// The ship accelerates at `a` m/s² for the first half, flips, decelerates for the second.
// All equations are in the relativistic (special relativity) regime.
//
// Key equations (derive from the relativistic rocket equations):
//   Position:   x(τ) = (c²/a)(cosh(aτ/c) - 1)          [per leg]
//   Earth time: t(τ) = (c/a) · sinh(aτ/c)               [per leg]
//   Velocity:   v(τ) = c · tanh(aτ/c)
//
// For the full flip trajectory covering distance d:
//   d/2 = (c²/a)(cosh(aτ_half/c) - 1)
//   → solve for τ_half, double it for total ship time
//
// Cross-validate at: https://www.omnicalculator.com/physics/space-travel

#include "Relativity.hpp"
#include <cmath>

namespace phm::physics {

struct BrachistochroneResult {
    double ship_time_s;    // proper time experienced by crew (τ), seconds
    double earth_time_s;   // coordinate time in Earth/inertial frame, seconds
    double peak_velocity;  // velocity at midpoint flip (m/s)
    double gamma_peak;     // Lorentz factor at peak velocity
};

// TODO: implement brachistochrone(dist_m, accel_ms2)
//   Compute the full flip-trajectory result for a one-way trip.
//   dist_m    — total one-way distance in metres
//   accel_ms2 — constant proper acceleration in m/s²
[[nodiscard]] inline BrachistochroneResult brachistochrone(double dist_m, double accel_ms2);

} // namespace phm::physics

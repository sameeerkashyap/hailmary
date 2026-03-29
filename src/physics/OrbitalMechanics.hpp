#pragma once
// Module 2 — Relativistic Physics Engine (Phase 1)
// Kepler orbit positions and RK4 N-body integrator.
//
// Kepler orbit solver:
//   1. Compute mean anomaly M = 2π · (t / T)
//   2. Solve Kepler's equation M = E - e·sin(E) for eccentric anomaly E
//      (use Newton-Raphson iteration, converges in ~5 steps to 1e-12 precision)
//   3. Convert to Cartesian (x, y) in the orbital plane:
//      x = a(cos E - e),  y = a·√(1-e²)·sin E
//
// RK4 integrator:
//   Standard 4th-order Runge-Kutta for ODE: d/dt[pos, vel] = [vel, accel]
//   k1..k4 evaluated at t, t+dt/2, t+dt/2, t+dt

#include <cmath>
#include <array>
#include <functional>

namespace phm::physics {

struct Vec3 {
    double x, y, z;
};

// TODO: implement Vec3 operator+ and operator* (scalar)

struct KeplerOrbit {
    double semi_major_AU;   // semi-major axis in AU
    double eccentricity;
    double inclination_rad;
    double period_years;
};

// Solar system planets in order (Mercury → Neptune).
// TODO: fill in correct orbital elements for all 8 planets.
inline constexpr std::array<KeplerOrbit, 8> kSolarPlanets = {{
    // { semi_major_AU, eccentricity, inclination_rad, period_years }
    { 0.0, 0.0, 0.0, 0.0 },  // TODO: Mercury
    { 0.0, 0.0, 0.0, 0.0 },  // TODO: Venus
    { 0.0, 0.0, 0.0, 0.0 },  // TODO: Earth
    { 0.0, 0.0, 0.0, 0.0 },  // TODO: Mars
    { 0.0, 0.0, 0.0, 0.0 },  // TODO: Jupiter
    { 0.0, 0.0, 0.0, 0.0 },  // TODO: Saturn
    { 0.0, 0.0, 0.0, 0.0 },  // TODO: Uranus
    { 0.0, 0.0, 0.0, 0.0 },  // TODO: Neptune
}};

// TODO: implement kepler_position(orbit, t_years)
//   Returns (x, y, 0) position in AU on the orbital plane for time t_years.
[[nodiscard]] inline Vec3 kepler_position(const KeplerOrbit& orbit, double t_years);

// RK4 integrator state: position + velocity pair.
struct RK4State {
    Vec3 pos;
    Vec3 vel;
};

using AccelFunc = std::function<Vec3(const RK4State&, double t)>;

// TODO: implement rk4_step(state, t, dt, accel)
//   Advance state by one time step dt using 4th-order Runge-Kutta.
//   accel is a callable: (state, t) → acceleration Vec3
[[nodiscard]] inline RK4State rk4_step(const RK4State& s,
                                        double t,
                                        double dt,
                                        const AccelFunc& accel);

} // namespace phm::physics

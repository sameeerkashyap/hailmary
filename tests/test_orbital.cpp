#include <gtest/gtest.h>
#include "physics/OrbitalMechanics.hpp"
#include <cmath>

using namespace phm::physics;

TEST(OrbitalMechanics, EarthAt1AU) {
    // Earth at t=0 should be approximately at (1, 0) AU
    const Vec3 pos = kepler_position(kSolarPlanets[2], 0.0);
    const double r = std::sqrt(pos.x * pos.x + pos.y * pos.y);
    EXPECT_NEAR(r, 1.0, 0.02);
}

TEST(OrbitalMechanics, EarthAfterOneYear) {
    // After 1 year, Earth returns to approximately the same position
    const Vec3 p0 = kepler_position(kSolarPlanets[2], 0.0);
    const Vec3 p1 = kepler_position(kSolarPlanets[2], 1.0);
    const double dx = p1.x - p0.x;
    const double dy = p1.y - p0.y;
    EXPECT_NEAR(dx, 0.0, 0.02);
    EXPECT_NEAR(dy, 0.0, 0.02);
}

TEST(OrbitalMechanics, JupiterOrbitalBounds) {
    // At t=0 (mean anomaly=0 = perihelion): r = a(1-e)
    // At t=T/2 (aphelion): r = a(1+e)
    const auto& jup = kSolarPlanets[4];
    const double r_peri  = jup.semi_major_AU * (1.0 - jup.eccentricity);
    const double r_aphe  = jup.semi_major_AU * (1.0 + jup.eccentricity);

    const Vec3 peri_pos = kepler_position(jup, 0.0);
    const double r0 = std::sqrt(peri_pos.x * peri_pos.x + peri_pos.y * peri_pos.y);
    EXPECT_NEAR(r0, r_peri, 0.01);

    const Vec3 aphe_pos = kepler_position(jup, jup.period_years * 0.5);
    const double r_half = std::sqrt(aphe_pos.x * aphe_pos.x + aphe_pos.y * aphe_pos.y);
    EXPECT_NEAR(r_half, r_aphe, 0.05);
}

TEST(OrbitalMechanics, RK4Step) {
    // Simple test: particle at (1,0,0) with velocity (0,1,0), no acceleration
    RK4State s{ {1, 0, 0}, {0, 1, 0} };
    auto accel = [](const RK4State&, double) -> Vec3 { return {0, 0, 0}; };
    const RK4State s1 = rk4_step(s, 0.0, 1.0, accel);
    EXPECT_NEAR(s1.pos.x, 1.0, 1e-10);
    EXPECT_NEAR(s1.pos.y, 1.0, 1e-10);
    EXPECT_NEAR(s1.vel.x, 0.0, 1e-10);
    EXPECT_NEAR(s1.vel.y, 1.0, 1e-10);
}

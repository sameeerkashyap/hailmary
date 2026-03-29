#include <gtest/gtest.h>
#include "physics/Brachistochrone.hpp"

using namespace phm::physics;

namespace {
    constexpr double kLY = 9.461e15;       // metres per light-year
    constexpr double kYear = 3.156e7;      // seconds per year
    constexpr double kG = 9.81;            // m/s²
}

TEST(Brachistochrone, EarthToTauCeti1g) {
    // 12 ly at 1g — computed from brachistochrone equations.
    // Ship time ~5.16 yr (NOT 3.58 — that's the Alpha Centauri value).
    // Earth time ~13.79 yr, peak velocity ~0.990c, γ ~7.2.
    const auto r = brachistochrone(12.0 * kLY, kG);

    // Ship time: ~5.16 years
    EXPECT_NEAR(r.ship_time_s / kYear, 5.16, 0.05);

    // Earth time: ~13.79 years
    EXPECT_NEAR(r.earth_time_s / kYear, 13.79, 0.1);

    // Peak velocity: ~0.990c
    EXPECT_NEAR(r.peak_velocity / C, 0.990, 0.002);

    // Lorentz factor at peak: ~7.2
    EXPECT_NEAR(r.gamma_peak, 7.2, 0.2);
}

TEST(Brachistochrone, ShipTimeShorterThanEarthTime) {
    const auto r = brachistochrone(4.24 * kLY, kG);  // Alpha Centauri
    EXPECT_LT(r.ship_time_s, r.earth_time_s);
}

TEST(Brachistochrone, PeakVelocitySubLuminal) {
    const auto r = brachistochrone(100.0 * kLY, kG);
    EXPECT_LT(r.peak_velocity, C);
}

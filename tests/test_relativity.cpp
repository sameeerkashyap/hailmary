#include <gtest/gtest.h>
#include "physics/Relativity.hpp"

using namespace phm::physics;

TEST(Relativity, LorentzFactorAtRest) {
    EXPECT_DOUBLE_EQ(lorentz_factor(0.0), 1.0);
}

TEST(Relativity, LorentzFactorAt0_5c) {
    // γ(0.5c) = 1/sqrt(1 - 0.25) = 1/sqrt(0.75) ≈ 1.1547
    const double v = 0.5 * C;
    EXPECT_NEAR(lorentz_factor(v), 1.1547005383792515, 1e-10);
}

TEST(Relativity, LorentzFactorAt0_9946c) {
    // Peak velocity for Earth → Tau Ceti at 1g — expect γ ≈ 10
    const double v = 0.9946 * C;
    EXPECT_GT(lorentz_factor(v), 9.0);
    EXPECT_LT(lorentz_factor(v), 12.0);
}

TEST(Relativity, RelativisticKEAtRest) {
    EXPECT_DOUBLE_EQ(relativistic_ke(1.0, 0.0), 0.0);
}

TEST(Relativity, VelocityAdditionSubluminal) {
    // Adding two 0.9c velocities must be < c
    const double combined = velocity_add(0.9 * C, 0.9 * C);
    EXPECT_LT(combined, C);
    EXPECT_GT(combined, 0.9 * C);
}

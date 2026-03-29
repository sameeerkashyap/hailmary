#pragma once
// Module 5 — Dear ImGui Dashboard (Phase 5)
// Mission control panel: live physics values, energy comparison bars, sliders.
//
// Requirements:
//   - DashboardState holds all mutable inputs (mass, accel, distance)
//     and cached physics outputs; set dirty=true when any input changes
//   - draw_dashboard() recomputes physics when dirty, then draws:
//       • "Mission Parameters" section: sliders for ship_mass, accel_g, distance_ly
//       • "Journey Summary" section: ship time, earth time, peak velocity, γ
//       • "Fuel Required" section: one row per EnergySource, color coded
//           green = feasible, red = IMPOSSIBLE
//   - Fuel rows use ImGui::TextColored

#include "physics/Brachistochrone.hpp"
#include "physics/RocketEquation.hpp"
#include "physics/EnergyComparison.hpp"

#ifdef __APPLE__
#include "imgui.h"
#endif

namespace phm::ui {

struct DashboardState {
    double ship_mass_kg = 1'000'000.0;  // 1000 tonnes default
    double accel_g      = 1.0;
    double distance_ly  = 12.0;         // Earth → Tau Ceti

    physics::BrachistochroneResult trajectory{};
    bool dirty = true;
};

#ifdef __APPLE__
// TODO: implement draw_dashboard(state)
//   Use ImGui::Begin/End, SliderDouble (or SliderScalar), SeparatorText,
//   Text, TextColored. Recompute trajectory when dirty.
inline void draw_dashboard(DashboardState& state);
#endif

} // namespace phm::ui

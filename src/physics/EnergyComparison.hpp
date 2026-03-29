#pragma once
// Module 2 — Relativistic Physics Engine (Phase 1)
// Energy density table: Chemical / Fission / Fusion / Astrophage.
//
// Energy density (J/kg) for each propulsion source.
// These are the numbers that make Astrophage remarkable:
//   Chemical  ~1.4 × 10^7  J/kg
//   Fission   ~8.2 × 10^13 J/kg
//   Fusion    ~3.4 × 10^14 J/kg
//   Astrophage ~c² (near-perfect mass-energy conversion) ~9 × 10^16 J/kg
//
// TODO: fill in the correct energy densities and implement the EnergySource table.

#include <string_view>
#include <array>

namespace phm::physics {

struct EnergySource {
    std::string_view name;
    double           energy_density_J_per_kg;
};

// TODO: define kEnergySources — array of 4 EnergySource entries
// TODO: define kAstrophageEnergyDensity — J/kg for a photon-drive Astrophage fuel

} // namespace phm::physics

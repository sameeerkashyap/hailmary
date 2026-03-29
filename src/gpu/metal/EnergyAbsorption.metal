// Module 3 — Metal Compute Kernel (Phase 4)
// Photon absorption, reproduction, and death for Astrophage particles.
//
// Per-particle rules each tick:
//   1. Skip dead particles (alive[tid] == 0)
//   2. Absorb photons: energy += photon_flux * absorption_rate * dt
//   3. Death:          if energy < death_threshold → set alive[tid] = 0, energy = 0, return
//   4. Reproduction:   if energy > reproduce_threshold → halve energy, set reproduce flag
//      (CPU reads the reproduce flag buffer each frame and spawns child particles)

#include <metal_stdlib>
using namespace metal;

// TODO: define AbsorptionParams struct
//   Fields: photon_flux (float), absorption_rate (float),
//           reproduce_threshold (float), death_threshold (float),
//           dt (float), num_particles (uint)

// TODO: implement energy_absorption kernel
//   Signature: kernel void energy_absorption(
//       device float4*               energies  [[ buffer(0) ]],  // x=energy, y=reproduce flag
//       device uint*                 alive     [[ buffer(1) ]],
//       constant AbsorptionParams&   params    [[ buffer(2) ]],
//       uint                         tid       [[ thread_position_in_grid ]]
//   )

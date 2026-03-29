// Module 3 — Metal Compute Kernel (Phase 4)
// Thread-per-particle position/velocity update.
//
// Requirements:
//   - SoA layout: separate float4 buffers for positions, velocities, energies
//   - One thread per particle: thread_position_in_grid → particle index
//   - Guard: return immediately if tid >= num_particles
//   - Integration: pos += vel * dt,  vel *= (1 - drag * dt)
//   - Write results back to the same buffers (in-place update)

#include <metal_stdlib>
using namespace metal;

// TODO: define SimParams struct
//   Fields: dt (float), drag (float), num_particles (uint),
//           energy_threshold_reproduce (float), energy_min_survive (float)

// TODO: implement particle_update kernel
//   Signature: kernel void particle_update(
//       device float4*      positions   [[ buffer(0) ]],
//       device float4*      velocities  [[ buffer(1) ]],
//       device float4*      energies    [[ buffer(2) ]],
//       constant SimParams& params      [[ buffer(3) ]],
//       uint                tid         [[ thread_position_in_grid ]]
//   )

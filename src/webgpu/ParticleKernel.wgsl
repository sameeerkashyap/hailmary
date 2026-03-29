// Module 6 — WebGPU Browser Port (Phase 6)
// Astrophage particle update — port of ParticleKernel.metal to WGSL.
//
// API differences vs Metal to document:
//   Metal threadgroups      →  WGSL workgroups (@workgroup_size)
//   Metal device buffers    →  WGSL storage buffers (var<storage, read_write>)
//   Metal constant buffers  →  WGSL uniform buffers (var<uniform>)
//   [[thread_position_in_grid]] → @builtin(global_invocation_id)
//   simd_group ops          →  subgroupBallot (requires enable subgroups)
//
// Requirements:
//   - Bind group 0: positions (storage rw), velocities (storage rw),
//                   energies (storage rw), params (uniform)
//   - @compute @workgroup_size(64)
//   - Guard: if global_invocation_id.x >= params.num_particles { return; }
//   - Same integration as the Metal kernel: pos += vel*dt, vel *= (1 - drag*dt)

// TODO: define SimParams struct (dt, drag, num_particles, ...)
// TODO: declare bindings: positions, velocities, energies, params
// TODO: implement particle_update compute entry point

// Module 3 — Metal Compute Kernel (Phase 4)
// Uniform grid spatial hash for Astrophage neighbor search.
//
// Two-pass algorithm:
//   Pass 1 — count:   for each particle, atomically increment its cell's counter
//   Pass 2 — scatter: for each particle, claim a slot in the cell and write its index
//
// Requirements:
//   - world_to_cell(): map a world-space position to a 3D grid cell index
//   - cell_index(): flatten a 3D cell coordinate to a 1D buffer index
//   - Pass 1 uses atomic_fetch_add_explicit on cell_counts
//   - Pass 2 uses atomic_fetch_add_explicit on cell_offsets to claim a slot,
//     then writes the particle id to particle_ids[slot]

#include <metal_stdlib>
using namespace metal;

// TODO: define GridParams struct
//   Fields: world_min (float3), cell_size (float), grid_dims (uint3), num_particles (uint)

// TODO: implement world_to_cell(pos, params) → uint3
// TODO: implement cell_index(cell, params) → uint

// TODO: implement spatial_hash_count kernel
//   Signature: kernel void spatial_hash_count(
//       device const float4*  positions   [[ buffer(0) ]],
//       device atomic_uint*   cell_counts [[ buffer(1) ]],
//       constant GridParams&  params      [[ buffer(2) ]],
//       uint                  tid         [[ thread_position_in_grid ]]
//   )

// TODO: implement spatial_hash_scatter kernel
//   Signature: kernel void spatial_hash_scatter(
//       device const float4*  positions     [[ buffer(0) ]],
//       device atomic_uint*   cell_offsets  [[ buffer(1) ]],
//       device uint*          particle_ids  [[ buffer(2) ]],
//       constant GridParams&  params        [[ buffer(3) ]],
//       uint                  tid           [[ thread_position_in_grid ]]
//   )

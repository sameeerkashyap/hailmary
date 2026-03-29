// Module 6 — WebGPU Browser Port (Phase 6)
// Spatial hash — port of SpatialHash.metal to WGSL.
//
// API differences vs Metal:
//   atomic_fetch_add_explicit(...)  →  atomicAdd(&cell_counts[idx], 1u)
//   device atomic_uint*             →  var<storage, read_write> array<atomic<u32>>
//
// Requirements:
//   - Same two-pass algorithm as the Metal version (count, then scatter)
//   - Bind group 0: positions (storage read), cell_counts (storage rw, atomic),
//                   params (uniform)
//   - world_to_cell() and cell_index() as plain functions
//   - @compute @workgroup_size(64) for both passes

// TODO: define GridParams struct
// TODO: declare bindings: positions, cell_counts (atomic), params
// TODO: implement world_to_cell(pos: vec3f) -> vec3u
// TODO: implement cell_index(cell: vec3u) -> u32
// TODO: implement spatial_hash_count compute entry point

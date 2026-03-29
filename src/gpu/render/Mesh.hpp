#pragma once
// Module 4 — Metal Render Pipeline (Phase 3)
// Holds vertex/index MTLBuffers and draw parameters for a single mesh primitive.
//
// Requirements:
//   - vertex_buffer: MTLBuffer containing interleaved or SoA vertex data
//   - index_buffer:  MTLBuffer containing uint16 or uint32 indices
//   - release(): call ->release() on both buffers, set to nullptr

#ifdef __APPLE__
#include <Metal/Metal.hpp>
#include <cstdint>

namespace phm::render {

struct Mesh {
    MTL::Buffer*   vertex_buffer = nullptr;
    MTL::Buffer*   index_buffer  = nullptr;
    uint32_t       index_count   = 0;
    uint32_t       vertex_count  = 0;
    MTL::IndexType index_type    = MTL::IndexTypeUInt32;

    // TODO: implement release()
    void release();
};

} // namespace phm::render

#endif // __APPLE__

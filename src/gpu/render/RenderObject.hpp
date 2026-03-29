#pragma once
// Module 4 — Metal Render Pipeline (Phase 3)
// Combines a Mesh + Material + world transform.
// The transform is written each frame by the physics/ECS layer.

#ifdef __APPLE__
#include "Mesh.hpp"
#include "Material.hpp"
#include <simd/simd.h>
#include <string>

namespace phm::render {

struct RenderObject {
    std::string    name;
    Mesh*          mesh      = nullptr;   // non-owning
    Material*      material  = nullptr;   // non-owning
    simd::float4x4 transform = matrix_identity_float4x4;
    bool           visible   = true;
};

} // namespace phm::render

#endif // __APPLE__

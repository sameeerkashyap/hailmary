#pragma once
// Module 4 — Metal Render Pipeline (Phase 3)
// Arcball orbit camera — orbit, zoom, pan. Outputs view + projection matrices.
//
// Internal representation: spherical coordinates (azimuth, elevation, distance)
// around a target point. Mouse drag → orbit(), scroll → zoom().
//
// view_matrix():
//   1. Convert spherical → Cartesian eye position
//   2. Build orthonormal basis: forward = normalize(target - eye),
//      right = normalize(cross(forward, world_up)),
//      up    = cross(right, forward)
//   3. Compose into a column-major 4x4 view matrix
//
// projection_matrix(): standard perspective projection (clip-space depth 0..1 for Metal)

#ifdef __APPLE__
#include <simd/simd.h>

namespace phm::render {

class Camera {
public:
    Camera() = default;

    // TODO: implement orbit(delta_azimuth, delta_elevation)
    //   Add deltas to azimuth/elevation; clamp elevation to [-π/2+ε, π/2-ε]
    void orbit(float delta_azimuth, float delta_elevation);

    // TODO: implement zoom(delta)
    //   Decrease distance by delta; clamp to a minimum (e.g. 0.1)
    void zoom(float delta);

    // TODO: implement pan(dx, dy)
    //   Translate target in the camera's right/up plane, scaled by distance
    void pan(float dx, float dy);

    // TODO: implement view_matrix()
    [[nodiscard]] simd::float4x4 view_matrix() const;

    // TODO: implement projection_matrix(aspect, fov_y_rad, near, far)
    //   Metal clip space: depth range [0, 1] (not [-1, 1] like OpenGL)
    [[nodiscard]] simd::float4x4 projection_matrix(float aspect, float fov_y_rad,
                                                     float near,   float far) const;

private:
    simd::float3 m_target    = {0, 0, 0};
    float        m_azimuth   = 0.0f;
    float        m_elevation = 0.3f;
    float        m_distance  = 10.0f;
};

} // namespace phm::render

#endif // __APPLE__

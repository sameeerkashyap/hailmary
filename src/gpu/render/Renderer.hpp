#pragma once
// Module 4 — Metal Render Pipeline (Phase 3)
// Top-level renderer: owns pipeline states, executes the draw loop.
//
// Each frame:
//   1. Acquire a drawable from the CAMetalLayer
//   2. Create a command buffer + render command encoder
//   3. Set pipeline state, depth state, viewport
//   4. For each visible RenderObject: bind buffers, set uniforms, draw indexed
//   5. Encode ImGui draw data (imgui_impl_metal)
//   6. Commit the command buffer and present the drawable

#ifdef __APPLE__
#include "RenderObject.hpp"
#include "Camera.hpp"
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <vector>

namespace phm::render {

class Renderer {
public:
    // TODO: constructor — load Shaders.metallib, create pipeline states, depth state
    explicit Renderer(MTL::Device* device);

    // TODO: destructor — release all Metal objects
    ~Renderer();

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;

    // TODO: implement draw(layer, objects, camera, dt)
    void draw(CA::MetalLayer*                  layer,
              const std::vector<RenderObject>& objects,
              const Camera&                    camera,
              float                            dt);

private:
    MTL::Device*              m_device;
    MTL::CommandQueue*        m_queue           = nullptr;
    MTL::RenderPipelineState* m_pbr_pipeline    = nullptr;
    MTL::RenderPipelineState* m_point_pipeline  = nullptr;
    MTL::DepthStencilState*   m_depth_state     = nullptr;
    MTL::Texture*             m_depth_texture   = nullptr;
};

} // namespace phm::render

#endif // __APPLE__

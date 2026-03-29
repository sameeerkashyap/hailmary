#pragma once
// Module 4 — Metal Render Pipeline (Phase 3)
// PBR material — MTLTexture refs + scalar factors matching the glTF 2.0 material model.
//
// Texture slots (all sourced from GLB binary blob via tinygltf + stb_image):
//   base_color:         sRGB RGBA8 — albedo / opacity
//   normal:             linear RGB8 — tangent-space normal map
//   metallic_roughness: linear — R=occlusion, G=roughness, B=metallic (glTF packing)
//   emissive:           sRGB RGB8 — self-illumination
//
// Star override: when is_star=true the fragment shader skips the BRDF entirely
// and outputs emissive.rgb * emissive_factor * luminosity_scale.

#ifdef __APPLE__
#include <Metal/Metal.hpp>
#include <simd/simd.h>

namespace phm::render {

struct Material {
    MTL::Texture* base_color         = nullptr;
    MTL::Texture* normal             = nullptr;
    MTL::Texture* metallic_roughness = nullptr;
    MTL::Texture* emissive           = nullptr;

    simd::float4 base_color_factor = {1, 1, 1, 1};
    float         metallic_factor  = 1.0f;
    float         roughness_factor = 1.0f;
    simd::float3  emissive_factor  = {0, 0, 0};

    bool  is_star          = false;
    float luminosity_scale = 1.0f;

    // TODO: implement release() — call ->release() on all non-null textures
    void release();
};

} // namespace phm::render

#endif // __APPLE__

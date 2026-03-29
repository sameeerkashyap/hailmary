// Module 4 — Metal Render Pipeline (Phase 3)
// Cook-Torrance PBR shader — GGX/Smith/Schlick BRDF.
//
// Vertex shader:
//   - Transform position: clip_pos = view_proj * model * vec4(pos, 1)
//   - Transform normal/tangent by the 3x3 normal matrix (upper-left of model)
//   - Pass world_pos, normal, tangent, bitangent, uv to fragment stage
//
// Fragment shader — PBR pipeline:
//   1. Sample base_color, normal map, metallic_roughness, emissive textures
//   2. Decode tangent-space normal: n = normalize(TBN * (sample * 2 - 1))
//   3. Compute BRDF terms:
//      D = GGX normal distribution    (roughness²)
//      G = Smith geometry function    (roughness)
//      F = Schlick Fresnel           (F0 = mix(0.04, albedo, metallic))
//      specular = DGF / (4 · NdotV · NdotL)
//      diffuse  = (1-F)(1-metallic) · albedo / π
//   4. color = (diffuse + specular) * NdotL + emissive
//   5. Reinhard tone map: color = color / (color + 1)
//   6. Gamma correct: color = pow(color, 1/2.2)
//
// Star override: if is_star → skip BRDF, output emissive * luminosity_scale

#include <metal_stdlib>
using namespace metal;

// TODO: define VertexIn struct (position, normal, uv, tangent — matching vertex buffer layout)
// TODO: define VertexOut struct (clip position, world_pos, normal, tangent, bitangent, uv)
// TODO: define SceneUniforms struct (view_proj, model matrix, camera_pos, light_dir)
// TODO: define MaterialUniforms struct (base_color_factor, metallic_factor, roughness_factor,
//                                       emissive_factor, is_star, luminosity_scale)

// TODO: implement vertex_main
// TODO: implement distribution_ggx(NdotH, roughness) → float
// TODO: implement geometry_smith(NdotV, NdotL, roughness) → float
// TODO: implement fresnel_schlick(cos_theta, F0) → float3
// TODO: implement fragment_main

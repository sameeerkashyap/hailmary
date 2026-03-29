#pragma once
// Module 4 — Metal Render Pipeline (Phase 3)
// Parses .glb files via tinygltf, uploads geometry + textures to Metal.
//
// Upload strategy (Apple Silicon unified memory):
//   Vertex/index data → MTLBuffer with MTLResourceStorageModeShared (zero-copy)
//   Textures          → MTLTexture (RGBA8 sRGB for color, linear for normal/MR)
//
// glTF PBR material mapping:
//   pbrMetallicRoughness.baseColorTexture      → Material::base_color
//   normalTexture                              → Material::normal
//   pbrMetallicRoughness.metallicRoughnessTexture → Material::metallic_roughness
//   emissiveTexture                            → Material::emissive

#ifdef __APPLE__
#include "Mesh.hpp"
#include "Material.hpp"
#include <Metal/Metal.hpp>
#include <string>
#include <vector>

namespace phm::render {

struct GLTFScene {
    std::vector<Mesh>     meshes;
    std::vector<Material> materials;
};

class GLTFLoader {
public:
    explicit GLTFLoader(MTL::Device* device);

    // TODO: implement load(path, scene_out, error_out)
    //   Parse the .glb at path using tinygltf.
    //   For each mesh primitive: call upload_mesh().
    //   For each material texture: call upload_texture().
    //   Return false and populate error_out on failure.
    [[nodiscard]] bool load(const std::string& path,
                            GLTFScene&         scene_out,
                            std::string&       error_out);

private:
    MTL::Device* m_device;

    // TODO: implement upload_mesh() — newBuffer with StorageModeShared
    [[nodiscard]] Mesh upload_mesh(const void* vertices, std::size_t vsize,
                                   const void* indices,  std::size_t isize,
                                   uint32_t    index_count);

    // TODO: implement upload_texture() — newTexture + replaceRegion
    [[nodiscard]] MTL::Texture* upload_texture(const unsigned char* data,
                                                int width, int height, bool srgb);
};

} // namespace phm::render

#endif // __APPLE__

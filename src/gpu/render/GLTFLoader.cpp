// Module 4 — Metal Render Pipeline (Phase 3)
// GLTFLoader implementation.

#ifdef __APPLE__
#include "GLTFLoader.hpp"

// One translation unit owns the tinygltf + stb_image implementations.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

namespace phm::render {

GLTFLoader::GLTFLoader(MTL::Device* device) : m_device(device) {}

bool GLTFLoader::load(const std::string& /*path*/,
                      GLTFScene&         /*scene_out*/,
                      std::string&       error_out) {
    // TODO: implement full glTF parsing
    //   1. tinygltf::TinyGLTF loader.LoadBinaryFromFile(...)
    //   2. Iterate model.meshes → primitives → call upload_mesh()
    //   3. Iterate model.materials → textures → call upload_texture()
    //   4. Build Material structs with PBR factor values from the glTF material
    error_out = "GLTFLoader::load — not yet implemented";
    return false;
}

Mesh GLTFLoader::upload_mesh(const void* /*vertices*/, std::size_t /*vsize*/,
                              const void* /*indices*/,  std::size_t /*isize*/,
                              uint32_t    /*index_count*/) {
    // TODO: newBuffer(data, size, MTLResourceStorageModeShared) for both buffers
    return {};
}

MTL::Texture* GLTFLoader::upload_texture(const unsigned char* /*data*/,
                                          int /*width*/, int /*height*/, bool /*srgb*/) {
    // TODO: newTexture(descriptor) + replaceRegion(...)
    return nullptr;
}

} // namespace phm::render

#endif // __APPLE__

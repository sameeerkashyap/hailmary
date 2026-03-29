# glTF / GLB Integration with Metal

How to load `.glb` models (planets, stars, spaceship) into the Metal render pipeline.

---

## Why glTF/GLB

- **GLB** = binary glTF — single self-contained file (geometry + textures + materials packed together). Easiest to ship and load.
- **glTF 2.0** uses PBR metallic-roughness materials that map directly to what you'd write in a Metal PBR shader.
- Free, high-quality assets exist for every object in this project (see Assets section below).
- Industry standard — every 3D tool (Blender, Cinema4D, Sketchfab) exports it.

---

## Library: tinygltf

Single-header C++ glTF 2.0 loader. Zero dependencies (bundles stb_image). Drop it in and go.

```
vcpkg install tinygltf
```

Or manually: download `tiny_gltf.h` from https://github.com/syoyo/tinygltf and drop into `vendor/`.

---

## The Loading Pipeline

```
GLB file on disk
      │
      ▼
tinygltf::TinyGLTF::LoadBinaryFromFile()
      │  parses JSON + binary blob
      ▼
tinygltf::Model
  ├── meshes[]      → vertex/index data
  ├── materials[]   → PBR material params + texture indices
  ├── textures[]    → image references
  ├── images[]      → raw pixel data
  └── nodes[]       → transform hierarchy (position, rotation, scale)
      │
      ▼
GLTFLoader (your class)
  ├── MTLBuffer (vertex)    ← upload raw accessor bytes to GPU
  ├── MTLBuffer (index)     ← upload index data
  ├── MTLTexture[]          ← upload images via MTLTextureDescriptor
  └── simd::float4x4[]      ← flattened node world transforms
      │
      ▼
Metal Render Pass
  setVertexBuffer / setFragmentTexture / drawIndexedPrimitives
```

---

## Directory Structure

```
hailmary/
├── assets/
│   └── models/
│       ├── sun.glb
│       ├── earth.glb
│       ├── mars.glb
│       ├── jupiter.glb
│       ├── saturn.glb          ← rings make this special
│       ├── tau_ceti.glb
│       └── hail_mary_ship.glb
│
├── src/
│   └── gpu/
│       └── render/
│           ├── GLTFLoader.hpp       ← parses GLB → Metal buffers
│           ├── GLTFLoader.cpp
│           ├── Mesh.hpp             ← holds MTLBuffer refs + draw params
│           ├── Material.hpp         ← holds MTLTexture refs + PBR factors
│           ├── RenderObject.hpp     ← mesh + material + transform
│           └── Shaders.metal        ← PBR vertex + fragment shaders
```

---

## GLTFLoader Implementation

```cpp
// src/gpu/render/GLTFLoader.hpp
#pragma once

// Define before including — only in ONE .cpp file
// #define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
#include <tiny_gltf.h>
#include <Metal/Metal.hpp>          // metal-cpp
#include <simd/simd.h>
#include <vector>
#include <string>

namespace PHM::Render {

struct Mesh {
    MTL::Buffer* vertexBuffer  = nullptr;
    MTL::Buffer* indexBuffer   = nullptr;
    uint32_t     indexCount    = 0;
    MTL::IndexType indexType   = MTL::IndexTypeUInt32;
    int          materialIndex = -1;
    simd::float4x4 transform   = matrix_identity_float4x4;
};

struct Material {
    MTL::Texture* baseColorTexture        = nullptr;  // albedo / diffuse
    MTL::Texture* normalTexture           = nullptr;  // normal map
    MTL::Texture* metallicRoughnessTexture = nullptr; // B=metallic, G=roughness
    MTL::Texture* emissiveTexture         = nullptr;  // for stars/engine glow
    simd::float4  baseColorFactor         = {1,1,1,1};
    float         metallicFactor          = 0.0f;
    float         roughnessFactor         = 1.0f;
    simd::float3  emissiveFactor          = {0,0,0};
};

struct LoadedModel {
    std::vector<Mesh>     meshes;
    std::vector<Material> materials;
    std::string           name;
};

class GLTFLoader {
public:
    explicit GLTFLoader(MTL::Device* device) : m_device(device) {}

    // Load a .glb file and return all meshes + materials GPU-ready
    LoadedModel load(const std::string& path);

private:
    MTL::Device* m_device;

    MTL::Buffer*  uploadBuffer(const tinygltf::Model& model,
                               int accessorIndex);
    MTL::Texture* uploadTexture(const tinygltf::Model& model,
                                int textureIndex);
    simd::float4x4 nodeWorldTransform(const tinygltf::Model& model,
                                      int nodeIndex);
    void flattenNode(const tinygltf::Model& model,
                     int nodeIndex,
                     simd::float4x4 parentTransform,
                     LoadedModel& out);
};

} // namespace PHM::Render
```

```cpp
// src/gpu/render/GLTFLoader.cpp
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "GLTFLoader.hpp"
#include <cassert>

namespace PHM::Render {

LoadedModel GLTFLoader::load(const std::string& path) {
    tinygltf::TinyGLTF loader;
    tinygltf::Model    gltf;
    std::string        err, warn;

    bool ok = loader.LoadBinaryFromFile(&gltf, &err, &warn, path);
    assert(ok && "Failed to load GLB file");

    // Upload all materials first
    LoadedModel result;
    result.name = path;

    for (auto& mat : gltf.materials) {
        Material m;
        m.baseColorFactor = simd::float4{
            (float)mat.pbrMetallicRoughness.baseColorFactor[0],
            (float)mat.pbrMetallicRoughness.baseColorFactor[1],
            (float)mat.pbrMetallicRoughness.baseColorFactor[2],
            (float)mat.pbrMetallicRoughness.baseColorFactor[3]
        };
        m.metallicFactor  = (float)mat.pbrMetallicRoughness.metallicFactor;
        m.roughnessFactor = (float)mat.pbrMetallicRoughness.roughnessFactor;

        int bcIdx = mat.pbrMetallicRoughness.baseColorTexture.index;
        int nIdx  = mat.normalTexture.index;
        int mrIdx = mat.pbrMetallicRoughness.metallicRoughnessTexture.index;
        int emIdx = mat.emissiveTexture.index;

        if (bcIdx >= 0) m.baseColorTexture         = uploadTexture(gltf, bcIdx);
        if (nIdx  >= 0) m.normalTexture             = uploadTexture(gltf, nIdx);
        if (mrIdx >= 0) m.metallicRoughnessTexture  = uploadTexture(gltf, mrIdx);
        if (emIdx >= 0) m.emissiveTexture            = uploadTexture(gltf, emIdx);

        result.materials.push_back(m);
    }

    // Traverse node hierarchy from default scene
    int sceneIdx = gltf.defaultScene >= 0 ? gltf.defaultScene : 0;
    for (int nodeIdx : gltf.scenes[sceneIdx].nodes) {
        flattenNode(gltf, nodeIdx, matrix_identity_float4x4, result);
    }

    return result;
}

void GLTFLoader::flattenNode(const tinygltf::Model& model,
                              int nodeIndex,
                              simd::float4x4 parentTransform,
                              LoadedModel& out)
{
    const tinygltf::Node& node = model.nodes[nodeIndex];

    // Build local transform
    simd::float4x4 local = matrix_identity_float4x4;
    if (node.matrix.size() == 16) {
        // Column-major matrix directly in node
        for (int col = 0; col < 4; col++)
            for (int row = 0; row < 4; row++)
                local.columns[col][row] = (float)node.matrix[col*4+row];
    } else {
        // TRS decomposition
        simd::float4x4 T = matrix_identity_float4x4;
        simd::float4x4 R = matrix_identity_float4x4;
        simd::float4x4 S = matrix_identity_float4x4;

        if (node.translation.size() == 3) {
            T.columns[3] = {(float)node.translation[0],
                            (float)node.translation[1],
                            (float)node.translation[2], 1.0f};
        }
        if (node.scale.size() == 3) {
            S.columns[0][0] = (float)node.scale[0];
            S.columns[1][1] = (float)node.scale[1];
            S.columns[2][2] = (float)node.scale[2];
        }
        // Quaternion to matrix
        if (node.rotation.size() == 4) {
            float x=(float)node.rotation[0], y=(float)node.rotation[1],
                  z=(float)node.rotation[2], w=(float)node.rotation[3];
            R.columns[0] = {1-2*(y*y+z*z), 2*(x*y+z*w), 2*(x*z-y*w), 0};
            R.columns[1] = {2*(x*y-z*w), 1-2*(x*x+z*z), 2*(y*z+x*w), 0};
            R.columns[2] = {2*(x*z+y*w), 2*(y*z-x*w), 1-2*(x*x+y*y), 0};
        }
        local = simd_mul(T, simd_mul(R, S));
    }

    simd::float4x4 worldTransform = simd_mul(parentTransform, local);

    // If node has a mesh, upload its primitives
    if (node.mesh >= 0) {
        const tinygltf::Mesh& gMesh = model.meshes[node.mesh];
        for (auto& prim : gMesh.primitives) {
            Mesh mesh;
            mesh.transform    = worldTransform;
            mesh.materialIndex = prim.material;

            // Interleave POSITION, NORMAL, TEXCOORD_0 into one vertex buffer
            // (or use separate buffers — separate is simpler, interleaved is faster)
            // Here we use the index accessor for the index buffer
            if (prim.indices >= 0) {
                mesh.indexBuffer = uploadBuffer(model, prim.indices);
                const tinygltf::Accessor& idxAcc = model.accessors[prim.indices];
                mesh.indexCount = (uint32_t)idxAcc.count;
                mesh.indexType  = (idxAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                                  ? MTL::IndexTypeUInt16 : MTL::IndexTypeUInt32;
            }

            // Upload each attribute as its own MTLBuffer
            // Bind them separately in the render encoder
            auto posIt = prim.attributes.find("POSITION");
            if (posIt != prim.attributes.end())
                mesh.vertexBuffer = uploadBuffer(model, posIt->second);

            out.meshes.push_back(mesh);
        }
    }

    // Recurse into children
    for (int child : node.children)
        flattenNode(model, child, worldTransform, out);
}

MTL::Buffer* GLTFLoader::uploadBuffer(const tinygltf::Model& model,
                                       int accessorIndex)
{
    const tinygltf::Accessor&   acc   = model.accessors[accessorIndex];
    const tinygltf::BufferView& view  = model.bufferViews[acc.bufferView];
    const tinygltf::Buffer&     buf   = model.buffers[view.buffer];

    const uint8_t* data = buf.data.data() + view.byteOffset + acc.byteOffset;
    size_t         size = view.byteLength;

    // Shared storage mode = CPU + GPU accessible on Apple Silicon (unified memory)
    return m_device->newBuffer(data, size, MTL::ResourceStorageModeShared);
}

MTL::Texture* GLTFLoader::uploadTexture(const tinygltf::Model& model,
                                         int textureIndex)
{
    const tinygltf::Texture& tex   = model.textures[textureIndex];
    const tinygltf::Image&   image = model.images[tex.source];

    MTL::TextureDescriptor* desc = MTL::TextureDescriptor::texture2DDescriptor(
        MTL::PixelFormatRGBA8Unorm_sRGB,
        image.width, image.height,
        /*mipmapped=*/true
    );
    desc->setUsage(MTL::TextureUsageShaderRead);
    desc->setStorageMode(MTL::StorageModeShared);

    MTL::Texture* texture = m_device->newTexture(desc);
    MTL::Region region = MTL::Region::Make2D(0, 0, image.width, image.height);
    texture->replaceRegion(region, 0, image.image.data(), image.width * 4);

    // TODO: generate mipmaps via MTLBlitCommandEncoder for better quality
    return texture;
}

} // namespace PHM::Render
```

---

## Metal Shaders for PBR glTF Materials

```metal
// src/gpu/render/Shaders.metal

#include <metal_stdlib>
using namespace metal;

// ── Vertex Input ─────────────────────────────────────────────────
struct VertexIn {
    float3 position  [[attribute(0)]];
    float3 normal    [[attribute(1)]];
    float2 texcoord  [[attribute(2)]];
    float4 tangent   [[attribute(3)]];  // .w = handedness
};

// ── Uniforms ──────────────────────────────────────────────────────
struct FrameUniforms {
    float4x4 viewProjection;
    float3   cameraPosition;
    float3   lightPosition;     // star position in world space
    float3   lightColor;
    float    time;
};

struct ObjectUniforms {
    float4x4 modelMatrix;
    float4x4 normalMatrix;      // inverse-transpose of model
};

// ── PBR Material Params ──────────────────────────────────────────
struct MaterialUniforms {
    float4 baseColorFactor;
    float  metallicFactor;
    float  roughnessFactor;
    float3 emissiveFactor;
};

// ── Vertex → Fragment ────────────────────────────────────────────
struct VertexOut {
    float4 position  [[position]];
    float3 worldPos;
    float3 normal;
    float2 texcoord;
    float3 tangent;
    float3 bitangent;
};

vertex VertexOut vertex_gltf(
    VertexIn         in       [[stage_in]],
    constant FrameUniforms&  frame  [[buffer(2)]],
    constant ObjectUniforms& obj    [[buffer(3)]])
{
    VertexOut out;
    float4 worldPos = obj.modelMatrix * float4(in.position, 1.0);
    out.worldPos    = worldPos.xyz;
    out.position    = frame.viewProjection * worldPos;
    out.normal      = normalize((obj.normalMatrix * float4(in.normal, 0.0)).xyz);
    out.texcoord    = in.texcoord;

    // TBN matrix for normal mapping
    float3 T = normalize((obj.modelMatrix * float4(in.tangent.xyz, 0.0)).xyz);
    float3 B = cross(out.normal, T) * in.tangent.w;
    out.tangent   = T;
    out.bitangent = B;

    return out;
}

// ── PBR Fragment Shader ──────────────────────────────────────────
// Cook-Torrance BRDF: D (GGX), G (Smith), F (Schlick)

float distributionGGX(float3 N, float3 H, float roughness) {
    float a  = roughness * roughness;
    float a2 = a * a;
    float NdotH = saturate(dot(N, H));
    float denom = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
    return a2 / (M_PI_F * denom * denom);
}

float geometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(float3 N, float3 V, float3 L, float roughness) {
    return geometrySchlickGGX(saturate(dot(N,V)), roughness)
         * geometrySchlickGGX(saturate(dot(N,L)), roughness);
}

float3 fresnelSchlick(float cosTheta, float3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

fragment float4 fragment_gltf(
    VertexOut                in           [[stage_in]],
    constant FrameUniforms&  frame        [[buffer(2)]],
    constant MaterialUniforms& mat        [[buffer(4)]],
    texture2d<float>         baseColorTex [[texture(0)]],
    texture2d<float>         normalTex    [[texture(1)]],
    texture2d<float>         mrTex        [[texture(2)]],  // metallic-roughness
    texture2d<float>         emissiveTex  [[texture(3)]],
    sampler                  samp         [[sampler(0)]])
{
    float4 baseColor = baseColorTex.sample(samp, in.texcoord) * mat.baseColorFactor;
    float2 mr        = mrTex.sample(samp, in.texcoord).bg;  // b=metallic, g=roughness
    float  metallic  = mr.x * mat.metallicFactor;
    float  roughness = mr.y * mat.roughnessFactor;

    // Normal mapping
    float3 normalSample = normalTex.sample(samp, in.texcoord).xyz * 2.0 - 1.0;
    float3x3 TBN = float3x3(in.tangent, in.bitangent, in.normal);
    float3 N = normalize(TBN * normalSample);

    float3 V = normalize(frame.cameraPosition - in.worldPos);
    float3 L = normalize(frame.lightPosition  - in.worldPos);
    float3 H = normalize(V + L);

    float3 F0 = mix(float3(0.04), baseColor.rgb, metallic);

    // Cook-Torrance BRDF
    float  D = distributionGGX(N, H, roughness);
    float  G = geometrySmith(N, V, L, roughness);
    float3 F = fresnelSchlick(saturate(dot(H, V)), F0);

    float3 numerator   = D * G * F;
    float  denominator = 4.0 * saturate(dot(N,V)) * saturate(dot(N,L)) + 0.0001;
    float3 specular    = numerator / denominator;

    float3 kD = (1.0 - F) * (1.0 - metallic);
    float  NdotL = saturate(dot(N, L));

    float3 Lo = (kD * baseColor.rgb / M_PI_F + specular) * frame.lightColor * NdotL;

    // Ambient + emissive
    float3 ambient   = float3(0.03) * baseColor.rgb;
    float3 emissive  = emissiveTex.sample(samp, in.texcoord).rgb * mat.emissiveFactor;

    float3 color = ambient + Lo + emissive;

    // Tone mapping (Reinhard) + gamma correction
    color = color / (color + 1.0);
    color = pow(color, 1.0 / 2.2);

    return float4(color, baseColor.a);
}
```

---

## Rendering a Loaded Model

```cpp
// In your Renderer::draw() loop, for each RenderObject:

void Renderer::drawModel(MTL::RenderCommandEncoder* encoder,
                         const LoadedModel& model,
                         simd::float4x4 modelMatrix)
{
    for (const Mesh& mesh : model.meshes) {
        // Combine node transform with scene placement
        simd::float4x4 finalTransform = simd_mul(modelMatrix, mesh.transform);

        ObjectUniforms objUniforms;
        objUniforms.modelMatrix  = finalTransform;
        objUniforms.normalMatrix = simd_transpose(simd_inverse(finalTransform));

        encoder->setVertexBytes(&objUniforms, sizeof(objUniforms), 3);

        // Bind material textures
        if (mesh.materialIndex >= 0) {
            const Material& mat = model.materials[mesh.materialIndex];
            if (mat.baseColorTexture)         encoder->setFragmentTexture(mat.baseColorTexture, 0);
            if (mat.normalTexture)            encoder->setFragmentTexture(mat.normalTexture,    1);
            if (mat.metallicRoughnessTexture) encoder->setFragmentTexture(mat.metallicRoughnessTexture, 2);
            if (mat.emissiveTexture)          encoder->setFragmentTexture(mat.emissiveTexture,  3);

            MaterialUniforms matUniforms{
                mat.baseColorFactor, mat.metallicFactor,
                mat.roughnessFactor, mat.emissiveFactor
            };
            encoder->setFragmentBytes(&matUniforms, sizeof(matUniforms), 4);
        }

        // Draw
        encoder->setVertexBuffer(mesh.vertexBuffer, 0, 0);
        encoder->drawIndexedPrimitives(
            MTL::PrimitiveTypeTriangle,
            mesh.indexCount,
            mesh.indexType,
            mesh.indexBuffer,
            0
        );
    }
}
```

---

## Special Cases for PHM

### Stars (Sun, Tau Ceti) — Emission Override
Stars don't receive lighting — they ARE the light source. Add a special draw path:
```metal
// In fragment shader, check a uniform flag
if (isEmissive) {
    return float4(emissiveFactor * baseColor.rgb * starBrightness, 1.0);
}
```
Set `starBrightness` based on the star's luminosity from `physics/`. Sol = 1.0, Tau Ceti = 0.516.

### Saturn — Rings
Saturn's ring system is a separate mesh in the GLB. It renders with `baseColorFactor.a < 1.0` (partially transparent). Enable alpha blending for this draw call:
```cpp
// In your MTLRenderPipelineDescriptor setup:
pipelineDesc->colorAttachments()->object(0)->setBlendingEnabled(true);
pipelineDesc->colorAttachments()->object(0)->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
pipelineDesc->colorAttachments()->object(0)->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
```

### Hail Mary Ship — Engine Glow
The ship's Astrophage drive emits a blue-white glow. Use the `emissiveTexture` + `emissiveFactor` with high values (>1.0 is fine in HDR). The glow intensity ties to `physics/` fuel burn rate — more thrust = brighter drive cone.

---

## Free Asset Sources

| Object | Source | Notes |
|---|---|---|
| Sun | NASA 3D Resources | `sun.glb` — free, high quality |
| Earth | NASA 3D Resources | `earth.glb` — includes cloud layer |
| Mars, Jupiter, Saturn | NASA 3D Resources | All planets available |
| Tau Ceti | Modify sun.glb | Swap texture, scale to 0.783 solar radii, tint orange |
| Hail Mary Ship | Sketchfab | Search "Project Hail Mary" — fan-made models exist, check license |
| Generic spacecraft | Sketchfab (free filter) | Countless options under CC license |

**NASA 3D Resources:** https://nasa3d.arc.nasa.gov/models
**Sketchfab free models:** https://sketchfab.com/features/free-3d-models

All NASA assets are public domain. Sketchfab — check each model's CC license. For your own ship, Blender → export GLB takes about an hour to learn.

---

## Coordinate System Note

glTF uses a **right-handed, Y-up** coordinate system. Metal's NDC is left-handed. Handle this in your projection matrix — flip the Z sign:

```cpp
// In your camera projection matrix construction:
// Standard perspective, then negate Z to convert RH → LH
float4x4 projection = makePerspective(fovY, aspect, zNear, zFar);
projection.columns[2][2] *= -1.0f;
projection.columns[2][3] *= -1.0f;
```

Or simply define your world as Y-up right-handed and let the view-projection matrix handle it. Consistency is what matters.

---

## Integration Checklist

- [ ] Add `tinygltf` to `vcpkg.json`
- [ ] Create `src/gpu/render/GLTFLoader.hpp/.cpp`
- [ ] Create `src/gpu/render/Mesh.hpp` and `Material.hpp`
- [ ] Update `Shaders.metal` with PBR vertex + fragment shaders above
- [ ] Download NASA planet GLBs into `assets/models/`
- [ ] Load Earth at 1 AU, render test — verify normals and textures look correct
- [ ] Add star emission override for Sun and Tau Ceti
- [ ] Add alpha blending pipeline state for Saturn rings + ship exhaust
- [ ] Connect ship emissive intensity to physics fuel burn rate

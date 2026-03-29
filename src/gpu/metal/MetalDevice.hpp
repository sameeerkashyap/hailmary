#pragma once
// Module 3 — Metal Compute Kernel (Phase 4)
// MTLDevice + MTLCommandQueue setup via metal-cpp.
//
// Requirements:
//   - Call MTL::CreateSystemDefaultDevice() to get the GPU
//   - Create a MTLCommandQueue from the device
//   - RAII ownership: release both in the destructor
//   - Non-copyable (GPU devices are singletons per process)

#ifdef __APPLE__
#include <Metal/Metal.hpp>
#include <stdexcept>

namespace phm::gpu {

class MetalDevice {
public:
    // TODO: construct — create system default device + command queue
    MetalDevice();

    // TODO: destructor — release command queue, then device
    ~MetalDevice();

    MetalDevice(const MetalDevice&)            = delete;
    MetalDevice& operator=(const MetalDevice&) = delete;

    [[nodiscard]] MTL::Device*       device()        const noexcept;
    [[nodiscard]] MTL::CommandQueue* command_queue()  const noexcept;

private:
    MTL::Device*       m_device        = nullptr;
    MTL::CommandQueue* m_command_queue = nullptr;
};

} // namespace phm::gpu

#endif // __APPLE__

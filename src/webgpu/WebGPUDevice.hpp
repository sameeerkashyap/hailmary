#pragma once
// Module 6 — WebGPU Browser Port (Phase 6)
// Dawn C++ device setup — mirrors the MetalDevice interface.
//
// Initialization sequence (Dawn C++ API):
//   1. wgpu::CreateInstance(nullptr) → WGPUInstance
//   2. instance.RequestAdapter(...)  → WGPUAdapter  (async callback)
//   3. adapter.RequestDevice(...)    → WGPUDevice    (async callback)
//   4. device.GetQueue()             → WGPUQueue
//
// Note: Dawn's async request callbacks require the instance to be polled
// (wgpuInstanceProcessEvents) or you can use the synchronous C API variant.

#ifdef PHM_WEBGPU_ENABLED
#include <webgpu/webgpu_cpp.h>
#include <stdexcept>

namespace phm::webgpu {

class WebGPUDevice {
public:
    // TODO: implement constructor — request adapter, request device, get queue
    WebGPUDevice();

    ~WebGPUDevice() = default;

    [[nodiscard]] wgpu::Device& device() noexcept;
    [[nodiscard]] wgpu::Queue&  queue()  noexcept;

private:
    wgpu::Instance m_instance;
    wgpu::Adapter  m_adapter;
    wgpu::Device   m_device;
    wgpu::Queue    m_queue;
};

} // namespace phm::webgpu
#endif // PHM_WEBGPU_ENABLED

#pragma once
// Module 1 — Memory Architecture (Phase 2)
// Linear bump allocator for per-frame scratch memory.
// Reset cost is O(1) — just resets the offset pointer.

#include <cstddef>
#include <memory>

namespace phm::core {

class ArenaAllocator {
public:
    explicit ArenaAllocator(std::size_t capacity);

    ArenaAllocator(const ArenaAllocator&)            = delete;
    ArenaAllocator& operator=(const ArenaAllocator&) = delete;

    // Allocate `size` bytes aligned to `alignment`. Returns nullptr on OOM.
    [[nodiscard]] void* alloc(std::size_t size, std::size_t alignment = alignof(std::max_align_t));

    // Construct a T in-place inside the arena. Returns nullptr on OOM.
    template <typename T, typename... Args>
    [[nodiscard]] T* alloc_one(Args&&... args);

    // O(1) reset — does NOT call destructors.
    void reset() noexcept;

    [[nodiscard]] std::size_t used()     const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;

private:
    // TODO: backing buffer, capacity, current offset
};

} // namespace phm::core

#pragma once
// Module 1 — Memory Architecture (Phase 2)
// Fixed-size block allocator for homogeneous objects (e.g. Astrophage particles).
// Free list gives O(1) alloc/free with zero fragmentation.
//
// Requirements:
//   - BlockSize must be >= sizeof(void*) to store the free-list pointer in-place
//   - Alignment must be a power of two
//   - alloc() pops the free-list head; free() pushes back
//   - No heap allocations after construction

#include <cstddef>

namespace phm::core {

template <std::size_t BlockSize, std::size_t Alignment = alignof(std::max_align_t)>
class SlabAllocator {
public:
    explicit SlabAllocator(std::size_t num_blocks);

    SlabAllocator(const SlabAllocator&)            = delete;
    SlabAllocator& operator=(const SlabAllocator&) = delete;

    // Pop a block from the free list. Returns nullptr when exhausted.
    [[nodiscard]] void* alloc() noexcept;

    // Push a block back onto the free list.
    void free(void* ptr) noexcept;

    [[nodiscard]] std::size_t used()     const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;

private:
    // TODO: backing pool, capacity, used count, free-list head
};

} // namespace phm::core

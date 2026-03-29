#pragma once
// Module 1 — Memory Architecture (Phase 2)
// Reusable handle pool for scene entities.
//
// Requirements:
//   - Handle encodes {index, generation} to detect use-after-free
//   - create() reuses slots from a free list; generation increments on destroy()
//   - get(handle) returns nullptr if the handle's generation doesn't match the slot
//   - All storage is a single flat vector (no per-object heap allocation)

#include <cstdint>
#include <vector>

namespace phm::core {

struct Handle {
    uint32_t index      : 24;
    uint32_t generation : 8;

    bool operator==(const Handle&) const = default;
    static constexpr Handle null() { return {0xFFFFFF, 0xFF}; }
};

template <typename T>
class ObjectPool {
public:
    explicit ObjectPool(std::size_t capacity);

    template <typename... Args>
    [[nodiscard]] Handle create(Args&&... args);

    void destroy(Handle h);

    [[nodiscard]] T*       get(Handle h);
    [[nodiscard]] const T* get(Handle h) const;

    [[nodiscard]] std::size_t alive_count() const;

private:
    // TODO: Slot struct (value, generation, alive flag), slot vector, free-index vector
};

} // namespace phm::core

#include <gtest/gtest.h>
#include "core/ArenaAllocator.hpp"
#include "core/SlabAllocator.hpp"
#include "core/ObjectPool.hpp"

using namespace phm::core;

// ── Arena ─────────────────────────────────────────────────────────────────────
TEST(ArenaAllocator, BasicAlloc) {
    ArenaAllocator arena(1024);
    void* p = arena.alloc(64);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(arena.used(), 64u);
}

TEST(ArenaAllocator, AllocOne) {
    ArenaAllocator arena(1024);
    int* x = arena.alloc_one<int>(42);
    ASSERT_NE(x, nullptr);
    EXPECT_EQ(*x, 42);
}

TEST(ArenaAllocator, Reset) {
    ArenaAllocator arena(1024);
    arena.alloc(512);
    EXPECT_EQ(arena.used(), 512u);
    arena.reset();
    EXPECT_EQ(arena.used(), 0u);
}

TEST(ArenaAllocator, OOMReturnsNull) {
    ArenaAllocator arena(16);
    arena.alloc(16);
    void* p = arena.alloc(1);
    EXPECT_EQ(p, nullptr);
}

TEST(ArenaAllocator, AlignmentRespected) {
    ArenaAllocator arena(4096);
    arena.alloc(1);  // misalign deliberately
    void* p = arena.alloc(1, 16);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(p) % 16, 0u);
}

// ── Slab ──────────────────────────────────────────────────────────────────────
TEST(SlabAllocator, BasicAllocFree) {
    SlabAllocator<64> slab(16);
    void* p = slab.alloc();
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(slab.used(), 1u);
    slab.free(p);
    EXPECT_EQ(slab.used(), 0u);
}

TEST(SlabAllocator, OOMReturnsNull) {
    SlabAllocator<64> slab(2);
    slab.alloc();
    slab.alloc();
    void* p = slab.alloc();
    EXPECT_EQ(p, nullptr);
}

TEST(SlabAllocator, ReuseAfterFree) {
    SlabAllocator<64> slab(1);
    void* p1 = slab.alloc();
    slab.free(p1);
    void* p2 = slab.alloc();
    EXPECT_NE(p2, nullptr);
}

// ── Object pool ───────────────────────────────────────────────────────────────
TEST(ObjectPool, CreateAndGet) {
    ObjectPool<int> pool(8);
    Handle h = pool.create(99);
    int* v = pool.get(h);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(*v, 99);
}

TEST(ObjectPool, DestroyInvalidatesHandle) {
    ObjectPool<int> pool(8);
    Handle h = pool.create(1);
    pool.destroy(h);
    EXPECT_EQ(pool.get(h), nullptr);
}

TEST(ObjectPool, HandleReuse) {
    ObjectPool<int> pool(2);
    Handle h1 = pool.create(1);
    pool.destroy(h1);
    Handle h2 = pool.create(2);
    // h1 is now stale — should not alias h2's data
    EXPECT_NE(pool.get(h1), pool.get(h2));
}

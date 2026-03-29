#include <benchmark/benchmark.h>
#include "core/ArenaAllocator.hpp"
#include "core/SlabAllocator.hpp"

using namespace phm::core;

// ── Arena vs std::allocator ───────────────────────────────────────────────────
static void BM_ArenaAlloc(benchmark::State& state) {
    ArenaAllocator arena(64 * 1024 * 1024);  // 64 MB scratch
    const std::size_t alloc_size = state.range(0);
    for (auto _ : state) {
        arena.reset();
        for (int i = 0; i < 1000; ++i) {
            benchmark::DoNotOptimize(arena.alloc(alloc_size));
        }
    }
    state.SetBytesProcessed(state.iterations() * 1000 * alloc_size);
}
BENCHMARK(BM_ArenaAlloc)->Arg(64)->Arg(256)->Arg(1024);

static void BM_StdAllocAlloc(benchmark::State& state) {
    const std::size_t alloc_size = state.range(0);
    for (auto _ : state) {
        std::vector<void*> ptrs(1000);
        for (auto& p : ptrs) p = ::operator new(alloc_size);
        for (auto  p : ptrs) ::operator delete(p);
    }
    state.SetBytesProcessed(state.iterations() * 1000 * alloc_size);
}
BENCHMARK(BM_StdAllocAlloc)->Arg(64)->Arg(256)->Arg(1024);

// ── Slab vs std::allocator (fixed size) ───────────────────────────────────────
static void BM_SlabAlloc(benchmark::State& state) {
    SlabAllocator<64> slab(1024);
    for (auto _ : state) {
        std::vector<void*> ptrs;
        ptrs.reserve(256);
        for (int i = 0; i < 256; ++i) ptrs.push_back(slab.alloc());
        for (void* p : ptrs) slab.free(p);
    }
}
BENCHMARK(BM_SlabAlloc);

static void BM_StdAllocFixed(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<void*> ptrs(256);
        for (auto& p : ptrs) p = ::operator new(64);
        for (auto  p : ptrs) ::operator delete(p);
    }
}
BENCHMARK(BM_StdAllocFixed);

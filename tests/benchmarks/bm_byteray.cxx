#include <benchmark/benchmark.h>
#include "byteray.hxx"

static void BM_Create(benchmark::State& state) {
    for (auto _ : state)
        ByteRay(state.range(0));
}
BENCHMARK(BM_Create)->RangeMultiplier(4)->Range(1, 1<<24);

BENCHMARK_MAIN();
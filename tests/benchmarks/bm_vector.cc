#include <benchmark/benchmark.h>
#include <vector>

static void BM_Create(benchmark::State& state) {
    for (auto _ : state)
        std::vector<char> v(state.range(0));
}
BENCHMARK(BM_Create)->RangeMultiplier(4)->Range(1, 1<<24);

BENCHMARK_MAIN();
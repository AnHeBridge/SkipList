#include "benchmark/benchmark.h"
#include "skiplist.hpp"
#include <limits>
#include <random>

int16_t generaterand_random(int16_t llimit,int16_t rlimit) {
	static std::mt19937 engine{ std::random_device{} ()};
	static std::uniform_int_distribution<int16_t>distribution(llimit,rlimit);
	return distribution(engine);
}

static void BM_Insert(benchmark::State& state) {
	std::unique_ptr<SkipList<int16_t,int16_t>> a(new SkipList<int16_t,int16_t>(32));
	for (auto _ : state)  {
		int16_t value = generaterand_random(std::numeric_limits<int16_t>::min(),std::numeric_limits<int16_t>::max());
		a->insert(value,value);
	}
}

static void BM_find(benchmark::State& state) {
	std::unique_ptr<SkipList<int16_t,int16_t>> a(new SkipList<int16_t,int16_t>(32));
	for (int i = 0 ; i < 10000 ; i++)  {
		int16_t value = generaterand_random(std::numeric_limits<int16_t>::min(),std::numeric_limits<int16_t>::max());
		a->insert(value,value);
	}
	for (auto _ : state)  {
		SkipList<int16_t,int16_t>::const_iterator skit = a->lower_bound(100);
	}
}

static void BM_rankele(benchmark::State& state) {
	std::unique_ptr<SkipList<int16_t,int16_t>> a(new SkipList<int16_t,int16_t>(32));
	for (int i = 0 ; i < 10000 ; i++)  {
		int16_t value = generaterand_random(std::numeric_limits<int16_t>::min(),std::numeric_limits<int16_t>::max());
		a->insert(value,value);
	}
	for (auto _ : state)  {
		SkipList<int16_t,int16_t>::const_iterator skit = a->getRankElement(100);
	}
}

BENCHMARK(BM_Insert);
BENCHMARK(BM_find);
BENCHMARK(BM_rankele);
BENCHMARK_MAIN();


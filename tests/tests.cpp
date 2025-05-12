#include "benchmark/benchmark.h"
#include "matrix.h"
#include <cstdlib>
#include <random>

std::mt19937 g_gen;

static void Setup(const benchmark::State& state) {
    g_gen.seed(100);
}

static Matrix<float> getUniformMatrix(int size, float min, float max) {
    std::uniform_real_distribution<float> dis(min, max);

    Matrix<float> matrix(0.0f, size);
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            matrix.set(row, col, dis(g_gen));
        }
    }
    return matrix;
}

static void BM_Trivial(benchmark::State& state) {
    const auto size = state.range(0);
    const auto A = getUniformMatrix(size, -10.0f, 10.0f);
    const auto B = getUniformMatrix(size, -10.0f, 10.0f);

    for (auto _ : state) {
        auto C = A * B;
    }
}

static void BM_Strassen3(benchmark::State& state) {
    const auto size = state.range(0);
    const auto A = getUniformMatrix(size, -10.0f, 10.0f);
    const auto B = getUniformMatrix(size, -10.0f, 10.0f);

    for (auto _ : state) {
        auto C = strassen3(A, B);
    }
}

static void BM_Strassen3_32(benchmark::State& state) {
    const auto size = state.range(0);
    const auto A = getUniformMatrix(size, -10.0f, 10.0f);
    const auto B = getUniformMatrix(size, -10.0f, 10.0f);

    for (auto _ : state) {
        auto C = strassen3(A, B, 32);
    }
}

//BENCHMARK(BM_Trivial)->RangeMultiplier(2)->Range(8, 2048)->Setup(Setup);
//BENCHMARK(BM_Strassen3)->RangeMultiplier(2)->Range(8, 2048)->Setup(Setup);
//BENCHMARK(BM_Strassen3_32)->RangeMultiplier(2)->Range(8, 2048)->Setup(Setup);

BENCHMARK(BM_Trivial)
    ->Arg(100)
    ->Arg(200)
    ->Arg(300)
    ->Arg(400)
    ->Arg(500)
    ->Arg(600)
    ->Arg(700)
    ->Arg(800)
    ->Arg(900)
    ->Arg(1000)
    ->Setup(Setup);
BENCHMARK(BM_Strassen3)
->Arg(100)
->Arg(200)
->Arg(300)
->Arg(400)
->Arg(500)
->Arg(600)
->Arg(700)
->Arg(800)
->Arg(900)
->Arg(1000)
->Setup(Setup);
BENCHMARK(BM_Strassen3_32)
->Arg(100)
->Arg(200)
->Arg(300)
->Arg(400)
->Arg(500)
->Arg(600)
->Arg(700)
->Arg(800)
->Arg(900)
->Arg(1000)
->Setup(Setup);
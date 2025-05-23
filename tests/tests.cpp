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

static void BM_Strassen3_50(benchmark::State& state) {
    const auto size = state.range(0);
    const auto A = getUniformMatrix(size, -10.0f, 10.0f);
    const auto B = getUniformMatrix(size, -10.0f, 10.0f);

    for (auto _ : state) {
        auto C = strassen3(A, B, 50);
    }
}

static void BM_Strassen3_100(benchmark::State& state) {
    const auto size = state.range(0);
    const auto A = getUniformMatrix(size, -10.0f, 10.0f);
    const auto B = getUniformMatrix(size, -10.0f, 10.0f);

    for (auto _ : state) {
        auto C = strassen3(A, B, 100);
    }
}

static void BM_Strassen3_150(benchmark::State& state) {
    const auto size = state.range(0);
    const auto A = getUniformMatrix(size, -10.0f, 10.0f);
    const auto B = getUniformMatrix(size, -10.0f, 10.0f);

    for (auto _ : state) {
        auto C = strassen3(A, B, 150);
    }
}

static void BM_Strassen3_200(benchmark::State& state) {
    const auto size = state.range(0);
    const auto A = getUniformMatrix(size, -10.0f, 10.0f);
    const auto B = getUniformMatrix(size, -10.0f, 10.0f);

    for (auto _ : state) {
        auto C = strassen3(A, B, 200);
    }
}

int multiplier = 3;
int start = 9;
int end = 81;

BENCHMARK(BM_Trivial)->RangeMultiplier(multiplier)->Range(start, end)->Setup(Setup);
BENCHMARK(BM_Strassen3)->RangeMultiplier(multiplier)->Range(start, end)->Setup(Setup);
BENCHMARK(BM_Strassen3_50)->RangeMultiplier(multiplier)->Range(start, end)->Setup(Setup);
BENCHMARK(BM_Strassen3_100)->RangeMultiplier(multiplier)->Range(start, end)->Setup(Setup);
BENCHMARK(BM_Strassen3_150)->RangeMultiplier(multiplier)->Range(start, end)->Setup(Setup);
BENCHMARK(BM_Strassen3_200)->RangeMultiplier(multiplier)->Range(start, end)->Setup(Setup);
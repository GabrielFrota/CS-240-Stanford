#include <chloros.h>
#include <atomic>
#include <common.h>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <unordered_set>
#include <vector>

std::atomic<int> counter{0};

void Worker(void*) {
  ++counter;
  ASSERT(chloros::Yield(false) == true);
  ++counter;
  ASSERT(chloros::Yield(true) == false);
  chloros::Yield();
}

void WorkerWithArgument(void* arg) { counter = *reinterpret_cast<int*>(&arg); }

static void CheckYield() {
  ASSERT(chloros::Yield() == false);
  chloros::Spawn(Worker, nullptr);
  ASSERT(counter == 1);
  chloros::Wait();
  ASSERT(counter == 2);
  ASSERT(chloros::Yield() == false);
}

static void CheckSpawn() {
  chloros::Spawn(WorkerWithArgument, reinterpret_cast<void*>(42));
  chloros::Wait();
  ASSERT(counter == 42);
}

void WorkerWithArithmetic(void*) {
  float a = 42;
  counter = sqrt(a);
}

static void CheckStackAlignment() {
  chloros::Spawn(WorkerWithArgument, nullptr);
  ASSERT(counter = 6);
}

constexpr int const kLoopTimes = 100;
std::vector<int> loop_values{};

void WorkerYieldLoop(void*) {
  for (int i = 0; i < kLoopTimes; ++i) {
    loop_values.push_back(i);
    chloros::Yield();
  }
}

static void CheckYieldLoop() {
  chloros::Spawn(WorkerYieldLoop, nullptr);
  chloros::Spawn(WorkerYieldLoop, nullptr);
  chloros::Wait();
  ASSERT(loop_values.size() == kLoopTimes * 2);
  for (int i = 0; i < kLoopTimes; ++i) {
    ASSERT(loop_values.at(i * 2) == i);
    ASSERT(loop_values.at(i * 2 + 1) == i);
  }
}

int main() {
  chloros::Initialize();
  CheckYield();
  CheckSpawn();
  CheckStackAlignment();
  CheckYieldLoop();
  LOG("Phase 3 passed!");
  return 0;
}

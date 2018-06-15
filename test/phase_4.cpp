#include <chloros.h>
#include <common.h>
#include <array>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <unordered_set>
#include <vector>

constexpr int const kThreads = 32;

void Worker(void* data) {
  int d = *reinterpret_cast<int*>(&data);
  auto count = chloros::GetThreadCount();
  ASSERT(count.first == kThreads - d + 1);
  ASSERT(count.second == 0);
  if (d != 0) {
    chloros::Spawn(Worker, reinterpret_cast<void*>(d - 1));
    count = chloros::GetThreadCount();
    ASSERT(count.first == d);
    ASSERT(count.second == 0);
  }
}

void CheckGc() {
  chloros::Initialize();
  chloros::Spawn(Worker, reinterpret_cast<void*>(kThreads));
  auto count = chloros::GetThreadCount();
  ASSERT(count.first == kThreads);
  ASSERT(count.second == 0);
  chloros::Wait();
  count = chloros::GetThreadCount();
  ASSERT(count.first == 0);
  ASSERT(count.second == 0);
}

int main() {
  CheckGc();
  LOG("Phase 4 passed!");
  return 0;
}

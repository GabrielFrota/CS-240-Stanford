#include <chloros.h>
#include <common.h>
#include <array>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <unordered_set>
#include <vector>

static void CheckIds() {
  constexpr int const kThreads = 512;
  for (int i = 0; i < kThreads; ++i) {
    auto&& thread = chloros::Thread(false);
    ASSERT(thread.id == static_cast<uint64_t>(i), "Wrong thread ID.");
  }
}

static void CheckState() {
  constexpr int const kThreads = 512;
  for (int i = 0; i < kThreads; ++i) {
    auto&& thread = chloros::Thread(false);
    ASSERT(thread.state == chloros::Thread::State::kWaiting, "Wrong state.");
  }
}

static void CheckNoStack() {
  auto&& thread = chloros::Thread(false);
  ASSERT(thread.stack == nullptr, "Stack is not null.");
}

static void CheckStack() {
  constexpr int const kThreads = 512;
  std::vector<std::unique_ptr<chloros::Thread>> threads;
  std::unordered_set<uint8_t*> stacks;

  for (int i = 0; i < kThreads; ++i) {
    threads.emplace_back(std::make_unique<chloros::Thread>(true));
  }
  for (int i = 0; i < kThreads; ++i) {
    NOT_NULL(threads[i]->stack);
    ASSERT(reinterpret_cast<intptr_t>(threads[i]->stack) % 16 == 0,
           "Stack is not aligned to 16 bytes.");
    ASSERT(stacks.count(threads[i]->stack) == 0, "Stack is not unique.");
    stacks.insert(threads[i]->stack);
  }
}

int main() {
  CheckIds();
  CheckState();
  CheckNoStack();
  CheckStack();
  LOG("Phase 1 passed!");
  return 0;
}

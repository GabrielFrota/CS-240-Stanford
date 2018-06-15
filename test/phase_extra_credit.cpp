#include <chloros.h>
#include <atomic>
#include <thread>
#include <vector>

constexpr int const kKernelThreads = 50;

void GreenThreadWorker(void*) { printf("Green thread on kernel thread.\n"); }

void KernelThreadWorker(int n) {
  chloros::Initialize();
  chloros::Spawn(GreenThreadWorker, nullptr);
  chloros::Wait();
  printf("Finished thread %d.\n", n);
}

int main() {
  std::vector<std::thread> threads{};
  for (int i = 0; i < kKernelThreads; ++i) {
    threads.emplace_back(KernelThreadWorker, i);
  }
  for (int i = 0; i < kKernelThreads; ++i) {
    threads[i].join();
  }
  return 0;
}

#ifndef CHLOROS_INCLUDE_CHLOROS_H_
#define CHLOROS_INCLUDE_CHLOROS_H_

#if !defined(__x86_64__)
#error Library only implemented for AMD64.
#endif

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

namespace chloros {

// This represents the execution context, specifically all callee-saved
// registers. But where is the instruction pointer saved?
struct Context {
  uint64_t rsp;
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t rbx;
  uint64_t rbp;
  uint32_t mxcsr;
  uint32_t x87;
};

// A thread consists of a state, an execution context, and possibly a stack. For
// this implementation, there are two kinds of threads: initial threads, and
// spawned threads. Initial threads are threads that are created using
// `Initialize`, and will probably call `Wait` to wait for all spawned threads
// to finish. It will enter `kWaiting` state when it does so. On the other hand,
// spawned threads are created using `Spawn`, and will enter through
// `ThreadEntry`.
struct Thread {
  enum class State {
    kWaiting,
    kReady,
    kRunning,
    kZombie,
  };

  // Global thread ID counter.
  static std::atomic<uint64_t> next_id;

  uint64_t id;
  State state;
  Context context;
  uint8_t* stack;

  // Constructor. `create_stack` specifies whether we want to create a stack
  // associated with this thread.
  Thread(bool create_stack);
  ~Thread();

  // Disable copying and moving.
  Thread(Thread const&) = delete;
  Thread(Thread&&) = delete;
  Thread& operator=(Thread const&) = delete;
  Thread& operator=(Thread&&) = delete;

  // Print debug information about the thread. Might be useful for your
  // debugging.
  void PrintDebug();
};

using Function = std::add_pointer<void(void*)>::type;

// Initialize with current context. This function will grab current running
// context and set it as `current_thread`, so we have something to switch from.
// This must be run before all other threading functions on each kernel thread.
// Before extra credit phase, there will be only one kernel thread, so this will
// be called only once (somewhere in the unit test). In other words, the number
// of initial threads is the same as the number of kernel threads. How to make
// sure this initial thread will not be scheduled onto other kernel threads (for
// extra credit phase)?
void Initialize();

// Create a new green thread and execute function inside it. After allocating
// and initializing the thread, current thread must yield execution to it.
void Spawn(Function fn, void* arg);

// Yield execution. Make sure it behaves like a round-robin scheduler! Returns
// whether the action was successful. If there are no other thread to yield to,
// it will return false. The argument specifies whether we also yield to waiting
// threads. This is important because if the initial threads yield to other
// initial threads that are stuck in `Wait()`, it will loop forever. Again, you
// will have multiple initial threads only in the extra credit phase where you
// have multiple kernel threads. So you could ignore this for now.
bool Yield(bool only_ready = false) __attribute__((noinline));

// Wait till all other green threads are done. Call this only from initial
// threads. It will wait for ready threads but not other waiting threads.
// Otherwise multiple waiting threads will wait for each other indefinitely. And
// this is the scenario where a thread will become waiting.
void Wait();

// Get rid of zombies before they overwhelm us!
void GarbageCollect();

// Get number of ready and zombie threads. Used only in testing.
std::pair<int, int> GetThreadCount();

extern "C" {

// Entry function for threads that are spawn. This will be called by the
// assembly function `start_thread`, which will fetch the arguments on stack and
// put them in the correct registers.
void ThreadEntry(Function fn, void* arg) __asm__("thread_entry");
}

}  // namespace chloros

#endif  // CHLOROS_INCLUDE_CHLOROS_H_

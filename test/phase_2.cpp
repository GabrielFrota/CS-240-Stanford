#include <chloros.h>
#include <common.h>
#include <array>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <unordered_set>
#include <vector>

extern "C" {
void ContextSwitch(chloros::Context* old_context,
                   chloros::Context* new_context) __asm__("context_switch");
}

static chloros::Context context_a;
static chloros::Context context_b;

static void CheckContextSwitch() {
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
  uint64_t rsp;

  context_b.r12 = 12;
  context_b.r13 = 13;
  context_b.r14 = 14;
  context_b.r15 = 15;

  asm("sub $8, %%rsp\n\t"
      "movq %%rsp, %0\n\t"
      "movq %%rbp, %1\n\t"
      "add $8, %%rsp"
      : "=m"(context_b.rsp), "=m"(context_b.rbp)
      :
      :);

  ContextSwitch(&context_a, &context_b);

  asm("movq %%r12, %0\n\t"
      "movq %%r13, %1\n\t"
      "movq %%r14, %2\n\t"
      "movq %%r15, %3\n\t"
      "movq %%rsp, %4\n\t"
      : "=m"(r12), "=m"(r13), "=m"(r14), "=m"(r15), "=m"(rsp)
      :
      :);

  ASSERT(r12 == context_b.r12, "Register R12 incorrect.");
  ASSERT(r13 == context_b.r13, "Register R13 incorrect.");
  ASSERT(r14 == context_b.r14, "Register R14 incorrect.");
  ASSERT(r15 == context_b.r15, "Register R15 incorrect.");
  ASSERT(rsp == context_b.rsp + 8, "Register RSP incorrect.");

  asm("add $1, %%r12\n\t"
      "add $2, %%r14\n\t"
      "movq %%r12, %%r13\n\t"
      "movq %%r14, %%r15"
      :
      :
      : "r13", "r15");

  ContextSwitch(&context_b, &context_a);
  ASSERT(context_b.r12 = context_b.r13);
  ASSERT(context_b.r14 = context_b.r15);
}

int main() {
  CheckContextSwitch();
  LOG("Phase 2 passed!");
  return 0;
}

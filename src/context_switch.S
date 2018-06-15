/**
 * Context switches to new_context from old_context.
 *
 * Saves the current execution context into the `Context` structure pointed to
 * by the first paramter and restores the execution context pointed to by the
 * second paramter. As such, when this function returns, it jumps to the address
 * at the top of the stack (pointed to by %rsp).
 *
 * This function has the following signature in C:
 * context_switch(Context* old_context, Context* new_context)
 */
.text
.globl context_switch
.align 16
context_switch:
  movq    %rsp, 0x0(%rdi)

  stmxcsr 0x38(%rdi)
  fnstcw  0x3C(%rdi)

  ldmxcsr 0x38(%rsi)
  fldcw   0x3C(%rsi)
  /* FIXME: Phase 2
   * Save and restore execution context.
   * Part of the code is provided.
   */

/**
 * Initial function implicitly executed by a thread.
 *
 * This function should be jumped into implicitly by the green threads library.
 * It expects a function pointer at the top of the stack, and its argument above
 * that. It sets up the arguments in the right registers accordingly, and call
 * `thread_entry` which will handle subsequent initialization.
 */
.globl start_thread
.align 16
start_thread:
  push    %rbp
  movq    %rsp, %rbp
  movq    0x8(%rbp), %rdi
  movq    0x10(%rbp), %rsi
  callq   thread_entry
  hlt

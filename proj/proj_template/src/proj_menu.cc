#include "proj_menu.h"

#include <stdio.h>
#include <stdint.h>

#include "cfu.h"
#include "menu.h"

namespace {

// ------------------------------------------------------------
// Cycle counter (CFU-Playground compatible)
// ------------------------------------------------------------
static inline uint32_t get_cycles() {
  uint32_t cycles;
  asm volatile ("rdcycle %0" : "=r"(cycles));
  return cycles;
}

// ------------------------------------------------------------
// Hello world
// ------------------------------------------------------------
void do_hello_world(void) {
  puts("Hello, World!!!\n");
}

// ------------------------------------------------------------
// Existing CFU test
// ------------------------------------------------------------
void do_exercise_cfu_op0(void) {
  puts("\r\nExercise CFU Op0 aka ADD\r\n");

  unsigned int a = 0;
  unsigned int b = 0;
  unsigned int cfu = 0;
  unsigned int count = 0;
  unsigned int pass_count = 0;
  unsigned int fail_count = 0;

  for (a = 0x00004567; a < 0xF8000000; a += 0x00212345) {
    for (b = 0x0000ba98; b < 0xFF000000; b += 0x00770077) {

      cfu = cfu_op0(0, a, b);

      if (cfu != a + b) {
        printf("[%4d] a:%08x b:%08x a+b=%08x cfu=%08x FAIL\r\n",
               count, a, b, a + b, cfu);
        fail_count++;
      } else {
        pass_count++;
      }

      count++;
    }
  }

  printf("\r\nPerformed %d comparisons, %d pass, %d fail\r\n",
         count, pass_count, fail_count);
}

// ------------------------------------------------------------
// 4-Tap FIR Benchmark + cycle counter
// ------------------------------------------------------------
void do_fir_benchmark(void) {
  puts("\r\nRunning 4-Tap FIR Benchmark\r\n");

  const int TAPS = 4;
  const int NUM_SAMPLES = 64;

  int coeffs[TAPS] = {1, 1, 1, 1};
  int delay_line[TAPS] = {0};
  int samples[NUM_SAMPLES];

  for (int i = 0; i < NUM_SAMPLES; i++) {
    samples[i] = i;
  }

  volatile int result = 0;

  uint32_t start = get_cycles();

  for (int iter = 0; iter < 1000; iter++) {

    for (int n = 0; n < NUM_SAMPLES; n++) {

      for (int i = TAPS - 1; i > 0; i--) {
        delay_line[i] = delay_line[i - 1];
      }

      delay_line[0] = samples[n];

      int acc = 0;

      for (int i = 0; i < TAPS; i++) {
        acc += delay_line[i] * coeffs[i];
      }

      result += (acc >> 2);
    }
  }

  uint32_t end = get_cycles();

  printf("\r\nFIR benchmark complete\r\n");
  printf("Cycles: %lu\r\n", (unsigned long)(end - start));
  printf("Result (ignore): %d\r\n", result);
}

// ------------------------------------------------------------
// Menu definition
// ------------------------------------------------------------
struct Menu MENU = {
    "Project Menu",
    "project",
    {
        MENU_ITEM('0', "exercise cfu op0", do_exercise_cfu_op0),
        MENU_ITEM('f', "run 4-tap FIR benchmark", do_fir_benchmark),
        MENU_ITEM('h', "say Hello", do_hello_world),
        MENU_END,
    },
};

};  // namespace

extern "C" void do_proj_menu() {
  menu_run(&MENU);
}
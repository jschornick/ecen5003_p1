#include <stdio.h>
#include <stdint.h>

extern int DEBUG;

uint32_t sqrt_trunc(uint32_t num) {
  uint32_t low = 0;
  uint32_t high = 1<<15+1;  // maximum sqrt of a 32-bit int
  uint32_t last_guess;
  uint32_t guess = 0;
  uint32_t squared;

  // optimize upper limit
  high = (num <= high) ? num : high;

  // edge case, avoids sqrt_trun(1) == 0
  if(num<=1) { return num;};

  do {
    last_guess = guess;
    guess = (low+high) >> 1;
    squared = guess*guess;

    if (squared != num) {
      if (squared < num) {
        low = guess;
      } else {
        high = guess;
      }
    }
    DEBUG && printf(" sqrt(%d) =?%6u, sq=%11u, err=%+12ld, l= %u, h=%6u\n",
                    num, guess, squared, (long)squared - num, low, high);
  } while (last_guess != guess);
  return guess;
}



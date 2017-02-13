#include <stdio.h>
#include <stdint.h>

#include "sqrt_trunc.c"
#include "tests.c"

int DEBUG=0;

int main(void) {
  unsigned int x;

  test_trunc(&sqrt_trunc);
  test_trunc_rand(&sqrt_trunc);

  uint32_t total = 0;
  for(int i = 0; i<10000; i++){
      total += sqrt_trunc(i);
  }
  printf("Total is %d (0x%x)\n", total, total);

  DEBUG=1;
  while(1) {
    scanf("%d", &x);
    printf("sqrt_trunc(%d) = %d\n\n", x, sqrt_trunc((uint32_t) x) );
  }

}

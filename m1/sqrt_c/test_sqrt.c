#include <stdio.h>
#include <stdint.h>

#include "sqrt_trunc.c"
#include "tests.c"

int DEBUG=0;

int main(void) {
  unsigned int x;

  test_trunc(&sqrt_trunc);
  test_trunc_rand(&sqrt_trunc);

  DEBUG=1;
  while(1) {
    scanf("%d", &x);
    printf("sqrt_trunc(%d) = %d\n\n", x, sqrt_trunc((uint32_t) x) );
  }

}

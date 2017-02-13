#include <stdlib.h>
#include <time.h>
#include <math.h>

extern int DEBUG;

// test function returns truncated approximation of sqrt
int test_trunc( uint32_t (*func)(uint32_t) ) {
  uint32_t test_cases[][2] = {
    0, 0,
    1, 1,
    2, 1,
    3, 1,
    4, 2,
    5, 2,
    6, 2,
    7, 2,
    8, 2,
    9, 3,
    10, 3,
    11, 3,
    4294836223, 65534,
    4294836224, 65534,
    4294836225, 65535,
    4294836226, 65535,
    4294967294, 65535,
    4294967295, 65535,
    -1,-1 // terminator
  };

  uint32_t input, output, func_out;
  int failed = 0;

  printf("TEST: truncation approximamtion\n");
  printf("-------------------------------\n");
  int i=0;
  for( ;; i++ ){
    input = test_cases[i][0];
    output = test_cases[i][1];
    if(output == -1) { break; };

    output = test_cases[i][1];
    func_out = func(input);
    if( func_out != output ) {
      printf("FAILED: sqrt(%u) : expected %u, got %u\n", input, output, func_out);
      failed++;
      DEBUG=1;
      func(input);
      DEBUG=0;
      printf("\n");
    } else {
      //printf("Passed: sqrt(%u) = %d\n", input, func_out);
    }
  }
  if(failed) {
    printf("Failed %d/%d tests\n", failed, i-1);
  } else {
    printf("Passed all %d tests\n", i-1);
  }
  printf("\n");
  return failed;
}


int test_trunc_rand( uint32_t (*func)(uint32_t) ) {
  uint32_t input, func_out;
  int failed = 0;
  int count = 1000000;
  uint32_t expected;

  printf("TEST: truncation approximamtion, random input\n");
  printf("---------------------------------------------\n");
  srand(time(NULL));
  for(int i=0; i<count; i++ ){
    input = rand() + rand(); // RAND_MAX is 2^31
    func_out = func(input);
    expected = (uint32_t) sqrt(input);

    if( expected != func_out ) {
      printf("FAILED: sqrt(%u), got %u, but expected %u\n", input, func_out, expected);
      DEBUG=1;
      func(input);
      DEBUG=0;
      failed++;
    }
  }
  if(failed) {
    printf("Failed %d/%d\n", failed, count);
  } else {
    printf("Passed %d random tests\n", count);
  }
  printf("\n");
  return failed;
}


// test function returns *best* integer approximation
int test_best( uint32_t (*func)(uint32_t) ) {
  uint32_t test_cases[][2] = {
    0, 0,
    1, 1,
    2, 1,
    3, 2,
    4, 2,
    5, 2,
    6, 2,
    7, 3,
    8, 3,
    9, 3,
    10, 3,
    4294967294, 65536,
    4294967295, 65536,
    -1,-1 // terminator
  };

  uint32_t input, output, func_out;
  int failed = 0;

  for(int i = 0; ; i++ ){
    input = test_cases[i][0];
    output = test_cases[i][1];
    if(output == -1) { break; };

    output = test_cases[i][1];
    func_out = func(input);
    if( func_out != output ) {
      printf("FAILED: sqrt(%u) : expected %u, got %u\n", input, output, func_out);
      failed++;
      DEBUG=1;
      func(input);
      DEBUG=0;
      printf("\n");
    } else {
      printf("Passed: sqrt(%u) = %d\n", input, func_out);
    }
  }
  printf("Test done\n");
  return failed;
}

int test_best_random( uint32_t (*func)(uint32_t) ) {
  uint32_t input, output, func_out;
  int failed = 0;
  int64_t sq_err, pre_sq_err, post_sq_err;
  int count = 1000000;

  srand(time(NULL));

  for(int i=0; i<count; i++ ){
    input = rand() + rand(); // RAND_MAX is 2^31
    func_out = func(input);
    sq_err = abs(func_out*func_out - input);
    pre_sq_err = abs((func_out-1)*(func_out-1) - input);
    post_sq_err = abs((func_out+1)*(func_out+1) - input);
    if( sq_err > pre_sq_err ) {
      printf("FAILED: sqrt(%u) != %u, %u is better\n", input, func_out, func_out-1);
      DEBUG=1;
      func(input);
      DEBUG=0;
      failed++;
    } else if( sq_err > post_sq_err ) {
      printf("FAILED: sqrt(%u) != %u, %u is better\n", input, func_out, func_out+1);
      DEBUG=1;
      func(input);
      DEBUG=0;
      failed++;
    } else {
      //printf("Passed: sqrt(%u) = %d\n", input, func_out);
    }
  }
  if(failed) {
    printf("Failed %d/%d\n", failed, count);
  } else {
    printf("Passed %d random tests\n", count);
  }
  return failed;
}


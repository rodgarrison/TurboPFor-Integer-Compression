#include <stdio.h> 
#include <sys/types.h>
#include <chrono>

#include <vector>
#include <random>                                                                
#include <algorithm>

#include "bitpack.h"

int main(int argc, char **argv) {
  const uint32_t maxLen = 16;
  const uint32_t maxRun = 16;

  std::mt19937_64 rng;                                                          

  std::uniform_int_distribution<uint32_t> distribution1(0, 1);  
  std::uniform_int_distribution<uint32_t> distribution2(0, 0xffffffffffffffff);  

  auto dice1 = bind(distribution1, rng);                                          
  auto dice2 = bind(distribution2, rng);                                          

  uint32_t sz;
  double nsPerByte;
  unsigned char out[maxLen*sizeof(uint32_t)];

  for (;;) {
    std::vector<uint32_t> ptrs;
    for (uint32_t i=0; i<maxLen; ++i) {
      uint32_t val(0);
      if (dice1()) {
        val = dice2();
      }
      ptrs.push_back(val);
      printf("%u ", val);
    }
    printf("\n");
 
    auto start = std::chrono::high_resolution_clock::now();
    for (uint32_t i=0; i<maxRun; ++i) {
      sz = bitnpack128v32(ptrs.data(), ptrs.size()*sizeof(uint32_t), out);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto time_in_nanoseconds =  std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
    nsPerByte = static_cast<double>(time_in_nanoseconds) / static_cast<double>(maxRun * sizeof(out));

    printf("%lu bytes in %lu ns over %u runs or %lf ns/byte outsize %u\n", sizeof(out), time_in_nanoseconds, maxRun, nsPerByte, sz);
  }

  return 0;
}

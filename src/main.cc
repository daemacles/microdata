/////////////////////////////////////////////////////////////////////////////
//                                   main.cc
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 Jim Youngquist under the MIT license, see LICENSE
//

#include <cassert>
#include <cstring>

#include <functional>
#include <iostream>
#include <random>

#include "microdata.h"

template <typename T>
void RunPackTest (uint32_t roll) {
  uint8_t buffer[sizeof(T)];
  uint8_t *start = buffer;
  uint8_t *end;

  T in = *(T*)&roll;
  T out{10};
  end = Pack (start, in);
  assert (end - start == sizeof(T));
  end = Unpack (start, &out);
  assert (in == out);
}

int main(int argc, char **argv) {
  // DELETE THESE.  Used to suppress unused variable warnings.
  (void)argc;
  (void)argv;

  // Test packing by testing over 10000 random numbers
  std::default_random_engine generator;
  std::uniform_int_distribution<uint32_t> dist(1, (1 << 31)-1);
  auto die = std::bind(dist, generator);

  for (int i = 0; i < 10000; ++i) {
    uint32_t roll = die();
    RunPackTest<uint8_t>(roll);
    RunPackTest<uint16_t>(roll);
    RunPackTest<uint32_t>(roll);
    int sign = ((roll % 2)*2-1);
    RunPackTest<int8_t>(sign*roll);
    RunPackTest<int16_t>(sign*roll);
    RunPackTest<int32_t>(sign*roll);
  }

  // Test SLIP by encode --> decode --> compare
  uint8_t in_buffer[250];
  for (uint32_t i = 0; i < sizeof(in_buffer); ++i) {
    in_buffer[i] = i;
  }

  uint8_t out_buffer[512];
  uint8_t len = SlipEncode (in_buffer, out_buffer, sizeof(in_buffer));
  assert(len > 0);
  memset (in_buffer, 0, sizeof(in_buffer));
  len = SlipDecode (out_buffer, in_buffer, sizeof(in_buffer));
  assert (len == sizeof(in_buffer));
  for (uint32_t i = 0; i < sizeof(in_buffer); ++i) {
    assert(in_buffer[i] == i);
  }

  // End to end Test
  typedef struct {
    uint8_t val1;
    uint8_t val2;
    int16_t val3;
    uint32_t val4;
  } MyData;

  MyData data = {
    .val1 = 42,
    .val2 = 55,
    .val3 = 24000,
    .val4 = 456234234
  };

  uint8_t *cursor = in_buffer;
  cursor = PackUint8 (cursor, data.val1);
  cursor = PackUint8 (cursor, data.val2);
  cursor = PackInt16 (cursor, data.val3);
  cursor = PackUint32 (cursor, data.val4);

  len = SlipEncode(in_buffer, out_buffer, sizeof(MyData));
  memset (in_buffer, 0, sizeof(in_buffer));
  len = SlipDecode(out_buffer, in_buffer, sizeof(in_buffer));
  assert(len > 0);
  cursor = in_buffer;
  MyData out_data;
  cursor = UnpackUint8 (cursor, &out_data.val1);
  cursor = UnpackUint8 (cursor, &out_data.val2);
  cursor = UnpackInt16 (cursor, &out_data.val3);
  cursor = UnpackUint32 (cursor, &out_data.val4);

  assert(data.val1 == out_data.val1);
  assert(data.val2 == out_data.val2);
  assert(data.val3 == out_data.val3);
  assert(data.val4 == out_data.val4);

  std::cout << "All tests pass" << std::endl;

  return 0;
}

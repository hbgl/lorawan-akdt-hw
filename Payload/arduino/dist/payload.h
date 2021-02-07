#ifndef _AKDT_PAYLOAD_H_
#define _AKDT_PAYLOAD_H_

#include <cstdint>

struct Reading {
  float temperatureGround;
  float moistureGround;
  float light;
  float temperatureAir;
  float moistureAir;
  float pressure;
  float battery;
};

struct Payload {
  uint8_t data[50] = {0};
  static constexpr uint8_t version = 1;
  void fill(const Reading* readings, size_t length);
};

#endif

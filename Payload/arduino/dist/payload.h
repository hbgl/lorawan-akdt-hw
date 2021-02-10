#ifndef _AKDT_PAYLOAD_H_
#define _AKDT_PAYLOAD_H_

#include <cstddef>
#include <cstdint>

struct Measurement {
  float timeOffset;
  float temperatureGround;
  float moistureGround;
  float light;
  float temperatureAir;
  float moistureAir;
  float pressure;
  float battery;
  float batteryVoltage;
};

struct Payload {
  uint8_t data[44] = {0};
  static constexpr uint8_t version = 1;
  void fill(const Measurement* measurements, size_t length);
  size_t get_byte_count();
};

#endif

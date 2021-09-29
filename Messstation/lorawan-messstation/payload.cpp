#include <math.h>

#include <algorithm>
#include <cmath>

#include "bit_writer.h"
#include "payload.h"

void Payload::fill(const Measurement* measurements, size_t length) {
  length = std::min(static_cast<size_t>(4), length);
  BitWriter bitWriter(data, 0);

  // Write version.
  bitWriter.write(version, 8);

  // Write length.
  bitWriter.write(length, 8);

  for (size_t i = 0; i < length; i++) {
    const auto& measurement = measurements[i];

    // 12-bit Measurement time offset -4095min .. 0min in 1min increments.
    uint32_t timeOffsetRaw = std::round(std::max(-4095.0f, std::min(measurement.timeOffset, 0.0f)) + 4095.0f);
    bitWriter.write(timeOffsetRaw, 12);

    // 8-bit Ground temperature -20°C .. 85°C in 0.5°C increments.
    uint32_t temperatureGroundRaw = std::round((std::max(-20.0f, std::min(measurement.temperatureGround, 85.0f)) + 20.0f) / 0.5f);
    bitWriter.write(temperatureGroundRaw, 8);

    // 6-bit Ground water content 0% .. 50% in 1% increments.
    uint32_t moistureGroundRaw = std::round(std::max(0.0f, std::min(measurement.moistureGround, 50.0f)));
    bitWriter.write(moistureGroundRaw, 6);

    // 17-bit Illuminance 0lx .. 120000lx in 1lx increments.
    uint32_t lightRaw = std::round(std::max(0.0f, std::min(measurement.light, 120000.0f)));
    bitWriter.write(lightRaw, 17);

    // 8-bit Air temperature -40°C .. 80°C in 0.5°C increments.
    uint32_t temperatureAirRaw = std::round((std::max(-40.0f, std::min(measurement.temperatureAir, 80.0f)) + 40.0f) / 0.5f);
    bitWriter.write(temperatureAirRaw, 8);

    // 7-bit Relative air humidity 0% .. 100% in 1% increments.
    uint32_t moistureAirRaw = std::round(std::max(0.0f, std::min(measurement.moistureAir, 100.0f)));
    bitWriter.write(moistureAirRaw, 7);

    // 10-bit Barometric pressure 300hPa .. 1100hPa in 1hPa increments.
    uint32_t pressureRaw = std::round(std::max(300.0f, std::min(measurement.pressure, 1100.0f)) - 300.0f);
    bitWriter.write(pressureRaw, 10);

    // 7-bit Battery charge level 0% .. 100% in 1% increments.
    uint32_t batteryRaw = std::round(std::max(0.0f, std::min(measurement.battery, 100.0f)));
    bitWriter.write(batteryRaw, 7);

    // 8-bit Battery voltage 2.8% .. 4.2% in 0.01% increments.
    uint32_t batteryVoltageRaw = std::round((std::max(2.8f, std::min(measurement.batteryVoltage, 4.2f)) - 2.8f) / 0.01f);
    bitWriter.write(batteryVoltageRaw, 8);
  }
}
size_t Payload::get_byte_count() {
  size_t measurement_count = data[1];
  size_t prelude_bits = 16;
  size_t measurement_bits = 83;
  size_t byte_count = (prelude_bits + (measurement_bits * measurement_count) + 7) >> 3;
  return byte_count;
}
#include <math.h>

#include <algorithm>
#include <cmath>

#include "bit_writer.h"
#include "payload.h"

void Payload::fill(const Reading* readings, size_t length) {
  length = std::min(static_cast<size_t>(6), length);
  BitWriter bitWriter(data, 0);

  // Write version.
  bitWriter.write(version, 8);

  // Write length.
  bitWriter.write(length, 8);

  for (size_t i = 0; i < length; i++) {
    const auto& reading = readings[i];

    // 8-bit Ground temperature -20°C .. 85°C in 0.5°C increments.
    uint32_t temperatureGroundRaw = std::round((std::max(-20.0f, std::min(reading.temperatureGround, 85.0f)) + 20.0f) / 0.5f);
    bitWriter.write(temperatureGroundRaw, 8);

    // 6-bit Ground water content 0% .. 50% in 1% increments.
    uint32_t moistureGroundRaw = std::round(std::max(0.0f, std::min(reading.moistureGround, 50.0f)));
    bitWriter.write(moistureGroundRaw, 6);

    // 17-bit Illuminance 0lx .. 120000lx in 1lx increments.
    uint32_t lightRaw = std::round(std::max(0.0f, std::min(reading.light, 120000.0f)));
    bitWriter.write(lightRaw, 17);

    // 8-bit Air temperature -40°C .. 80°C in 0.5°C increments.
    uint32_t temperatureAirRaw = std::round((std::max(-40.0f, std::min(reading.temperatureAir, 80.0f)) + 40.0f) / 0.5f);
    bitWriter.write(temperatureAirRaw, 8);

    // 7-bit Relative air humidity 0% .. 100% in 1% increments.
    uint32_t moistureAirRaw = std::round(std::max(0.0f, std::min(reading.moistureAir, 100.0f)));
    bitWriter.write(moistureAirRaw, 7);

    // 10-bit Barometric pressure 300hPa .. 1100hPa in 1hPa increments.
    uint32_t pressureRaw = std::round(std::max(300.0f, std::min(reading.pressure, 1100.0f)) - 300.0f);
    bitWriter.write(pressureRaw, 10);

    // 7-bit Battery charge level 0% .. 100% in 1% increments.
    uint32_t batteryRaw = std::round(std::max(0.0f, std::min(reading.battery, 100.0f)));
    bitWriter.write(batteryRaw, 7);
  }
}

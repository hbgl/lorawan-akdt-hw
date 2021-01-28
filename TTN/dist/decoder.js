"use strict";

/**
 * This code was automatically generated from payload field definitions.
 */

function Decoder(bytes, port) {
    // There must be at least one byte given for the version.
    if (bytes.length === 0) {
        throw new Error("Empty message received.");
    }

    var version = bytes[0];
    var decode = decoders["v".concat(version)];
    if (!decode) {
        throw new Error("Unknown message version: ".concat(version));
    }

    // Call decode function for version
    var decoded = { version: version };
    decode(bytes, port, decoded);
    return decoded;
}

var decoders = {};

decoders.v1 = function (bytes, port, decoded) {
    // Check that enough bytes were received.
    var requiredBytes = 9;
    if (bytes.length < requiredBytes) {
        throw new Error("Insufficient number of bytes received. Required ".concat(requiredBytes));
    }

    // 8-bit Ground temperature -20°C .. 85°C in 0.5°C increments.
    var temperatureGroundRaw = bytes[0];
    decoded.temperatureGround = (temperatureGroundRaw * 0.5 - 20).toFixed(1);

    // 6-bit Ground water content 0% .. 50% in 1% increments.
    var moistureGroundRaw = bytes[1] & ~(~0 << 6);
    decoded.moistureGround = moistureGroundRaw.toFixed(0);

    // 17-bit Illuminance 0lx .. 120000lx in 1lx increments.
    var lightRaw = ((bytes[3] & ~(~0 << 7)) << 10) | (bytes[2] << 2) | ((bytes[1] >>> 6) & ~(~0 << 2));
    decoded.light = lightRaw.toFixed(0);

    // 8-bit Air temperature -40°C .. 80°C in 0.5°C increments.
    var temperatureAirRaw = ((bytes[4] & ~(~0 << 7)) << 1) | ((bytes[3] >>> 7) & ~(~0 << 1));
    decoded.temperatureAir = (temperatureAirRaw * 0.5 - 40).toFixed(1);

    // 7-bit Relative air humidity 0% .. 100% in 1% increments.
    var moistureAirRaw = ((bytes[5] & ~(~0 << 6)) << 1) | ((bytes[4] >>> 7) & ~(~0 << 1));
    decoded.moistureAir = moistureAirRaw.toFixed(0);

    // 10-bit Barometric pressure 300hPa .. 1100hPa in 1hPa increments.
    var pressureRaw = (bytes[6] << 2) | ((bytes[5] >>> 6) & ~(~0 << 2));
    decoded.pressure = (pressureRaw + 300).toFixed(0);

    // 7-bit Battery charge level 0% .. 100% in 1% increments.
    var batteryRaw = bytes[7] & ~(~0 << 7);
    decoded.battery = batteryRaw.toFixed(0);

    return decoded;
};

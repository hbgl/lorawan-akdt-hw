const { describe, it } = require('mocha');
const assert = require('chai').assert;
const { BitReader } = require('../common/bit-reader');
const decoder = require('./decoder');
const constants = require('../common/constants');

describe('Integration', () => {
    describe('version 1', () => {
        it('normal case single measurement', () => {
            const bytes = parseBinStrValues([
                '0000 0001', // Version
                '0000 0001', // Length

                '1111 1111 1111', // Time offset
                '0011 0010', // Ground temperature
                '00 1100', // Ground moisture
                '0 0000 0100 1011 0000', // Illuminance
                '0101 1110', // Air temperature
                '100 1100', // Air humidity
                '10 1100 1110', // Air pressure
                '100 0101', // Battery charge level
                '0110 1110', // Battery voltage
            ]);
            const result = decoder(bytes, 1);
            const measurements = result[constants.MEASUREMENTS_PROPERTY];
            assert.equal(result[constants.VERSION_PROPERTY], 1);
            assert.equal(measurements.length, 1);

            const measurement = measurements[0];
            assert.equal(measurement.timeOffset, '0');
            assert.equal(measurement.temperatureGround, '5.0');
            assert.equal(measurement.moistureGround, '12');
            assert.equal(measurement.light, '1200');
            assert.equal(measurement.temperatureAir, '7.0');
            assert.equal(measurement.moistureAir, '76');
            assert.equal(measurement.pressure, '1018');
            assert.equal(measurement.battery, '69');
            assert.equal(measurement.batteryVoltage, '3.90');
        });
        it('normal case many measurements', () => {
            const bytes = parseBinStrValues([
                '0000 0001', // Version
                '0000 0100', // Length

                '1111 1111 1111', // Time offset
                '0011 0010', // Ground temperature
                '00 1100', // Ground moisture
                '0 0000 0100 1011 0000', // Illuminance
                '0101 1110', // Air temperature
                '100 1100', // Air humidity
                '10 1100 1110', // Air pressure
                '100 0101', // Battery charge level
                '0110 1110', // Battery voltage

                '1111 1110 0001', // Time offset
                '0011 0010', // Ground temperature
                '00 1100', // Ground moisture
                '0 0000 0100 1011 0000', // Illuminance
                '0101 1110', // Air temperature
                '100 1100', // Air humidity
                '10 1100 1110', // Air pressure
                '100 0101', // Battery charge level
                '0110 1110', // Battery voltage

                '1111 1100 0011', // Time offset
                '0011 0010', // Ground temperature
                '00 1100', // Ground moisture
                '0 0000 0100 1011 0000', // Illuminance
                '0101 1110', // Air temperature
                '100 1100', // Air humidity
                '10 1100 1110', // Air pressure
                '100 0101', // Battery charge level
                '0110 1110', // Battery voltage

                '1111 1010 0101', // Time offset
                '0011 0010', // Ground temperature
                '00 1100', // Ground moisture
                '0 0000 0100 1011 0000', // Illuminance
                '0101 1110', // Air temperature
                '100 1100', // Air humidity
                '10 1100 1110', // Air pressure
                '100 0101', // Battery charge level
                '0110 1110', // Battery voltage
            ]);
            const result = decoder(bytes, 1);
            const measurements = result[constants.MEASUREMENTS_PROPERTY];
            assert.equal(result[constants.VERSION_PROPERTY], 1);
            assert.equal(measurements.length, 4);

            let offset = 0;
            for (let i = 0; i < measurements.length; i++) {
                const measurement = measurements[i];
                assert.equal(measurement.timeOffset, offset.toString());
                assert.equal(measurement.temperatureGround, '5.0');
                assert.equal(measurement.moistureGround, '12');
                assert.equal(measurement.light, '1200');
                assert.equal(measurement.temperatureAir, '7.0');
                assert.equal(measurement.moistureAir, '76');
                assert.equal(measurement.pressure, '1018');
                assert.equal(measurement.battery, '69');
                offset -= 30;
            }
        });
    });
});

describe('BitReader', () => {
    it('normal case', () => {
        const bytes = [
            0b00001111, 0b00001111, 0b00001111, 0b00001111,
            0b00001111, 0b00001111, 0b00001111, 0b00001111,
            0b00001111, 0b00001111, 0b00001111, 0b00001111,
            0b00001111, 0b00001111, 0b00001111, 0b00001111,
        ];

        const reader = new BitReader(bytes, 0);

        assert.equal(reader.read(3), 0b111);
        assert.equal(reader.read(2), 0b01);
        assert.equal(reader.read(1), 0b0);
        assert.equal(reader.read(5), 0b11100);
        assert.equal(reader.read(32), 0b11100001111000011110000111100001);
        assert.equal(reader.read(5), 0b00001);
        assert.equal(reader.read(8), 0b00001111);
        assert.equal(reader.read(16), 0b0000111100001111);
        assert.equal(reader.read(31), 0b0001111000011110000111100001111);

    });
});

function parseBinStrValues(values) {
    const bytes = [];
    const bitStr = values.reverse().join('').replace(/ /g, '');
    const bitCount = bitStr.length;
    for (let i = bitCount; i > 0; i -= 8) {
        const str = bitStr.substring(i - 8, i);
        const i32 = parseInt(str, 2);
        bytes.push(i32);
    }
    return bytes;
}
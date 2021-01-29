
const { describe, it } = require('mocha');
const assert = require('chai').assert;
const requireFromString = require('require-from-string');
const versions = require('./versions');
const generateDecoderCode = require('./generator');

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

describe('integration', () => {
    describe('version 1', () => {
        const version1 = versions.find(v => v.number === 1);
        const code = generateDecoderCode([version1]) + `
        module.exports = Decoder;
        `;
        const decoder = requireFromString(code);
        it('normal case', () => {
            const bytes = parseBinStrValues([
                '0000 0001', // Version
                '0011 0010', // Ground temperature
                '00 1100', // Ground moisture
                '0 0000 0100 1011 0000', // Illuminance
                '0101 1110', // Air temperature
                '100 1100', // Air humidity
                '10 1100 1110', // Air pressure
                '100 0101', // Battery charge level
            ]);
            const result = decoder(bytes, 1);
            assert.equal(result.version, 1);
            assert.equal(result.temperatureGround, '5.0');
            assert.equal(result.moistureGround, '12');
            assert.equal(result.light, '1200');
            assert.equal(result.temperatureAir, '7.0');
            assert.equal(result.moistureAir, '76');
            assert.equal(result.pressure, '1018');
            assert.equal(result.battery, '69');
        });
    });
});
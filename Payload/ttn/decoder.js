const { BitReader } = require('../common/bit-reader');
const versions = require('../versions');
const constants = require('../common/constants');
const { fieldBits, fractionDigits } = require('../common/utilities');

this.Decoder = function(bytes, _port) {
    // There must be at least one byte given for the version.
    if (bytes.length === 0) {
        throw new Error('Empty message received.');
    }
    const bitReader = new BitReader(bytes, 0);

    var version = bitReader.read(constants.VERSION_BITS);
    var messageFormat = versions[version];
    if (!messageFormat) {
        throw new Error('Unknown message version: '.concat(version));
    }

    // Calculate bits used by a field.
    const fields = messageFormat.fields;
    for (let field of fields) {
        field.bits = fieldBits(field);
        field.fractionDigits = fractionDigits(field.resolution);
    }

    const preludeBits = constants.VERSION_BITS + constants.MEASUREMENT_COUNT_BITS;
    const measurementBits = fields.reduce((sum, f) => sum + f.bits, 0);
    const minBytes = Math.ceil((preludeBits + measurementBits) / 8);

    // Check that enough bytes were received.
    if (bytes.length < minBytes) {
        throw new Error(`Not enough bytes received. Minimum: ${messageFormat.minBytes} bytes, received: ${bytes.length} bytes.`);
    }

    const measurementCount = bitReader.read(constants.MEASUREMENT_COUNT_BITS);
    const expectedBytes = Math.ceil((preludeBits + (measurementBits * measurementCount) )/ 8);

    // Check that the message is exactly the right size.
    if (bytes.length !== expectedBytes) {
        throw new Error(`Expected message size for ${measurementCount} measurements to be exactly ${expectedBytes} bytes. Received: ${bytes.length}.`);
    }

    const decoded = {
        version: version,
        measurements: [],
    };

    for (let i = 0; i < measurementCount; i++) {
        const measurement = {};
        for (let field of fields) {
            const rawValue = bitReader.read(field.bits);
            const value = rawValue * field.resolution + field.range.min;
            measurement[field.property] = value.toFixed(field.fractionDigits);
        }
        decoded.measurements.push(measurement);
    }

    // Call decode function for version
    return decoded;
};

module.exports = this.Decoder;
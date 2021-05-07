const { BitReader } = require('../common/bit-reader');
const versions = require('../versions');
const constants = require('../common/constants');
const { messageSizeInfo } = require('../common/utilities');

function Decoder(bytes, _port) {
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

    const fields = messageFormat.fields;
    const sizeInfo = messageSizeInfo(messageFormat);

    // Check that enough bytes were received.
    if (bytes.length < sizeInfo.messageBytesMin) {
        throw new Error(`Not enough bytes received. Minimum: ${sizeInfo.messageBytesMin} bytes, received: ${bytes.length} bytes.`);
    }

    const measurementCount = bitReader.read(constants.MEASUREMENT_COUNT_BITS);
    if (measurementCount <= 0) {
        throw new Error('Expected measurement count to be greater than 0.');
    }

    const expectedBytes = Math.ceil((sizeInfo.preludeBits + (sizeInfo.measurementBits * measurementCount)) / 8);

    // Check that the message is exactly the right size.
    if (bytes.length !== expectedBytes) {
        throw new Error(`Expected message size for ${measurementCount} measurements to be exactly ${expectedBytes} bytes. Received: ${bytes.length}.`);
    }

    const decoded = {};
    decoded[constants.VERSION_PROPERTY] = version;
    decoded[constants.MEASUREMENTS_PROPERTY] = [];

    for (let i = 0; i < measurementCount; i++) {
        const measurement = {};
        for (let field of fields) {
            const rawValue = bitReader.read(field.bits);
            const value = rawValue * field.resolution + field.range.min;
            measurement[field.property] = value.toFixed(field.fractionDigits);
        }
        decoded[constants.MEASUREMENTS_PROPERTY].push(measurement);
    }

    // Call decode function for version
    return decoded;
}

function decodeUplink(input) {
    return {
        data: Decoder(input.bytes, input.port),
        warnings: [],
        errors: [],
    };
}

if (typeof ttnglobal !== 'undefined') {
    ttnglobal.Decoder = Decoder;
    ttnglobal.decodeUplink = decodeUplink;
}
module.exports = Decoder;
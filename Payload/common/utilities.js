const constants = require('./constants');

function fieldBits(field) {
    // Calculate bits used by field.
    const distinctValueCount = ((field.range.max - field.range.min) / field.resolution) + 1;
    const bits = Math.ceil(Math.log2(distinctValueCount));
    return bits;
}

function fractionDigits(number) {
    const resolutionStr = number.toString();
    const dotIndex = resolutionStr.lastIndexOf('.');
    const fractionDigits = dotIndex === -1 ? 0 : (resolutionStr.length - dotIndex - 1);
    return fractionDigits;
}

function unparen(s) {
    if (s.length >= 2 && s[0] === '(' && s[s.length - 1] === ')') {
        return s.substr(1, s.length - 2);
    }
    return s;
}

function prepareFields(fields) {
    for (let field of fields) {
        field.bits = fieldBits(field);
        field.fractionDigits = fractionDigits(field.resolution);
    }
    return fields;
}

function messageSizeInfo(version) {
    const preludeBits = constants.VERSION_BITS + constants.MEASUREMENT_COUNT_BITS;
    const measurementBits = version.fields.reduce((sum, f) => sum + f.bits, 0);
    const payloadBytesMax = constants.PAYLOAD_MAX_SIZE_BYTES;
    const payloadBitsMax = payloadBytesMax * 8;
    const bodyBitsMax = payloadBitsMax - preludeBits;
    const measurementsPerMessage = measurementBits === 0 ? 0 : Math.max(0, Math.floor(bodyBitsMax / measurementBits));
    const messageBitsMax = preludeBits + (measurementsPerMessage * measurementBits);
    const messageBytesMax = Math.ceil(messageBitsMax / 8);
    const messageBitsMin = preludeBits + measurementBits;
    const messageBytesMin = Math.ceil(messageBitsMin / 8);
    const properties = {
        payloadBitsMax,
        payloadBytesMax,
        bodyBitsMax,
        preludeBits,
        messageBitsMin,
        messageBytesMin,
        messageBitsMax,
        messageBytesMax,
        measurementBits,
        measurementsPerMessage,
    };
    return properties;
}

function formatFieldRange(field) {
    return `${field.range.min}${field.unit} .. ${field.range.max}${field.unit}`;
}

function formatFieldAccuracy(field) {
    const accuracy = field.accuracy;
    if (accuracy === null || accuracy === undefined) {
        return '';
    } else if (typeof accuracy === 'number') {
        return `+- ${accuracy} ${field.unit}`;
    } else if (typeof accuracy === 'object') {
        if (typeof accuracy.value === 'number' && accuracy.type === '%') {
            return `+- ${accuracy.value}%`;
        }
    }
    return '';
}

module.exports = {
    fieldBits,
    formatFieldRange,
    formatFieldAccuracy,
    fractionDigits,
    unparen,
    messageSizeInfo,
    prepareFields,
};
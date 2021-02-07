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

module.exports = {
    fieldBits,
    fractionDigits,
    unparen,
};
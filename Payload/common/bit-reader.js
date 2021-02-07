const MAX_READ_SIZE = 32;

/**
 * Read bits from an array of bytes.
 * @class
 * @param {number[]} bytes The buffer that is read from.
 * @param {number} bitOffset The bit offet after which the reading starts.
 */
function BitReader(bytes, bitOffset) {
    this.bytes = bytes;
    this.bitIndex = bitOffset;
}

/**
 * Read the next bits.
 * The internal index will get incremented with every read.
 * @param {number} bitLength
 * @return {number} The read bits as an unsigned integer.
 */
BitReader.prototype.read = function (bitLength) {
    if (bitLength > MAX_READ_SIZE) {
        throw new Error(`Cannot read more than ${MAX_READ_SIZE} bits. Requested: ${bitLength} bits.`);
    }
    if (bitLength <= 0) {
        throw new Error(`Must at least read one bit. Requested: ${bitLength} bits.`);
    }
    let bitsLeft = bitLength;
    const bytes = this.bytes;
    let byteIndex = this.bitIndex >>> 3;
    let bitIndex = this.bitIndex & 7;

    let value = 0;

    while (bitsLeft > 0) {
        // The number of bits to read.
        const bitsRead = Math.min(bitsLeft, 8 - bitIndex);

        let chunk = bytes[byteIndex];

        // Mask off previously read low bits.
        chunk = chunk >>> bitIndex;


        // Mask off unused high bits.
        chunk = chunk & ~(~0 << bitsRead);

        // Shift chunk into correct position.
        chunk = chunk << (bitLength - bitsLeft);

        // Or into value.
        value = chunk | value;

        // Increment byte index if all bits of its bits were read.
        byteIndex += (bitIndex + bitsRead) >>> 3;

        // Increase bit index and wrap if necessary.
        bitIndex = (bitIndex + bitsRead) & 7;

        bitsLeft -= bitsRead;
    }

    this.bitIndex = (byteIndex * 8) + bitIndex;

    // Convert signed int to unsigned int.
    value = value >>> 0;

    return value;
};

module.exports = {
    BitReader,
};
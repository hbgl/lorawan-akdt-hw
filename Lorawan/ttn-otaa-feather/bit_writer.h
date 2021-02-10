#ifndef _AKDT_BIT_WRITER_H_
#define _AKDT_BIT_WRITER_H_

#include <cstdint>
#include <cstddef>

/**
 * Writes bits to an array of bytes.
 */
class BitWriter {
    private:
        uint8_t * const _bytes;
        size_t _bitIndex;
    public:
        /**
         * Create a BitWriter that writes to the given buffer starting from the offset.
         * @param bytes The buffer that is written to. Must be initialized.
         * @param bitOffset The bit offset after which bits will be written.
         */
        BitWriter(uint8_t * bytes, size_t bitOffset);

        /**
         * Write the specified number of bits from the value to the byte array.
         * The internal index will get incremented for every bit written.
         * @param value The input bits.
         * @param bitLength The number of bits to write starting at 0.
         */
        void write(uint32_t value, size_t bitLength);
};

#endif

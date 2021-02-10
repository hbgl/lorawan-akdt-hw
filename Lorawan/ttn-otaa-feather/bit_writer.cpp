#include "bit_writer.h"
#include <algorithm>

BitWriter::BitWriter(uint8_t * bytes, size_t bitOffset) : _bytes(bytes), _bitIndex(bitOffset) {}

void BitWriter::write(uint32_t value, size_t bitLength) {
    size_t bitsTotal = bitLength;
    size_t bitsLeft = bitLength;
    uint8_t * const bytes = _bytes;
    size_t byteIndex = _bitIndex >> 3;
    size_t bitIndex = _bitIndex & 7;
    
    while (bitsLeft > 0) {
        // The number of bits to place into the current byte.
        const size_t bitsRead = std::min(bitsLeft, 8 - bitIndex);
        
        // The bits to place into the current byte.
        const uint8_t chunk = (value >> (bitsTotal - bitsLeft)) & ~(~0 << bitsRead);
        
        // Place the chunk into the byte at the correct position.
        bytes[byteIndex] = (chunk << bitIndex) | (bytes[byteIndex] & ~(~0 << bitIndex));
        
        // Increment byte index if all bits of its bits were read.
        byteIndex += (bitIndex + bitsRead) >> 3;

        // Increase bit index and wrap if necessary.
        bitIndex = (bitIndex + bitsRead) & 7;

        bitsLeft -= bitsRead;
    }
    
    _bitIndex = (byteIndex * 8) + bitIndex;
}

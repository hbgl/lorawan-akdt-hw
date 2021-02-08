#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include "./dist/payload.h"
#include <memory>
#include <stdexcept>
#include <stdlib.h>
#include <sstream>
#include <algorithm>

void test(bool condition, std::string message);

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args);

std::string bytes_to_binary_string(const uint8_t * bytes, size_t length);

std::string join_binary_values(const std::vector<const char*>& values);

template<typename T>
void append_vec(std::vector<T>& vec, const std::vector<T>& other);

struct TestCase {
    uint8_t version;
    size_t messageSize;
    std::vector<Reading> readings;
    std::vector<const char *> expectedBinaryValues;
};

void run_test_case(const TestCase& testCase);

int main()
{
    Reading reading1 = { 0.0f, 5.0f, 12.0f, 1200.0f, 7.0f, 76.0f, 1018.0f, 69.0f };
    Reading reading2 = { 30.0f, 24.0f, 2.0f, 50000.0f, 34.0f, 50.0f, 1058.0f, 43.0f };

    auto expectedBinaryValues1 = std::vector<const char*> {
        "0000 0000 0000", // Time offset
        "0011 0010", // Ground temperature
        "00 1100", // Ground moisture
        "0 0000 0100 1011 0000", // Illuminance
        "0101 1110", // Air temperature
        "100 1100", // Air humidity
        "10 1100 1110", // Air pressure
        "100 0101", // Battery charge level
    };

    auto expectedBinaryValues2 = std::vector<const char*> {
        "0000 0001 1110", // Time offset
        "0101 1000", // Ground temperature
        "00 0010", // Ground moisture
        "0 1100 0011 0101 0000", // Illuminance
        "1001 0100", // Air temperature
        "011 0010", // Air humidity
        "10 1111 0110", // Air pressure
        "010 1011", // Battery charge level
    };

    TestCase testCase;
    testCase.version = 1;
    testCase.messageSize = 50;
    testCase.readings = std::vector<Reading> {
        reading1,
        reading2,
        reading2,
        reading1,
    };
    testCase.expectedBinaryValues = {
        "0000 0001", // Version
        "0000 0100", // Length
    };
    append_vec(testCase.expectedBinaryValues, expectedBinaryValues1);
    append_vec(testCase.expectedBinaryValues, expectedBinaryValues2);
    append_vec(testCase.expectedBinaryValues, expectedBinaryValues2);
    append_vec(testCase.expectedBinaryValues, expectedBinaryValues1);

    run_test_case(testCase);

    std::cout << std::endl << "OK\nAll tests passed." << std::endl;
}

void run_test_case(const TestCase& testCase) {
    Payload payload;

    // Test version.
    test(payload.version == testCase.version, string_format("Invalid version.\nExpected: %u\nActual: %u", payload.version, testCase.version));

    // Test correct message size.
    test(sizeof(payload.data) == 49, string_format("Invalid payload data size.\nExpected: 50\nActual: %zu", sizeof(payload.data)));

    // Fill payload with data.
    payload.fill(testCase.readings.data(), testCase.readings.size());

    auto expectedBinary = join_binary_values(testCase.expectedBinaryValues);
    expectedBinary.insert(expectedBinary.end(), sizeof(payload.data) * 8 - expectedBinary.size(), '0');
    auto actualBinary = bytes_to_binary_string(payload.data, sizeof(payload.data));
    test(expectedBinary == actualBinary, string_format("Invalid payload.\nExpected: %s\nActual: %s", expectedBinary.c_str(), actualBinary.c_str()));

    std::cout << "+ Test case passed" << std::endl;
}

void test(bool condition, std::string message) {
    if (!condition) {
        std::cout << message << std::endl;
        exit(1);
    }
}

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
    // Calculate size.
    auto size = snprintf(nullptr, 0, format.c_str(), args...) + 1; // +1 for '\0'
    if (size <= 0) {
        throw std::runtime_error("Error during formatting.");
    }

    // Format into buffer.
    std::unique_ptr<char[]> buf(new char[size]); 
    snprintf(buf.get(), size, format.c_str(), args...);

    return std::string(buf.get(), buf.get() + size - 1); // Remove '\0'
}

std::string bytes_to_binary_string(const uint8_t * bytes, size_t length) {
    std::stringstream buffer;
    
    for (size_t i = 0; i < length; i++) {
        std::bitset<sizeof(uint8_t) * 8> byte(bytes[i]);
        buffer << byte;
    }

    return buffer.str();
}

std::string join_binary_values(const std::vector<const char*>& values) {
    auto str = std::string();
    for (auto it = values.rbegin(); it != values.rend(); ++it)
    {
        str.append(*it);
    }
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());

    auto expectedBinary = std::string();
    expectedBinary.reserve(str.length());
    int remaining = str.length();
    while (remaining > 0) {
        auto pos = std::max(remaining - 8, 0);
        auto len = std::min(remaining, 8);
        expectedBinary.append(8 - len, '0');
        expectedBinary.append(str, pos, len);
        remaining -= len;

    }
    return expectedBinary;
}


template<typename T>
void append_vec(std::vector<T>& vec, const std::vector<T>& other) {
    vec.insert(std::end(vec), std::begin(other), std::end(other));
}
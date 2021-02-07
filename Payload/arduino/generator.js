const clangFormat = require('clang-format');
const { spawnSync } = require('child_process');
const { fieldBits, unparen } = require('../common/utilities');
const fs = require('fs');
const path = require('path');
const _ = require('lodash');

const VERSION_BITS = 8;
const LENGTH_BITS = 8;
const PAYLOAD_MAX_SIZE_BYTES = 51;
const PAYLOAD_MAX_SIZE_BITS = PAYLOAD_MAX_SIZE_BYTES * 8;
const PAYLOAD_STRUCT_NAME = 'Payload';
const READING_DATA_STRUCT_NAME = 'Reading';
const DATA_FIELD_NAME = 'data';
const VERSION_FIELD_NAME = 'version';
const LENGTH_FIELD_NAME = 'length';
const FILL_FUNCTION_NAME = 'fill';
const READING_PARAM_NAME = 'readings';
const READING_PARAM_LENGTH = 'length';
const BIT_WRITER_HEADER = 'bit_writer.h';

function formatCode(code) {
    const binaryPath = clangFormat.getNativeBinary();
    const result = spawnSync(binaryPath, ['-style', '{Language: Cpp, BasedOnStyle: Google, ColumnLimit: 140}'], {
        input: code,
        stdio: 'pipe',
    });
    if (result.error) {
        throw result.error;
    }
    return result.stdout.toString();
}

function floatify(number) {
    let s = number.toString();
    if (s.indexOf('.') === -1) {
        s += '.0';
    }
    s += 'f';
    return s;
}

function generateHeaderCode(version, context) {
    const fields = version.fields;

    const headerCode = `#ifndef _AKDT_PAYLOAD_H_
    #define _AKDT_PAYLOAD_H_

    #include <cstdint>

    struct ${READING_DATA_STRUCT_NAME} {
        ${fields.map(f => `float ${f.property};`).join('\n')}
    };

    struct ${PAYLOAD_STRUCT_NAME} {
        uint8_t ${DATA_FIELD_NAME}[${context.payloadBytesTotal}] = { 0 };
        static constexpr uint8_t ${VERSION_FIELD_NAME} = ${version.number};
        void ${FILL_FUNCTION_NAME}(const ${READING_DATA_STRUCT_NAME} * ${READING_PARAM_NAME}, size_t ${READING_PARAM_LENGTH});
    };

    #endif
    `;

    return headerCode;
}

function generateImplementationCode(version, context) {
    const fields = version.fields;

    let code = `#include <math.h>
    #include <algorithm>
    #include "${context.headerFile}"
    #include "${BIT_WRITER_HEADER}"
    #include <cmath>
    
    void ${PAYLOAD_STRUCT_NAME}::${FILL_FUNCTION_NAME}(const ${READING_DATA_STRUCT_NAME} * ${READING_PARAM_NAME}, size_t ${READING_PARAM_LENGTH}) {
        ${READING_PARAM_LENGTH} = std::min(static_cast<size_t>(${context.readingsPerMessage}), ${READING_PARAM_LENGTH});
    `;

    code += `BitWriter bitWriter(${DATA_FIELD_NAME}, 0);

        // Write version.
        bitWriter.write(${VERSION_FIELD_NAME}, ${VERSION_BITS});

        // Write length.
        bitWriter.write(${LENGTH_FIELD_NAME}, ${LENGTH_BITS});

        for (size_t i = 0; i < ${READING_PARAM_LENGTH}; i++) {
            const auto& reading = ${READING_PARAM_NAME}[i];

    `;

    for (let field of fields) {

        code += `// ${field.bits}-bit ${field.name} ${field.range.min}${field.unit} .. ${field.range.max}${field.unit} in ${field.resolution}${field.unit} increments.
        `;

        // Check if field is a constant.
        if (field.bits === 0) {
            code += `// Contant value ${field.range.min}`;
            continue;
        }

        // Clamp value to min and max.
        let rawExpr = `std::max(${floatify(field.range.min)}, std::min(reading.${field.property}, ${floatify(field.range.max)}))`;

        // Align value.
        let minValue = field.range.min;
        if (minValue !== 0) {
            rawExpr = `(${rawExpr} ${minValue < 0 ? '+' : '-'} ${floatify(Math.abs(minValue))})`;
        }

        // Apply resolution if necessary.
        if (field.resolution !== 1) {
            rawExpr = `(${rawExpr} / ${floatify(field.resolution)})`;
        }

        // Round to nearest integer.
        rawExpr = `std::round(${unparen(rawExpr)})`;

        code += `uint32_t ${field.property}Raw = ${rawExpr};
        bitWriter.write(${field.property}Raw, ${field.bits});

        `;


    }

    code += `}
    `; // Close for loop

    code += `}
    `; // Close function

    return code;
}

function generateArduinoCode(version, options) {
    const maxVersion = ~(~0 << VERSION_BITS);
    if (version.number > maxVersion) {
        throw new Error(`The version number must not be greater than ${maxVersion}. Given: ${version.number}`);
    }

    const context = Object.assign({}, options);

    version = JSON.parse(JSON.stringify(version));
    const fields = version.fields;
    for (let field of fields) {
        field.bits = fieldBits(field);
    }

    // Calculate payload and reading sizes.
    const preludeBits = VERSION_BITS + LENGTH_BITS;
    const payloadBitsMax = PAYLOAD_MAX_SIZE_BITS - preludeBits;
    const readingBitsTotal = fields.reduce((sum, f) => sum + f.bits, 0);
    const readingsPerMessage = readingBitsTotal === 0 ? 0 : Math.max(0, Math.floor(payloadBitsMax / readingBitsTotal));
    const payloadBitsTotal = preludeBits + (readingsPerMessage * readingBitsTotal);
    const payloadBytesTotal = Math.ceil(payloadBitsTotal / 8);
    if (payloadBytesTotal > PAYLOAD_MAX_SIZE_BYTES) {
        throw new Error(`The payload must not exceed ${PAYLOAD_MAX_SIZE_BYTES} bytes. Minimum required payload size: ${payloadBytesTotal} bytes`);
    }
    context.payloadBitsTotal = payloadBitsTotal;
    context.payloadBytesTotal = payloadBytesTotal;
    context.readingsPerMessage = readingsPerMessage;

    const headerCode = generateHeaderCode(version, context);
    const beautifiedHeaderCode = formatCode(headerCode);

    const implementationCode = generateImplementationCode(version, context);
    const beautifiedImplementationCode = formatCode(implementationCode);

    return {
        header: beautifiedHeaderCode,
        implementation: beautifiedImplementationCode,
    };
}

const versions = require('../versions');
if (versions.length === 0) {
    throw new Error('There are no versions defined. Please define a version first.');
}

const buildVersion = process.argv[2];
const version = buildVersion !== undefined ? versions[buildVersion] : versions[_.findLastKey(versions)];

if (version === undefined) {
    throw new Error(`Invalid version ${buildVersion}`);
}

console.log(`Building version ${version.number}.`);

const options = {
    headerFile: 'payload.h',
    sourceFile: 'payload.cpp',
};

const result = generateArduinoCode(version, options);
const outputPath = path.resolve(__dirname, 'dist');
fs.writeFileSync(path.join(outputPath, options.headerFile), result.header);
fs.writeFileSync(path.join(outputPath, options.sourceFile), result.implementation);

module.exports = generateArduinoCode;
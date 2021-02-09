const clangFormat = require('clang-format');
const { spawnSync } = require('child_process');
const { unparen, messageSizeInfo } = require('../common/utilities');
const constants = require('../common/constants');
const fs = require('fs');
const path = require('path');
const _ = require('lodash');

const PAYLOAD_STRUCT_NAME = 'Payload';
const MEASUREMENT_DATA_STRUCT_NAME = 'Measurement';
const DATA_FIELD_NAME = 'data';
const VERSION_FIELD_NAME = 'version';
const MEASUREMENTS_PER_MESSAGE_FIELD_NAME = 'measurements_per_message';
const LENGTH_FIELD_NAME = 'length';
const FILL_FUNCTION_NAME = 'fill';
const MEASUREMENTS_PARAM_NAME = 'measurements';
const MEASUREMENTS_PARAM_LENGTH = 'length';
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

    struct ${MEASUREMENT_DATA_STRUCT_NAME} {
        ${fields.map(f => `float ${f.property};`).join('\n')}
    };

    struct ${PAYLOAD_STRUCT_NAME} {
        uint8_t ${DATA_FIELD_NAME}[${context.messageBytesMax}] = { 0 };
        static constexpr uint8_t ${VERSION_FIELD_NAME} = ${version.number};
        static constexpr size_t ${MEASUREMENTS_PER_MESSAGE_FIELD_NAME} = ${context.measurementsPerMessage};
        void ${FILL_FUNCTION_NAME}(const ${MEASUREMENT_DATA_STRUCT_NAME} * ${MEASUREMENTS_PARAM_NAME}, size_t ${MEASUREMENTS_PARAM_LENGTH});
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
    
    void ${PAYLOAD_STRUCT_NAME}::${FILL_FUNCTION_NAME}(const ${MEASUREMENT_DATA_STRUCT_NAME} * ${MEASUREMENTS_PARAM_NAME}, size_t ${MEASUREMENTS_PARAM_LENGTH}) {
        ${MEASUREMENTS_PARAM_LENGTH} = std::min(static_cast<size_t>(${context.measurementsPerMessage}), ${MEASUREMENTS_PARAM_LENGTH});
    `;

    code += `BitWriter bitWriter(${DATA_FIELD_NAME}, 0);

        // Write version.
        bitWriter.write(${VERSION_FIELD_NAME}, ${constants.VERSION_BITS});

        // Write length.
        bitWriter.write(${LENGTH_FIELD_NAME}, ${constants.MEASUREMENT_COUNT_BITS});

        for (size_t i = 0; i < ${MEASUREMENTS_PARAM_LENGTH}; i++) {
            const auto& measurement = ${MEASUREMENTS_PARAM_NAME}[i];

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
        let rawExpr = `std::max(${floatify(field.range.min)}, std::min(measurement.${field.property}, ${floatify(field.range.max)}))`;

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
    const maxVersion = ~(~0 << constants.VERSION_BITS);
    if (version.number > maxVersion) {
        throw new Error(`The version number must not be greater than ${maxVersion}. Given: ${version.number}`);
    }

    const context = Object.assign({}, options);

    // Calculate payload and measurement sizes.
    const messageSize = messageSizeInfo(version);
    if (messageSize.messageBitsMin > messageSize.payloadBitsMax) {
        throw new Error(`The payload must not exceed ${messageSize.payloadBitsMax} bits. Minimum required payload size: ${messageSize.messageBitsMin} bits.`);
    }
    context.messageBytesMax = messageSize.messageBytesMax;
    context.measurementsPerMessage = messageSize.measurementsPerMessage;

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
const babel = require('@babel/core');
const prettier = require('prettier');

const WORD_SIZE = 32;

function generateDecoder(versions) {
    const versionLength = versions.length;
    let code = '';

    code += `
    
    /**
     * This code was automatically generated from payload field definitions.
     */

    `;


    code += 'function Decoder(bytes, port) {';

    code += `
    // There must be at least one byte given for the version.
    if (bytes.length === 0) {
        throw new Error('Empty message received.');
    }
    `;

    code += `
    const version = bytes[0];
    const decode = decoders[\`v\${version}\`];
    if (!decode) {
        throw new Error(\`Unknown message version: \${version}\`);
    }

    // Call decode function for version
    var decoded = { version: version };
    decode(bytes, port, decoded);
    `;

    code += `return decoded;
    `;


    // Close Decoder
    code += `}

    `;

    // Generate decode function for each version. Newest version first.
    code += `const decoders = {};
    
    `;
    for (let i = versionLength - 1; i >=0 ; i--) {
        const version = versions[i];
        code += `decoders.v${version.number} = ${generateAnonymousDecodeFunction(version)};
        
        `;
    }

    return code;
}

function generateAnonymousDecodeFunction(version) {
    const fields = version.fields;

    // Require 1 byte for version.
    let requiredBits = 8;

    for (let field of fields) {
        // Calculate bits used by field.
        const distinctValueCount = ((field.range.max - field.range.min) / field.resolution) + 1;
        const bits = Math.ceil(Math.log2(distinctValueCount));
        // Make sure that the field does not exceed the word size.
        if (bits > WORD_SIZE) {
            throw new Error(`Bit length of field "${field.name}" must not exceed ${WORD_SIZE} bits (actual: ${bits}).`);
        }
        field.bits = bits;
        requiredBits += bits;
    }

    const requiredBytes = Math.ceil(requiredBits / 8);

    let code = '';

    code += `function (bytes, port, decoded) {
    `;

    code += `
        // Check that enough bytes were received.
        const requiredBytes = ${requiredBytes};
        if (bytes.length < requiredBytes) {
            throw new Error(\`Insufficient number of bytes received. Required \${requiredBytes}\`);
        }

    `;

    let byteIndex = 0;
    let bitIndex = 0;
    for (let field of fields) {
        let bitsTotal = field.bits;
        let bitsLeft = field.bits;

        code += `// ${field.bits}-bit ${field.name} ${field.range.min}${field.unit} .. ${field.range.max}${field.unit} in ${field.resolution}${field.unit} increments.
        `;

        // The numeric expression of the value.
        let valueExpr = '';

        if (bitsLeft === 0) {
            // The field is constant.
            valueExpr = `(${field.range.min})`;
        } else {
            // Read the field value from the bytes.
            const orExpressions = [];
            while (bitsLeft > 0) {
                let read = Math.min(bitsLeft, 8 - bitIndex);

                let expr = `bytes[${byteIndex}]`;

                // Mask of previously used low bits.
                if (bitIndex > 0) {
                    expr = `(${expr} >>> ${bitIndex})`;
                }

                // Mask of unused high bits.
                if (read < 8) {
                    expr = `(${expr} & ~(~0 << ${read}))`;
                }

                // Shift bits into correct position.
                const position = bitsTotal - bitsLeft;
                if (position > 0) {
                    expr =  `(${expr} << ${position})`;
                }

                orExpressions.push(expr);

                // Increment byte index if all bits of its bits were read.
                byteIndex += (bitIndex + read) >>> 3;

                // Increase bit index and wrap if all if necessary.
                bitIndex = (bitIndex + read) & 7;

                bitsLeft -= read;
            }
            const rawExpr = orExpressions.reverse().join(' | ');
            code += `const ${field.property}Raw = ${rawExpr};
            `;

            valueExpr = `${field.property}Raw`;
            if (field.resolution !== 1) {
                valueExpr = `(${valueExpr} * ${field.resolution})`;
            }
            let minValue = field.range.min;
            if (minValue !== 0) {
                valueExpr = `(${valueExpr} ${minValue < 0 ? '-' : '+'} ${Math.abs(minValue)})`;
            }
        }

        const resolutionStr = field.resolution.toString();
        const dotIndex = resolutionStr.lastIndexOf('.');
        const decimalPlaces = dotIndex === -1 ? 0 : (resolutionStr.length - dotIndex - 1);
        code += `decoded.${field.property} = ${valueExpr}.toFixed(${decimalPlaces});
        
        `;
    }

    code += `return decoded;
    }`; // Close decode function

    return code;
}

function generateDecoderCode(versions) {
    const code = generateDecoder(versions);
    const transpiled = babel.transformSync(code);
    const beautified = prettier.format(transpiled.code, {
        printWidth: Number.MAX_SAFE_INTEGER,
        tabWidth: 4,
        semi: true,
        parser: 'babel',
    });
    return beautified;
}

module.exports = generateDecoderCode;
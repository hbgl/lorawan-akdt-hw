const versions = require('./versions');
const generateDecoderCode = require('./generator');
const fs = require('fs');

const code = generateDecoderCode(versions);

fs.writeFileSync('./dist/decoder.js', code);
const del = require('del');

del.sync([
    'arduino/dist/**',
    '!arduino/dist/',
    '!arduino/dist/bit_writer.cpp',
    '!arduino/dist/bit_writer.h',
    'arduino/target/**',
    'ttn/dist/**',
    '!ttn/dist/',
    '!.gitkeep',
]);
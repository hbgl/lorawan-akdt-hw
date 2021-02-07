module.exports = {
    'extends': 'eslint:recommended',
    'env': {
        'browser': true,
        'node': true,
    },
    'parserOptions': {
        'ecmaVersion': 6,
    },
    'globals': {
        'global': 'writable',
    },
    'rules': {
        'indent': [
            'error',
            4,
        ],
        'linebreak-style': [
            'error',
            'unix',
        ],
        'quotes': [
            'error',
            'single',
        ],
        'semi': [
            'error',
            'always',
        ],
        'no-trailing-spaces': [
            'error',
        ],
        'comma-dangle': [
            'error',
            'always-multiline',
        ],
        'no-unused-vars': [
            'error',
            {
                'varsIgnorePattern': '^_',
                'argsIgnorePattern': '^_',
            },
        ],
    },
};
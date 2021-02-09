// #region Fields Version 1
const fieldsVersion1 = [
    {
        name: 'Measurement time offset',
        unit: 'min',
        range: { min: -4095, max: 0 },
        resolution: 1,
        property: 'timeOffset',
        accuracy: 0,
        sensor: null,
    },
    {
        name: 'Ground temperature',
        unit: '°C',
        range: { min: -20, max: 85 },
        resolution: 0.5,
        property: 'temperatureGround',
        accuracy: 0.8,
        sensor: 'SMT50',
    },
    {
        name: 'Ground water content',
        unit: '%',
        range: { min: 0, max: 50 },
        resolution: 1,
        property: 'moistureGround',
        accuracy: 3,
        sensor: 'SMT50',
    },
    {
        name: 'Illuminance',
        unit: 'lx',
        range: { min: 0, max: 120000 },
        resolution: 1,
        property: 'light',
        accuracy: { value: 10, type: '%' },
        sensor: 'VEML7700',
    },
    {
        name: 'Air temperature',
        unit: '°C',
        range: { min: -40, max: 80 },
        resolution: 0.5,
        property: 'temperatureAir',
        accuracy: 0.5,
        sensor: 'BME280',
    },
    {
        name: 'Relative air humidity',
        unit: '%',
        range: { min: 0, max: 100 },
        resolution: 1,
        property: 'moistureAir',
        accuracy: 3,
        sensor: 'BME280',
    },
    {
        name: 'Barometric pressure',
        unit: 'hPa',
        range: { min: 300, max: 1100 },
        resolution: 1,
        property: 'pressure',
        accuracy: 1,
        sensor: 'BME280',
    },
    {
        name: 'Battery charge level',
        unit: '%',
        range: { min: 0, max: 100 },
        resolution: 1,
        property: 'battery',
        accuracy: 0,
        sensor: null,
    },
    {
        name: 'Battery voltage',
        unit: '%',
        range: { min: 2.8, max: 4.2 },
        resolution: 0.01,
        property: 'batteryVoltage',
        accuracy: 0,
        sensor: null,
    },
];
// #endregion

const { prepareFields } = require('./common/utilities');

const versions = {
    '1': { number: 1, fields: prepareFields(fieldsVersion1) },
};

module.exports = versions;

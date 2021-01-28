// #region Fields Version 1
const fieldsVersion1 = [
    {
        name: 'Ground temperature',
        unit: '°C',
        range: { min: -20, max: 85 },
        resolution: 0.5,
        property: 'temperatureGround',
    },
    {
        name: 'Ground water content',
        unit: '%',
        range: { min: 0, max: 50 },
        resolution: 1,
        property: 'moistureGround',
    },
    {
        name: 'Illuminance',
        unit: 'lx',
        range: { min: 0, max: 120000 },
        resolution: 1,
        property: 'light',
    },
    {
        name: 'Air temperature',
        unit: '°C',
        range: { min: -40, max: 80 },
        resolution: 0.5,
        property: 'temperatureAir',
    },
    {
        name: 'Relative air humidity',
        unit: '%',
        range: { min: 0, max: 100 },
        resolution: 1,
        property: 'moistureAir',
    },
    {
        name: 'Barometric pressure',
        unit: 'hPa',
        range: { min: 300, max: 1100 },
        resolution: 1,
        property: 'pressure',
    },
    {
        name: 'Battery charge level',
        unit: '%',
        range: { min: 0, max: 100 },
        resolution: 1,
        property: 'battery',
    },
];
// #endregion

const versions = [
    { number: 1, fields: fieldsVersion1 },
];

module.exports = versions;

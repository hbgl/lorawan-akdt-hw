const versions = require('./versions');
const constants = require('./common/constants');
const { messageSizeInfo, formatFieldRange, formatFieldAccuracy } = require('./common/utilities');
const markdownTable = require('markdown-table');
const path = require('path');
const fs = require('fs');

const versionSections = Object.values(versions).reverse().map(version => {
    const messageTable = {
        header: 'Nachricht',
        rows: [
            ['Bezeichnung', 'Bits', 'Anmerkung'],
        ],
        markdown: '',
    };

    const measurementTable = {
        header: 'Messung',
        rows: [
            ['Bezeichnung', 'Bits', 'Sensor', 'Einheit', 'Bereich', 'Genauigkeit', 'Auflösung'],
        ],
        markdown: '',
    };

    const sizeInfo = messageSizeInfo(version);

    messageTable.rows.push([
        'Version',
        constants.VERSION_BITS,
        'Versionsnummer des Nachrichtenformats.',
    ]);
    messageTable.rows.push([
        'Anzahl Messungen',
        constants.MEASUREMENT_COUNT_BITS,
        'Anzahl der Messungen in dieser Nachricht.',
    ]);
    messageTable.rows.push([
        'Messungen',
        `${sizeInfo.measurementBits} {1, ${sizeInfo.measurementsPerMessage}}`,
        `1 bis ${sizeInfo.measurementsPerMessage} Messungen pro Nachricht je ${sizeInfo.measurementBits} Bits`,
    ]);

    const decodedSample = {};
    const sampleMeasurement = {};
    decodedSample[constants.VERSION_PROPERTY] = version.number;

    for (let field of version.fields) {
        sampleMeasurement[field.property] = (field.range.max / 2).toFixed(field.fractionDigits);
        measurementTable.rows.push([
            field.name,
            field.bits,
            field.sensor,
            field.unit,
            formatFieldRange(field),
            field.resolution,
            formatFieldAccuracy(field),
        ]);
    }

    const measurement1 = JSON.parse(JSON.stringify(sampleMeasurement));
    measurement1.timeOffset = '0';
    const measurement2 = JSON.parse(JSON.stringify(sampleMeasurement));
    measurement2.timeOffset = '30';

    decodedSample[constants.MEASUREMENTS_PROPERTY] = [measurement1, measurement2];

    messageTable.markdown = markdownTable(messageTable.rows);
    measurementTable.markdown = markdownTable(measurementTable.rows);

    return {
        version: version.number,
        header: `Version ${version.number}`,
        tables: [messageTable, measurementTable],
        decodedSample: JSON.stringify(decodedSample, null, 4),
    };
});


const readmePath = path.resolve(__dirname, 'README.md');
const readmeContent = fs.readFileSync(readmePath).toString();

const beginTag = '<!---#VERSIONS--->';
const endTag = '<!---#ENDVERSIONS--->';
const versionBeginIndex = readmeContent.indexOf(beginTag);
const versionEndIndex = readmeContent.indexOf(endTag);

if (versionBeginIndex === -1 || versionEndIndex === -1 || versionBeginIndex > versionEndIndex) {
    throw new Error(`Please insert opening tag ${beginTag} and closing tag ${endTag} into the ${readmePath} file.`);
}

let versionMarkdown = '\n\n' ;
for (let versionSection of versionSections) {
    versionMarkdown += `### ${versionSection.header}\n`;
    for (let table of versionSection.tables) {
        versionMarkdown += `<br>\n\n${table.header}\n${table.markdown}\n`;
    }
    versionMarkdown += '<br>\n\nBeispiel:\n```json\n' + versionSection.decodedSample + '\n```\n';
}

const newReadmeContent = readmeContent.substr(0, versionBeginIndex + beginTag.length) +
    versionMarkdown +
    readmeContent.substr(versionEndIndex);

fs.writeFileSync(readmePath, newReadmeContent);
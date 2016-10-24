/*  Download the worksheets from Google Sheet as individual source files.
gsheets --key=1Spf1qlaAH2hzwa2MvuDNv2pzeSAhlU451IRfEV6stMI --title="all-types.h" --pretty
 */
'use strict';

const spreadsheet_key = '1Spf1qlaAH2hzwa2MvuDNv2pzeSAhlU451IRfEV6stMI';
const filenames = [
  'all-types.h',
  'all-registers.h',
  'all-names.h',
  'all-units.h'
];

const fs = require('fs');
const gsheets = require('gsheets');

for (const filename of filenames) {
  gsheets.getWorksheet(spreadsheet_key, filename, (err, res) => {
    if (err) return console.error(err);
    const lines = [];
    const data = res.data;
    let prev_line = null;
    for (const item of data) {
      for (const key in item) {
        const line = item[key];
        //  Skip multiple blank lines.
        if (line === '' && line == prev_line)
          continue;
        prev_line = line;
        lines.push(line);
        console.log(line);
      }
      fs.writeFileSync(filename, lines.join('\n'));
    }
  });
}

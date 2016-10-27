/*  Download the worksheets from Google Sheet as individual source files.
gsheets --key=1Spf1qlaAH2hzwa2MvuDNv2pzeSAhlU451IRfEV6stMI --title="all-types.h" --pretty
 */
'use strict';

//  Key of Google Sheet.  The sheet must be published publicly.
const spreadsheet_key = '1Spf1qlaAH2hzwa2MvuDNv2pzeSAhlU451IRfEV6stMI';

//  Worksheets to be downloaded as files.
const filenames = [
  'all-types.h',
  'all-registers.h',
  'all-names.h',
  'all-units.h'
];

const fs = require('fs');
const gsheets = require('gsheets');

//  Download each file from the sheet.
for (const filename of filenames) {
  gsheets.getWorksheet(spreadsheet_key, filename, (err, res) => {
    if (err) return console.error(err);
    const lines = [];
    const data = res.data;
    let prev_line = null;

    //  Write all lines.
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

      //  Write the file.
      fs.writeFileSync(filename, lines.join('\n'));
    }
  });
}

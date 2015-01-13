var path = require('path');
var fs = require('fs');
var os = require('os');
var zipfile = require('./lib');

/*

Setup:

wget http://mapbox.s3.amazonaws.com/tmp/too-large.zip

*/

var filepath = './too-large.zip'

if (!fs.existsSync(filepath)) {
    console.log('file not downloaded '+ filepath)
    process.exit(1)
}

function unzip(err) {
    if (err) throw err;
    console.log('Unzipping...');
    var zf = new zipfile.ZipFile(filepath);
    zf.copyFileSync('US_OG_022014.dbf', 'out.dbf');
}

unzip()
var path = require('path');
var fs = require('fs');
var os = require('os');
var zipfile = require('../lib');
var assert = require('assert');
var crypto = require('crypto');
var shasum = crypto.createHash('md5');

/*

Setup:

wget http://mapbox.s3.amazonaws.com/tmp/too-large.zip

Usage:

  node bench/largefile.js /path/to/file.zip

*/

if (process.argv.length < 3) {
  console.log("please provide path to zip archive")
  process.exit(1)
}

var filepath = process.argv[2];

var expected_md5 = 'e0da2edc680bf10dd11decdcec1e521a';

if (!fs.existsSync(filepath)) {
    console.log('file not downloaded '+ filepath);
    process.exit(1);
}

function unzip(err) {
    if (err) throw err;
    console.log('Unzipping...');
    console.time('opening')
    var zf = new zipfile.ZipFile(filepath);
    console.timeEnd('opening')
    console.time('copying')
    zf.names.forEach(function(name) {
        zf.copyFileSync(name,name);
    })
    console.timeEnd('copying')
    validate('out.dbf');
}

function validate(filename) {
    var s = fs.ReadStream(filename);
    console.time('validating')
    s.on('data', function(d) {
      shasum.update(d);
    });

    s.on('end', function() {
      var d = shasum.digest('hex');
      console.log(d + '  ' + filename);
      assert.equal(d,expected_md5);
      console.timeEnd('validating')
    });
}

unzip()
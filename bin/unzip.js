#!/usr/bin/env node

var zip = require('zipfile');
var fs = require('fs');
var path = require('path');

// node compatibility for mkdirs below
var constants = (!process.EEXIST >= 1) ?
    require('constants') :
    { EEXIST: process.EEXIST };

var usage = 'usage: unzip.js <zipfile>';

var file = process.ARGV[2];
if (!file) {
   console.log(usage);
   process.exit(1);
}

var zf = new zip.ZipFile(file);

zf.names.forEach(function(name) {
    var uncompressed = path.join('.', name);
    var dirname = path.dirname(uncompressed);
    fs.mkdir(dirname, 0755 , function(err) {
        if (err && err.errno != constants.EEXIST) throw err;
        if (path.extname(name)) {
            var buffer = zf.readFileSync(name);
            fd = fs.openSync(uncompressed, 'w');
            fs.writeSync(fd, buffer, 0, buffer.length, null);
            fs.closeSync(fd);
        }
    });
});

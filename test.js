#!/usr/bin/env node

var zipfile = require('./lib/zipfile');
var assert = require('assert');

// node compatibility for mkdirs below
var constants = (!process.EEXIST >= 1) ?
    require('constants') :
    { EEXIST: process.EEXIST };


var zf = new zipfile.ZipFile('./data/world_merc.zip');

// test writing with Sync reading method
// and sync node writing functions
zf.names.forEach(function(name) {
    var uncompressed = path.join('/tmp/sync', name);
    var dirname = path.dirname(uncompressed);
    mkdirP(dirname, 0755 , function(err) {
        if (err && err.errno != constants.EEXIST) throw err;
        if (path.extname(name)) {
            var buffer = zf.readFileSync(name);
            fd = fs.openSync(uncompressed, 'w');
            fs.writeSync(fd, buffer, 0, buffer.length, null);
            fs.closeSync(fd);
        }
    });
});


// test writing with Async reading method
// and async node writing functions
zf.names.forEach(function(name) {
    var uncompressed = path.join('/tmp/async', name);
    var dirname = path.dirname(uncompressed);
    mkdirP(dirname, 0755 , function(err) {
        if (err && err.errno != constants.EEXIST) throw err;
        if (path.extname(name)) {
            zf.readFile(name, function(err, buffer) {
                if (err) throw err;
                fs.open(uncompressed, 'w', 0755, function(err,fd) {
                    if (err) throw err;
                    fs.write(fd, buffer, 0, buffer.length, null, function(err,written) {
                         if (err) throw err;
                         // written is number of bytes written
                         assert.ok(written > 0);
                         fs.close(fd, function(err) {
                             if (err) throw err;
                         });
                    });
                });
            });
        }
    });
});

console.log('All tests pass...');

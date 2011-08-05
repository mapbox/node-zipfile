#!/usr/bin/env node

var zipfile = require('./lib/zipfile');
var assert = require('assert');
var path = require('path');
var fs = require('fs');

// node compatibility for mkdirs below
var constants = (!process.EEXIST >= 1) ?
    require('constants') :
    { EEXIST: process.EEXIST };

// created with:
// $ zip -9vr test.zip *txt -c "a comment" -z "zipfile comment"
var zf = new zipfile.ZipFile('./data/test.zip');
//console.log(zf)
assert.equal(zf.count, 2);
assert.deepEqual(zf.names, ['one.txt', 'two.txt']);

// created with /System/Library/CoreServices/Archive Utility.app
var zf = new zipfile.ZipFile('./data/Archive.zip');
//console.log(zf)
assert.equal(zf.count, 2);
assert.deepEqual(zf.names, ['one.txt', 'two.txt']);

var zf = new zipfile.ZipFile('./data/folder.zip');
assert.equal(zf.count, 3); // one folder, two files
assert.deepEqual(zf.names, ['folder/', 'folder/one.txt', 'folder/two.txt']);

assert.throws(function() { zf.readFileSync('foo')});

var zf = new zipfile.ZipFile('./data/world_merc.zip');


function mkdirP(p, mode, f) {
    var cb = f || function() {};
    // if relative
    if (p.charAt(0) != '/') {
        // if >= node 0.3.0 use path.resolve()
        p = path.join(__dirname, p);
    }
    var ps = path.normalize(p).split('/');
    path.exists(p, function(exists) {
        if (exists) cb(null);
        else mkdirP(ps.slice(0, -1).join('/'), mode, function(err) {
            if (err && err.errno != contants.EEXIST) cb(err);
            else fs.mkdir(p, mode, cb);
        });
    });
}

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

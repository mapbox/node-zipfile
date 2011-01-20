#!/usr/bin/env node

var zipfile = require('zipfile');
var assert = require('assert');
var path = require('path');
var fs = require('fs');

// node 0.2 <-> 0.3 compatibility
var constants = {};
if (!process.EEXIST >= 1) 
    constants = require('constants');
else
    constants.EEXIST = process.EEXIST;

// created with:
// $ zip -9vr test.zip *txt -c "a comment" -z "zipfile comment"
var zf = new zipfile.ZipFile('./data/test.zip');
//console.log(zf)
assert.equal(zf.count,2)
assert.deepEqual(zf.names,['one.txt','two.txt'])

// created with /System/Library/CoreServices/Archive Utility.app
var zf = new zipfile.ZipFile('./data/Archive.zip');
//console.log(zf)
assert.equal(zf.count,2)
assert.deepEqual(zf.names,['one.txt','two.txt'])


function mkdirP (p, mode, f) {
    var cb = f || function () {};
    // if relative
    if (p.charAt(0) != '/') {
        // if >= node 0.3.0 use path.resolve()
        p = path.join(__dirname,p);
    }
    var ps = path.normalize(p).split('/');
    path.exists(p, function (exists) {
        if (exists) cb(null);
        else mkdirP(ps.slice(0,-1).join('/'), mode, function (err) {
            if (err && err.errno != constants.EEXIST) cb(err)
            else fs.mkdir(p, mode, cb);
        });
    });
};


var zf = new zipfile.ZipFile('./data/test.zip');
//var zf = new zipfile.ZipFile('./data/folder.zip');

zf.names.forEach(function(name) {
    var uncompressed = path.join('/tmp/',name);
    var dirname = path.dirname(uncompressed);
    mkdirP(dirname, 0755 , function(err) {
        if (err && err.errno != constants.EEXIST) throw err;
        if (path.extname(name)) {
            var buffer = zf.readFileSync(name);
            fd = fs.openSync(uncompressed,'w');
            fs.writeSync(fd, buffer, 0, buffer.length, null);
            fs.closeSync(fd);
        }
    })
})

//console.log('All tests pass...');

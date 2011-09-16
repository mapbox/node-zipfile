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

// test writing with Async reading method
// and async node writing functions
names = zf.names.slice(0);
function asyncDecompress(cb) {
    if (names.length == 0) {
      cb();
      return;
    }
    var name = names.shift();
    console.log("Decompressing " + name);
    var uncompressed = path.join('/tmp/async', name);
    var dirname = path.dirname(uncompressed);
    mkdirP(dirname, 0755 , function(err) {
        if (err && err.errno != constants.EEXIST) throw err;
        if (path.extname(name)) {
            zip_stream = zf.createReadStream(name);
            file_stream = fs.createWriteStream(uncompressed);
            zip_stream.pipe(file_stream);
            zip_stream.on('close', function () { asyncDecompress(cb)});
        }
    });
};

try { fs.unlinkSync('/tmp/compress.zip'); }
catch (err) {}
var new_zf = new zipfile.ZipFile('/tmp/compress.zip');

function asyncCompress() {
    zf.names.forEach(function (name) { 
      console.log("Adding " + name);
      var uncompressed = path.join('/tmp/async', name);
      new_zf.addFile(name, uncompressed);
    });
    new_zf.save(function (err) {
      if (err) 
        console.log("Error while saving file: " + err);
      else {
        new_zf.names.forEach(function (name) {
          console.log("Replacing " + name);
          var uncompressed = path.join('/tmp/async', name);
          new_zf.replaceFile(name, uncompressed);
        });
        new_zf.save(function (err) {
          if (err) console.log("Error while saving file: " + err);
          else console.log("All test passed!");
        });
      }
    });
}



asyncDecompress(asyncCompress);

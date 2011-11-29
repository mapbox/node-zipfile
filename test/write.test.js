var zipfile = require('zipfile');
var assert = require('assert');
var support = require('./support/helper');
var path = require('path');
var fs = require('fs');
var constants = require('constants');

describe('Async Writes', function(){

    var zf = new zipfile.ZipFile('./test/data/world_merc.zip');
    var wrote = 0;

    it('async write', function(done){
        zf.names.forEach(function(name) {
            var dest = path.join('/tmp/async', name);
            support.mkdirP(path.dirname(dest), 0755 , function(err) {
                if (err) throw err;
                if (path.extname(name)) {
                    zf.readFile(name, function(err, buffer) {
                        if (err) throw err;
                        fs.open(dest, 'w', 0644, function(err, fd) {
                            if (err) throw err;
                            fs.write(fd, buffer, 0, buffer.length, null, function(err,written) {
                                 if (err) throw err;
                                 // written is number of bytes written
                                 assert.ok(written > 0);
                                 fs.close(fd, function(err) {
                                     wrote++;
                                     if (wrote >= 4) done();
                                     if (err) throw err;
                                 });
                            });
                        });
                    });
                }
            });
        });
    });

    after(function(){
      assert.equal(wrote, 4);
    });

});

describe('Sync Writes', function(){

    var zf = new zipfile.ZipFile('./test/data/world_merc.zip');
    var wrote = 0;

    it('sync write', function(){
        zf.names.forEach(function(name) {
            var uncompressed = path.join('/tmp/sync', name);
            var dirname = path.dirname(uncompressed);
            support.mkdirP(dirname, 0755 , function(err) {
                if (err && !err.message.match(/^EEXIST/)) throw err;
                if (path.extname(name)) {
                    var buffer = zf.readFileSync(name);
                    var fd = fs.openSync(uncompressed, 'w');
                    fs.writeSync(fd, buffer, 0, buffer.length, null);
                    fs.closeSync(fd);
                    assert.ok(path.existsSync(uncompressed));
                    ++wrote;
                }
            });
        });
    });

    after(function(){
      assert.equal(wrote, 4);
    });

});
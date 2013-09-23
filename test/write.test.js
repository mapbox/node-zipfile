var zipfile = require('..');
var assert = require('assert');
var path = require('path');
var fs = require('fs');
var constants = require('constants');
var mkdirp = require('mkdirp');
var existsSync = require('fs').existsSync || require('path').existsSync;

describe('Async Writes', function(){

    var zf = new zipfile.ZipFile('./test/data/world_merc.zip');
    zf.names.forEach(function(name) {
        it('async write '+ name, function(done){
            var dest = path.join(__dirname,'tmp', name);
            mkdirp(path.dirname(dest), 0755 , function(err) {
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
                                     if (err) throw err;
                                     done();
                                 });
                            });
                        });
                    });
                }
            });
        });
    });

});

describe('Sync Writes', function(){
    var zf = new zipfile.ZipFile('./test/data/world_merc.zip');
    zf.names.forEach(function(name) {
        var uncompressed = path.join('/tmp/sync', name);
        var dirname = path.dirname(uncompressed);
        mkdirp(dirname, 0755 , function(err) {
            it('sync write '+ name, function(done){
                if (err && !err.message.match(/^EEXIST/)) throw err;
                if (path.extname(name)) {
                    var buffer = zf.readFileSync(name);
                    var fd = fs.openSync(uncompressed, 'w');
                    fs.writeSync(fd, buffer, 0, buffer.length, null);
                    fs.closeSync(fd);
                    assert.ok(existsSync(uncompressed));
                    done();
                }
            });
        });
    });
});
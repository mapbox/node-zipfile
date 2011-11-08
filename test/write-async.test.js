var zipfile = require('zipfile');
var support = require('./support/helper');
var path = require('path');
var fs = require('fs');
var constants = require('constants');

exports['test async write'] = function(beforeExit, assert) {
    var zf = new zipfile.ZipFile('./test/data/world_merc.zip');
    var wrote = 0;

    // test writing with Async reading method
    // and async node writing functions
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
                                 if (err) throw err;
                             });
                        });
                    });
                });
            }
        });
    });

    beforeExit(function() {
        assert.equal(wrote, 4);
    });
};

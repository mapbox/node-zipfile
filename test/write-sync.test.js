var zipfile = require('zipfile');
var support = require('./support/helper');
var path = require('path');
var fs = require('fs');
var constants = require('constants');

exports['test sync write'] = function(beforeExit, assert) {
    var zf = new zipfile.ZipFile('./test/data/world_merc.zip');
    var wrote = 0;

    // test writing with Sync reading method
    // and sync node writing functions
    zf.names.forEach(function(name) {
        var uncompressed = path.join('/tmp/sync', name);
        var dirname = path.dirname(uncompressed);
        support.mkdirP(dirname, 0755 , function(err) {
            if (err && !err.message.match(/^EEXIST/)) throw err;
            if (path.extname(name)) {
                var buffer = zf.readFileSync(name);
                fd = fs.openSync(uncompressed, 'w');
                fs.writeSync(fd, buffer, 0, buffer.length, null);
                fs.closeSync(fd);
                assert.ok(path.existsSync(uncompressed));
                ++wrote;
            }
        });
    });

    beforeExit(function() {
        assert.equal(wrote, 4);
    });
};

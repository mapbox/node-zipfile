var zipfile = require('..');
var assert = require('assert');
var path = require('path');
var fs = require('fs');
var constants = require('constants');
var mkdirp = require('mkdirp');
var existsSync = require('fs').existsSync || require('path').existsSync;

describe('Writes', function(){

var sizes = {
    'world_merc.dbf':24641,
    'world_merc.prj':384,
    'world_merc.shp':428328,
    'world_merc.shx':2060,
};

describe('Async Writes', function(){

    var zf = new zipfile.ZipFile('./test/data/world_merc.zip');
    zf.names.forEach(function(name) {
        it('async write '+ name, function(done){
            var dest = path.join(__dirname,'tmp', name);
            mkdirp(path.dirname(dest), function(err) {
                if (err) throw err;
                if (path.extname(name)) {
                    zf.readFile(name, function(err, buffer) {
                        if (err) throw err;
                        assert.equal(sizes[name],buffer.length)
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
        var dest = path.join(__dirname,'tmp', 'sync',name);
        var dest2 = path.join(__dirname,'tmp', 'sync-copy',name);
        mkdirp.sync(path.dirname(dest));
        mkdirp.sync(path.dirname(dest2));
        it('sync write '+ name, function(done){
            if (path.extname(name)) {
                var buffer = zf.readFileSync(name);
                assert.equal(sizes[name],buffer.length)
                fs.writeFileSync(dest,buffer);
                assert.ok(existsSync(dest));
                zf.copyFileSync(name,dest2);
                assert.ok(existsSync(dest2));
                assert.equal(sizes[name],fs.readFileSync(dest2).length)
                done();
            }
        });
    });
});

});
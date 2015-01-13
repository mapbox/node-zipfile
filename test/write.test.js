var zipfile = require('..');
var assert = require('assert');
var path = require('path');
var fs = require('fs');
var constants = require('constants');
var mkdirp = require('mkdirp');
var existsSync = require('fs').existsSync || require('path').existsSync;
var crypto = require('crypto');

describe('Writes', function(){

var meta = {
    'world_merc.dbf': {size: 24641, md5: '7d087de3e8c9ed2aee0e81615ff91e13'},
    'world_merc.prj': {size: 384, md5: '1fac28f31277bfbbf8a1b0050cc8fb85'},
    'world_merc.shp': {size: 428328, md5: '9042bca3485687bef43a18ef8edc6469'},
    'world_merc.shx': {size: 2060, md5: 'b51caf61d1b821025bb8994151cfc0cc'},
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
                        assert.equal(meta[name].size,buffer.length);
                        var shasum = crypto.createHash('md5');
                        shasum.update(buffer);
                        var md5 = shasum.digest('hex');
                        assert.equal(meta[name].md5,md5);
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
                assert.equal(meta[name].size,buffer.length)
                var shasum = crypto.createHash('md5');
                shasum.update(buffer);
                var md5 = shasum.digest('hex');
                assert.equal(meta[name].md5,md5);
                fs.writeFileSync(dest,buffer);
                assert.ok(existsSync(dest));
                zf.copyFileSync(name,dest2);
                assert.ok(existsSync(dest2));
                assert.equal(meta[name].size,fs.readFileSync(dest2).length)
                done();
            }
        });
    });
});

});
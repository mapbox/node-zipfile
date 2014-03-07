var zipfile = require('../lib/zipfile');
var assert = require('assert');
var fs = require('fs');
var existsSync = require('fs').existsSync || require('path').existsSync;

describe('Handling unicode paths, filenames, and data', function(){

    it('open zip with folder with unicode', function(){
        var zippath = './test/data/Clément/foo.csv.zip';
        assert.ok(existsSync(zippath));
        var zf = new zipfile.ZipFile(zippath);
        assert.equal(zf.count, 3);
        var buffer = zf.readFileSync(zf.names[0]);
        assert.equal(buffer.toString('utf8'),fs.readFileSync(zippath.replace('.zip','')).toString('utf8').replace(/\r/g,''));
    });

    it('open zip with folder and file with unicode', function(){
        var zippath = './test/data/Clément/Olá.zip';
        assert.ok(existsSync(zippath));
        var zf = new zipfile.ZipFile(zippath);
        assert.equal(zf.count, 1);
    });

    it('open zip with unicode paths and unicode filenames in archive', function(){
        var zippath = './test/data/Clément/Olá.zip';
        assert.ok(existsSync(zippath));
        var zf = new zipfile.ZipFile(zippath);
        assert.equal(zf.count, 1);
        assert.deepEqual(zf.names, ['Olá.txt']);
        zf.names.forEach(function(name) {
            var buffer = zf.readFileSync(name);
            assert.equal(buffer.toString('utf8'),'Olá\n');
        });
    });

    it('open zip with ansi path but unicode filename in archive', function(){
        // created with OS X 10.7 with right-click, then "compress"
        var zippath = './test/data/points.csv.zip';
        assert.ok(existsSync(zippath));
        var zf = new zipfile.ZipFile(zippath);
        assert.equal(zf.count, 3);
        assert.deepEqual(zf.names, ["你好_points.csv","__MACOSX/","__MACOSX/._你好_points.csv"]);
        var buffer = zf.readFileSync(zf.names[0]);
        assert.equal(buffer.toString('utf8'),fs.readFileSync('./test/data/你好_points.csv').toString('utf8').replace(/\r/g,''));
    });

});

var zipfile = require('zipfile');
var assert = require('assert');
var fs = require('fs')

describe('Handling unicode paths, filenames, and data', function(){

    it('open zip with unicode paths', function(){
        var zf = new zipfile.ZipFile('./test/data/Clément/Olá.txt.zip');
        assert.equal(zf.count, 1);
    });

    it('open zip with unicode paths and unicode filenames in archive', function(){
        var zf = new zipfile.ZipFile('./test/data/Clément/Olá.txt.zip');
        assert.equal(zf.count, 1);
        assert.deepEqual(zf.names, ['Olá.txt']);
        zf.names.forEach(function(name) {
            var buffer = zf.readFileSync(name);
            assert.equal(buffer.toString('utf8'),'Olá\n');
        });
    });

    it('open zip with ansi paths and but unicode filenames in archive', function(){
        // created with OS X 10.7 with right-click, then "compress"
        var zf = new zipfile.ZipFile('./test/data/你好_points.csv.zip');
        assert.equal(zf.count, 3);
        assert.deepEqual(zf.names, ["你好_points.csv","__MACOSX/","__MACOSX/._你好_points.csv"]);
        var buffer = zf.readFileSync(zf.names[0]);
        assert.equal(buffer.toString('utf8'),fs.readFileSync('./test/data/你好_points.csv').toString('utf8'));
    });

});

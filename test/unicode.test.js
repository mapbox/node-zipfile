var zipfile = require('zipfile');
var assert = require('assert');

describe('Handling unicode paths, filenames, and data', function(){

    it('open zip with unicode paths', function(){
        var zf = new zipfile.ZipFile('./test/data/Clément/Olá.txt.zip');
        assert.equal(zf.count, 1); // one folder, two files
        assert.deepEqual(zf.names, ['Olá.txt']);
        zf.names.forEach(function(name) {
            var buffer = zf.readFileSync(name);
            assert.equal(buffer.toString('utf8'),'Olá\n');
        });
    });

});

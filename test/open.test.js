var zipfile = require('..');
var assert = require('assert');

describe('Opening', function(){

    it('folder based archive should open', function(){
        var zf = new zipfile.ZipFile('./test/data/folder.zip');
        assert.equal(zf.count, 3); // one folder, two files
        assert.deepEqual(zf.names, ['folder/', 'folder/one.txt', 'folder/two.txt']);
    });

    it('opening unix zip created archive', function(){
        // created with:
        // $ zip -9vr test.zip *txt -c "a comment" -z "zipfile comment"
        var zf = new zipfile.ZipFile('./test/data/test.zip');
        assert.equal(zf.count, 2);
        assert.deepEqual(zf.names, ['one.txt', 'two.txt']);
    });

    it('test opening mac created archive', function(){
        // created with /System/Library/CoreServices/Archive Utility.app
        var zf = new zipfile.ZipFile('./test/data/Archive.zip');
        assert.equal(zf.count, 2);
        assert.deepEqual(zf.names, ['one.txt', 'two.txt']);
    });
});

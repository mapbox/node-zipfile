var zipfile = require('zipfile');

exports['test opening unix zip created archive'] = function(beforeExit, assert) {
    // created with:
    // $ zip -9vr test.zip *txt -c "a comment" -z "zipfile comment"
    var zf = new zipfile.ZipFile('./test/data/test.zip');
    assert.equal(zf.count, 2);
    assert.deepEqual(zf.names, ['one.txt', 'two.txt']);
};


exports['test opening mac created archive'] = function(beforeExit, assert) {
    // created with /System/Library/CoreServices/Archive Utility.app
    var zf = new zipfile.ZipFile('./test/data/Archive.zip');
    assert.equal(zf.count, 2);
    assert.deepEqual(zf.names, ['one.txt', 'two.txt']);
};

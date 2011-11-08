var zipfile = require('zipfile');

exports['test opening zip with folders'] = function(beforeExit, assert) {
    var zf = new zipfile.ZipFile('./test/data/folder.zip');
    assert.equal(zf.count, 3); // one folder, two files
    assert.deepEqual(zf.names, ['folder/', 'folder/one.txt', 'folder/two.txt']);
};



var zipfile = require('zipfile');

exports['test non-existant archive'] = function(beforeExit, assert) {
    assert.throws(function() { new zipfile.ZipFile('thisdoesnotexist.zip'); });
};

exports['test invalid file'] = function(beforeExit, assert) {
    assert.throws(function() { new zipfile.ZipFile('./test/data/invalid.zip'); });
};

exports['test reading non-existant file'] = function(beforeExit, assert) {
    var zf = new zipfile.ZipFile('./test/data/folder.zip');
    assert.throws(function() { zf.readFileSync('foo'); });
};



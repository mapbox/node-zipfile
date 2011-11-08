var zipfile = require('zipfile');
var info = require('package.json');

exports['test version updated for release'] = function(beforeExit, assert) {
    assert.equal(info.version, zipfile.version);
};


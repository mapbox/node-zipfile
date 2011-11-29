var zipfile = require('zipfile');
var assert = require('assert');
var info = require('package.json');

describe('Version', function(){

    it('version updated for release', function(){
        assert.equal(info.version, zipfile.version);
    });
});


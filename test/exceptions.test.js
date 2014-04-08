var zipfile = require('..');
var assert = require('assert');

describe('Exceptions', function(){

    it('should throw as archive does not exit', function(){
      assert.throws(function() { new zipfile.ZipFile('thisdoesnotexist.zip'); });
    });

    it('should throw as the file is invalid', function(){
      assert.throws(function() { new zipfile.ZipFile('./test/data/invalid.zip'); });
    });

    it('should throw as the file in the archive does not exit', function(){
      var zf = new zipfile.ZipFile('./test/data/folder.zip');
      assert.throws(function() { zf.readFileSync('foo'); });
    });

    it('should not crash if we read invalid filename', function(done){
      var zf = new zipfile.ZipFile('./test/data/World_mercator.zip');
      var remaining = zf.names.length;
      zf.names.forEach(function(name) {
          zf.readFile(name.toLowerCase(),function(err, buffer) {
              assert.equal(err.message.indexOf('No file found'),0);
              remaining--;
              if (remaining <= 0) done();
          });
      });
    });

});
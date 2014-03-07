var binary = require('node-pre-gyp');
var path = require('path');
var binding_path = binary.find(path.resolve(path.join(__dirname,'../package.json')));
var zipfile = require(binding_path);

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

});
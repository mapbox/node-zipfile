#!/usr/bin/env node

var zipfile = require('zipfile');
var assert = require('assert');
//var path = require('path');
var fs = require('fs');


// created with:
// $ zip -9vr test.zip *txt -c "a comment" -z "zipfile comment"
var zf = new zipfile.ZipFile('./data/test.zip');
console.log(zf)
assert.equal(zf.count,2)
assert.deepEqual(zf.names,['one.txt','two.txt'])

// created with /System/Library/CoreServices/Archive Utility.app
var zf = new zipfile.ZipFile('./data/Archive.zip');
console.log(zf)
assert.equal(zf.count,2)
assert.deepEqual(zf.names,['one.txt','two.txt'])

console.log('All tests pass...');

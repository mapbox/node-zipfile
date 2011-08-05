var assert = require('assert');
var zipfile = require('./_zipfile');

/* assert ABI compatibility */
assert.ok(zipfile.versions.node === process.versions.node, 'The running node version "' + process.versions.node + '" does not match the node version that node-zipfile was compiled against: "' + zipfile.versions.node + '"');

// push all C++ symbols into js module
for (var k in zipfile) { exports[k] = zipfile[k]; }
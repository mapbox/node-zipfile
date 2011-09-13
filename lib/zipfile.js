var assert  = require('assert');
var fs      = require('fs');
var Stream  = require('stream').Stream;
var util    = require('util')
var zipfile = require('./_zipfile');

/* assert ABI compatibility */
assert.ok(zipfile.versions.node === process.versions.node, 'The running node version "' + process.versions.node + '" does not match the node version that node-zipfile was compiled against: "' + zipfile.versions.node + '"');

exports.ZipFile = zipfile.ZipFile;
exports.addFile = function (zf, archiveFile, name, offset, len) {
  if (!(zf instanceof zipfile.ZipFile)) throw "First argument needs to be a ZipFile instance.";
  if (offset === undefined) offset = 0;
  if (len === undefined) len = -1;
  if (name === undefined) name = archiveFile;
  fs.statSync(name);
  zipfile.addFile(zf, archiveFile, name, offset, len);
};
exports.replaceFile = function (zf, archiveFile, name, offset, len) {
  if (!(zf instanceof zipfile.ZipFile)) throw "First argument needs to be a ZipFile instance.";
  if (offset === undefined) offset = 0;
  if (len === undefined) len = -1;
  if (name === undefined) name = archiveFile;
  fs.statSync(name);
  zipfile.replaceFile(zf, archiveFile, name, offset, len);
};
exports.save = function (zf, cb) {
  if (!(zf instanceof zipfile.ZipFile)) throw "First argument needs to be a ZipFile instance.";
  zipfile.save(zf, cb);
};

var pool;
var kMinPoolSpace = 128;
var kPoolSize = 40 * 1024;

function allocNewPool() {
  pool = new Buffer(kPoolSize);
  pool.used = 0;
}

exports.createReadStream = function (zf, file) {
  if (!(zf instanceof zipfile.ZipFile)) throw "First argument needs to be a ZipFile instance.";
  return new ReadStream(zf, file);
}

var ReadStream = function(zf, file) {
  if (!(this instanceof ReadStream)) return new ReadStream(zip, file);

  Stream.call(this);

  this.readable = true;
  this.paused = false;
  this.bufferSize = 64 * 1024;

  if (this.encoding) this.setEncoding(this.encoding);

  this.zf = zf;

  zipfile.open(zf, file);
  this._read();
}
util.inherits(ReadStream, Stream);

ReadStream.prototype.setEncoding = function(encoding) {
    var StringDecoder = require('string_decoder').StringDecoder; // lazy load
      this._decoder = new StringDecoder(encoding);
};

ReadStream.prototype._read = function() {
  var self = this;
  if (!self.readable || self.paused || self.reading) return;

  self.reading = true;

  if (!pool || pool.length - pool.used < kMinPoolSpace) {
    // discard the old pool. Can't add to the free list because
    // users might have refernces to slices on it.
    pool = null;
    allocNewPool();
  }

  // Grab another reference to the pool in the case that while we're in the
  // thread pool another read() finishes up the pool, and allocates a new
  // one.
  var thisPool = pool;
  var toRead = Math.min(pool.length - pool.used, this.bufferSize);
  var start = pool.used;

  if (this.pos !== undefined) {
    toRead = Math.min(this.end - this.pos + 1, toRead);
  }

  function afterRead(err, bytesRead) {
    self.reading = false;
    if (err) {
      self.emit('error', err);
      self.readable = false;
      return;
    }

    if (bytesRead === 0) {
      self.emit('end');
      self.destroy();
      return;
    }

    var b = thisPool.slice(start, start + bytesRead);

    // Possible optimizition here?
    // Reclaim some bytes if bytesRead < toRead?
    // Would need to ensure that pool === thisPool.

    // do not emit events if the stream is paused
    if (self.paused) {
      self.buffer = b;
      return;
    }

    // do not emit events anymore after we declared the stream unreadable
    if (!self.readable) return;

    self._emitData(b);
    self._read();
  }

  zipfile.read(self.zf, pool, pool.used, toRead, afterRead);

  if (self.pos !== undefined) {
    self.pos += toRead;
  }
  pool.used += toRead;
};

ReadStream.prototype._emitData = function(d) {
  if (this._decoder) {
    var string = this._decoder.write(d);
    if (string.length) this.emit('data', string);
  } else {
    this.emit('data', d);
  }
};


ReadStream.prototype.destroy = function() {
  this.readable = false;
  zipfile.close(this.zf);
  this.emit('close');
};


ReadStream.prototype.pause = function() {
  this.paused = true;
};

ReadStream.prototype.resume = function() {
  this.paused = false;

  if (this.buffer) {
    this._emitData(this.buffer);
    this.buffer = null;
  }

  this._read();
};

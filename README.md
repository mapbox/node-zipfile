# node-zipfile
      
Bindings to [libzip](http://nih.at/libzip/libzip.html) for handling zipfile archives in [node](http://nodejs.org).

[![Build Status](https://secure.travis-ci.org/mapbox/node-zipfile.png)](http://travis-ci.org/mapbox/node-zipfile)
[![Build status](https://ci.appveyor.com/api/projects/status/fb8n98fc1smsjmum)](https://ci.appveyor.com/project/Mapbox/node-zipfile)
[![Dependencies](https://david-dm.org/mapbox/node-zipfile.png)](https://david-dm.org/mapbox/node-zipfile)
[![Coverage Status](https://coveralls.io/repos/mapbox/node-zipfile/badge.svg?branch=master&service=github)](https://coveralls.io/github/mapbox/node-zipfile?branch=master)

## Example

```js
var zipfile = require('zipfile');

// Creating a zipfile object
var zf = new zipfile.ZipFile('./test/data/world_merc.zip');

// the zipfile has a list of names:
// zf.names[0] === 'world_merc.prj'

// the zipfile also has a count property that is the number of files contained
// zf.count == 2

// finally it has a readFile method that uncompresses a single file
// into a Buffer object
zf.readFile('world_merc.prj', function(err, buffer) {
    if (err) throw err;
    console.log(buffer.toString());
});
```

### Large files

To handle large zipfiles and avoid the overhead of passing data from C++ to JS use the `copyFile` interface:

```js
var zipfile = require('zipfile');
var zf = new zipfile.ZipFile('./test/data/world_merc.zip');
var zip_entry_name = 'world_merc.shp';
var output_file = 'out/world_merc.shp';
zf.copyFile(zip_entry_name,output_file, function(err) {
  if (err) throw err;
  console.log('Successfully wrote ' + output_file);
});
```

## Depends

 - Node v0.10.x, v4.x, v5.x, or v6.x

## Installation

Install from binary:

    npm install

Install from source:

    npm install --build-from-source

## Dynamically linking against libzip

`node-zipfile` depends on libzip, but by default
bundles a copy in deps/ which is statically linked and packaged as a binary.

If you want to use an external libzip first install it:

Debian:

    sudo apt-get install libzip-dev libzip1

OS X:
  
    brew install libzip

Then configure node-zipfile with the --shared_libzip option:
 
    npm install  --build-from-source --shared_libzip

If you installed libzip in a custom location then configure like:
  
    npm install  --build-from-source --shared_libzip \
      --shared_libzip_includes=/opt/local/include \
      --shared_libzip_libpath=/opt/local/lib

If you installed libzip with homebrew do:

    npm install --build-from-source --shared_libzip \
      --shared_libzip_includes=`brew --prefix`/include \
      --shared_libzip_libpath=`brew --prefix`/lib

## License

  BSD, see LICENSE.txt

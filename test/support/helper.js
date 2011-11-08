var path = require('path');
var fs = require('fs');

exports.mkdirP = function(p, mode, callback) {
    var that = this;
    // if relative
    if (p.charAt(0) != '/') {
        // if >= node 0.3.0 use path.resolve()
        p = path.join(__dirname, p);
    }
    var ps = path.normalize(p).split('/');
    path.exists(p, function(exists) {
        if (exists) {
            callback(null);
        } else {
            that.mkdirP(ps.slice(0, -1).join('/'), mode, function(err) {
                if (err && !err.code.match(/^EEXIST/)) {
                    callback(err);
                } else {
                    fs.mkdir(p, mode, function(err) {
                        if (err && !err.code.match(/^EEXIST/)) {
                            callback(err);
                        } else {
                            callback(null);
                        }
                    });
                }
            });
        };
    });
};

#!/bin/bash

CURRENT_DIR="$( cd "$( dirname $BASH_SOURCE )" && pwd )"
cd $CURRENT_DIR/../
source ~/.nvm/nvm.sh
nvm use 0.10

set -u -e

npm install node-gyp
npm install aws-sdk
export PATH=`pwd`/node_modules/.bin:$PATH

function doit {
    rm -rf build
    node-pre-gyp clean build package testpackage "$@"
    npm test
    ./node_modules/.bin/node-pre-gyp publish info "$@"
    npm install --fallback-to-build=false
    npm test
}

doit
nvm use 0.8
doit --target=0.8.26
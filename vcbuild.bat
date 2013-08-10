rd /q /s build
del lib\\node_zipfile.node
npm install
copy build\Release\libzip.dll lib
rem test!
set NODE_PATH=lib
npm test

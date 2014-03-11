rd /q /s build
del lib\\zipfile.node
del lib\\libzip.dll
npm install
@rem copy build\Release\libzip.dll lib
rem test!
@rem set NODE_PATH=lib
npm test

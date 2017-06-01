@ECHO OFF
SETLOCAL
SET EL=0

ECHO =========== %~f0 ===========

IF /I "%APPVEYOR%"=="True" powershell Install-Product node %nodejs_version% %PLATFORM%
IF %ERRORLEVEL% NEQ 0 ECHO could not install requested node version && GOTO ERROR

:: use 64 bit python if platform is 64 bit
IF /I "%PLATFORM%"=="x64" set PATH=C:\Python27-x64;%PATH%

:: node 0.10: workaround 'win_delay_load_hook.cc: redefinition' error by updating node-gyp
:: https://github.com/mapbox/node-pre-gyp/issues/209
:: https://github.com/nodejs/node-gyp/issues/972#issuecomment-229935374
IF /I "%PLATFORM%"=="x86" IF /I "%nodejs_version:~0,1%"=="0" (GOTO UPDATE_NPM) ELSE (GOTO AFTER_UPDATE_NPM)
:UPDATE_NPM
ECHO updating npm
CALL npm i npm@2 -g
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

:AFTER_UPDATE_NPM


IF /I "%PLATFORM%"=="x86" IF /I "%nodejs_version:~0,1%"=="4" (GOTO SET_CA_FILE) ELSE (GOTO AFTER_CA_FILE_FIX)
:SET_CA_FILE
ECHO workaround node 4.x x86 bug by setting 'cafile' and 'strict-ssl'
:: HACK!! to make node@4.x x86 builds work
:: see: https://github.com/mapbox/node-pre-gyp/issues/209#issuecomment-217690537
:: be careful when doing this locally as it might have unwanted side effects
CALL npm config set -g cafile=package.json
CALL npm config set -g strict-ssl=false

:AFTER_CA_FILE_FIX


ECHO activating VS command prompt...
IF /I %platform% == x64 CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
IF /I %platform% == x86 CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86

ECHO available node.exe^:
where node
node -v
node -e "console.log(process.argv,process.execPath)"
ECHO available npm^:
where npm
CALL npm -v

ECHO building node-zipfile
:: --msvs_version=2015 is passed along to node-gyp here
CALL npm install --build-from-source --msvs_version=2015 --loglevel=http --node_shared=true
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

CALL npm test
:: comment next line to allow build to work even if tests do not pass
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

ECHO APPVEYOR_REPO_COMMIT_MESSAGE^: %APPVEYOR_REPO_COMMIT_MESSAGE%
SET CM=%APPVEYOR_REPO_COMMIT_MESSAGE%

:: publish binaries to v140 path
IF NOT "%CM%"=="%CM:[publish binary]=%" (CALL node_modules\.bin\node-pre-gyp package publish --toolset=v140) ELSE (ECHO not publishing)
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
IF NOT "%CM%"=="%CM:[republish binary]=%" (CALL node_modules\.bin\node-pre-gyp package unpublish publish --toolset=v140) ELSE (ECHO not republishing)
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

ECHO packaging ...
CALL node_modules\.bin\node-pre-gyp package
IF %ERRORLEVEL% NEQ 0 ECHO error during packaging && GOTO ERROR

:: publish binaries to default path
:: in the future this may change depending on:
:: 1) what visual studio versions we support
:: 2) how visual studio binaries are or are not compatible with each other
:: more details: https://github.com/mapnik/node-mapnik/issues/756
IF NOT "%CM%" == "%CM:[publish binary]=%" (ECHO publishing... && CALL node_modules\.bin\node-pre-gyp publish) ELSE (ECHO not publishing)
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
IF NOT "%CM%" == "%CM:[republish binary]=%" (ECHO republishing ... && CALL node_modules\.bin\node-pre-gyp unpublish publish) ELSE (ECHO not republishing)
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

GOTO DONE

:ERROR
ECHO =========== ERROR %~f0 ===========
ECHO ERRORLEVEL^: %ERRORLEVEL%
SET EL=%ERRORLEVEL%

:DONE
ECHO =========== DONE %~f0 ===========

EXIT /b %EL%
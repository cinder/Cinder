@echo off
@setlocal

rem prevent an error only present on HP computers
set Platform=
set platformcode=

:permissions
rem check admin permissions
net session >nul 2>&1
if not errorlevel 1 goto environment
echo Run this script as administrator (right-click the file, then select the appropriate option).
goto done 

:environment
echo Preparing environment...
setlocal enableextensions
cd /d "%~dp0"
rem enable Visual Studio 2015 environment
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64 8.1
echo Done.
goto submodules

:submodules
echo Updating Git submodules...
git submodule update --init --recursive
if errorlevel 1 goto error
echo Done.
goto application

:application
echo Compiling 32-bit Debug target...
msbuild ".\cinder.sln" /m /p:Configuration=Debug /p:Platform=Win32
if errorlevel 1 goto error
echo Done.
echo Compiling 32-bit Release target...
msbuild ".\cinder.sln" /m /p:Configuration=Release /p:Platform=Win32
if errorlevel 1 goto error
echo Done.
echo Compiling 64-bit Debug target...
msbuild ".\cinder.sln" /m /p:Configuration=Debug /p:Platform=x64
if errorlevel 1 goto error
echo Done.
echo Compiling 64-bit Release target...
msbuild ".\cinder.sln" /m /p:Configuration=Release /p:Platform=x64
if errorlevel 1 goto error
echo Done.
goto done

:error
echo An error occurred. See README.md for more information.

:done
echo.
pause
@echo off
setlocal enableextensions
set me=%~n0
set parent=%~dp0

rmdir .\tmp /q /s
mkdir .\tmp

if not exist C:\msys64 ( 
	echo %me%: Didn't find msys64. You must download msys64 to continue. Exiting!
	exit
)

set PATH=%PATH%;C:\msys64\usr\bin
echo %me%: set msys64 into path.

if not exist C:\msys64\usr\bin\curl.exe pacman -S curl

if not exist C:\msys64\usr\bin\tar.exe pacman -S tar

if not exist C:\msys64\usr\bin\make.exe pacman -S make

set cairo_final_lib_path="%cd%\..\lib\msw\"
rmdir %cairo_final_lib_path% /q /s
mkdir %cairo_final_lib_path%
echo Cairo final path: %cairo_final_lib_path%

set cairo_final_include_path="%cd%\..\include\msw\cairo"
rmdir %cairo_final_include_path% /q /s
mkdir %cairo_final_include_path%
set pixman_final_include_path="%cd%\..\include\msw\pixman"
rmdir %pixman_final_include_path% /q /s
mkdir %pixman_final_include_path%
set libpng_final_include_path="%cd%\..\include\msw\libpng"
rmdir %libpng_final_include_path% /q /s
mkdir %libpng_final_include_path%

cd tmp

echo %me%: Downloading libs...

curl http://zlib.net/zlib-1.2.8.tar.gz -o zlib.tar.gz
tar -xf zlib.tar.gz
mv zlib-* zlib
rm zlib.tar.gz

curl ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/libpng-1.6.25.tar.gz -o libpng.tar.gz
tar -xf libpng.tar.gz
mv libpng-* libpng 
rm libpng.tar.gz

curl https://www.cairographics.org/releases/pixman-0.34.0.tar.gz -o pixman.tar.gz
tar -xf pixman.tar.gz
mv pixman-* pixman 
rm pixman.tar.gz

curl https://www.cairographics.org/releases/cairo-1.14.6.tar.xz -o cairo.tar.xz
tar -xf cairo.tar.xz
mv cairo-* cairo 
rm cairo.tar.xz

echo %me%: Finished downloading. Preparing to build...

set ROOTDIR=%cd%
echo "%me%: setting working directory. %ROOTDIR%"

rmdir "%ROOTDIR%\zlib\projects\visualc71" /q /s
mkdir "%ROOTDIR%\zlib\projects\visualc71"
cd "%ROOTDIR%\zlib\projects\visualc71"
copy "%ROOTDIR%\libpng\projects\visualc71\zlib.vcproj" .

echo Building zlib...
start /w devenv zlib.vcproj /upgrade 
sed /RuntimeLibrary=/s/2/0/ zlib.vcproj > fixed.vcproj 
move /Y fixed.vcproj zlib.vcproj
sed 's/Win32/x64/g' zlib.vcxproj > fixed.vcxproj
move /Y fixed.vcxproj zlib.vcxproj
msbuild zlib.vcxproj /p:PlatformToolset=v140 /p:Configuration="LIB Release" /t:Build 

move /Y x64_LIB_Release\Zlib\zlib.lib "%ROOTDIR%\zlib"

echo Building libpng...
cd "%ROOTDIR%\libpng\projects\visualc71"
for %%i in (*) do if not %%i == libpng.vcproj del %%i
start /w devenv libpng.vcproj /upgrade 
sed /RuntimeLibrary=/s/2/0/ libpng.vcproj > fixed.vcproj
move /Y fixed.vcproj libpng.vcproj
sed 's/Win32/x64/g' libpng.vcxproj > fixed.vcxproj
move /Y fixed.vcxproj libpng.vcxproj
sed 's/Win32/x64/g' libpng.sln > fixed.sln
move /Y fixed.sln libpng.sln
sed 's/x86/x64/g' libpng.sln > fixed.sln
move /Y fixed.sln libpng.sln
msbuild libpng.vcxproj /p:PlatformToolset=v140 /p:Configuration="LIB Release" /t:Build 

move /Y x64_LIB_Release\libpng.lib "%ROOTDIR%\libpng"

echo Building Pixman...
cd %ROOTDIR%\pixman\pixman
sed s/-MD/-MT/ Makefile.win32 > Makefile.fixed
move /Y Makefile.fixed Makefile.win32
make -f Makefile.win32 "CFG=release" "MMX=off"

set INCLUDE=%INCLUDE%;"%ROOTDIR%\zlib"
set INCLUDE=%INCLUDE%;"%ROOTDIR%\libpng"
set INCLUDE=%INCLUDE%;"%ROOTDIR%\pixman\pixman"
set INCLUDE=%INCLUDE%;"%ROOTDIR%\cairo\boilerplate"
set INCLUDE=%INCLUDE%;"%ROOTDIR%\cairo\src"

set LIB=%LIB%;"%ROOTDIR%\zlib\projects\visualc71\x64_LIB_Release\Zlib"
set LIB=%LIB%;"%ROOTDIR%\libpng\projects\visualc71\x64_LIB_Release"

echo Building Cairo...
cd %ROOTDIR%\cairo
sed s/-MD/-MT/ build\Makefile.win32.common > build\Makefile.fixed
move /Y build\Makefile.fixed build\Makefile.win32.common
sed s/zdll.lib/zlib.lib/ build\Makefile.win32.common > build\Makefile.fixed
move /Y build\Makefile.fixed build\Makefile.win32.common

make -f Makefile.win32 "CFG=release"

move /Y "%ROOTDIR%\libpng\libpng.lib" %cairo_final_lib_path%
cd "%ROOTDIR%\libpng\"
for %%I in (pnglibconf.h pngconf.h png.h) do copy %%I %libpng_final_include_path%

move /Y "%ROOTDIR%\pixman\pixman\release\pixman-1.lib" %cairo_final_lib_path%
cd "%ROOTDIR%\pixman\pixman\"
for %%I in (pixman.h pixman-version.h) do copy %%I %pixman_final_include_path%
   
move /Y "%ROOTDIR%\cairo\src\release\cairo-static.lib" %cairo_final_lib_path%
cd "%ROOTDIR%\cairo\"
copy cairo-version.h %cairo_final_include_path%
cd src\
for %%I in (cairo-features.h cairo.h cairo-deprecated.h cairo-win32.h cairo-script.h cairo-ps.h cairo-pdf.h cairo-svg.h cairo-pdf.h) do copy %%I %cairo_final_include_path%
cd "%ROOTDIR%\.."
rmdir tmp\ /q /s      

set VcVer=%1
if "%VcVer%"=="" set VcVer=vc141

pushd bld_lib_bat
call bld_zlib.bat   %VcVer% static release
call bld_libpng.bat %VcVer% static release
call bld_pixman.bat %VcVer% static release
call bld_cairo.bat  %VcVer% static release
popd

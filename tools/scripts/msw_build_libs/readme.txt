Build libpng.lib, pixman-1.lib, cairo-static.lib for vc2017(vc2015)

1. Install mozilla-build, c:\mozilla-build is possible.
   (https://wiki.mozilla.org/MozillaBuild)

2. In bld_lib_bat/libs_config.bat  
     set CcWinGnuMake=C:\mozilla-build\bin\mozmake.exe  
     set CcMsys1Paths=C:\mozilla-build\msys\local\bin;C:\mozilla-build\msys\bin  
     
   Adjust to your environment. Required for building pixman, cairo.  

3. Make git available on the command line.

4. Run download.bat.

5. Run build.bat.  
   for_vc2015> build vc140

6. Run copy_to.bat.  
   for_vc2015> copy_to vc140

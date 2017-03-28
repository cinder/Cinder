import os
import sys
import shutil
import re

global gCompiler
global gPlatform

def copyIgnore( path, names ):
    result = []
    if re.search( "__AppTemplates", path ) != None:
        return result
    for name in names:
        if name == 'vc2013' and gCompiler != 'vc2013':
            result.append( name )
        if name == 'vc2015_winrt' and gCompiler != 'vc2013':
            result.append( name )
        elif name == 'vc2015' and gCompiler != 'vc2015':
            result.append( name )
        elif name == 'ios' and gCompiler != 'xcode':
            result.append( name )
        elif name == 'ios-sim' and gCompiler != 'xcode':
            result.append( name )
        elif name == 'macosx' and gCompiler != 'xcode':
            result.append( name )
        elif name == 'xcode' and gCompiler != 'xcode':
            result.append( name )
        elif name == 'xcode_ios' and gCompiler != 'xcode':
            result.append( name )
        elif name == 'msw' and gCompiler == 'xcode':
            result.append( name )
        elif name == 'msw-uwp' and gCompiler == 'xcode':
            result.append( name )
        elif gPlatform == 'mac' and name == 'TinderBox-Win':
            result.append( name )
        elif gPlatform == 'msw' and name == 'TinderBox-Mac':
            result.append( name )
        elif name == 'Readme.md':
            result.append( name )
        elif name == 'scripts':
            result.append( name )
        elif name == 'linux' and re.search( "lib", path ) != None:
            result.append( name )
        elif name == 'android':
            result.append( name )
        elif name == 'androidstudio':
            result.append( name )
        elif re.search( "libboost.*-vc120.*", name ) != None and gCompiler != 'vc2013':
            result.append( name )
        elif re.search( "^.git.*", name ) != None:
            result.append( name )
    return result

def printUsage():
    print "Run from the root of the repository (having run vcvars):"
    print "python tools/packageRelease.py (version number) (xcode|vc2013|vc2015)"

def processExport( outputName, compilerName, version ):
    print "creating a clean clone of cinder"
    baseDir = os.getcwd()
    fileUrl = "file://" + baseDir
    fileUrl.replace( os.sep, "/" )
    os.system( "git clone --recursive --depth 1 -b master " + fileUrl + " .." + os.sep + "cinder_temp" )
    os.chdir( baseDir + os.sep + ".." + os.sep + "cinder_temp" )
    outputDir = baseDir + os.sep + ".." + os.sep + "cinder_" + version + "_" + outputName + os.sep
    print "performing selective copy to " + outputDir
    shutil.copytree( ".", outputDir, ignore=copyIgnore )
    print "removing test"
    shutil.rmtree( outputDir + "test" )
    return outputDir
    
def cleanupMswLibDir( dir ):
    for f in os.listdir( dir ):
        if os.path.splitext( f ) == ".idb":
            os.remove( os.path.join(dir, f) )
    
if len(sys.argv) != 3:
    printUsage()
elif sys.argv[2] == 'xcode':
    gCompiler = 'xcode'
    gPlatform = 'mac'
    outputDir = processExport( "mac", "xcode", sys.argv[1] )
    os.chdir( outputDir + "proj/xcode" )
    os.system( "./fullbuild.sh" )
    shutil.rmtree( outputDir + "proj/xcode/build" )
    # strip debug symbols
    os.chdir( outputDir + "lib/ios/Debug" )
    os.system( "strip -S -r libcinder.a" )
    os.chdir( outputDir + "lib/ios/Release" )
    os.system( "strip -S -r libcinder.a" )
    os.chdir( outputDir + "lib/ios-sim/Debug" )
    os.system( "strip -S -r libcinder.a" )
    os.chdir( outputDir + "lib/ios-sim/Release" )
    os.system( "strip -S -r libcinder.a" )
    os.chdir( outputDir + "lib/macosx/Debug" )
    os.system( "strip -S -r libcinder.a" )
    os.chdir( outputDir + "lib/macosx/Release" )
    os.system( "strip -S -r libcinder.a" )
elif sys.argv[2] == 'vc2013':
    gCompiler = 'vc2013'
    gPlatform = 'msw'
    outputDir = processExport( "vc2013", "vc2013", sys.argv[1] )
    os.chdir( outputDir + "proj\\vc2013" )
    os.system( "msbuild cinder.vcxproj /p:platform=win32 /p:configuration=debug" )
    os.system( "msbuild cinder.vcxproj /p:platform=win32 /p:configuration=release" )
    shutil.rmtree( outputDir + "proj\\vc2013\\build" )
#    os.system( "msbuild cinder.vcxproj /p:platform=x64 /p:configuration=debug" )
#    shutil.rmtree( outputDir + "vc2013\\x64\\Debug" )
#    os.system( "msbuild cinder.vcxproj /p:platform=x64 /p:configuration=release" )
#    shutil.rmtree( outputDir + "vc2013\\x64\\Release" )
    cleanupMswLibDir( outputDir + "lib\\msw\\x86\\debug" )
#    cleanupMswLibDir( outputDir + "lib\\msw\\x64" )
elif sys.argv[2] == 'vc2015':
    gCompiler = 'vc2015'
    gPlatform = 'msw'
    outputDir = processExport( "vc2015", "vc2015", sys.argv[1] )
    os.chdir( outputDir + "vc2013" )
    os.system( "msbuild cinder.vcxproj /p:platform=win32 /p:configuration=debug" )
    shutil.rmtree( outputDir + "vc2013\\Debug" )
    os.system( "msbuild cinder.vcxproj /p:platform=win32 /p:configuration=release" )
    shutil.rmtree( outputDir + "vc2013\\Release" )
    os.system( "msbuild cinder.vcxproj /p:platform=x64 /p:configuration=debug" )
    shutil.rmtree( outputDir + "vc2013\\x64\\Debug" )
    os.system( "msbuild cinder.vcxproj /p:platform=x64 /p:configuration=release" )
    shutil.rmtree( outputDir + "vc2013\\x64\\Release" )
    cleanupMswLibDir( outputDir + "lib\\msw\\x86" )
    cleanupMswLibDir( outputDir + "lib\\msw\\x64" )
else:
    printUsage()

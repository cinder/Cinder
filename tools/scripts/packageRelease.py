import os
import sys
import shutil
import re

global gCompiler
global gPlatform

def copyIgnore( path, names ):
    result = []
    for name in names:
        if name == 'vc2012' and gCompiler != 'vc2012':
            result.append( name )
        if name == 'vc2013' and gCompiler != 'vc2013':
            result.append( name )
        elif name == 'ios' and gCompiler != 'xcode':
            result.append( name )
        elif name == 'ios-sim' and gCompiler != 'xcode':
            result.append( name )
        elif name == 'macosx' and gCompiler != 'xcode':
            result.append( name )
        elif name == 'xcode' and gCompiler != 'xcode':
            result.append( name )
        elif name == 'msw' and gCompiler == 'xcode':
            result.append( name )
        elif gPlatform == 'mac' and name == 'TinderBox-Win':
            result.append( name )
        elif gPlatform == 'msw' and name == 'TinderBox-Mac':
            result.append( name )
        elif name == 'Readme.md':
            result.append( name )
        elif name == 'scripts':
            result.append( name )
        elif re.search( "libboost.*-vc110.*", name ) != None and gCompiler != 'vc2012':
            result.append( name )
        elif re.search( "libboost.*-vc120.*", name ) != None and gCompiler != 'vc2013':
            result.append( name )
        elif re.search( ".git.*", name ) != None:
            result.append( name )
    return result

def printUsage():
    print "Run from the root of the repository (having run vcvars):"
    print "python tools/packageRelease.py (version number) (xcode|vc2012|vc2013)"

def processExport( outputName, compilerName, version, doxygenPath ):
    print "creating a clean clone of cinder"
    baseDir = os.getcwd()
    os.system( "git clone --recursive --depth 1 -b dev . ../cinder_temp/" )
    print "creating a clean clone of Cinder-OpenCV"
    os.system( "git clone --recursive --depth 1 -b dev ./blocks/OpenCV ../cinder_temp/blocks/OpenCV/" )
    os.chdir( baseDir + os.sep + ".." + os.sep + "cinder_temp" )
    outputDir = baseDir + os.sep + ".." + os.sep + "cinder_" + version + "_" + outputName + os.sep
    print "performing selective copy to " + outputDir
    shutil.copytree( ".", outputDir, ignore=copyIgnore )
    os.chdir( outputDir + "docs" + os.sep + "doxygen" )
    print "generating cinder docs " + outputDir
    os.system( doxygenPath + " Doxyfile" )
    print "generating Cinder-OpenCV docs"
    os.chdir( outputDir + "blocks" + os.sep + "OpenCV" + os.sep + "docs" + os.sep + "doxygen" )
    os.system( doxygenPath + " Doxyfile" )
#    os.remove( outputDir + "docs" + os.sep + "doxygen" + os.sep + "cinder.tag" )
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
    outputDir = processExport( "mac", "xcode", sys.argv[1], "/Applications/Doxygen.app/Contents/Resources/doxygen" )
    os.chdir( outputDir + "xcode" )
    os.system( "./fullbuild.sh" )
    shutil.rmtree( outputDir + "xcode/build" )
    os.chdir( outputDir + "lib" )
    # strip debug symbols
    os.system( "strip -S -r *.a" )
elif sys.argv[2] == 'vc2012':
    gCompiler = 'vc2012'
    gPlatform = 'msw'
    outputDir = processExport( "vc2012", "vc2012", sys.argv[1], "doxygen" )
    os.chdir( outputDir + "vc2012" )
    os.system( "msbuild cinder.vcxproj /p:platform=win32 /p:configuration=debug" )
    shutil.rmtree( outputDir + "vc2012\\Debug" )
    os.system( "msbuild cinder.vcxproj /p:platform=win32 /p:configuration=release" )
    shutil.rmtree( outputDir + "vc2012\\Release" )
    #os.system( "msbuild cinder.vcxproj /p:platform=x64 /p:configuration=debug" )
    #shutil.rmtree( outputDir + "vc2012\\x64\\Debug" )
    #os.system( "msbuild cinder.vcxproj /p:platform=x64 /p:configuration=release" )
    #shutil.rmtree( outputDir + "vc2012\\x64\\Release" )
    cleanupMswLibDir( outputDir + "lib\\msw\\x86" )
    #cleanupMswLibDir( outputDir + "lib\\msw\\x64" )
elif sys.argv[2] == 'vc2013':
    gCompiler = 'vc2013'
    gPlatform = 'msw'
    outputDir = processExport( "vc2013", "vc2013", sys.argv[1], "doxygen" )
    os.chdir( outputDir + "vc2013" )
    os.system( "msbuild cinder.vcxproj /p:platform=win32 /p:configuration=debug" )
    shutil.rmtree( outputDir + "vc2013\\Debug" )
    os.system( "msbuild cinder.vcxproj /p:platform=win32 /p:configuration=release" )
    shutil.rmtree( outputDir + "vc2013\\Release" )
    #os.system( "msbuild cinder.vcxproj /p:platform=x64 /p:configuration=debug" )
    #shutil.rmtree( outputDir + "vc2013\\x64\\Debug" )
    #os.system( "msbuild cinder.vcxproj /p:platform=x64 /p:configuration=release" )
    #shutil.rmtree( outputDir + "vc2013\\x64\\Release" )
    cleanupMswLibDir( outputDir + "lib\\msw\\x86" )
    #cleanupMswLibDir( outputDir + "lib\\msw\\x64" )
else:
    printUsage()

import os
import sys
import shutil
import re

global gCompiler
global gPlatform

def copyIgnore( path, names ):
    result = []
    for name in names:
        if name == 'vc10' and gCompiler != 'vc10':
            result.append( name )
        if name == 'vc11' and gCompiler != 'vc11':
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
        elif re.search( "libboost.*-vc100.*", name ) != None and gCompiler != 'vc10':
            result.append( name )
        elif re.search( "libboost.*-vc110.*", name ) != None and gCompiler != 'vc11':
            result.append( name )
        elif gPlatform == 'mac' and name == 'TinderBox-Win':
        	result.append( name )
        elif gPlatform == 'msw' and name == 'TinderBox-Mac':
        	result.append( name )
        elif name == 'boost':
            result.append( name )
        elif name == 'Readme.md':
        	result.append( name )
        elif name == 'scripts':
        	result.append( name )
        elif re.search( ".git.*", name ) != None:
            result.append( name )
    return result

def printUsage():
    print "Run from the root of the repository (having run vcvars):"
    print "python tools/packageRelease.py (version number) (xcode|vc10|vc11)"

def processExport( outputName, compilerName, version, doxygenPath ):
    print "creating a clean clone of cinder"
    baseDir = os.getcwd()
    os.system( "git checkout-index -a -f --prefix=../cinder_temp/" )
    print "creating a clean clone of Cinder-OpenCV"
    os.chdir( baseDir + os.sep + "blocks" + os.sep + "OpenCV" )
    os.system( "git checkout-index -a -f --prefix=../../../cinder_temp/blocks/OpenCV/" )
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
    print "copying boost headers"
    shutil.copytree( baseDir + os.sep + "boost" + os.sep + "boost", outputDir + "boost" + os.sep + "boost" )
    print "copying TinderBox"
    if gPlatform == 'msw':
        shutil.copytree( baseDir + "\\tools\\TinderBox-Win", outputDir + "\\tools\\TinderBox-Win" )
    else:
        shutil.copytree( baseDir + "/tools/TinderBox-Mac", outputDir + "/tools/TinderBox-Mac" )
    return outputDir
    

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
elif sys.argv[2] == 'vc10':
    gCompiler = 'vc10'
    gPlatform = 'msw'
    processExport( "vc2010", "vc10", sys.argv[1], "doxygen" )
elif sys.argv[2] == 'vc11':
    gCompiler = 'vc11'
    gPlatform = 'msw'
    processExport( "vc2012", "vc11", sys.argv[1], "doxygen" )
else:
    printUsage()

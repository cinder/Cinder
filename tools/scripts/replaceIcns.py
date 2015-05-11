# expects 2 args
# python replaceIcns.py <path> <icon absolute path string>
import os
import fileinput
import sys

from os.path import join, getsize

icnsAbsolutePath = os.path.abspath( sys.argv[2] )
print "Replacing icons to be relative to " + icnsAbsolutePath

for root, dirs, files in os.walk(sys.argv[1]):
    for name in files:
        if name[name.rfind("."):] != ".pbxproj" or join(root, name).find( "/xcode" ) == -1:
            continue
        f = open( join(root, name), "r" )
        fileLines = f.readlines(10000000)
        f.close()
        f = open( join(root, name), "w" )
        f.seek( 0, os.SEEK_SET )
        icnsRelativePath = os.path.relpath( icnsAbsolutePath, join( root, ".." ) )
        for line in fileLines:
            exists = line.find( "name = CinderApp.icns; path = " );
            if exists != -1:
                start = line.find( "..", exists )
                end = line.find( ";", start )
                existingPath = line[start:end]
                if existingPath.find( "data" ) == -1:
                    print "Found string " + existingPath
                    line = line.replace( existingPath, icnsRelativePath )
                    existingAbsolutePath = join( join( root, ".." ), existingPath )
                    if os.path.exists( existingAbsolutePath ):
                        print "Deleting " + existingAbsolutePath
                        os.unlink( existingAbsolutePath )
            f.write( line )
 
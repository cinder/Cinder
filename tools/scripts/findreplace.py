# expects 3 args
# python findreplace.py <path> <find string> <replace string>
import os
import fileinput
import sys

from os.path import join, getsize

print "Replacing " + sys.argv[2] + " with " + sys.argv[3]

for root, dirs, files in os.walk(sys.argv[1]):
    for name in files:
        if name[name.rfind("."):] != ".pbxproj" or join(root, name).find( "/xcode" ) == -1:
            continue
        f = open( join(root, name), "r" )
        fileLines = f.readlines(10000000)
        f.close()
        f = open( join(root, name), "w" )
        f.seek( 0, os.SEEK_SET )
        for line in fileLines:
            replaced = line.replace( sys.argv[2], sys.argv[3] )
            if replaced != line:
                print "Replaced line in " + str(join(root,name))
                print replaced
            f.write( replaced )
    if '.svn' in dirs:
        dirs.remove('.svn')  # don't visit CVS directories

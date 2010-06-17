# expects 2 args
# python findreplace.py <path> <find string>
# example:  python deleteline.py . "some text a match with which will delete the whole line"
import os
import fileinput
import sys

from os.path import join, getsize

print "Deleting " + sys.argv[2]

for root, dirs, files in os.walk(sys.argv[1]):
    for name in files:
#        if name[name.rfind("."):] != ".vcproj":
#           continue
        f = open( join(root, name), "r" )
        fileLines = f.readlines(10000000)
        f.close()
        f = open( join(root, name), "w" )
        f.seek( 0, os.SEEK_SET )
        for line in fileLines:
            if line.find( sys.argv[2] ) != -1:
                print "Deleting line in " + str(join(root,name))
                print line
            else:
                f.write( line )
        f.close()
    if '.svn' in dirs:
        dirs.remove('.svn')  # don't visit CVS directories

import os
import fileinput
import sys

from os.path import join, getsize

licenseFile = open( "COPYING" )
licenseLines = licenseFile.readlines( 100000 )

total = 0
for root, dirs, files in os.walk(sys.argv[1]):
    for name in files:
        f = open( join(root, name), "r+" )
        fileLines = f.readlines(100000)
        total += len( fileLines )
        f.seek( 0, os.SEEK_SET )
        for line in licenseLines:
            f.write( line )
        for line in fileLines:
            f.write( line )
    if '.svn' in dirs:
        dirs.remove('.svn')  # don't visit CVS directories

print "Total lines: " + str(total)
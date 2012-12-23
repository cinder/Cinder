# expects 3 args
# python findreplace.py <path> <find string> <line to insert before>
import os
import fileinput
import sys

from os.path import join, getsize

print "Inserting " + sys.argv[3] + " before any line containing " + sys.argv[2]

for root, dirs, files in os.walk(sys.argv[1]):
	for name in files:
		if os.path.splitext( name )[1] != ".pbxproj":
			continue;
		f = open( join(root, name), "r" )
		fileLines = f.readlines(10000000)
		f.close()
		alreadyFound = False
		for line in fileLines:
			if line.find( sys.argv[3] ) != -1:
				alreadyFound = True
				print "Already found in " + str(join(root,name))
		if not alreadyFound:
			f = open( join(root, name), "w" )
			f.seek( 0, os.SEEK_SET )
			for line in fileLines:
				if line.find( sys.argv[2] ) != -1:
					print "Inserting before line " + str(join(root,name))
					print line
					f.write( sys.argv[3] + os.linesep )
				f.write( line )				
		f.close()
	if '.svn' in dirs:
		dirs.remove('.svn')	 # don't visit CVS directories

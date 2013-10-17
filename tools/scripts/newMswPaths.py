import os
import fileinput
import sys
import shutil
import xml.dom.minidom
import xml.sax.saxutils
import codecs

from os.path import join, getsize

if len(sys.argv) != 3:
	print "Usage:\npython newMswPaths.py [path] [newLibPath]\n	Ex: python setvc11Compiler.py samples \"..\\..\\..\\lib\\msw\\$(PlatformTarget)\""
	sys.exit(-1)

for root, dirs, files in os.walk(sys.argv[1]):
	for name in dirs:
		if name == 'vc2012':
			print join( root, name )
			files = os.listdir( join( root, "vc2012" ) )
			for file in files:
				filePath = join( join( root, "vc2012" ), file )
				if os.path.splitext( filePath )[1] == '.vcxproj':
					print "processing: " + filePath
					dom = xml.dom.minidom.parse( filePath )
					for node in dom.getElementsByTagName( 'AdditionalDependencies' ):
						val = node.firstChild.wholeText
						val = val.replace( "cinder_d.lib", "cinder-$(PlatformToolset)_d.lib" )
						val = val.replace( "cinder.lib", "cinder-$(PlatformToolset).lib" )
						node.firstChild.data = val
						print node.firstChild.data
					os.remove( filePath )
					f = codecs.open( filePath, 'w', 'utf-8-sig' )
					dom.writexml( f, encoding='utf-8' )
					f.close()
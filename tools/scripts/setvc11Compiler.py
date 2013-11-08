import os
import fileinput
import sys
import shutil
import xml.dom.minidom
import codecs

from os.path import join, getsize

if len(sys.argv) != 3:
	print "Usage:\npython setvc11Compiler.py [path] [compiler]\n  Ex: python setvc11Compiler.py samples v110_xp"
	sys.exit(-1)

for root, dirs, files in os.walk(sys.argv[1]):
    for name in dirs:
        if name == 'vc11':
            print join( root, name )
            files = os.listdir( join( root, "vc2012" ) )
            for file in files:
                filePath = join( join( root, "vc2012" ), file )
                if os.path.splitext( filePath )[1] == '.vcxproj':
                    print "processing: " + filePath
                    dom = xml.dom.minidom.parse( filePath )
                    for node in dom.getElementsByTagName( 'PropertyGroup' ):
                        if node.hasAttribute( 'Label' ) and (node.attributes['Label'].value == 'Configuration'):
                           x = dom.createElement( 'PlatformToolset' )
                           val = dom.createTextNode( 'v110' )
                           x.appendChild( val )
                           node.appendChild( x )
                    os.remove( filePath )
                    f = codecs.open( filePath, 'w', 'utf-8-sig' )
                    dom.writexml( f, encoding='utf-8' )
                    f.close()
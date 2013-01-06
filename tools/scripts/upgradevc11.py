import os
import fileinput
import sys
import shutil
import xml.dom.minidom
import codecs

from os.path import join, getsize

for root, dirs, files in os.walk(sys.argv[1]):
    for name in dirs:
        if name == 'vc10':
            print join( root, name )
            shutil.copytree( join( root, name ), join( root, "vc11" ) )
            files = os.listdir( join( root, "vc11" ) )
            for file in files:
                filePath = join( join( root, "vc11" ), file )
                if os.path.splitext( filePath )[1] == '.vcxproj':
                    print "processing: " + filePath
                    dom = xml.dom.minidom.parse( filePath )
                    for node in dom.getElementsByTagName( 'PropertyGroup' ):
                        if node.hasAttribute( 'Label' ) and (node.attributes['Label'].value == 'Configuration'):
                           x = dom.createElement( 'PlatformToolset' )
                           val = dom.createTextNode( 'v110_xp' )
                           x.appendChild( val )
                           node.appendChild( x )
                    os.remove( filePath )
                    f = codecs.open( filePath, 'w', 'utf-8-sig' )
                    dom.writexml( f, encoding='utf-8' )
                    f.close()
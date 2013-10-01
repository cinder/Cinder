import os
import fileinput
import sys
import shutil
import xml.dom.minidom
import codecs

from os.path import join, getsize

for root, dirs, files in os.walk(sys.argv[1]):
    for name in dirs:
        if name == 'vc11':
            s = ( 'git mv "' + join( root, name ) + '" "' + join( root, "vc2012" ) ) + '"'
            print s
            os.system( s )

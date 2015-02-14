#! /usr/bin/python
import sys
import os
import codecs
import shutil
from xml.dom.minidom import parse
from bs4 import BeautifulSoup

def getText(nodelist):
	rc = []
	for node in nodelist:
		if node.nodeType == node.TEXT_NODE:
			rc.append(node.data)
	return ''.join(rc)

def findCompoundName( tagDom ):
#	root = tagDom.getElementsByTagName( "doxygen" )[0]
	for compoundDef in tagDom.getElementsByTagName( "compounddef" ):
		for compoundName in compoundDef.getElementsByTagName( "compoundname" ):
			print compoundName.toxml()
			return getText( compoundName.childNodes )

def processXmlFile( inPath, outPath ):
	tagDom = parse( inPath )
	soup = BeautifulSoup()
	htmlTag = soup.new_tag( "html", xmlns="http://www.w3.org/1999/xhtml" )
	soup.append( htmlTag )
	headTag = soup.new_tag( "head" )
	htmlTag.insert( 0, headTag )
	titleTag = soup.new_tag( "title" )
	titleTag.append( "Cinder: " + str(findCompoundName( tagDom )) )
	headTag.insert( 0, titleTag )

	# write the file	
	outFile = codecs.open( outPath, "w", "UTF-8" )
	outFile.write( soup.prettify() )


if __name__ == "__main__":
	doxygenHtmlPath = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'test' + os.sep
	htmlSourcePath = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'htmlsrc' + os.sep

	if len( sys.argv ) == 3: # process a specific file
		processXmlFile( sys.argv[1], os.path.join( doxygenHtmlPath, sys.argv[2] ) )
	else:
		print "Unknown usage"
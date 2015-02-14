#! /usr/bin/python
import sys
import os
import codecs
import shutil
import copy
import xml.etree.ElementTree as ET
from bs4 import BeautifulSoup

def findCompoundName( tree ):
	for compoundDef in tree.iter( "compounddef" ):
		for compoundName in tree.iter( "compoundname" ):
			print compoundName.text
			return compoundName.text

def markupFunction( fnXml, parent, bs4 ):
	li = bs4.new_tag( "li" )
	definition = bs4.new_tag( "em" )
	definition.append( fnXml.find( "definition" ).text );
	li.append( definition )
	li.append( fnXml.find( "argsstring" ).text )
	parent.append( li )
	
def processClassXmlFile( inPath, outPath, html ):
	tree = ET.parse( inPath )
	
	# add title
	headTag = html.head
	titleTag = html.new_tag( "title" )
	titleTag.append( "Cinder: " + str(findCompoundName( tree )) )
	headTag.insert( 0, titleTag )

	contentsTag = html.find( "div", "contents" )
	ulTag = html.new_tag( "ul" )
	contentsTag.append( ulTag )

	# member functions
	for memberFn in tree.findall( r'compounddef/sectiondef/memberdef[@kind="function"]' ):
		markupFunction( memberFn, ulTag, html )

	# write the file	
	outFile = codecs.open( outPath, "w", "UTF-8" )
	outFile.write( html.prettify() )

def parseTemplateHtml( templatePath ):
	file = codecs.open( templatePath, "r", "UTF-8" )
	return BeautifulSoup( file )

if __name__ == "__main__":
	htmlSourcePath = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'htmlsrc' + os.sep
	doxygenHtmlPath = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'html' + os.sep

	if len( sys.argv ) == 3: # process a specific file
		templateHtml = parseTemplateHtml( os.path.join( htmlSourcePath, "cinderClassTemplate.html" ) )
		processClassXmlFile( sys.argv[1], os.path.join( doxygenHtmlPath, sys.argv[2] ), copy.deepcopy( templateHtml ) )
	else:
		print "Unknown usage"
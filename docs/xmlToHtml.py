#! /usr/bin/python
import sys
import os
import codecs
import shutil
import copy
import re
import xml.etree.ElementTree as ET
from bs4 import BeautifulSoup

CLASS_PREFIX = "classcinder_1_1_"
NAMESPACE_PREFIX = "namespacecinder_1"

CLASS_HTML = "classcinder.html"
NAMESPACE_HTML = "namespacecinder.html"

def findCompoundName( tree ):
	for compoundDef in tree.iter( "compounddef" ):
		for compoundName in tree.iter( "compoundname" ):
			print compoundName.text
			return compoundName.text

def findClassDescription( className ):
	# find the xml file for that class
	# within that xml file, find the brief description
	return ""

def addClassToTag( tag, className ):
	tag["class"] = tag.get("class", []) + [className]


def genTag( bs4, tagType, classes = None, contents = None ):
	newTag = bs4.new_tag( tagType )

	if classes :
		for c in classes:
			addClassToTag( newTag, c )

	if contents:
		newTag.append( contents )

	return newTag


def markupFunction( fnXml, parent, bs4 ):

	# create new line
	# left side = return type
	# right side = function name
	# under right side = definition
	# 
	# ---------------------------------------------------
	# | returnType	| function( param1, param2, etc )   |
	# ---------------------------------------------------
	# | 			| description 						|
	# ---------------------------------------------------
	
	li = bs4.new_tag( "li" )

	# wrapper
	functionDiv = genTag( bs4, "div", ["functionDef"] )
	li.append( functionDiv )

	# left side / return type
	returnDiv = genTag( bs4, "div", ["returnCol"] )
	iterateMarkup( fnXml.find( "type/" ), returnDiv, bs4 )
	functionDiv.append( returnDiv )

	# right side (function name and description)
	definitionCol = genTag( bs4, "div", ["definitionCol"] )
	functionDiv.append( definitionCol )

	# function name
	definitionDiv = genTag( bs4, "div", ["definition"] )
	emTag = genTag( bs4, "em", [], fnXml.find( "definition" ).text )
	definitionDiv.append( emTag );
	definitionDiv.append( fnXml.find( "argsstring" ).text )
	definitionCol.append( definitionDiv );

	# detailed description
	# ET.dump(fnXml)
	descriptionDiv = markupDescription( fnXml, bs4 );
	# markupParagraph( tree.find( r'compounddef/detaileddescription/' ), descTag, html );
	# descriptionDiv = genTag( bs4, "div", ["description"], "This is the detailed description" )
	definitionCol.append( descriptionDiv )
	
	parent.append( li )


def defineLinkTag( tag, attrib, html ):

	refId = None
	kind = None

	if( "refid" in attrib ) :
		refId = attrib["refid"]
		print "REF ID: " + refId
		href = refId + ".html"

	if( "kindref" in attrib ) :
		kind = attrib["kindref"]
		print "KIND: " + kind

		if kind == "member":
			strList = refId.rsplit("_1", 1)
			href = strList[0] + ".html#" + strList[1]

	if "linkid" in attrib :
		href = "../../include/cinder/" + attrib["linkid"]

	tag["href"] = href

def defineTag( tagName, tree, html ):

	newTag = html.new_tag( tagName )
	if( tagName == "a" ):
		# print "LINK TAG: "
		defineLinkTag( newTag, tree.attrib, html )
	
	return newTag

def genIncludesTag( tree, bs4 ):

	# wrap the content in <code> tag
	# prepend with "#include"
	wrapperEl = genTag( bs4, "code", ["include"], "#include " )
	# wrap include file name in a tag
	includeLink = genTag( bs4, "a", None, tree.text )
	defineLinkTag( includeLink, {'linkid':tree.text}, bs4 )
	wrapperEl.append( includeLink )
	# return <code> tag
	return wrapperEl


def replaceTag( tree, parentTag, html ):

	tag = tree.tag
	attrib = tree.attrib
	hasParent = False

	if parentTag and parentTag.parent:
		hasParent = True

	# find html tag based on tag
	if( tag == "para" ):
		if hasParent and parentTag.parent.dl :
			tagName = "dd"
		else :
			tagName = "p"

	elif( tag == "linebreak" ):
		tagName = "br"
	elif( tag == "emphasis" ):
		tagName = "em"
	elif (tag == "ref" ):
		tagName = "a"
	elif( tag == "computeroutput" ):
		tagName = "code"
	elif( tag == "simplesect"):
		tagName = "dl"
	elif( tag == "includes" ):
		tagName = "code"
	else :
		tagName = tag

	# newTag = html.new_tag( tagName );
	newTag = defineTag( tagName, tree, html );

	if( tag == "simplesect" ) :
		seeTag = html.new_tag( "dt" )
		addClassToTag( seeTag, "section")

		if( attrib["kind"] == "see"):
			addClassToTag( seeTag, "see")
			seeTag.string = "See Also"
		newTag.append( seeTag )

	print "TAG: " + tagName
	return newTag


def iterateMarkup( tree, parent, html ):

	if tree == None :
		return
	
	origTag = tree.tag


	# print tree.attrib

	# if tree.text :
		# print "TEXT: " + tree.text

	currentTag = parent
	
	# append any new tags
	if tree.tag != None :
		htmlTag = replaceTag( tree, currentTag, html )
		currentTag = htmlTag
		parent.append( currentTag )
	
	# add content to tag
	if tree.text != None:
		currentTag.append( tree.text.strip() )
	
	# iterate through children tags
	for child in list( tree ):
		output = iterateMarkup( child, currentTag, html )

	# tail is any extra text that isn't wrapped in another tag
	# that exists before the next tag
	if tree.tail != None:
		parent.append( tree.tail.strip() )
	
	return currentTag

def markupDescription( tree, bs4 ):

	print "\n-- MARKING UP PARAGRAPH --"
	
	# description_el = iterateMarkup( paraXml, parent, html )
	# parent.append( description_el )
	# return iterateMarkup( paraXml, parent, html )

	description_el = genTag( bs4, "div", ["description"] )
	iterateMarkup( tree.find( r'briefdescription/' ), description_el, bs4 )
	iterateMarkup( tree.find( r'detaileddescription/' ), description_el, bs4 )
	return description_el
	
	
def processClassXmlFile( inPath, outPath, html ):
	print "Processing file: " + inPath + " > " + outPath;
	tree = ET.parse( inPath )
	
	# add title
	headTag = html.head
	titleTag = html.new_tag( "title" )
	titleTag.append( "Cinder: " + str(findCompoundName( tree )) )
	headTag.insert( 0, titleTag )

	# find contents wrapper
	contentsTag = html.find( "div", "contents" )

	# ----------
	#  Includes
	# ----------
	print "--INCLUDES--"
	contentsTag.append( genTag( html, "h3", None, "Includes") )
	includesUl = genTag( html, "ul" )
	for includeDef in tree.findall( r"compounddef/includes" ):
		# includesUl.append( genTag( html, "li", None, includeDef.text ) )
		includeLi = genTag( html, "li" )
		# includeContent = replaceTag( includeDef, includeLi, html )
		# includeContent.append( includeDef.text );
		includeContent = genIncludesTag( includeDef, html );
		includeLi.append( includeContent )
		includesUl.append( includeLi )
	contentsTag.append( includesUl )
	print "--END INCLUDES--"


	# +-----------+
	#  Description
	# +-----------+
	descriptionHeader = html.new_tag( "h3")
	descriptionHeader.string = "Description"
	contentsTag.append( descriptionHeader )

	# descTag = html.new_tag( "div" )
	# descTag['class'] = "description"
	# addClassToTag( descTag, "description" )


	# markupDescription( tree.find( r'compounddef/briefdescription/' ), descTag, html );
	# markupDescription( tree.find( r'compounddef/briefdescription/' ), descTag, html );
	# print tree.dump( tree.find(r'compounddef/') )
	descTag = markupDescription( tree.find( r'compounddef' ), html );
	
	
	contentsTag.append( descTag )

	# +-------+
	#  Classes
	# +-------+
	classesHeader = genTag( html, "h3", None, "Classes" )
	contentsTag.append( classesHeader )

	classesUl = genTag( html, "ul" )
	for classDef in tree.findall( r"compounddef/innerclass" ):
		classesUl.append( genTag( html, "li", None, classDef.text ) )
		# TODO: pull class brief descrption from class page xml
	contentsTag.append( classesUl )

	# +----------------+
	#  Member Functions
	# +----------------+

	# create regular and static member function ul wrappers
	ulTag = html.new_tag( "ul" )
	staticUlTag = html.new_tag( "ul" )
	addClassToTag( staticUlTag, "static" )

	funcCount = 0
	staticFuncCount = 0

	# public member functions
	for memberFn in tree.findall( r'compounddef/sectiondef/memberdef[@kind="function"][@prot="public"]' ):
		# split between static or not
		isStatic = memberFn.attrib[ "static" ]

		if isStatic == 'yes':
			staticFuncCount += 1
			markupFunction( memberFn, staticUlTag, html )
		else:
			funcCount += 1
			markupFunction( memberFn, ulTag, html )
			
	# if function count > 0 add it
	if funcCount > 0 :
		header = html.new_tag( "h3")
		header.string = "Public Member Functions"
		contentsTag.append( header )
		contentsTag.append( ulTag )

	# if static function count > 0 add it
	if staticFuncCount > 0 :
		header = html.new_tag( "h3")
		header.string = "Static Public Member Functions"
		contentsTag.append( header )
		contentsTag.append( staticUlTag )

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
#! /usr/bin/python
import sys
import os
import codecs
import shutil
import copy
import re
import xml.etree.ElementTree as ET
from bs4 import BeautifulSoup

tagDictionary = {
	"linebreak": "br",
	"emphasis": "em",
	"ref": "a",
	"computeroutput": "code",
	"includes": "code",
	"simplesect": "dl",
	"para": "p"
}


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


# Generates a new html element and optionally adds classes and content
# bs4		beautiful soup
# tagType	html tag/element (p, a, em, etc)
# classes 	array of strings that you want as classes for the element
# contents	any content that you want to populate your tag with, if known
def genTag( bs4, tagType, classes = None, contents = None ):
	newTag = bs4.new_tag( tagType )

	if classes :
		for c in classes:
			addClassToTag( newTag, c )

	if contents:
		newTag.append( contents )

	return newTag


def markupFunction( bs4, fnXml, parent, isConstructor ):

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
	
	# li = bs4.new_tag( "li" )
	li = genTag( bs4, "li", ["row"] )

	# wrapper
	functionDiv = genTag( bs4, "div", ["functionDef"] )
	li.append( functionDiv )

	# left side / return type
	if not isConstructor:
		returnDiv = genTag( bs4, "div", ["returnCol columns large-3"] )
		print "RETURN COL"
		iterateMarkup( bs4, fnXml.find( r"type" ), returnDiv  )
		functionDiv.append( returnDiv )

	# right side (function name and description)
	definitionCol = genTag( bs4, "div", ["definitionCol columns large-9"] )
	functionDiv.append( definitionCol )

	# function name
	definitionDiv = genTag( bs4, "div", ["definition"] )
	emTag = genTag( bs4, "em", [], fnXml.find( "definition" ).text )
	definitionDiv.append( emTag );
	definitionDiv.append( fnXml.find( "argsstring" ).text )
	definitionCol.append( definitionDiv );

	# detailed description
	# ET.dump(fnXml)
	descriptionDiv = markupDescription( bs4, fnXml );
	# markupParagraph( tree.find( r'compounddef/detaileddescription/' ), descTag, html );
	# descriptionDiv = genTag( bs4, "div", ["description"], "This is the detailed description" )
	definitionCol.append( descriptionDiv )
	
	parent.append( li )


def defineLinkTag( tag, attrib ):

	refId = None
	kind = None

	if( "refid" in attrib ) :
		refId = attrib["refid"]
		# print "REF ID: " + refId
		href = refId + ".html"

	if( "kindref" in attrib ) :
		kind = attrib["kindref"]
		# print "KIND: " + kind

		if kind == "member":
			strList = refId.rsplit("_1", 1)
			href = strList[0] + ".html#" + strList[1]

	if "linkid" in attrib :
		href = "../../include/cinder/" + attrib["linkid"]

	tag["href"] = href

def defineTag( bs4, tagName, tree ):

	newTag = bs4.new_tag( tagName )
	if( tagName == "a" ):
		# print "LINK TAG: "
		defineLinkTag( newTag, tree.attrib )
	
	return newTag

# generate includes tag group
def genIncludesTag( bs4, tree ):

	# wrap the content in <code> tag
	# prepend with "#include"
	wrapperEl = genTag( bs4, "code", ["include"], "#include " )
	# wrap include file name in <a> tag
	includeLink = genTag( bs4, "a", None, tree.text )
	defineLinkTag( includeLink, {'linkid':tree.text} )
	wrapperEl.append( includeLink )
	return wrapperEl


def replaceTag( bs4, tree, parentTag ):

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
			tagName = tagDictionary[tag]

	# get tag equivalent
	if tag in tagDictionary:
		tagName = tagDictionary[tag]
	else:
		# TODO: replace with nothing - no new tag
		tagName = "span"



	newTag = defineTag( bs4, tagName, tree );

	# if simplesect, construct with some content
	if( tag == "simplesect" ) :
		seeTag = bs4.new_tag( "dt" )
		addClassToTag( seeTag, "section")

		# "see also" reference
		if( attrib["kind"] == "see"):
			addClassToTag( seeTag, "see")
			seeTag.string = "See Also"
		newTag.append( seeTag )

	# print "TAG: " + tagName
	return newTag


def iterateMarkup( bs4, tree, parent ):

	if tree == None :
		return
	
	origTag = tree.tag


	# print tree.attrib

	print "TAG: " + tree.tag
	if tree.text :
		print "TEXT: " + tree.text

	currentTag = parent
	
	# append any new tags
	if tree.tag != None :
		htmlTag = replaceTag( bs4, tree, currentTag )
		currentTag = htmlTag
		parent.append( currentTag )
	
	# add content to tag
	if tree.text != None:
		currentTag.append( tree.text.strip() )
	
	# iterate through children tags
	for child in list( tree ):
		output = iterateMarkup( bs4, child, currentTag )

	# tail is any extra text that isn't wrapped in another tag
	# that exists before the next tag
	if tree.tail != None:
		parent.append( tree.tail.strip() )
	
	return currentTag

def markupDescription( bs4, tree ):

	print "\n-- MARKING UP PARAGRAPH --"
	
	# description_el = iterateMarkup( paraXml, parent, html )
	# parent.append( description_el )
	# return iterateMarkup( paraXml, parent, html )

	description_el = genTag( bs4, "div", ["description"] )
	iterateMarkup( bs4, tree.find( r'briefdescription/' ), description_el )
	iterateMarkup( bs4, tree.find( r'detaileddescription/' ), description_el )
	return description_el
	
	
def processClassXmlFile( inPath, outPath, html ):

	print "Processing file: " + inPath + " > " + outPath;
	tree = ET.parse( inPath )
	
	# add title
	headTag = html.head
	titleTag = html.new_tag( "title" )
	compoundName = str(findCompoundName( tree )) 		# with "cinder::"
	className = compoundName.rsplit( "cinder::", 1 )[1]	# without "cinder::"

	titleTag.append( compoundName )
	headTag.insert( 0, titleTag )

	# find contents wrapper
	contentsTag = html.find( "div", "contents" )

	# description area
	descriptionEl = html.find( id="description" )
	descriptionProseEl = descriptionEl.find( "div", "prose" )
	sideEl = descriptionEl.find( "div", "side" )

	# ----------
	#  Includes
	# ----------
	sideEl.append( genTag( html, "h3", None, "Includes") )
	includesUl = genTag( html, "ul" )
	for includeDef in tree.findall( r"compounddef/includes" ):
		# includesUl.append( genTag( html, "li", None, includeDef.text ) )
		includeLi = genTag( html, "li", ["row"] )
		# includeContent = replaceTag( includeDef, includeLi, html )
		# includeContent.append( includeDef.text );
		includeContent = genIncludesTag( html, includeDef );
		includeLi.append( includeContent )
		includesUl.append( includeLi )
	sideEl.append( includesUl )


	# +-----------+
	#  Description
	# +-----------+
	descriptionHeader = html.new_tag( "h1")
	descriptionHeader.string = compoundName + "<T> Class Template Reference"
	descriptionProseEl.append( descriptionHeader )

	# descTag = html.new_tag( "div" )
	# descTag['class'] = "description"
	# addClassToTag( descTag, "description" )


	# markupDescription( tree.find( r'compounddef/briefdescription/' ), descTag, html );
	# markupDescription( tree.find( r'compounddef/briefdescription/' ), descTag, html );
	# print tree.dump( tree.find(r'compounddef/') )
	descTag = markupDescription( html, tree.find( r'compounddef' ) );
	
	
	descriptionProseEl.append( descTag )

	# +-------+
	#  Classes
	# +-------+
	sectionTag = genTag( html, "section" )
	classesHeader = genTag( html, "h1", None, "Classes" )
	contentsTag.append( classesHeader )

	classesUl = genTag( html, "ul" )
	for classDef in tree.findall( r"compounddef/innerclass" ):
		classesUl.append( genTag( html, "li", ["row"], classDef.text ) )
		# TODO: pull class brief descrption from class page xml
	sectionTag.append( classesUl )
	sectionTag.append( genTag( html, "hr" ) )
	contentsTag.append( sectionTag );

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
		memberName = memberFn.find(r"name")
		isConstructor = False

		# determine if it is a constructor
		if memberName is not None and memberName.text == className:
			isConstructor = True

		# determine if static
		if isStatic == 'yes':
			staticFuncCount += 1
			markupFunction( html, memberFn, staticUlTag, isConstructor )
		else:
			funcCount += 1
			markupFunction( html, memberFn, ulTag, isConstructor )
			
	# if function count > 0 add it
	if funcCount > 0 :
		header = html.new_tag( "h1")
		header.string = "Public Member Functions"
		sectionTag = genTag( html, "section" );
		sectionTag.append( header )
		sectionTag.append( ulTag )
		contentsTag.append( sectionTag );
		sectionTag.append( genTag( html, "hr" ) )

	# if static function count > 0 add it
	if staticFuncCount > 0 :
		header = html.new_tag( "h1")
		header.string = "Static Public Member Functions"
		sectionTag = genTag( html, "section" );
		sectionTag.append( header )
		sectionTag.append( staticUlTag )
		contentsTag.append( sectionTag )
		sectionTag.append( genTag( html, "hr" ) )

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
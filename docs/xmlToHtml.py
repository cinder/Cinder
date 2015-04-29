#! /usr/bin/python
import sys
import os
import codecs
import shutil
import copy
import re
import xml.etree.ElementTree as ET
from bs4 import BeautifulSoup, Tag, NavigableString

# soup = None

tagDictionary = {
	"linebreak": "br",
	"emphasis": "em",
	"ref": "a",
	"computeroutput": "code",
	"includes": "span",
	"simplesect": "dl",
	"para": "p"
}

tagXml = None

# various HTML elements on the page
sideEl = None

fileTags = None
classTags = None

def findCompoundName( tree ):
	for compoundDef in tree.iter( "compounddef" ):
		for compoundName in tree.iter( "compoundname" ):
			print compoundName.text
			return compoundName.text

def findClassDescription( className ):
	# find the xml file for that class
	# within that xml file, find the brief description
	return ""

def findClassTag( className ):
	# find the class definition in tag file
	for c in classTags :
		# print c.find('name').text
		if c.find('name').text == className:
			return c

def findFileTag( includeFile ):
	# find the file definition in tag file
	for f in fileTags :
		if f.find('name').text == includeFile:
			return f

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
		# print "RETURN COL"
		iterateMarkup( bs4, fnXml.find( r"type" ), returnDiv  )
		functionDiv.append( returnDiv )


	# right side (function name and description)
	definitionCol = genTag( bs4, "div", ["definitionCol columns"] )

	# width is dependent on if it has the return column
	if isConstructor:
		addClassToTag(definitionCol, "large-12")
	else:
		addClassToTag(definitionCol, "large-9")

	functionDiv.append( definitionCol )

	# function name
	definitionDiv = genTag( bs4, "div", ["definition"] )
	print fnXml.find( "definition" )
	emTag = genTag( bs4, "em", [], fnXml.find( "definition" ).text )
	definitionDiv.append( emTag );
	definitionDiv.append( fnXml.find( "argsstring" ).text )
	definitionCol.append( definitionDiv );

	# detailed description
	descriptionDiv = markupDescription( bs4, fnXml );
	if descriptionDiv is not None :
		definitionCol.append( descriptionDiv )
		addClassToTag( li, "expandable" )
		
	# else :
		# print "NO DESCRIPTION"
	
	parent.append( li )


def defineLinkTag( tag, attrib ):

	refId = None
	kind = None

	if( "refid" in attrib ) :
		refId = attrib["refid"]
		href = refId + ".html"

	if( "kindref" in attrib ) :
		kind = attrib["kindref"]

		if kind == "member":
			strList = refId.rsplit("_1", 1)
			href = strList[0] + ".html#" + strList[1]

	if "linkid" in attrib :
		href = "../../include/cinder/" + attrib["linkid"]

	if "href" in attrib :
		href = attrib["href"]	

	tag["href"] = href


def defineTag( bs4, tagName, tree ):

	newTag = bs4.new_tag( tagName )
	if tagName == "a":
		defineLinkTag( newTag, tree.attrib )
	# elif tagName == "code":
		# addClassToTag( newTag, "language-cpp" );
	
	return newTag

# generate includes tag group
def genIncludesTag( bs4, text ):

	# wrap the content in <code> tag
	# prepend with "#include"
	# wrapperEl = genTag( bs4, "span", ["include"], "#include" )
	# wrap include file name in <a> tag
	includeLink = genTag( bs4, "a", None, text )
	defineLinkTag( includeLink, {'linkid':text} )
	# wrapperEl.append( includeLink )
	return includeLink

def genTypeDefs( bs4, tree ):
	# get typedefs from fileTagTree in tagfile
	typeDefs = tree.findall( r'member[@kind="typedef"]' )

	if len(typeDefs) == 0 :
		return

	# create html from template
	side = getTemplate( bs4, "side-expandable" )
	sideEl.append( side )

	# fill heading
	side.find('h4').append("Typedefs:")

	typeDefUl = None
	contentDiv = side.find( "div", "content" )
	if len(typeDefs) > 0 :
		typeDefUl = genTag( bs4, "ul" )

	# fill list of typedefs
	for typeDef in typeDefs :
		typeDefLi = genTag( bs4, "li" )
		typeDefLi.append( typeDef.find( "name" ).text );
		typeDefUl.append( typeDefLi )

	# plug into html
	if typeDefUl is not None:
		contentDiv.append( typeDefUl )

	# print typeDefs


def iterClassBase( tree, heirarchy ) :

	base = tree.find( 'base' )
	
	# tree.find( 'base' ).text

	if base is None:
		return
	else :
		# print "BASE: " + base.text
		newTree = findClassTag( base.text )
		# add to heirarchy
		# heirarchy.append( base.text )
		heirarchy.append( newTree )
		iterClassBase( newTree, heirarchy )

def genClassHierarchy( bs4, tree ):

	if tree is None:
		return

	# first item in the list will be the original class
	heirarchy = [ tree ]

	# get the class' heirarchy
	iterClassBase( tree, heirarchy )
	# print heirarchy
	 
	if len( heirarchy ) == 1 :
		return
	
	# create html from template
	side = getTemplate( bs4, "side-expandable" )
	sideEl.append( side )

	# fill heading
	side.find('h4').append("Class Heirarchy:")

	# create all of the markup
	contentDiv = side.find('div', 'content')
	ul = genTag( bs4, "ul" )
	addClassToTag( ul, "inheritence" )
	contentDiv.append( ul )

	# go through the heirarchy and add a list item for each member
	for index, base in enumerate( reversed( heirarchy ) ):
		li = genTag( bs4, "li" )
		addClassToTag( li, "depth" + str( index + 1 ) )
		
		a = genTag( bs4, "a", [], base.find("name").text )
		defineLinkTag( a, {'href':base.find("filename").text} )

		li.append( a )
		ul.append( li )

	
def genClassList( bs4, tree ):

	classes = tree.findall( r"compounddef/innerclass" )

	if len( classes ) < 1 :
		return

	# create html from template
	side = getTemplate( bs4, "side-expandable" )
	sideEl.append( side )

	# fill heading
	side.find('h4').append("Classes:")

	# create all of the markup
	classesUl = genTag( bs4, "ul" )
	contentDiv = side.find('div', 'content')
	for classDef in classes:
		# print classDef.attrib
		li = genTag( bs4, "li" )
		classesUl.append( li )

		a = genTag( bs4, "a", [], classDef.text )
		defineLinkTag( a, classDef.attrib )
		li.append( a )
	
	contentDiv.append( classesUl )



def replaceTag( bs4, tree, parentTag, content ):

	tag = tree.tag
	attrib = tree.attrib
	hasParent = False
	contentTag = None

	if parentTag and parentTag.parent:
		hasParent = True

	# change parentTag if necessary
	if tag == "codeline" :
		parentTag = parentTag.code

	# find html tag based on tag
	if tag == "para" :
		if hasParent and parentTag.parent.dl :
			tagName = "dd"
		else :
			tagName = tagDictionary[tag]
	elif tag == "sp" :
		if content is None :
			content = " "
		else:
			content.append( " " )
	# 	print ET.dump(tree)

		# if hasParent and parentTag.parent.pre :
			# tagName = "code"
			# addClassToTag( codeTag, "language-cpp" )

	# get tag equivalent
	if tag in tagDictionary:
		tagName = tagDictionary[tag]
		newTag = defineTag( bs4, tagName, tree );
	else:
		# TODO: replace with nothing - no new tag
		tagName = "div"
		newTag = defineTag( bs4, tagName, tree );
		addClassToTag( newTag, tag )
	
	contentTag = newTag

	# if simplesect, construct with some content
	if( tag == "simplesect" ) :
		seeTag = bs4.new_tag( "dt" )
		addClassToTag( seeTag, "section")

		# "see also" reference
		if( attrib["kind"] == "see"):
			addClassToTag( seeTag, "see")
			seeTag.string = "See Also"
		newTag.append( seeTag )

	# if tag == "computeroutput" :
	if tag == "programlisting" :
		# print "pre--"
		# preTag = bs4.new_tag( "pre" )
		codeTag = bs4.new_tag( "code" )
		addClassToTag( codeTag, "language-cpp" )
		# addClassToTag( codeTag, "highlight" )
		# preTag.append( codeTag )
		newTag.append( codeTag )
		# newTag.append( codeTag )
		contentTag = codeTag
		# contentTag = preTag
	

	
		

	# TODO: figure out why adding a <pre> instead of a <code> breaks the <p>

	if content is not None :
		contentTag.append( content )
	

	# if tag == "computeroutput" :

	# 	# if pre tag == p, append to parent, then add new p to continue from
	# 	print ""
	# 	print " -------------------"
	# 	print " ---- ADD NEW P ----"
	# 	newPTag =  genTag( bs4, "p")
	# 	parentTag.parent.append( newTag )
	# 	parentTag.append( newPTag )
	# 	newTag = newPTag
	# 	# print parentTag
	# else  :
		# parentTag.append( newTag )
	parentTag.append( newTag )



	return newTag


def iterateMarkup( bs4, tree, parent ):

	if tree == None :
		return
	
	origTag = tree.tag


	# print tree.attrib

	# print "TAG: " + tree.tag
	# if tree.text :
		# print "TEXT: " + tree.text

	currentTag = parent
	content = None

	# add content to tag
	if tree.text != None:
		# currentTag.append( tree.text.strip() )
		content = tree.text.strip()

	# append any new tags
	if tree.tag != None :
		htmlTag = replaceTag( bs4, tree, currentTag, content )

		# if tree parent == <p> && newTag == <pre>
		# add a new pre tag in and make that the current parent again

		currentTag = htmlTag	
	
	
	# iterate through children tags
	for child in list( tree ):
		output = iterateMarkup( bs4, child, currentTag )

	print "--"
	print tree.tail
	print "--"
	print ""
	

	# tail is any extra text that isn't wrapped in another tag
	# that exists before the next tag
	if tree.tail != None:
		parent.append( tree.tail.strip() )

		if tree.tail.endswith( ";" ):
			print "END LINE"
			parent.append( genTag( bs4, "br") )
	
	return currentTag

def markupDescription( bs4, tree ):

	# print "\n-- MARKING UP PARAGRAPH --"
	
	# description_el = iterateMarkup( paraXml, parent, html )
	# parent.append( description_el )
	# return iterateMarkup( paraXml, parent, html )

	description_el = genTag( bs4, "div", ["description", "content"] )
	briefDesc = tree.find( r'briefdescription/' )
	if briefDesc is None :
		return
	else :
		iterateMarkup( bs4, tree.find( r'briefdescription/' ), description_el )
	
	detailedDesc = tree.find( r'detaileddescription/' )
	if detailedDesc is not None:
		iterateMarkup( bs4, tree.find( r'detaileddescription/' ), description_el )	

	# print '---'
	# print tree.find( r'briefdescription/' )
	# print tree.find( r'detaileddescription/' )
	# print ''
	
	return description_el

# Looks though the html and replaces any code chunks that exist
# in a paragraph and splits them up so that we can use pre tags.
def replaceCodeChunks( bs4 ) :

	# find all p tags
	pTags = bs4.find_all( "p" )
	for p in pTags :

		# if the paragraph as a "div.computeroutput"
		# codeChunks = p.find_all( "div", "computeroutput" )
		codeChunks = p.find_all( "div", "programlisting" )
		
		if( len(codeChunks) > 0 ) :

			newTags = []
			curP = None

			for c in p.contents:
				
				# create a new paragraph to wrap content in
				if curP is None:
					curP = genTag( bs4, "p" )
					newTags.append( curP )

				
				addToP = True

				if type( c ) == Tag:
					if c.has_attr("class") :
						# print c["class"]

						# look for any tags with "computeroutput"
						if "programlisting" in c["class"] :

							# add a new pre
							preTag = genTag( bs4, "pre" )
							

							for code in c.contents :

								# clone code
								cClone = clone(code)
								
								# code string
								codeString = ""
								if type(code) == Tag:

									# clear the original tag
									code.clear()

									# add clone to holder so that we can iterate through
									holder = genTag(bs4, "div")
									holder.append( cClone )
									
									# append line breaks for each line of code
									for content in holder.code.contents :
										codeString += content.getText() + "\n"

								if len(cClone) > 0:
									code.append( codeString )
									preTag.append( code )

							newTags.append( preTag )

							# create a new p tag for any following content to add itself to
							curP = genTag( bs4, "p" )
							newTags.append( curP )
							addToP = False
				
				if addToP :
					cClone = clone(c)
					curP.append( cClone )

			# replace p tag with an empty span
			replacementTag = genTag(bs4, "span") 
			p.replace_with( replacementTag )

			# add the newly organized tags to the new span
			for t in newTags:
				replacementTag.append( t )

			# unwrap / remove the span since that was only there to place this new content into
			replacementTag.unwrap()
			

# clone an element
# from: http://stackoverflow.com/questions/23057631/clone-element-with-beautifulsoup/23058678#23058678
def clone(el) :
    if isinstance(el, NavigableString):
        return type(el)(el)

    copy = Tag(None, el.builder, el.name, el.namespace, el.nsprefix)
    # work around bug where there is no builder set
    # https://bugs.launchpad.net/beautifulsoup/+bug/1307471
    copy.attrs = dict(el.attrs)
    copy.index = el.index
    for attr in ('can_be_empty_element', 'hidden'):
        setattr(copy, attr, getattr(el, attr))
    for child in el.contents:
        copy.append(clone(child))
    return copy


def getTemplate( bs4, elementId ) :
	templates = bs4.find_all( 'template' ) 
	template = None

	for t in templates :	
		# [0] is a string before the enclosed div, so used [1] instead
		template = clone( list(t.contents)[1] )

	return template

	# return clone( bs4.find( id=elementId ) )

	
def processClassXmlFile( inPath, outPath, html ):

	global sideEl
	global fileTags
	global classTags
	# soup = html;

	print "Processing file: " + inPath + " > " + outPath;

	try:
		tree = ET.parse( inPath )

	except ET.ParseError :
		print "--- PARSE ERROR ---"
		# write the file with error	
		outFile = codecs.open( outPath, "w", "UTF-8" )
		html.append( 'COULD NOT PARSE FILE: ' + inPath )
		outFile.write( html.prettify() )
		return

	# add title
	headTag = html.head
	titleTag = html.new_tag( "title" )
	compoundName = str(findCompoundName( tree )) 				# with "cinder::"
	className = compoundName.rsplit( "cinder::", 1 )[1]			# without "cinder::"

	# find include name
	includeTrees = tree.findall( r"compounddef/includes" )
	includeDef = None
	if len( includeTrees ) > 0 :
		includeDef = tree.findall( r"compounddef/includes" )[0].text

	fileTagTree = None
	if includeDef != None :
		fileTags = tagXml.findall( r'compound[@kind="file"]' )
		fileTagTree = findFileTag( includeDef )
		# print ET.dump(fileTagTree)
	
	classTags = tagXml.findall( r'compound[@kind="class"]' )
	# classTagTree = findClassTag( "cinder::CameraPersp" )
	classTagTree = findClassTag( className )

	# print ET.dump(classTagTree)

	titleTag.append( compoundName )
	headTag.insert( 0, titleTag )

	# find contents wrapper
	contentsTag = html.find( "div", "contents" )

	# descriptiogn area
	descriptionEl = html.find( id="description" )
	descriptionProseEl = descriptionEl.find( "div", "prose" )
	sideEl = descriptionEl.find( "div", "side" )
	includeEl = sideEl.find( "div", "include" )

	# ----------
	#  Includes
	# ----------
	if includeDef != None :
		includeContent = genIncludesTag( html, includeDef );
		includeEl.append( includeContent )
	
	
	# -----------
	#  Side Area
	# -----------
	if fileTagTree != None:
		genTypeDefs( html, fileTagTree )
	genClassHierarchy( html, classTagTree )
	genClassList( html, tree )
	

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
	# print ET.dump( tree.find(r'compounddef/') )
	descTag = markupDescription( html, tree.find( r'compounddef' ) );
	if descTag is not None :
		descriptionProseEl.append( descTag )

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

	# replace any code chunks with <pre> tags, which is not possible on initial creation
	replaceCodeChunks( html )

	# write the file	
	outFile = codecs.open( outPath, "w", "UTF-8" )
	outFile.write( html.prettify() )

def parseTemplateHtml( templatePath ):
	file = codecs.open( templatePath, "r", "UTF-8" )
	return BeautifulSoup( file )

if __name__ == "__main__":
	htmlSourcePath = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'htmlsrc' + os.sep
	doxygenHtmlPath = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'html' + os.sep
	
	# Load tag file
	tagXml = ET.ElementTree( ET.parse( "doxygen/cinder.tag" ).getroot() )
	
	# parse template files
	classTemplateHtml = parseTemplateHtml( os.path.join( htmlSourcePath, "cinderClassTemplate.html" ) )

	if len( sys.argv ) == 3: 
		if os.path.isfile( sys.argv[1] ): # process a specific file

	# if sys.argv[1] # if a file
	# if a directory

			templateHtml = parseTemplateHtml( os.path.join( htmlSourcePath, "cinderClassTemplate.html" ) )
			processClassXmlFile( sys.argv[1], os.path.join( doxygenHtmlPath, sys.argv[2] ), copy.deepcopy( templateHtml ) )

		elif os.path.isdir( sys.argv[1] ): # process a directory
		
			count = 0
	
			# get list of files
			# getTemplateType();

			for file in os.listdir(sys.argv[1]):
				if file.endswith(".xml"):
					if file.startswith( "classcinder" ):
						filePrefix = os.path.splitext( os.path.basename( file ) )[0]
						print(filePrefix)
						processClassXmlFile( os.path.join( sys.argv[1], file ), os.path.join( sys.argv[2], filePrefix + ".html" ), copy.deepcopy( classTemplateHtml ) )
						count = count + 1
						#if count > 5:
						#	break;

	else:
		print "Unknown usage"
#! /usr/bin/python
import sys
import os
import codecs
import shutil
import copy
import re
import xml.etree.ElementTree as ET
from bs4 import BeautifulSoup, Tag, NavigableString
from distutils.dir_util import copy_tree

XML_SOURCE_PATH = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'xml' + os.sep
DOXYGEN_HTML_PATH = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'html' + os.sep
HTML_SOURCE_PATH = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'htmlsrc' + os.sep

# convert docygen markup to html markup
tagDictionary = {
	"linebreak": "br",
	"emphasis": "em",
	"ref": "a",
	"computeroutput": "code",
	"includes": "span",
	"simplesect": "dl",
	"para": "p"
}

titleDictionary = {
	"namespace": "Namespace",
	"class" : "Class",
	"struct": "Struct"
}

headerDictionary = {
	"namespace": "Namespace Reference",
	"class":"<T> Class Template Reference",
	"struct": "Struct Reference"
}

tagXml = None

# various HTML elements on the page
sideEl = None

# globals that may change, but may need to accessed by other routines.
# Sometimes stored since they are costly to parse
global_classTags = None
global_classTagsDict = {}
global_structTags = None
global_structTagsDict = {}

namespaceNav = None
g_symbolMap = None

# mapping for the tag file with helper functions
class SymbolMap (object):
	def __init__( self  ):
		self.namespaces = {}
		self.classes = {}
		self.typedefs = {}
		self.functions = {}
		self.files = {}

	class Class (object):
		def __init__( self, name, base, fileName ):
			self.name = name
			self.base = base
			self.fileName = fileName

	class Namespace( object ):
		def __init__( self, name, fileName ):
			self.name = name
			self.fileName = fileName
		
	class Typedef (object):
		def __init__( self, name, type, path ):
			self.name = name
			self.type = type
			self.path = path

	class Function (object):
		def __init__( self, name, base, path ):
			self.name = name
			self.base = base
			self.path = path

	class File (object):
		def __init__( self, name, path, typedefs ):
			self.name = name
			self.path = path
			self.typedefs = typedefs
	
	# searches the symbolMap for a given symbol, prepending cinder:: if not found as-is
	def findClass( self, name ):
		# replace leading ci:: with cinder:: instead
		if name.find( "ci::" ) == 0:
			name.replace( "ci::", "cinder::" )

		if name in self.classes:
			return self.classes[name]
		elif ("cinder::" + name) in self.classes:
			return self.classes["cinder::"+name]
		else:
			return None

	def findNamespace( self, name ):
		return self.namespaces.get( name );

	def getOrderedNamespaces( self ):
		# create an array of strings that include all of the namespaces and return 
		# the array in alphabetical order
		namespaces = []
		for nsKey in self.namespaces:
			ns = self.namespaces[nsKey]
			namespaces.append( ns )

		# sort by lowercased name
		namespaces = sorted( namespaces, key=lambda s: s.name.lower() )
		
		return namespaces

	def findTypedef( self, name ):
		return self.typedefs.get( name )

	def findFunction( self, name ):
		return self.functions.get( name )

	def findFile( self, name ):
		return self.files.get( name )

	def findFileTypedefs( self, name ):
		return self.findFile( name ).typedefs

	def getClassAncestors( self, name ):
		result = []
		existingClass = self.findClass( name )
		while existingClass and existingClass.base:
			result.insert( 0, existingClass )
			existingClass = self.findClass( existingClass.base )
		
		if result:
			return result
		else:
			return []

	def getClassDescendants( self, name ):
		result = []
		for aClass in self.classes:
			if self.classes[aClass].base == name:
				result.append( self.classes[aClass] )
		
		if result:
			return result
		else:
			return []

class FileData:
	m_compoundName = ""		# fill compound name (with namespace if present)
	m_name = ""				# stripped name (w/o namespace)
	m_tree = None			# xml file that describes the page
	m_bs4 = None			# html file of the actual page
	m_kind = ""				# kind of file that we are parsing (class, namespace, etc)


def findCompoundName( tree ):
	for compoundDef in tree.iter( "compounddef" ):
		for compoundName in tree.iter( "compoundname" ):
			return compoundName.text

def findClassTag( className ):
	# find the class definition in tag file
	for c in global_classTags :
		if c.find('name').text == className:
			return c

def findClassLink( className ) :
	link = None
	if className in global_classTagsDict:
		link = global_classTagsDict[className].find('filename').text
	elif className in global_structTagsDict :
		link = global_structTagsDict[className].find('filename').text
	else :
		link = ""
	return link

def findFileKind( tree ) :
	kind = tree.find( r"compounddef").attrib['kind']
	return kind

def findCompoundNameStripped( tree ) :
	compoundName = findCompoundName( tree )
	name = stripCompoundName( compoundName )
	return name

def stripCompoundName( fullString ) :
	nsParts = fullString.split("::")
	prefix = "::".join( nsParts[:-1] )	# parent namespace up to last ::
	name = "".join( nsParts[-1] )
	return name

def addClassToTag( tag, className ):
	tag["class"] = tag.get("class", []) + [className]

def genAnchorTag( bs4, anchorName ) :
	anchor = genTag( bs4, "a" )
	anchor["name"] = anchorName
	return anchor

def populateTitle( bs4, tree ) :
	# find tags
	headTag = bs4.head
	titleTag = bs4.new_tag( "title" )

	# find text
	kind = findFileKind( tree )
	name = findCompoundNameStripped( tree )

	# populate tags
	titleTag.append( titleDictionary[kind] + " | " + name )	
	headTag.insert( 0, titleTag )

def populateHeader( fileData ) :
	headerTag = fileData.m_bs4.find( id="page-title" )
	headerTag.string = fileData.m_compoundName + " " + headerDictionary[fileData.m_kind]

def parseFile( bs4, inPath, outPath ) :

	tree = None;
	try:
		xml_file = codecs.open( inPath, "r", "UTF-8" )
		tree = ET.parse(xml_file)
		# tree = ET.parse( inPath )

	except:
		exc = sys.exc_info()[0]
		print "\n--- PARSE ERROR ---"
		print "COULD NOT PARSE FILE: " + inPath
		print exc
		# write the file with error	
		outFile = codecs.open( outPath, "w", "UTF-8" )
		bs4.append( 'COULD NOT PARSE FILE: ' + inPath )
		outFile.write( bs4.prettify() )
		print "-------------------\n"
		# return tree

	return tree

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

def genLinkTag( bs4, text, link ) :
	linkTag = genTag( bs4, "a", [], text )
	defineLinkTag( linkTag, {"href": link} )
	return linkTag


def defineLinkTag( tag, attrib ):

	refId = None
	kind = None
	href = None

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

	if "typedef" in attrib :
		data = attrib["typedef"]
		fileName = data.find("anchorfile").text
		anchor = data.find("anchor").text
		href = fileName + "#" + anchor

	if href is None :
		print "ERROR DEFINING LINK TAG"
	else :
		tag["href"] = href


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
	
	li = genTag( bs4, "li", ["row"] )

	# wrapper
	functionDiv = genTag( bs4, "div", ["functionDef"] )
	li.append( functionDiv )

	# left side / return type
	if not isConstructor:
		returnDiv = genTag( bs4, "div", ["returnCol columns large-3"] )
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
	
	emTag = genTag( bs4, "em", [], fnXml.find( "name" ).text )    
	definitionDiv.append( emTag )
	
	argstring = fnXml.find( "argsstring" )
	if argstring is None :
		argstring = fnXml.find( "arglist" )
	argstringText = argstring.text if argstring.text is not None else ""
	
	definitionDiv.append( argstringText )
	definitionCol.append( definitionDiv );

	# detailed description
	descriptionDiv = markupDescription( bs4, fnXml );
	if descriptionDiv is not None :
		definitionCol.append( descriptionDiv )
		addClassToTag( li, "expandable" )
		
	# else :
		# print "NO DESCRIPTION"
	
	parent.append( li )

def markupEnum( bs4, fnXml, parent ):

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
	
	li = genTag( bs4, "li", ["row"] )

	# wrapper
	enumDiv = genTag( bs4, "div", ["enumDef"] )
	li.append( enumDiv )

	# left side / return type
	leftDiv = genTag( bs4, "div", ["returnCol columns large-2"] )
	leftDiv.append("enum")

	# right side (function name and description)
	rightDiv = genTag( bs4, "div", ["definitionCol columns"] )
	addClassToTag(rightDiv, "large-12")

	enumDiv.append( rightDiv )

	# function name
	definitionDiv = genTag( bs4, "div", ["definition"] )
	
	emTag = genTag( bs4, "em", [], fnXml.find( "name" ).text )    
	definitionDiv.append( emTag )
	rightDiv.append( definitionDiv );

	# detailed description
	descriptionDiv = markupDescription( bs4, fnXml );

	# iterate all of the enumvalues
	enumUl = genTag(bs4, "ul")
	for enum in fnXml.findall("enumvalue") :
		enumLi = genTag(bs4, "li", [], enum.find("name").text )
		enumUl.append(enumLi)

	if descriptionDiv is None and len(list(enumUl.children)) > 0 :
		descriptionDiv = genTag(bs4, "div", ["description", "content"])
		descriptionDiv.append( enumUl );

	if descriptionDiv is not None :
		rightDiv.append( descriptionDiv )
		addClassToTag( li, "expandable" )
		
	parent.append( li )


def defineTag( bs4, tagName, tree ):

	newTag = bs4.new_tag( tagName )
	if tagName == "a":
		defineLinkTag( newTag, tree.attrib )
	# elif tagName == "code":
		# addClassToTag( newTag, "language-cpp" );
	
	return newTag

# Generate includes tag group
def genIncludesTag( bs4, text ):

	includeLink = genTag( bs4, "a", None, text )
	defineLinkTag( includeLink, {'linkid':text} )
	return includeLink

def genTypeDefs( bs4, typeDefs ):
	"""	Generates the typedefs side bar, with each linking out
		to its full definition.

	Args:
		bs4: The current beautifulSoup html instance
		typeDefs: An array of SymbolMap::TypeDef Objects

	Returns:
		Empty if the typeDefs list is empty
	"""

	# return if empty array
	if len(typeDefs) == 0 :
		return

	# create html from template
	side = getTemplate( bs4, "side-expandable" )
	sideEl.append( side )
	contentDiv = side.find( "div", "content" )

	# fill heading
	side.find('h4').append("Typedefs:")
	
	# fill list of typedefs
	typeDefUl = genTag( bs4, "ul" )
	for typeDef in typeDefs :
		typeDefLi = genTag( bs4, "li" )
		aTag = genTag( bs4, "a", [], typeDef.name )
		defineLinkTag( aTag, {"href": typeDef.path } )
		typeDefLi.append( aTag );
		typeDefUl.append( typeDefLi )

	# plug into html
	if typeDefUl is not None:
		contentDiv.append( typeDefUl )


def iterClassBase( tree, heirarchy ) :

	if tree is None:
		return False

	base = tree.find( 'base' )
	print "ITER CLASS BASE"
	print base

	if base is None:
		return False
	else :
		newTree = findClassTag( base.text )
		# add to heirarchy
		# heirarchy.append( base.text )
		if( iterClassBase( newTree, heirarchy ) is not False ) :
			heirarchy.append( newTree )

		

def genClassHierarchy( bs4, tree ):

	if tree is None:
		return

	# first item in the list will be the original class
	heirarchy = [ tree ]
	# heirarchy = tree

	# get the class' heirarchy
	iterClassBase( tree, heirarchy )
	 
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

	# print "GEN CLASS HIERACHY"
	# print tree
	# print heirarchy
	# go through the heirarchy and add a list item for each member
	for index, base in enumerate( reversed( heirarchy ) ):
		li = genTag( bs4, "li" )
		addClassToTag( li, "depth" + str( index + 1 ) )
		
		# print base
		# if( base == None ):
			# return

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

	if tag == "programlisting" :
		codeTag = bs4.new_tag( "code" )
		addClassToTag( codeTag, "language-cpp" )
		newTag.append( codeTag )
		contentTag = codeTag

	if content is not None :
		contentTag.append( content )
	
	parentTag.append( newTag )
	return newTag


def iterateMarkup( bs4, tree, parent ):

	if tree == None :
		return
	
	origTag = tree.tag
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
	

	# tail is any extra text that isn't wrapped in another tag
	# that exists before the next tag
	if tree.tail != None:
		parent.append( tree.tail.strip() )

		if tree.tail.endswith( ";" ):
			parent.append( genTag( bs4, "br") )
	
	return currentTag

def markupDescription( bs4, tree ):

	description_el = genTag( bs4, "div", ["description", "content"] )
	briefDesc = tree.find( r'briefdescription/' )
	if briefDesc is None :
		return
	else :
		iterateMarkup( bs4, tree.find( r'briefdescription/' ), description_el )
	
	detailedDesc = tree.find( r'detaileddescription/' )
	if detailedDesc is not None:
		iterateMarkup( bs4, tree.find( r'detaileddescription/' ), description_el )	
	
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


# pull a templated chunk of html out of the selected bs4 file
def getTemplate( bs4, elementId ) :
	templates = bs4.find_all( 'template' ) 
	template = None

	for t in templates :	
		# [0] is a string before the enclosed div, so used [1] instead
		template = clone( list(t.contents)[1] )

	return template


def iterateNamespace( bs4, namespaces, tree, index, label ) :

	# Get namespace of previous child, unless first
	if index == 0:
		parentNs = ""
	else :
		parentNs = namespaces[index-1].name

	thisNamespace = namespaces[index].name

	count = index
	childCount = 0

	# iterate to find all children of parentNs
	for ns in namespaces[index:] :
		namespace = ns.name					# full namespace
		nsParts = namespace.split("::")
		prefix = "::".join( nsParts[:-1] )	# parent namespace up to last ::
		name = "".join( nsParts[-1] )
		nodeLabel = label + str(childCount)

		# check if derived from any parent
		parentIsDerived = hasAncestor( namespaces, namespace )

		# create a list item for the namespace
		nsLi = genTag( bs4, "li" )
		
		# create link for each item
		aTag = genTag( bs4, "a")
		defineLinkTag( aTag, {"href": ns.fileName} )
		aTag.append( name )

		# is decendent of parent namespace
		if prefix == parentNs:

			childCount += 1

			# append to parent
			tree.append( nsLi )

			#generate new nested ul in case there are children
			nsUl = genTag( bs4, "ul" )
			if count + 1 < len( namespaces ) :

				# if there are children, add to the parent ul
				if iterateNamespace( bs4, namespaces, nsUl, count + 1, nodeLabel ) > 0 :
					# addClassToTag( nsLi, "item" )
					# add input
					inputEl = genTag( bs4, "input" )
					inputEl["type"] = "checkbox"
					inputEl["id"] = "item-" + "-".join( list(nodeLabel) )
					labelTag = genTag( bs4, "label" )
					labelTag["for"] = "item-" + "-".join( list(nodeLabel) )
					labelTag.append( aTag )

					nsLi.insert( 0, inputEl )
					nsLi.append( labelTag )
					nsLi.append( nsUl )
				else :
					nsLi.append( aTag )

		else :

			# has no direct descendent on the parent, so add it independently
			if parentIsDerived == False and index == 0:
				childCount += 1
				indieLi = genTag( bs4, "li" )
				# indieLi.append( prefix )

				# TODO: refactor and simplify some of this stuff
				inputEl = genTag( bs4, "input" )
				inputEl["type"] = "checkbox"
				inputEl["id"] = "item-" + "-".join( list(nodeLabel) )
				indieLi.insert( 0, inputEl )

				labelTag = genTag( bs4, "label" )
				labelTag["for"] = "item-" + "-".join( list(nodeLabel) )
				labelTag.append( prefix )
				indieLi.append( labelTag )
				
				indieUl = genTag( bs4, "ul" )
				indieLi.append( indieUl )
				indieUl.append( nsLi )
				nsLi.append( aTag )

				tree.append( indieLi )
		
		count += 1
		
	return childCount


def hasAncestor( namespaces, compareNamespace ):

	comparePrefix = "::".join( compareNamespace.split("::")[0] )
	hasAncestor = False
	for ns in namespaces :
		namespace = ns.name
		prefix = "::".join( namespace.split("::")[0] )
		if prefix == comparePrefix and compareNamespace != namespace :
			return True

	return False


def generateNamespaceNav( bs4 ) :

	namespaces = g_symbolMap.getOrderedNamespaces()
	# print g_symbolMap.getOrderedNamespaces()

	tree = genTag( bs4, "div" )
	ul = genTag( bs4, "ul" )
	tree.append( ul )
	addClassToTag( tree, "css-treeview" )

	# for ns in namespaces :
		# namespace = ns.find('name').text
	

	iterateNamespace( bs4, namespaces, ul, 0, "" )
	return tree


def processClassXmlFile( inPath, outPath, html ):

	global sideEl
	global global_classTags

	# define the tree that contains all the data we need to populate this page
	tree = parseFile( html, inPath, outPath )
	if tree == None:
		return

	# get common data for the file
	compoundName = str(findCompoundName( tree )) 				# with "cinder::"
	nameParts = compoundName.rsplit( "cinder::", 1 )
	if len( nameParts ) > 1 :
		className = nameParts[1]			# without "cinder::"
	else :
		className = nameParts[0]
	kind = findFileKind( tree )

	# store data in a FileData type
	fileData = FileData()
	fileData.m_compoundName = compoundName
	fileData.m_name = className
	fileData.m_tree = tree
	fileData.m_bs4 = html
	fileData.m_kind = kind
	
	# dictionary for subnav anchors
	subnavAnchors = []

	print "Processing file: " + inPath + " > " + outPath;

	# find include name (header file)
	includeTrees = tree.findall( r"compounddef/includes" )
	includeDef = None
	if len( includeTrees ) > 0 :
		includeDef = tree.findall( r"compounddef/includes" )[0].text

	# find file tree in tag file, which is a list of all of the file nodes
	fileDef = None
	if includeDef != None :
		fileDef = g_symbolMap.findFile( includeDef )
	
	# find class tree in tag file, which is all of the class nodes
	classTagTree = findClassTag( className )


	# +-------------+
	#  Page metadata
	# +-------------+
	populateTitle( html, tree )

	# +-----------------------------------------+
	#  find parts in template that already exist
	# +-----------------------------------------+
	contentsTag = html.find( "div", "contents" )

	# description area
	descriptionTag = html.find( id="description" )
	descriptionProseEl = descriptionTag.find( "div", "prose" )

	# side area
	sideNavTag = html.find( id="side-nav" )
	sideEl = descriptionTag.find( "div", "side" )
	includeEl = sideEl.find( "div", "include" )

	# +-------------+
	#  Namespace Nav
	# +-------------+
	sideNavTag.append( namespaceNav )


	# +---------------+
	#  Side Area Nodes
	# +---------------+
	# includes
	if includeDef != None :
		includeContent = genIncludesTag( html, includeDef );
		includeEl.append( includeContent )	
	# typedefs
	if fileDef != None:
		genTypeDefs( html, fileDef.typedefs )
	# class heirarchy
	genClassHierarchy( html, classTagTree )
	# class list
	genClassList( html, tree )
	

	# +-----------+
	#  Description
	# +-----------+
	# page title
	populateHeader( fileData )

	# description prose
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
		# add anchor tag to page
		anchor = genAnchorTag( html, "public-member-functions" )
		subnavAnchors.append({"name":"Public Member Functions", "link": anchor})
		contentsTag.append( anchor )

		# public member functions
		header = html.new_tag( "h1")
		header.string = "Public Member Functions"
		sectionTag = genTag( html, "section" );
		sectionTag.append( header )
		sectionTag.append( ulTag )
		contentsTag.append( sectionTag );
		sectionTag.append( genTag( html, "hr" ) )


	# if static function count > 0 add it
	if staticFuncCount > 0 :

		anchor = genAnchorTag( html, "static-public-member-functions" )
		subnavAnchors.append({"name":"Static Public Member Functions", "link":anchor})
		contentsTag.append( anchor )

		# static public member functions
		header = html.new_tag( "h1")
		header.string = "Static Public Member Functions"
		sectionTag = genTag( html, "section" );
		sectionTag.append( header )
		sectionTag.append( staticUlTag )
		contentsTag.append( sectionTag )
		# sectionTag.append( genTag( html, "hr" ) )

	# replace any code chunks with <pre> tags, which is not possible on initial creation
	replaceCodeChunks( html )


	# +-------------------------------------------------+
	#  SubNav
	#  Fill subnav based on what is actually in the page
	# +-------------------------------------------------+
	subnavEl = html.find( id="sub-nav" )
	subnavUl = genTag( html, "ul" )

	# for all of the subnav anchors, add a link into the subnav list
	for anchor in subnavAnchors :
		li = genTag( html, "li" )
		link = genTag( html, "a", [], anchor["name"] )
		defineLinkTag( link, {"href": "#" + anchor["link"]["name"]} )
		li.append( link )
		subnavUl.append( li )
	subnavEl.append( subnavUl )
	
	# write the file
	outFile = codecs.open( outPath, "w", "UTF-8" )
	outFile.write( html.prettify() )

def addRowLi( bs4, container, leftContent, rightContent, colBreakdown=None ) :

	leftCol = "3"
	rightCol = "9"
	if colBreakdown is not None :
		cols = colBreakdown.split("-")
		leftCol = cols[0]
		rightCol = cols[1]

	li = genTag( bs4, "li", ["row"] )

	#left side
	leftDiv = genTag( bs4, "div", ["returnCol columns"] )
	addClassToTag( leftDiv, "large-" + leftCol )
	if leftContent is not None:
		leftDiv.append( leftContent )
		li.append( leftDiv )
	
	#right side
	rightDiv = genTag( bs4, "div", ["definitionCol columns"] )
	addClassToTag( rightDiv, "large-" + rightCol )
	li.append( rightDiv )
	rightDiv.append( rightContent )

	container.append( li )

def processNamespaceXmlFile( inPath, outPath, html ):

	print "Processing namespace file: " + inPath + " > " + outPath;

	# define the tree that contains all the data we need to populate this page
	tree = parseFile( html, inPath, outPath )
	if tree == None:
		return

	# get common data for the file
	compoundName = str(findCompoundName( tree )) 				# with "cinder::"
	nameParts = compoundName.rsplit( "cinder::", 1 )
	if len( nameParts ) > 1 :
		className = nameParts[1]			# without "cinder::"
	else :
		className = nameParts[0]
	kind = findFileKind( tree )

	# store data in a FileData type
	fileData = FileData()
	fileData.m_compoundName = compoundName
	fileData.m_name = className
	fileData.m_tree = tree
	fileData.m_bs4 = html
	fileData.m_kind = kind


	populateTitle( html, tree )

	
	# +-----------------------------------------+
	#  find parts in template that already exist
	# +-----------------------------------------+
	contentsTag = html.find( "div", "contents" )

	# +-------------+
	#  Namespace Nav
	# +-------------+
	# side area
	sideNavTag = html.find( id="side-nav" )
	sideNavTag.append( namespaceNav )


	# +-----------+
	#  Description
	# +-----------+
	namespaceTag = g_symbolMap.findNamespace( compoundName )
	if namespaceTag is None:
		return
	
	# description title
	populateHeader( fileData )

	# +----------+
	#  Namespaces
	# +----------+
	namespacesSection = genTag( html, "section" )
	contentsTag.append( namespacesSection )

	namespacesUl = genTag( html, "ul" )
	namespaces = tree.findall(r"compounddef/innernamespace")
	if namespaces is not None:
		for c in namespaces :
			namespaceLinkTag = genLinkTag( html, c.text, c.attrib["refid"] + ".html" )
			addRowLi( html, namespacesUl, None, namespaceLinkTag )
		
		namespacesSection.append( genTag( html, "h2", [], "Namespaces") )
		namespacesSection.append( namespacesUl )


	contentsTag.append( genTag( html, "br" ) )
	
	# +-------+
	#  Classes
	# +-------+
	classesSection = genTag( html, "section" )
	contentsTag.append( classesSection )

	classesSection.append( genTag( html, "h2", [], "Classes") )
	classUl = genTag( html, "ul" )
	for c in tree.findall(r"compounddef/innerclass") :
		link = c.attrib["refid"] + ".html"
		kind = "struct" if link.startswith("struct") else "class"
		linkTag = genLinkTag( html, c.text, link )
		addRowLi( html, classUl, kind, linkTag, "1-11")
	classesSection.append( classUl )

	contentsTag.append( genTag( html, "br" ) )

	# +--------+
	#  TypeDefs
	# +--------+
	typedefSection = genTag( html, "section" )
	contentsTag.append( typedefSection )

	typedefSection.append( genTag( html, "h2", [], "Typedefs") )
	typedefUl = genTag( html, "ul" )
	for c in tree.findall(r"compounddef/sectiondef/[@kind='typedef']/memberdef/[@kind='typedef']") :
		markupFunction( html, c, typedefUl, False )
	typedefSection.append( typedefUl )

	contentsTag.append( genTag( html, "br" ) )
	
	# +------------+
	#  Enumerations
	# +------------+
	enumSection = genTag( html, "section" )
	contentsTag.append( enumSection )

	enumSection.append( genTag( html, "h2", [], "Enumerations") )
	enumUl = genTag( html, "ul" )
	for c in tree.findall(r"compounddef/sectiondef/[@kind='enum']/memberdef/[@kind='enum']") :
		markupEnum(html, c, enumUl)
	enumSection.append( enumUl )

	contentsTag.append( genTag( html, "br" ) )

	# +---------+
	#  Functions
	# +---------+
	functionSection = genTag( html, "section" )
	contentsTag.append( functionSection )

	functionSection.append( genTag( html, "h2", [], "Functions") )
	functionUl = genTag( html, "ul" )
	for c in tree.findall( r"compounddef/sectiondef/[@kind='func']/memberdef/[@kind='function']") :
		markupFunction( html, c, functionUl, False )
	functionSection.append( functionUl )

	contentsTag.append( genTag( html, "br" ) )

	# +---------+
	#  Variables
	# +---------+
	varsSection = genTag( html, "section" )
	contentsTag.append( varsSection )

	varsSection.append( genTag( html, "h2", [], "Variables") )
	varsUl = genTag( html, "ul" )
	for c in tree.findall( r"compounddef/sectiondef/[@kind='var']/memberdef/[@kind='variable']") :
		typeStr = c.find('type').text
		name = c.find( 'name' ).text
		initializer = c.find('initializer').text if c.find('initializer') is not None else None
		if typeStr == None:
			typeStr = ""
		if initializer is not None:
			name += initializer
		addRowLi( html, varsUl, typeStr, name )
	varsSection.append( varsUl )

	# write the file
	outFile = codecs.open( outPath, "w", "UTF-8" )
	outFile.write( html.prettify() )

# puts together a beautiful soup instance by mashing a bunch of html files together
def constructTemplate( templates ) :

	masterTemplate = ""
	for templatePath in templates :
		# file = codecs.open( os.path.join( htmlSourcePath, templatePath ), "r", "UTF-8" )
		masterTemplate += open(os.path.join( HTML_SOURCE_PATH, templatePath )).read()
		# masterTemplate += file

	masterTemplate.decode("UTF-8")
	return BeautifulSoup( masterTemplate )

# returns a dictionary from Cinder class name to file path
def getSymbolToFileMap( path ):	
	print "Parsing tag file: " + path

	tagDom = ET.ElementTree( ET.parse( path ).getroot() )
	
	symbolMap = SymbolMap()
	# find namespaces
	nsTags = tagXml.findall( r'compound/[@kind="namespace"]')
		
	for ns in nsTags :
		namespaceName = ns.find('name').text
		fileName = ns.find('filename').text

		# skip namespaces with '@' in them
		if namespaceName.find('@') > -1:
			continue

		symbolMap.namespaces[namespaceName] = SymbolMap.Namespace( namespaceName, fileName )
		
		for member in ns.findall(r"member/[@kind='typedef']"):
			name = member.find("name").text
			type = member.find("type").text
			name = namespaceName+"::"+name
			type = namespaceName+"::"+type
			filePath = member.find('anchorfile').text + "#" + member.find("anchor").text
			symbolMap.typedefs[name] = SymbolMap.Typedef( name, type, filePath )
		
	# find classes
	classTags = tagXml.findall( r'compound/[@kind="class"]')
	for c in classTags :
		name = c.find('name').text
		filePath = c.find('filename').text
		baseClass = c.find('base').text if c.find('base') is not None else ""
		symbolMap.classes[name] = SymbolMap.Class( name, baseClass, filePath )

		# find functions and add to symbol map
		members = c.findall(r"member[@kind='function']")
		for member in members:
			
			fnName = member.find("name").text
			anchor = member.find("anchor").text
			filePath = member.find("anchorfile").text + "#" + anchor
			symbolMap.functions[name+"::"+fnName] = SymbolMap.Function( fnName, baseClass, filePath )

	# find files
	fileTags = tagXml.findall( r'compound/[@kind="file"]')
	for f in fileTags:
		name = f.find('name').text
		filePath = f.find('filename').text
		typedefs = []
		for t in f.findall( r'member[@kind="typedef"]'):
			tdName = t.find("name").text
			type = t.find("type").text
			filePath = t.find('anchorfile').text + "#" + t.find("anchor").text
			typedef = SymbolMap.Typedef( tdName, type, filePath )
			typedefs.append( typedef )
		symbolMap.files[name] = SymbolMap.File( name, filePath, typedefs )

	return symbolMap

def getFilePrefix( filePath ):
	return os.path.splitext( os.path.basename( filePath ) )[0]

def parseTemplateHtml( templatePath ):
	file = codecs.open( templatePath, "r", "UTF-8" )
	return BeautifulSoup( file )

def processFile( inPath, outPath ):
	
	file = inPath
	filePrefix = os.path.splitext( os.path.basename( file ) )[0]

	if filePrefix.startswith( "class" ) or filePrefix.startswith( "struct" ):
		processClassXmlFile( sys.argv[1], os.path.join( DOXYGEN_HTML_PATH, outPath ), copy.deepcopy( classTemplateHtml ) )

	elif filePrefix.startswith( "namespace" ) :
		processNamespaceXmlFile( inPath, os.path.join( DOXYGEN_HTML_PATH, outPath ), copy.deepcopy( namespaceTemplateHtml ) )

def processDir(inPath, outPath):
	inPath = sys.argv[1]
	outPath = sys.argv[2]
	for file in os.listdir(inPath):
		if file.endswith(".xml"):
			filePrefix = os.path.splitext( os.path.basename( file ) )[0]
			
			if file.startswith( "class" ) or file.startswith( "struct" ):
				# name minus file extension
				processClassXmlFile( os.path.join( inPath, file ), os.path.join( outPath, filePrefix + ".html" ), copy.deepcopy( classTemplateHtml ) )

			elif file.startswith( "namespace" ) :
				processNamespaceXmlFile( os.path.join( inPath, file ), os.path.join( outPath, filePrefix + ".html" ), copy.deepcopy( namespaceTemplateHtml ) )

			elif file.startswith( "_" ) :
				print "TODO: parse source file type"
			else :
				print "PARSE " + file
				# base template and just iterate do an html iteration
				# 
				# processOtherFile( os.path.join( sys.argv[1], file ), os.path.join( sys.argv[2], filePrefix + ".html" ), copy.deepcopy( classTemplateHtml ) )


# -- Main Function --
# Can pass in a single xml file to process by passing in path to xml file and optionally, the resulting html file
# If no out path is supplied, outputs to DOXYGEN_HTML_PATH
# Can alternatively pass in a directory to process by providing the xml directory
if __name__ == "__main__":

	# Load tag file
	tagXml = ET.ElementTree( ET.parse( "doxygen/cinder.tag" ).getroot() )

	# construct full template out of multiple html templates
	classTemplateHtml = constructTemplate( ["headerTemplate.html", "mainNavTemplate.html", "cinderClassTemplate.html", "footerTemplate.html"] )
	namespaceTemplateHtml = constructTemplate( ["headerTemplate.html", "mainNavTemplate.html", "cinderNamespaceTemplate.html", "footerTemplate.html"] )

	# find all class definitions and store for retrieval later
	global_classTags = tagXml.findall( r'compound[@kind="class"]' )
	for c in global_classTags :
		global_classTagsDict[c.find( 'name' ).text] = c

	global_structTags = tagXml.findall( r'compound[@kind="struct"]' )
	for s in global_structTags :
		global_structTagsDict[s.find( 'name' ).text] = s	

	# copy files from assets/ to html/
	copy_tree("assets/", "html/")

	# generate symbol map from tag file
	g_symbolMap = getSymbolToFileMap( "doxygen/cinder.tag" )

	namespaceNav = generateNamespaceNav( classTemplateHtml )

	inPath = sys.argv[1]
	if os.path.isfile( inPath ): # process a specific file
		outPath = sys.argv[2] if len( sys.argv) > 2 else DOXYGEN_HTML_PATH + getFilePrefix( inPath ) + ".html"
		print outPath
		processFile( inPath, outPath )

	elif os.path.isdir( inPath ): # process a directory
		if len( sys.argv ) == 3: 
			processDir( inPath, sys.argv[2])

	else:
		print "Unknown usage"
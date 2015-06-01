#! /usr/bin/python
import sys
import os
import codecs
import shutil
import copy
import re
import xml.etree.ElementTree as ET
import glob, os, shutil
from bs4 import BeautifulSoup, Tag, NavigableString
from distutils.dir_util import copy_tree

XML_SOURCE_PATH = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'xml' + os.sep
DOXYGEN_HTML_PATH = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'html' + os.sep
HTML_SOURCE_PATH = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'htmlsrc' + os.sep
TEMPLATE_PATH = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'htmlsrc' + os.sep + "_templates" + os.sep

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
g_symbolMap = None
g_currentFile = None

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

	def getLinkForClass( self, className ):
		""" Get the link for the definition of a class.
			It may include namespace or not.
		"""
		# strip down to the name of the class (without namespace)
		return ""

class FileData (object):
	def __init__( self, tree, bs4 ):

		self.tree = tree			# xml file that describes the page
		self.bs4 = bs4				# html file of the actual page
		
		# fill compound name (with namespace if present)
		self.compoundName = str(findCompoundName( tree )) 		

		# stripped name (w/o namespace)
		nameParts = self.compoundName.rsplit( "cinder::", 1 )
		if len( nameParts ) > 1 :
			self.name = nameParts[1]			# without "cinder::"
		else :
			self.name = nameParts[0]

		# kind of file that we are parsing (class, namespace, etc)
		self.kind = findFileKind( tree )		

		# find any common html elements and populate common elements
		self.parseTemplate();


	def parseTemplate( self ):
		self.contentsEl = self.bs4.find( "div", "contents" )
		if self.kind == "class" or self.kind == "struct":
			self.descriptionEl = self.bs4.find( id="description" )
			self.descriptionProseEl = self.descriptionEl.find( "div", "prose" )
			self.sideEl = self.descriptionEl.find( "div", "side" )
		self.sideNavEl = self.bs4.find( id="side-nav" )

		self.populateTitle()
		self.populateHeader()

	def populateTitle( self ) :
		# find tags
		headTag = self.bs4.head
		titleTag = self.bs4.new_tag( "title" )

		# populate tags
		titleTag.append( titleDictionary[self.kind] + " | " + self.name )	
		headTag.insert( 0, titleTag )

	def populateHeader( self ) :
		headerTag = self.bs4.find( id="page-title" )
		headerTag.string = self.compoundName + " " + headerDictionary[self.kind]

	def appendToSideEl( self, el ):
		self.sideEl.append( el )

def findCompoundName( tree ):
	for compoundDef in tree.iter( "compounddef" ):
		for compoundName in tree.iter( "compoundname" ):
			return compoundName.text

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

def genTag( bs4, tagType, classes = None, contents = None ):
	""" Generates a new html element and optionally adds classes and content
		
	Args:
		bs4:		beautiful soup
		tagType:	html tag/element (p, a, em, etc)
		classes: 	array of strings that you want as classes for the element
		contents:	any content that you want to populate your tag with, if known
	"""
	
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
	""" Mark up a function using the function definition
	
	create new line
	left side = return type
	right side = function name
	under right side = definition
	
	---------------------------------------------------
	| returnType	| function( param1, param2, etc ) |
	---------------------------------------------------
	| 				| description 					  |
	---------------------------------------------------
	"""
	
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
	""" Mark up a function using the function definition
	
	create new line
	left side = return type
	right side = function name
	under right side = definition
	
	---------------------------------------------------
	| returnType	| function( param1, param2, etc ) |
	---------------------------------------------------
	| 				| description 					  |
	---------------------------------------------------
	"""
	
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
	parent.append( li )

def markupEnum( bs4, fnXml, parent ):
	""" Mark up an enum using the function definition
	
	create new line
	left side = return type
	right side = function name
	under right side = definition
	
	---------------------------------------------------
	| returnType	| function( param1, param2, etc ) |
	---------------------------------------------------
	| 				| description 					  |
	---------------------------------------------------
	"""
	
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

def genIncludes( includeDef ):

	bs4 = g_currentFile.bs4

	# create html from template
	side = getTemplate( bs4, "side-static" )
	contentDiv = side.find( "div", "content" )

	# fill heading
	side.find('h4').append("#include")

	# add include link
	includeLink = genTag( bs4, "a", None, includeDef )
	defineLinkTag( includeLink, {'linkid':includeDef} )
	contentDiv.append( includeLink )

	g_currentFile.appendToSideEl( side )

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

	# append to the current file's side element
	g_currentFile.appendToSideEl( side )


def iterClassBase( classDef, hierarchy ) :
	""" Iterates the class to find all of their base classes
		and iterate through them

	Args:
		classDef: The instance of SymbolMap::Class Object whose base we are searching for
		hierachy: The current hierachy of classes to append to if we find another base
	"""

	if classDef is None or hasattr(classDef, 'name') == False:
		return False

	base = classDef.base

	if base is None:
		return False
	else :
		newTree = g_symbolMap.findClass( base )
		# add to hierarchy if it continues
		if( iterClassBase( newTree, hierarchy ) is not False ) :
			hierarchy.append( newTree )

		
def genClassHierarchy( bs4, classDef ):
	"""	Generates the class heirarchy side bar, with each class linking 
		out to its class file.

	Args:
		bs4: The current beautifulSoup html instance
		classDef: The instance of SymbolMap::Class Object that we are generating 
			the hierachy for

	Returns:
		Empty if there is no base class
	"""

	if classDef is None:
		return

	# first item in the list will be the original class
	hierarchy = [ classDef ]

	# get the class' hierarchy
	iterClassBase( classDef, hierarchy )
	 
	if len( hierarchy ) == 1 :
		return
	
	# create html from template
	side = getTemplate( bs4, "side-expandable" )

	# fill heading
	side.find('h4').append("Class Heirarchy:")

	# create all of the markup
	contentDiv = side.find('div', 'content')
	ul = genTag( bs4, "ul" )
	addClassToTag( ul, "inheritence" )
	contentDiv.append( ul )

	# go through the hierarchy and add a list item for each member
	for index, base in enumerate( reversed( hierarchy ) ):
		li = genTag( bs4, "li" )
		addClassToTag( li, "depth" + str( index + 1 ) )

		# link out only if a base class, not the original class
		if( index < len(hierarchy) - 1 ) :
			a = genTag( bs4, "a", [], base.name )
			defineLinkTag( a, {'href':base.fileName} )
			li.append( a )
		else :
			li.append( base.name )
		ul.append( li )

def genClassList( bs4, tree ):
	classes = tree.findall( r"compounddef/innerclass" )

	if len( classes ) < 1 :
		return

	# create html from template
	side = getTemplate( bs4, "side-expandable" )

	# fill heading
	side.find('h4').append("Classes:")

	# create all of the markup
	classesUl = genTag( bs4, "ul" )
	contentDiv = side.find('div', 'content')
	for classDef in classes:
		li = genTag( bs4, "li" )
		classesUl.append( li )

		a = genTag( bs4, "a", [], stripCompoundName( classDef.text ) )
		defineLinkTag( a, classDef.attrib )
		li.append( a )
	
	contentDiv.append( classesUl )

	g_currentFile.appendToSideEl( side )


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

def replaceCodeChunks( bs4 ) :
	""" Looks though the html and replaces any code chunks that exist
		in a paragraph and splits them up so that we can use pre tags.
	"""
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
		if t['id'] == elementId:
			template = clone( list(t.contents)[1] )
		else:
			continue

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

	tree = genTag( bs4, "div" )
	ul = genTag( bs4, "ul" )
	tree.append( ul )
	addClassToTag( tree, "css-treeview" )

	# for ns in namespaces :
		# namespace = ns.find('name').text

	iterateNamespace( bs4, namespaces, ul, 0, "" )
	return tree


def processClassXmlFile( inPath, outPath, html ):
	global g_currentFile
	print "Processing file: " + inPath + " > " + outPath;

	# define the tree that contains all the data we need to populate this page
	tree = parseHtml( html, inPath, outPath )
	if tree == None:
		return

	# get common data for the file
	g_currentFile = FileData( tree, html )
	compoundName = g_currentFile.compoundName
	className = g_currentFile.name
	kind = g_currentFile.kind
	
	# dictionary for subnav anchors
	subnavAnchors = []

	# find include name (header file)
	includeTrees = tree.findall( r"compounddef/includes" )
	includeDef = None
	if len( includeTrees ) > 0 :
		includeDef = tree.findall( r"compounddef/includes" )[0].text

	# find file definition for the include file
	fileDef = None
	if includeDef != None :
		fileDef = g_symbolMap.findFile( includeDef )
	
	# find class definition
	classDef = g_symbolMap.findClass(className)

	# +-----------------------------------------+
	#  find parts in template that already exist
	# +-----------------------------------------+
	contentsEl = g_currentFile.contentsEl
	sideNavEl = g_currentFile.sideNavEl

	# +-------------+
	#  Namespace Nav
	# +-------------+
	sideNavEl.append( g_namespaceNav )

	# +---------------+
	#  Side Area Nodes
	# +---------------+
	# includes
	if includeDef != None :
		genIncludes( includeDef )

	# typedefs
	if fileDef != None:
		genTypeDefs( html, fileDef.typedefs )
	# class hierarchy
	genClassHierarchy( html, classDef )
	# class list
	genClassList( html, tree )
	
	# +-----------+
	#  Description
	# +-----------+
	# description prose
	descTag = markupDescription( html, tree.find( r'compounddef' ) );
	if descTag is not None :
		g_currentFile.descriptionProseEl.append( descTag )

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
		contentsEl.append( anchor )

		# public member functions
		header = html.new_tag( "h1")
		header.string = "Public Member Functions"
		sectionTag = genTag( html, "section" );
		sectionTag.append( header )
		sectionTag.append( ulTag )
		contentsEl.append( sectionTag );
		sectionTag.append( genTag( html, "hr" ) )


	# if static function count > 0 add it
	if staticFuncCount > 0 :

		anchor = genAnchorTag( html, "static-public-member-functions" )
		subnavAnchors.append({"name":"Static Public Member Functions", "link":anchor})
		contentsEl.append( anchor )

		# static public member functions
		header = html.new_tag( "h1")
		header.string = "Static Public Member Functions"
		sectionTag = genTag( html, "section" );
		sectionTag.append( header )
		sectionTag.append( staticUlTag )
		contentsEl.append( sectionTag )

	# replace any code chunks with <pre> tags, which is not possible on initial creation
	replaceCodeChunks( html )

	# generate subnav
	genSubNav( subnavAnchors )

	# write the file
	writeHtml( html, outPath )

def genSubNav( subnavAnchors ):
	# +-------------------------------------------------+
	#  SubNav
	#  Fill subnav based on what is actually in the page
	# +-------------------------------------------------+
	bs4 = g_currentFile.bs4
	subnavEl = bs4.find( id="sub-nav" )
	subnavUl = genTag( bs4, "ul" )

	# for all of the subnav anchors, add a link into the subnav list
	for anchor in subnavAnchors :
		li = genTag( bs4, "li" )
		link = genTag( bs4, "a", [], anchor["name"] )
		defineLinkTag( link, {"href": "#" + anchor["link"]["name"]} )
		li.append( link )
		subnavUl.append( li )
	subnavEl.append( subnavUl )

def addRowLi( bs4, container, leftContent, rightContent, colBreakdown=None, dropdownContent=None ) :

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

	# if there is dropdown content, the right side will have an additional row below it
	if dropdownContent is not None:
		rightDiv.append( dropdownContent )
		addClassToTag( li, "expandable" )


def dropAnchor(anchorList, anchorName, linkName):
	bs4 = g_currentFile.bs4
	anchor = genAnchorTag( bs4, anchorName )
	anchorList.append({"name":linkName, "link": anchor})
	g_currentFile.contentsEl.append( anchor )

def processNamespaceXmlFile( inPath, outPath, html ):
	global g_currentFile
	print "Processing namespace file: " + inPath + " > " + outPath;

	# define the tree that contains all the data we need to populate this page
	tree = parseHtml( html, inPath, outPath )
	if tree == None:
		return

	# get common data for the file
	g_currentFile = FileData( tree, html )
	compoundName = g_currentFile.compoundName
	className = g_currentFile.name
	kind = g_currentFile.kind

	# dictionary for subnav anchors
	subnavAnchors = []

	# +-----------------------------------------+
	#  find parts in template that already exist
	# +-----------------------------------------+
	# contentsTag = html.find( "div", "contents" )
	contentsEl = g_currentFile.contentsEl

	# +-------------+
	#  Namespace Nav
	# +-------------+
	# side area
	sideNavTag = html.find( id="side-nav" )
	sideNavTag.append( g_namespaceNav )

	# +-----------+
	#  Description
	# +-----------+
	namespaceTag = g_symbolMap.findNamespace( compoundName )
	if namespaceTag is None:
		return

	# +----------+
	#  Namespaces
	# +----------+
	namespaces = tree.findall(r"compounddef/innernamespace")
	if len( namespaces ) > 0:
		dropAnchor( subnavAnchors, "namespaces", "Namespaces" );
		namespacesSection = genTag( html, "section" )
		contentsEl.append( namespacesSection )

		namespacesUl = genTag( html, "ul" )
		for c in namespaces :
			namespaceLinkTag = genLinkTag( html, c.text, c.attrib["refid"] + ".html" )
			addRowLi( html, namespacesUl, None, namespaceLinkTag )
		
		namespacesSection.append( genTag( html, "h2", [], "Namespaces") )
		namespacesSection.append( namespacesUl )

		contentsEl.append( genTag( html, "br" ) )
	
	# +-------+
	#  Classes
	# +-------+
	classes = tree.findall(r"compounddef/innerclass")
	if len( classes ) > 0:
		dropAnchor( subnavAnchors, "classes", "Classes" );
		classesSection = genTag( html, "section" )
		contentsEl.append( classesSection )

		classesSection.append( genTag( html, "h2", [], "Classes") )
		classUl = genTag( html, "ul" )
		for c in classes :
			link = c.attrib["refid"] + ".html"
			kind = "struct" if link.startswith("struct") else "class"
			linkTag = genLinkTag( html, stripCompoundName( c.text ), link )
			addRowLi( html, classUl, kind, linkTag, "1-11")
		classesSection.append( classUl )

		contentsEl.append( genTag( html, "br" ) )

	# +--------+
	#  TypeDefs
	# +--------+
	typedefs = tree.findall(r"compounddef/sectiondef/[@kind='typedef']/memberdef/[@kind='typedef']") 
	if len( typedefs ) > 0:
		dropAnchor( subnavAnchors, "typedefs", "Typedefs" );
		typedefSection = genTag( html, "section" )
		contentsEl.append( typedefSection )

		typedefSection.append( genTag( html, "h2", [], "Typedefs") )
		typedefUl = genTag( html, "ul" )
		for c in typedefs:
			markupFunction( html, c, typedefUl, False )
		typedefSection.append( typedefUl )

		contentsEl.append( genTag( html, "br" ) )
	
	# +------------+
	#  Enumerations
	# +------------+
	enumerations = tree.findall(r"compounddef/sectiondef/[@kind='enum']/memberdef/[@kind='enum']")
	if len( enumerations ) > 0:
		dropAnchor( subnavAnchors, "enumerations", "Enumerations" );
		enumSection = genTag( html, "section" )
		contentsEl.append( enumSection )

		enumSection.append( genTag( html, "h2", [], "Enumerations") )
		enumUl = genTag( html, "ul" )
		for c in enumerations :
			markupEnum(html, c, enumUl)
		enumSection.append( enumUl )

		contentsEl.append( genTag( html, "br" ) )

	# +---------+
	#  Functions
	# +---------+
	functions = tree.findall( r"compounddef/sectiondef/[@kind='func']/memberdef/[@kind='function']")
	if len( functions ) > 0:
		dropAnchor( subnavAnchors, "functions", "Functions" );
		functionSection = genTag( html, "section" )
		contentsEl.append( functionSection )

		functionSection.append( genTag( html, "h2", [], "Functions") )
		functionUl = genTag( html, "ul" )
		for c in functions:
			markupFunction( html, c, functionUl, False )
		functionSection.append( functionUl )

		contentsEl.append( genTag( html, "br" ) )

	# +---------+
	#  Variables
	# +---------+
	vars = tree.findall( r"compounddef/sectiondef/[@kind='var']/memberdef/[@kind='variable']")
	if len( vars ) > 0:
		dropAnchor( subnavAnchors, "variables", "Variables" );
		varsSection = genTag( html, "section" )
		contentsEl.append( varsSection )

		varsSection.append( genTag( html, "h2", [], "Variables") )
		varsUl = genTag( html, "ul" )
		for c in vars:
			typeStr = c.find('type').text
			name = genTag( html, "div" )
			name.append( genTag( html, "b", [], c.find( 'name' ).text ) )
			initializer = c.find('initializer').text if c.find('initializer') is not None else None
			description = markupDescription( html, c );
			if typeStr == None:
				typeStr = ""
			if initializer is not None:
				name.append( " " + initializer )
			addRowLi( html, varsUl, typeStr, name, None, description )
		varsSection.append( varsUl )

	genSubNav( subnavAnchors )

	# write the file
	writeHtml( html, outPath )

def processHtmlFile( inPath, outPath ):
	""" Parses an html file.
	- Adds template around the html
	- Copy original css and js links into new hmtl
	- Save html in destination dir
	"""

	# construct template
	bs4 = constructTemplate( ["headerTemplate.html", "mainNavTemplate.html", "htmlContentTemplate.html", "footerTemplate.html"] )
	# parse original html file
	origHtml = generateBs4( inPath )	
	
	# replace all of the bs4 css and js links and make them relative to the outpath
	for link in bs4.find_all("link"):
		if link.has_attr("href"):
			link["href"] = updateLink( link["href"], outPath )

	for script in bs4.find_all("script"):
		if script.has_attr("src"):
			script["src"] = updateLink( script["src"], outPath )

	for img in bs4.find_all("img"):
		if img.has_attr("src"):
			img["src"] = updateLink( img["src"], outPath )


	# copy all js and css paths that may be in the original html and paste into new file

	# plug original html content into template
	bs4.body.find(id="template-content").append(origHtml.body)
	
	writeHtml( bs4, outPath )

def updateLink( link, path ):
	if link.startswith("http"):
		return link
	depth = len( path.split("html/")[1].split("/") ) - 1
	pathPrepend = ""
	for i in range( depth ):
		pathPrepend = "../"
	return pathPrepend + link

def constructTemplate( templates ) :
	""" Constructs a beautiful soup instance by mashing a bunch of html files together
	"""
	masterTemplate = ""
	for templatePath in templates :
		masterTemplate += open(os.path.join( HTML_SOURCE_PATH, TEMPLATE_PATH + templatePath )).read()
	masterTemplate.decode("UTF-8")
	return BeautifulSoup( masterTemplate )

def generateBs4( filePath ):
	ouputFile = open(os.path.join( filePath )).read()
	ouputFile.decode("UTF-8")
	return BeautifulSoup( ouputFile )

def getSymbolToFileMap( tagDom ):	
	""" Returns a dictionary from Cinder class name to file path
	"""
	print "generating symbol map from tag file"
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

		# find typedefs for each file
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

def getFileExtension( filePath ):
	return os.path.splitext( os.path.basename( filePath ) )[1]

def parseHtml( bs4, inPath, outPath ) :
	tree = None;
	try:
		xml_file = codecs.open( inPath, "r", "UTF-8" )
		tree = ET.parse(xml_file)

	except:
		exc = sys.exc_info()[0]
		print "\n--- PARSE ERROR ---"
		print "COULD NOT PARSE FILE: " + inPath
		print exc
		# write the file with error	
		bs4.append( 'COULD NOT PARSE FILE: ' + inPath )
		writeHtml( bs4, outPath )
		print "-------------------\n"

	return tree

def writeHtml( html, savePath ):
	if not os.path.exists(os.path.dirname(savePath)):
	    os.makedirs(os.path.dirname(savePath))
	with codecs.open(savePath, "w", "UTF-8") as outFile:
		outFile.write( html.prettify() )
	

def processFile( inPath, outPath ):
	""" Generate documentation for a single file

		Args:
			inPath: The file to process
			outPath: The file to save the generated html file to
	""" 
	file = inPath
	filePrefix = getFilePrefix( file )
	isHtmlFile = True if getFileExtension( file ).lower() == ".html" else False

	if isHtmlFile:
		filePath = "/".join( inPath.split('/')[1:] )
		savePath = outPath if outPath is not None else DOXYGEN_HTML_PATH + filePath
	else:
		savePath = outPath if outPath is not None else DOXYGEN_HTML_PATH + getFilePrefix( inPath ) + ".html"

	if isHtmlFile:
		processHtmlFile( inPath, savePath )
	else :
		if filePrefix.startswith( "class" ) or filePrefix.startswith( "struct" ):
			processClassXmlFile( sys.argv[1], os.path.join( DOXYGEN_HTML_PATH, savePath ), copy.deepcopy( classTemplateHtml ) )

		elif filePrefix.startswith( "namespace" ) :
			processNamespaceXmlFile( inPath, os.path.join( DOXYGEN_HTML_PATH, savePath ), copy.deepcopy( namespaceTemplateHtml ) )



def processDir(inPath, outPath):
	""" Iterates a directory and generates documentation for each xml file
		in the directory as long as it is a class, struct or namespace
		
		Args:
			inPath: The directory to process
			outPath: The directory to save the generated html file to
	"""

	for file in os.listdir(inPath):
		if file.endswith(".xml"):
			# name minus file extension
			filePrefix = getFilePrefix( file )
			
			# process class and struct files
			if file.startswith( "class" ) or file.startswith( "struct" ):
				processClassXmlFile( os.path.join( inPath, file ), os.path.join( outPath, filePrefix + ".html" ), copy.deepcopy( classTemplateHtml ) )

			# process namespace files
			elif file.startswith( "namespace" ) :
				processNamespaceXmlFile( os.path.join( inPath, file ), os.path.join( outPath, filePrefix + ".html" ), copy.deepcopy( namespaceTemplateHtml ) )

			else :
				print "SKIPPING: " + file
				# TODO: base template and just iterate do an html iteration

def processHtmlDir(inPath, outPath):
	# process all html files in dirs in the html source dir that do not start with "_"
	processHtmlFile( inPath, outPath )
	
# def copyFiles( HTML_SOURCE_PATH, DOXYGEN_HTML_PATH ):
def copyFiles():
	src = HTML_SOURCE_PATH
	dest =  DOXYGEN_HTML_PATH

	try:
		copytree( src, dest, ignore = shutil.ignore_patterns("_templates*", "*.html") )
	except OSError as e:
		print( 'Directory not copied. Error: %s' % e)

# from http://stackoverflow.com/a/22331852/680667
def copytree(src, dst, symlinks = False, ignore = None):
	""" Copies all of the files from the source directory
		to a destination directory. Pass in anything that should be ignored.
	"""

	if not os.path.exists( dst ):
		os.makedirs(dst)
		shutil.copystat(src, dst)
	lst = os.listdir(src)
	
	# make list of files and directories minus the ignored stuff
	if ignore:
		excl = ignore(src, lst)
		lst = [x for x in lst if x not in excl]

	for item in lst:
		s = os.path.join(src, item)
		d = os.path.join(dst, item)

		if symlinks and os.path.islink(s):
			if os.path.lexists(d):
				os.remove(d)
				os.symlink(os.readlink(s), d)
			try:
				st = os.lstat(s)
				mode = stat.S_IMODE(st.st_mode)
				os.lchmod(d, mode)
			except:
				pass # lchmod not available
		elif os.path.isdir(s):
			copytree(s, d, symlinks, ignore)
		else:
			shutil.copy2(s, d)

if __name__ == "__main__":
	""" Main Function for generating html documentation from doxygen generated xml files
	
	Args:
		- 	Can pass in a single xml file to process by passing in path to xml file 
			and optionally, the resulting html file.
			if no out path is supplied, outputs to DOXYGEN_HTML_PATH
			Ex: python xmlToHtml.py xml/classcinder_1_1_surface_t.xml 
		- 	Can alternatively pass in a directory to process by providing the xml directory
			Ex: python xmlToHtml.py xml/ html/
	"""

	# Load tag file
	print "parsing tag file"
	tagXml = ET.ElementTree( ET.parse( "doxygen/cinder.tag" ).getroot() )
	# generate symbol map from tag file
	g_symbolMap = getSymbolToFileMap( tagXml )

	# construct full template out of multiple html templates
	classTemplateHtml = constructTemplate( ["headerTemplate.html", "mainNavTemplate.html", "cinderClassTemplate.html", "footerTemplate.html"] )
	namespaceTemplateHtml = constructTemplate( ["headerTemplate.html", "mainNavTemplate.html", "cinderNamespaceTemplate.html", "footerTemplate.html"] )

	# copy files from assets/ to html/
	# copy_tree("assets/", "html/")

	# copy all files from "htmlsrc" into "html" except .html file
	# allfiles = glob.iglob(os.path.join( HTML_SOURCE_PATH, "*.*") )
	# print allfiles
	# for file in allfiles:
		# print file
	#     if os.path.isfile(file):
	#         shutil.copy2(file, dest_dir)
	copyFiles()

	# generate namespace navigation
	g_namespaceNav = generateNamespaceNav( classTemplateHtml )

	inPath = sys.argv[1]
	# process a specific file
	if os.path.isfile( inPath ):
		processFile( inPath, sys.argv[2] if len( sys.argv ) > 2 else None )

	# process a directory
	elif os.path.isdir( inPath ): 
		if len( sys.argv ) == 3: 
			processDir( inPath, sys.argv[2] )
			processHtmlDir( HTML_SOURCE_PATH, sys.argv[2] )

	else:
		print "Unknown usage"
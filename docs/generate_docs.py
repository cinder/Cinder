#! /usr/bin/python
import sys
import os
import codecs
import shutil
from xml.dom.minidom import parse
from bs4 import BeautifulSoup

class SymbolMap (object):
	def __init__( self  ):
		self.classes = {}
		self.typedefs = {}

	class Class (object):
		def __init__( self, name, base, fileName ):
			self.name = name
			self.base = base
			self.fileName = fileName
		
	class Typedef (object):
		def __init__( self, type ):
			self.type = type
	
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

	def findTypedef( self, name ):
		return self.typedefs.get( name )

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


def getText(nodelist):
	rc = []
	for node in nodelist:
		if node.nodeType == node.TEXT_NODE:
			rc.append(node.data)
	return ''.join(rc)

# returns a dictionary from Cinder class name to file path
def getSymbolToFileMap( path ):	
	print "Parsing tag file: " + path
	f = open( path )
	tagDom = parse( f )
	
	# iterate all <compound>s looking for classes or namespaces
	symbolMap = SymbolMap()
	compounds = tagDom.getElementsByTagName( "compound" )
	for compound in compounds:
		# found a namespace
		if compound.getAttribute( "kind" ) == "namespace":
			namespaceName = getText( compound.getElementsByTagName("name")[0].childNodes )
			# we're only interested in cinder's namespaces
			if namespaceName.find( "cinder" ) != 0:
				print "Ignoring namespace " + namespaceName
				continue				
			else:
				print "Processing namespace " + namespaceName
			# iterate the namespace's symbols; we'll pull out member typedefs
			members = compound.getElementsByTagName("member")
			for member in members:
				if member.getAttribute( "kind" ) == "typedef":
					name = getText( member.getElementsByTagName("name")[0].childNodes )
					type = getText( member.getElementsByTagName("type")[0].childNodes )
					name = namespaceName+"::"+name
					type = namespaceName+"::"+type
					print "Adding " + name + " AS " + type
					symbolMap.typedefs[name] = SymbolMap.Typedef( type )
		# found a class
		elif compound.getAttribute( "kind" ) == "class":
			name = getText( compound.getElementsByTagName("name")[0].childNodes )
			filePath = getText( compound.getElementsByTagName("filename")[0].childNodes )
			baseClass = None
			if compound.getElementsByTagName("base"):
				baseClass = getText( compound.getElementsByTagName("base")[0].childNodes )
			symbolMap.classes[name] = SymbolMap.Class( name, baseClass, filePath )

	return symbolMap

# replace all <d> tags in HTML string 'html' based on Doxygen symbols
# in 'symbolMap', making the paths relative to 'doxyHtmlPath'
def processHtml( html, symbolMap, doxyHtmlPath ):
	soup = BeautifulSoup( html )
	for link in soup.find_all('d'):
		searchString = ''
		if link.get( 'dox' ) != None:
			searchString = link.get( 'dox' )
		else:
			searchString = link.contents[0]
		existingClass = symbolMap.findClass( searchString )
		if not existingClass:
			print "   ** Warning: Could not find Doxygen tag for " + searchString
		else:
			link.name = 'a'
			link['href'] = doxyHtmlPath + existingClass.fileName
			
	return soup.prettify()

def processHtmlFile( inPath, symbolMap, doxygenHtmlPath, outPath ):
	outFile = codecs.open( outPath, "w", "ISO-8859-1" )
	outFile.write( processHtml( codecs.open( inPath, "r", "ISO-8859-1" ), symbolMap, doxygenHtmlPath ) )

# walks all files in 'htmlSourceDir' and process in .html, copies/outputs to htmlOutDir 
def processHtmlDir( htmlSourceDir, symbolMap, doxygenHtmlPath ):
	for root, subFolders, files in os.walk( htmlSourceDir ):
		for fileName in files:
			inPath = os.path.join( root, fileName )
			relativePath = inPath[len(htmlSourceDir):] # path relative to 'htmlSourceDir'
			outPath = os.path.join( doxygenHtmlPath, relativePath )
			name,ext = os.path.splitext( fileName )
		
			# create destination directory if necessary
			outParentPath = os.path.abspath(os.path.join( outPath,"..") )
			if not os.path.exists( outParentPath ):
				os.makedirs( outParentPath )
			
			if ext == '.html':
				print "[" + outPath + "]"
				processHtmlFile( inPath, symbolMap, doxygenHtmlPath, outPath )
			else:
				print fileName + "->" + outPath
				shutil.copyfile( inPath, outPath )

def insertClassInheritances( symbolMap, doxygenHtmlPath ):
	for className in symbolMap.classes:
		ancestors = symbolMap.getClassAncestors( className )
		descendants = symbolMap.getClassDescendants( className )
		if not ancestors and not descendants:
			continue
		filePath = doxygenHtmlPath + symbolMap.classes[className].fileName
		soup = BeautifulSoup( codecs.open( filePath, "r", "ISO-8859-1" ) )
		contentsDiv = soup.find( 'div', attrs={"class","contents"} )
		unorderedList = soup.new_tag( "ul" )
		indentCount = 0
		for ancestor in ancestors:
			newLi = soup.new_tag( "li" )
			newA = soup.new_tag( "a", href=ancestor.fileName )
			newA.append( ancestor.name )
			newLi.string = u'-' * indentCount
			newLi.append( newA )
			unorderedList.append( newLi )
			indentCount = indentCount + 1
		for descendant in descendants:
			newLi = soup.new_tag( "li" )
			newA = soup.new_tag( "a", href=descendant.fileName )
			newA.append( descendant.name )
			newLi.string = u'+' * indentCount
			newLi.append( newA )
			unorderedList.append( newLi )
			
		print unorderedList
		contentsDiv.insert( 0, unorderedList )
		outFile = codecs.open( filePath, "w", "utf-8" )
		outFile.write( soup.prettify() )


def linkTemplatedTypedefs( symbolMap, doxygenHtmlPath ):
	classNameToTypedefs = {}
	# iterate all typedefs; we're focused on the templated ones
	# Ex: typedef SurfaceT<uint_t> Surface;
	#    resolvesTo = "SurfaceT<uint_t>"
	#    className = "SurfaceT"
	#    classInfo
	for typedef in symbolMap.typedefs:
		resolvesTo = symbolMap.typedefs[typedef].type
		firstLeftAngle = resolvesTo.find( "<" )
		# is this typedef's type templated?
		if firstLeftAngle != -1:
			className = resolvesTo[0:firstLeftAngle]
			# see if we have a class that matches the template
			classInfo = symbolMap.findClass( className )
			if classInfo: # found one
#					print symbol + " -> " + symbolMap[symbol][1] + " @ " + classInfo[1]
				if not classNameToTypedefs.get( className ):
					classNameToTypedefs[className] = []
				classNameToTypedefs[className].append( typedef )
			else:
				print "No info for " + className
					
	# Iterate the templated classes for which we have typedefs
	for className in classNameToTypedefs:
		classInfo = symbolMap.classes.get( className )
		if classInfo:
			filePath = doxygenHtmlPath + classInfo.fileName
			print "Adding " + str(classNameToTypedefs[className]) + " to file " + filePath
			soup = BeautifulSoup( codecs.open( filePath, "r", "ISO-8859-1" ) )
			contentsDiv = soup.find( 'div', attrs={"class","contents"} )
			newContents = "Typedefs:"
			for typedef in classNameToTypedefs[className]:
				newContents += "[" + typedef + "]"
			newTag = soup.new_tag( "b" )
			newTag.append( newContents )
			contentsDiv.insert( 0, newTag )			
			outFile = codecs.open( filePath, "w", "utf-8" )
			outFile.write( soup.prettify() )
		else:
			print "Error: couldn't find class: " + className



if __name__ == "__main__":
	doxygenHtmlPath = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'html' + os.sep
	htmlSourcePath = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'htmlsrc' + os.sep

	symbols = SymbolMap()

	if len( sys.argv ) == 1: # default; generate all docs
		symbolMap = getSymbolToFileMap( "doxygen/cinder.tag" )
		insertClassInheritances( symbolMap, doxygenHtmlPath )
		linkTemplatedTypedefs( symbolMap, doxygenHtmlPath )
		processHtmlDir( htmlSourcePath, symbolMap, doxygenHtmlPath )
	elif len( sys.argv ) == 3: # process a specific file
		symbolMap = getSymbolToFileMap( "doxygen/cinder.tag" )
		processHtmlFile( sys.argv[1], symbolMap, doxygenHtmlPath, sys.argv[2] )
	else:
		print "Unknown usage"
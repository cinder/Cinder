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

# returns a dictionary from Cinder class name to file path
def getSymbolToFileMap( path ):	
	print "Parsing tag file: " + path
	f = open( path )
	tagDom = parse( f )
	
	# iterate all <compound>s looking for classes or namespaces
	symbolMap = {}
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
					symbolMap[name] = ("typedef",type)
		# found a class
		elif compound.getAttribute( "kind" ) == "class":
			name = getText( compound.getElementsByTagName("name")[0].childNodes )
			filePath = getText( compound.getElementsByTagName("filename")[0].childNodes )
			symbolMap[name] = ("class",filePath)

	return symbolMap

# searches the symbolMap for a given symbol, prepending cinder:: if not found as-is
def getFilePathForSymbol( symbolMap, symbolName ):
	# replace ci:: with cinder::
	if symbolName.find( "ci::" ) == 0:
		symbolName.replace( "ci::", "cinder::" )
	# if starst with 'cinder::'
	if symbolName.find( "ci::" ) == 0:
		return symbolMap[symbolName][1]
	elif symbolName in symbolMap:
		return symbolMap[symbolName][1]
	elif ("cinder::" + symbolName) in symbolMap:
		return symbolMap["cinder::"+symbolName][1]
	else:
		return None

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
		fileName = getFilePathForSymbol( symbolMap, searchString )
		if fileName == None:
			print "   ** Warning: Could not find Doxygen tag for " + searchString
		else:
			link.name = 'a'
			link['href'] = doxyHtmlPath + fileName
			
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

def linkTemplatedTypedefs( symbolMap, doxygenHtmlPath ):
	classNameToTypedefs = {}
	for symbol in symbolMap:
		# iterate all typedefs; we're focused on the templated ones
		if symbolMap[symbol][0] == "typedef":
			resolvesTo = symbolMap[symbol][1]
			firstLeftAngle = resolvesTo.find( "<" )
			# is this typedef's type templated?
			if firstLeftAngle != -1:
				className = resolvesTo[0:firstLeftAngle]
				# see if we have a class that matches the template
				classInfo = symbolMap.get( className )
				if classInfo != None: # found one
#					print symbol + " -> " + symbolMap[symbol][1] + " @ " + classInfo[1]
					if classNameToTypedefs.get( className ) == None:
						classNameToTypedefs[className] = [symbol]
					else:
						classNameToTypedefs[className].append( symbol )
				else:
					print "No info for " + className
					
	# Iterate the templated classes for which we have typedefs
	for className in classNameToTypedefs:
		classInfo = symbolMap.get( className )
		if classInfo != None:
			filePath = doxygenHtmlPath + classInfo[1]
			print "Adding " + str(classNameToTypedefs[className]) + " to file " + filePath
			soup = BeautifulSoup( codecs.open( filePath, "r", "ISO-8859-1" ) )
			contents = soup.find( 'div', attrs={"class","contents"} )
			newContents = "Typedefs:"
			for typedef in classNameToTypedefs[className]:
				newContents += "[" + typedef + "]"
			newTag = soup.new_tag( "b" )
			newTag.append( newContents )
			contents.insert( 0, newTag )			
			outFile = codecs.open( filePath, "w", "ISO-8859-1" )
			outFile.write( soup.prettify() )
		else:
			print "Error: couldn't find class: " + className

doxygenHtmlPath = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'html' + os.sep
htmlSourcePath = os.path.dirname( os.path.realpath(__file__) ) + os.sep + 'htmlsrc' + os.sep

if len( sys.argv ) == 1: # default; generate all docs
	symbolMap = getSymbolToFileMap( "doxygen/cinder.tag" )
	linkTemplatedTypedefs( symbolMap, doxygenHtmlPath )
	processHtmlDir( htmlSourcePath, symbolMap, doxygenHtmlPath )
elif len( sys.argv ) == 3: # process a specific file
	symbolMap = getSymbolToFileMap( "doxygen/cinder.tag" )
	processHtmlFile( sys.argv[1], symbolMap, doxygenHtmlPath, sys.argv[2] )
else:
	print "Unknown usage"
# expects 1 arg
# python findreplace.py <path>
# this doesn't use xml parsing directly because Python won't write XML with the same quirks VC does
import os
import fileinput
import sys

from os.path import join, getsize

def getTagContents( line, tag ):
	line = line.replace( "&quot;", "" )
	line = line.replace( "\"", "" )
	rawTag = tag.replace( "<", "" ).replace( ">", "" )
	start = line.find( tag ) + len( tag )
	end = line.find( "</" + rawTag + ">" )
	return line[start:end]

def replaceTagContents( line, tag, contents ):
	rawTag = tag.replace( "<", "" ).replace( ">", "" )
	start = line.find( tag ) + len( tag )
	end = line.find( "</" + rawTag + ">" )
	return line[0:start] + contents + line[end:]
	
def replaceSemiSeparatedList( line, tag, find, replace ):
	if line.find( tag ) != -1:
		elements = getTagContents( line, tag ).split( ";" )
		for i in range( 0, len( elements ) ):
			elements[i] = elements[i].replace( find, replace )
		return replaceTagContents( line, tag, ";".join( elements ) )
	else:
		return line	

def appendSemiSeparatedList( line, tag, addition ):
	if line.find( tag ) != -1:
		elements = getTagContents( line, tag ).split( ";" )
		elements.append( addition )
		return replaceTagContents( line, tag, ";".join( elements ) )
	else:
		return line	

# searches 'line' for 'tag'; if found, searches for 'find' in tag contents; if found, returns found+suffix, else returns line
def appendSuffixedCopySemiSeparatedList( line, tag, find, suffix ):
	if line.find( tag ) != -1:
		elements = getTagContents( line, tag ).split( ";" )
		found = ""
		for i in range( 0, len( elements ) ):
			if elements[i].find( find ) != -1:
				found = elements[i]
				break
		if found:
			elements.append( found + suffix )
			return replaceTagContents( line, tag, ";".join( elements ) )
		else:
			return line
	else:
		return line	
		
def replaceStaticLib( line, find, replace ):
	return replaceSemiSeparatedList( line, "<AdditionalDependencies>", find, replace )
		
def replaceLibDir( line, find, replace ):
	return replaceSemiSeparatedList( line, "<AdditionalLibraryDirectories>", find, replace )

def appendSuffixedCopyLibDir( line, find, addition ):
	return appendSuffixedCopySemiSeparatedList( line, "<AdditionalLibraryDirectories>", find, addition )
	
def processLine( line ):
	#return replaceLibDir( line, r"..\..\..\\lib\msw\$(PlatformTarget)", r"..\..\..\lib\msw\$(PlatformTarget)" )
	return appendSuffixedCopyLibDir( line, r"..\..\..\lib\winrt\$(PlatformTarget)", "$(Configuration)\\" )
	#return replaceStaticLib( line, "cinder-$(PlatformToolset)_d.lib", "cinder-$(PlatformToolset).lib" )
	#return replaceStaticLib( line, "cinder-$(PlatformToolset)-ANGLE_d.lib", "cinder-$(PlatformToolset)-ANGLE.lib" )
	#return replaceStaticLib( line, "cinder-$(PlatformToolset)-ANGLE.lib", "cinder-$(PlatformToolset).lib" )

for root, dirs, files in os.walk(sys.argv[1]):
    for name in files:
        if name[name.rfind("."):] != ".vcxproj":
            continue
        f = open( join(root, name), "r" )
        fileLines = f.readlines(10000000)
        f.close()
        f = open( join(root, name), "w" )
        f.seek( 0, os.SEEK_SET )
        for line in fileLines:
            replaced = processLine( line )
            if replaced != line:
                print "Replaced line in " + str(join(root,name))
                print replaced
            f.write( replaced )

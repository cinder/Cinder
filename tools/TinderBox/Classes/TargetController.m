//
//  TargetController.m
//  TinderBox
//
//  Created by andrewfb on 2/7/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "TargetController.h"
#import "AppController.h"

@implementation TargetController

- (id)initWithApp:(AppController *)aApp nibName:(NSString *)nibName
{
	if( ! [super initWithNibName:nibName bundle:nil] )
		return nil;
	
	app = aApp;
	[self setTitle:nibName];	
	[self enumerateTemplates];
	
	return self;
}

- (void)dealloc
{
	[templatesArray release];
	[super dealloc];
}

// Default conditionals list is empty
- (NSMutableDictionary *)prepareConditionals
{
	return [[[NSMutableDictionary alloc] init] autorelease];
}

- (NSString *)makePathRelative:(NSString *)path to:(NSString *)relativeTo
{
	NSString *result = @"";
	path = [path stringByStandardizingPath];
	relativeTo = [relativeTo stringByStandardizingPath];
	// determine the common prefix
	NSString *commonPrefix = [path commonPrefixWithString:relativeTo options:0]; 
	// remove it from both paths
	relativeTo = [relativeTo substringFromIndex:[commonPrefix length]];
	path = [path substringFromIndex:[commonPrefix length]];
	// result is '../' * number of directories in relativeTo + (path - commonPrefix)
	while( [relativeTo length] > 1 ) {
		relativeTo = [relativeTo stringByDeletingLastPathComponent];
		result = [result stringByAppendingPathComponent:@".."];
	}
	
	result = [result stringByAppendingPathComponent:path];

	return result;
}

- (NSMutableDictionary *)prepareDefaultReplacementDictionary
{
	NSMutableDictionary *result = [[NSMutableDictionary alloc] init];
	
	NSString *projectPath = [[app projectLocation] stringByAppendingPathComponent:[app projectName]];
	NSString *relativeFlintPath = [self makePathRelative:[app flintPath] to:projectPath];
	
	[result setObject:[app projectName] forKey:@"«PROJECTNAME»"];
	[result setObject:[app namingPrefix] forKey:@"«PREFIX»"];
	[result setObject:relativeFlintPath forKey:@"«FLINTPATH»"];
	
	NSString *winFlintPath = [[[app winPrefix] stringByAppendingString:relativeFlintPath] stringByReplacingOccurrencesOfString:@"/" withString:@"\\"];
	[result setObject:winFlintPath forKey:@"«WINFLINTPATH»"];
	
	[result autorelease];
	return result;
}

- (NSString *)replaceStrings:(NSString *)s forDictionary:(NSMutableDictionary *)dict
{
	NSString *result = s;
	for( NSString *key in [dict allKeys] ) {
		result = [result stringByReplacingOccurrencesOfString:key withString:[dict valueForKey:key]];
	}
	
	return result;
}

- (NSString *)stringReplacePath:(NSString *)dir file:(NSString*)file dict:(NSMutableDictionary *)replacementDictionary
{
	NSString *srcPath = [dir stringByAppendingPathComponent:file];
	NSString *destFileName = [self replaceStrings:file forDictionary:replacementDictionary];
	NSString *destPath = [dir stringByAppendingPathComponent:destFileName];
	if( [destPath compare:srcPath] != NSOrderedSame ) {
		BOOL success = [[NSFileManager defaultManager] movePath:srcPath toPath:destPath handler:nil];
		if( ! success )
			NSLog( @"Error moving!" );
	}
	
	return destPath;
}

- (BOOL)startsWithFalseConditional:fileName conditionals:(NSMutableDictionary *)conditionals
{
	for( NSString *key in [conditionals allKeys] ) {
		NSRange range = [fileName rangeOfString:key];
		if( range.location == 0 ) {
			NSNumber *num = [conditionals objectForKey:key];
			if( ! [num boolValue] )
				return true;
		}
	}
	
	return false;
}

- (void)stringReplaceDirectory:(NSString *)dir dict:(NSMutableDictionary *)replacementDictionary conditionals:(NSMutableDictionary *)conditionals
{
	NSString *file;
	BOOL isDir;
	NSDirectoryEnumerator *dirEnum = [[NSFileManager defaultManager] enumeratorAtPath:dir];
	while( file = [dirEnum nextObject] ) {
		[dirEnum skipDescendents];
		
		// if we have a conditional prefix, and that conditional is false, skip this
		if( [self startsWithFalseConditional:file conditionals:conditionals] )
			continue;
		
		if( [[NSFileManager defaultManager] fileExistsAtPath:[dir stringByAppendingPathComponent:file] isDirectory:&isDir] && isDir ) {
			// recurse into yourself if you're a directory
			[self stringReplaceDirectory:[dir stringByAppendingPathComponent:file] dict:replacementDictionary conditionals:conditionals];
			// then rename yourself
			[self stringReplacePath:dir file:file dict:replacementDictionary];
		}
		else { // file, not a directory
			NSString *destPath = [self stringReplacePath:dir file:file dict:replacementDictionary];
			// replace strings on the contents of this file
			NSString *destContents = [self replaceStrings:[NSString stringWithContentsOfFile:destPath encoding:NSUTF8StringEncoding error:NULL] forDictionary:replacementDictionary];
			[destContents writeToFile:destPath atomically:NO encoding:NSUTF8StringEncoding error:NULL];
		}
	}
}

- (void)conditionalRecursiveCopy:(NSString*)dir toPath:(NSString *)toPath withConditionals:(NSMutableDictionary *)conditionals
{
	// Start by making the directory itself
	[[NSFileManager defaultManager] createDirectoryAtPath:toPath attributes:nil];

	NSString *item;
	BOOL isDir;
	NSDirectoryEnumerator *dirEnum = [[NSFileManager defaultManager] enumeratorAtPath:dir];
	while( item = [dirEnum nextObject] ) {
		[dirEnum skipDescendents];
		
		// if we have a conditional prefix, and that conditional is false, skip this
		if( [self startsWithFalseConditional:item conditionals:conditionals] )
			continue;
		
		if( [[NSFileManager defaultManager] fileExistsAtPath:[dir stringByAppendingPathComponent:item] isDirectory:&isDir] && isDir ) {
			NSString *srcDir = [dir stringByAppendingPathComponent:item];
			NSString *toDir = [toPath stringByAppendingPathComponent:item];
			// recurse into yourself
			[self conditionalRecursiveCopy:srcDir toPath:toDir withConditionals:conditionals];
		}
		else { // file, not a directory
			[[NSFileManager defaultManager] copyPath:[dir stringByAppendingPathComponent:item] toPath:[toPath stringByAppendingPathComponent:item] handler:nil];
		}
	}
}

- (void)createProject
{
	NSMutableDictionary *replacementDictionary = [self prepareDefaultReplacementDictionary];
	// [self prepareReplacementDictionary];

	NSString *srcDir = [[app getTargetTemplatesDir:[self title]] stringByAppendingPathComponent:[templatesArray objectAtIndex:[app getTemplateIndex]]];
	NSString *projectName = [app projectName];
	NSString *expandedProjectLocation = [[app projectLocation] stringByExpandingTildeInPath];
	NSString *projectPath = [expandedProjectLocation stringByAppendingPathComponent:projectName];

	// let's see if the destination already exists
	BOOL isDirectory;
	if( ! ( [[NSFileManager defaultManager] fileExistsAtPath:expandedProjectLocation isDirectory:&isDirectory] ) ) {
		int choice = NSRunAlertPanel( @"Directory Does Not Exist", @"The directory \"%@\" does not exist. Do you want to create it?", @"Create", @"Cancel", nil, [app projectLocation] );
		if( choice == NSAlertDefaultReturn ) {
			NSError *error;
			[[NSFileManager defaultManager] createDirectoryAtPath:expandedProjectLocation withIntermediateDirectories:YES attributes:nil error:&error];
		}
		else {
			return;
		}
	}
	else { // and make sure the destination isn't actually a file
		if( ! isDirectory ) {
			NSString *str = [NSString stringWithFormat:@"The path \"%@\" already exists and is not a directory", [app projectLocation]];
			NSAlert *alert = [NSAlert alertWithMessageText:@"Path is a file" defaultButton:@"OK" alternateButton:nil otherButton:nil informativeTextWithFormat:str];
			[alert runModal];
			return;
		}
	}

	// Get this target's conditionals
	NSMutableDictionary *conditionals = [self prepareConditionals];

	// first, copy the template to the destination based on the conditionals
	NSLog( @"Copying %@ to %@", srcDir, projectPath );
	[self conditionalRecursiveCopy:srcDir toPath:projectPath withConditionals:conditionals];
	
//	[[NSFileManager defaultManager] copyPath:srcDir toPath:projectPath handler:nil];

	// Add each conditional to the replacement dictionary with a blank string as the replace value
	// so that we can remove all the prefixes automatically
	for( NSString *conditionalsKey in [conditionals allKeys] )
		[replacementDictionary setObject:@"" forKey:conditionalsKey];

	// Recursive string replace on destination file names and contents
	[self stringReplaceDirectory:projectPath dict:replacementDictionary conditionals:conditionals];
	
	// Open the Finder at the resulting directory
	[[NSWorkspace sharedWorkspace] selectFile:projectPath inFileViewerRootedAtPath:nil];
}

- (void)enumerateTemplates
{
	NSString *file;
	NSString *dir = [app getTargetTemplatesDir:[self title]];
	NSDirectoryEnumerator *dirEnum = [[NSFileManager defaultManager] enumeratorAtPath:dir];

	templatesArray = [[NSMutableArray alloc] init];

	while( file = [dirEnum nextObject] ) {
		[dirEnum skipDescendents];
		NSString *templateName = [dir stringByAppendingPathComponent:file];
		NSLog( @"Adding %@", templateName );
		[templatesArray addObject:file];
	}
	
	NSLog( @"Total templates: %d", [templatesArray count] );
}

- (NSMutableArray *)templates
{
	return templatesArray;
}

@end

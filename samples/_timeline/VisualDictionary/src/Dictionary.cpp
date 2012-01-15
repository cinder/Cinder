/*
 * Code Copyright 2011 Robert Hodgin ( http://roberthodgin.com ) and Andrew Bell ( http://drawnline.net )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 */

#include "cinder/Buffer.h"
#include "cinder/Utilities.h"
#include "cinder/Stream.h"

#include <iostream>
#include <algorithm>
#include <set>
#include <string.h>

#include "Dictionary.h"

using namespace std;
using namespace ci;

Dictionary::Dictionary( DataSourceRef dataSource )
{
	Buffer decompressed = decompressBuffer( Buffer( dataSource ), false, true );
	
	// read out all the words, which are separated by lines and are already alphabetized
	IStreamMemRef stream = IStreamMem::create( decompressed.getData(), decompressed.getDataSize() );
	while( ! stream->isEof() )
		mWords.push_back( stream->readLine() );
}

// Functor used to determine whether the prefix of length 'compareLength' of two words are the same
struct CompareStringPrefix {
	CompareStringPrefix( int compareLength ) : mCompareLength( compareLength ){}
	
	bool operator()( string test1, string test2 ) const {
		return strncmp( test1.c_str(), test2.c_str(), mCompareLength ) < 0;
	}
	
	int mCompareLength;
};

vector<string> Dictionary::getDescendants( const string &word ) const
{
	vector<string> result;
	set<char> foundLetters;

	// we want to figure out what letters follow 'word'
	// so for "victor" we want 'i' (victories), 's' (victors), 'y' (victory)
	pair<vector<string>::const_iterator, vector<string>::const_iterator> range;
	range = std::equal_range( mWords.begin(), mWords.end(), word, CompareStringPrefix( word.size() ) );

	// iterate all the words in our range, and add their last letter to our set of 'foundLetters'	
	for( vector<string>::const_iterator wordIt = range.first; wordIt != range.second; ++wordIt ) {
		if( wordIt->size() > word.size() )
			foundLetters.insert( (*wordIt)[word.size()] );
	}
	
	// now iterate all the foundLetters; each result word will be 'word' + foundLetter
	// ex: victor + i (victories)
	for( set<char>::const_iterator letIt = foundLetters.begin(); letIt != foundLetters.end(); ++letIt )
		result.push_back( word + *letIt );
	
	return result;
}

bool Dictionary::isCompleteWord( const std::string &word ) const
{
	vector<string>::const_iterator lower = std::lower_bound( mWords.begin(), mWords.end(), word );
	return ( lower != mWords.end() ) && *lower == word;
}

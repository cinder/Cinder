/*
 * Code Copyright 2011 Robert Hodgin ( http://roberthodgin.com ) and Andrew Bell ( http://drawnline.net )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 */

#pragma once

#include "cinder/DataSource.h"

#include <vector>

class Dictionary {
  public:
	Dictionary( ci::DataSourceRef dictionaryFile );
	
	// Returns a vector<> of words that share 'word' as a prefix
	std::vector<std::string>	getDescendants( const std::string &word ) const;

	// Returns whether a given word is in the dictionary
	bool			isCompleteWord( const std::string &word ) const;

  private:	
	std::vector<std::string>	mWords;
};
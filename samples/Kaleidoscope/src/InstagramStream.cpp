//
//	InstagramStream.cpp
//	InstaScope
//
//	Created by Greg Kepler on 6/9/12.
//	Copyright (c) 2012 The Barbarian Group. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
#include "cinder/Function.h"
#include "cinder/Json.h"
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"

#include "InstagramStream.h"

static const int BUFFER_SIZE = 10;

using namespace std;
using namespace ci;
using namespace ci::app;

static const string INSTAGRAM_API_URL = "https://api.instagram.com/v1";

JsonTree queryInstagram( const std::string &query );


InstagramStream::InstagramStream(const string &clientId )
: mBuffer( BUFFER_SIZE ), // our buffer of instagrams can hold up to 10
mCanceled( false ),
mClientId( clientId )
{
	startThread(INSTAGRAM_API_URL + "/media/popular?client_id="+ mClientId);
}

InstagramStream::InstagramStream( const std::string &searchPhrase, const string &clientId )
: mBuffer( BUFFER_SIZE ), // our buffer of instagrams can hold up to 10
mCanceled( false ),
mSearchPhrase( searchPhrase ),
mClientId( clientId )
{
	startThread(INSTAGRAM_API_URL + "/tags/" + Url::encode( mSearchPhrase ) + "/media/recent?client_id="+ mClientId);
}

InstagramStream::InstagramStream(const std::string &searchPhrase, const int &minId, const int &maxId, const std::string &clientId )
: mBuffer( BUFFER_SIZE ), // our buffer of instagrams can hold up to 10
mCanceled( false ),
mSearchPhrase( searchPhrase ),
mClientId( clientId )
{
	startThread(INSTAGRAM_API_URL + "/tags/" + Url::encode( mSearchPhrase ) + "/media/recent?client_id="+ mClientId + "&min_id=" + toString(minId) + "&max_id=" + toString(maxId));
}


InstagramStream::InstagramStream(ci::Vec2f loc, float dist, int minTs, int maxTs, std::string clientId )
: mBuffer( BUFFER_SIZE ), // our buffer of instagrams can hold up to 10
mCanceled( false ),
mClientId( clientId )
{
	startThread(INSTAGRAM_API_URL + "/media/search?lat=" + toString(loc.x) + "&lng=" + toString(loc.y) + "&distance=" + toString(dist) + "&min_timestamp=" + toString(minTs) + "&max_timestamp=" + toString(maxTs) + "&client_id="+ mClientId);
}

InstagramStream::InstagramStream(ci::Vec2f loc, float dist, std::string clientId )
: mBuffer( BUFFER_SIZE ), // our buffer of instagrams can hold up to 10
mCanceled( false ),
mClientId( clientId )
{
	startThread(INSTAGRAM_API_URL + "/media/search?lat=" + toString(loc.x) + "&lng=" + toString(loc.y) + "&distance=" + toString(dist) + "&client_id="+ mClientId);
}

InstagramStream::InstagramStream(ci::Vec2f loc, std::string clientId )
: mBuffer( BUFFER_SIZE ), // our buffer of instagrams can hold up to 10
mCanceled( false ),
mClientId( clientId )
{
	startThread(INSTAGRAM_API_URL + "/media/search?lat=" + toString(loc.x) + "&lng=" + toString(loc.y) + "&client_id="+ mClientId);
}

void InstagramStream::startThread(string url){
	mThread = make_shared<thread>( bind( &InstagramStream::serviceGrams, this, url ) );
}

InstagramStream::~InstagramStream()
{
	mCanceled = true;
	mBuffer.cancel();
	mThread->join();
}

// Non-autenticated queries
// X	search for recent images of a certain tag
// X	get popular images	
// _	search for users with by user id							https://api.instagram.com/v1/users/search?q=userId&client_id=xxx
// _	get info about a user										https://api.instagram.com/v1/users/1574083/?client_id=xxx
// _	see who a user follows										https://api.instagram.com/v1/users/1574083/follows?client_id=xxx
// _	see who a user follows										https://api.instagram.com/v1/users/1574083/followed-by?client_id=xxx
// _	Get information about a media object.						https://api.instagram.com/v1/media/3/?client_id=xxx
// X	Search for media in a given area.							https://api.instagram.com/v1/media/search?lat=48.858844&lng=2.294351&client_id=xxx
// _	Get comments of a speficic media							https://api.instagram.com/v1/media/555/comments?client_id=xxx
// _	Get likes of a speficic media								https://api.instagram.com/v1/media/555/likes?client_id=xxx
// _	Get information about a tag object.							https://api.instagram.com/v1/tags/nofilter?client_id=xxx
// _	Get information about a location							https://api.instagram.com/v1/locations/1?client_id=xxx
// _	Get a list of recent media objects from a given location.	https://api.instagram.com/v1/tags/snow/media/recent?client_id=xxx
// _	Search for a location by geographic coordinate.				https://api.instagram.com/v1/locations/search?lat=48.858844&lng=2.294351&client_id=xxx


// Function the background thread lives in
void InstagramStream::serviceGrams(string url)
{
	ThreadSetup threadSetup;
	std::string nextQueryString = url;
	
	JsonTree searchResults;
	JsonTree::ConstIter resultIt = searchResults.end();
	
	// This function loops until the app quits. Each iteration a pulls out the next result from the Twitter API query.
	// When it reaches the last result of the current query it issues a new one, based on the "refresh_url" property
	// of the current query.
	// The loop doesn't spin (max out the processor) because ConcurrentCircularBuffer.pushFront() non-busy-waits for a new
	// slot in the circular buffer to become available.
	JsonTree queryResult;
	while( ! mCanceled ) {
		if( resultIt == searchResults.end() ) {			// are we at the end of the results of this JSON query?
			// issue a new query
			try {
				queryResult = queryInstagram( nextQueryString );
				// the next query will be the "refresh_url" of this one.
				
				try {
					nextQueryString = queryResult["pagination"].getChild("next_url").getValue();
				}
				catch(...) {
					
				}
				
				searchResults = queryResult.getChild("data");
				resultIt = searchResults.begin();
				mIsConnected = true;
			}
			catch( ... ) {
				
				console() << "something broke" << endl;
				console() << queryResult << endl;
				console() << nextQueryString << endl;
				
				// check if it's a 420 error
				if(queryResult.getChild("meta").getChild("code").getValue() == "420"){
					console() << "420 error" << endl;
					mIsConnected = false;
				}
				
				ci::sleep( 1000 ); // try again in 1 second
			}
		}
		if( resultIt != searchResults.end() ) {
			try {
				
				string userName = (*resultIt)["user"]["username"].getValue();
				
				// get the URL and load this instagram image
				string imageUrl = (*resultIt)["images"]["standard_resolution"]["url"].getValue();
				Surface image( loadImage( loadUrl( imageUrl ) ) );
				// string imageUrl = "http://distilleryimage5.s3.amazonaws.com/1dd174cca14611e1af7612313813f8e8_7.jpg"; // Test image
				mBuffer.pushFront( Instagram( userName, imageUrl, image ) );
			}
			catch( ... ) { // just ignore any errors
				console() << "ERRORS FOUND" << endl;
			}
			++resultIt;
		}
	}
}

JsonTree queryInstagram( const std::string &searchUrl )
{
	Url url(searchUrl , true );
	return JsonTree( loadUrl( url ) );
}

bool InstagramStream::hasInstagramAvailable()
{
	return mBuffer.isNotEmpty();
}

Instagram InstagramStream::getNextInstagram()
{
	Instagram result;
    mBuffer.popBack( &result );
	return result;
}

bool InstagramStream::isConnected()
{
	return mIsConnected;
}
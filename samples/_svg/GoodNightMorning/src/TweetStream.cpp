#include "TweetStream.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
#include "cinder/Function.h"
#include "cinder/Json.h"
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;

string replaceEscapes( const string &original );
JsonTree queryTwitter( const std::string &query );


TweetStream::TweetStream( const std::string &searchPhrase )
	: mBuffer( 10 ), // our buffer of tweets can hold up to 10
	mCanceled( false ),
	mSearchPhrase( searchPhrase )
{
	mThread = thread( bind( &TweetStream::serviceTweets, this ) );
}

TweetStream::~TweetStream()
{
	mCanceled = true;
	mBuffer.cancel();
	mThread.join();
}

// Function the background thread lives in
void TweetStream::serviceTweets()
{
	ThreadSetup threadSetup;
	std::string nextQueryString = "?q=" + Url::encode( mSearchPhrase );
	JsonTree searchResults;
	JsonTree::ConstIter resultIt = searchResults.end();

	// This function loops until the app quits. Each iteration a pulls out the next result from the Twitter API query.
	// When it reaches the last result of the current query it issues a new one, based on the "refresh_url" property
	// of the current query.
	// The loop doesn't spin (max out the processor) because ConcurrentCircularBuffer.pushFront() non-busy-waits for a new
	// slot in the circular buffer to become available.
	while( ! mCanceled ) {
		if( resultIt == searchResults.end() ) { 		// are we at the end of the results of this JSON query?
			// issue a new query
			try {
				JsonTree queryResult = queryTwitter( nextQueryString );
				// the next query will be the "refresh_url" of this one.
				nextQueryString = queryResult["refresh_url"].getValue();
				searchResults = queryResult.getChild( "results" );
				resultIt = searchResults.begin();
			}
			catch( ci::Exception &exc ) {
				// our API query failed: put up a "tweet" with our error
				CI_LOG_W( "exception caught parsing query: " << exc.what() );
				mBuffer.pushFront( Tweet( "Twitter API query failed", "sadness", SurfaceRef() ) );
				ci::sleep( 2000 ); // try again in 2 seconds
			}
		}
		if( resultIt != searchResults.end() ) {
			try {
				// get the URL and load the image for this profile
				Url profileImgUrl = (*resultIt)["profile_image_url"].getValue<Url>();
				SurfaceRef userIcon = Surface::create( loadImage( loadUrl( profileImgUrl ) ) );
				// pull out the text of the tweet and replace any XML-style escapes
				string text = replaceEscapes( (*resultIt)["text"].getValue() );
				string userName = (*resultIt)["from_user"].getValue();
				mBuffer.pushFront( Tweet( text, userName, userIcon ) );
			}
			catch( ci::Exception &exc ) {
				CI_LOG_W( "exception caught parsing search results: " << exc.what() );
			}
			++resultIt;
		}
	}
}

JsonTree queryTwitter( const std::string &query )
{
	Url url( "http://search.twitter.com/search.json" + query + "&rpp=10&result_type=recent", true );
	return JsonTree( loadUrl( url ) );
}

void findAndReplaceAll( std::string& data, const std::string &toSearch, const std::string &replaceStr )
{
	size_t pos = data.find(toSearch);
	while( pos != std::string::npos ) {
		data.replace(pos, toSearch.size(), replaceStr);
		pos = data.find(toSearch, pos + replaceStr.size());
	}
}

// Replaces XML escaped characters with their equivalents
string replaceEscapes( const string &original )
{
	string result = original;
	findAndReplaceAll( result, "&lt;", "<" );
	findAndReplaceAll( result, "&gt;", ">" );
	findAndReplaceAll( result, "&amp;", "&" );
	return result;
}

bool TweetStream::hasTweetAvailable()
{
	return mBuffer.isNotEmpty();
}

Tweet TweetStream::getNextTweet()
{
	Tweet result;
	mBuffer.popBack( &result );
	return result;
}
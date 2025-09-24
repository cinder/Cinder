#include "MastadonHashtagStream.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
#include "cinder/Function.h"
#include "cinder/Json.h"
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"
#include "cinder/Log.h"
#include "cinder/Url.h"
#include "cinder/Utilities.h"

using namespace std;
using namespace ci;

string stripHtmlTags( const string& htmlText );
Json   queryMastodon( const std::string& hashtag, const std::string& sinceId = "" );

MastadonHashtagStream::MastadonHashtagStream( const std::string& hashtag )
	: mBuffer( 10 )
	, // our buffer of tweets can hold up to 10
	mCanceled( false )
	, mHashtag( hashtag )
{
	mThread = thread( bind( &MastadonHashtagStream::serviceTweets, this ) );
}

MastadonHashtagStream::~MastadonHashtagStream()
{
	mCanceled = true;
	mBuffer.cancel();
	mThread.join();
}

void MastadonHashtagStream::serviceTweets()
{
	ThreadSetup threadSetup;

	// Loop until the app quits, fetching new posts from Mastodon
	while( ! mCanceled ) {
		try {
			Json posts = queryMastodon( mHashtag, mLastPostId );

			// Process posts in reverse order to maintain chronological order in buffer
			for( auto it = posts.rbegin(); it != posts.rend(); ++it ) {
				if( mCanceled )
					break;

				try {
					const Json& post = *it;

					// Extract the data we need
					string content = stripHtmlTags( post["content"].get<string>() );
					string username = post["account"]["username"].get<string>();
					string avatarUrl = post["account"]["avatar"].get<string>();

					// Load the avatar image
					SurfaceRef userIcon;
					try {
						userIcon = Surface::create( loadImage( loadUrl( Url( avatarUrl ) ) ) );
					}
					catch( const Exception& exc ) {
						CI_LOG_W( "Failed to load avatar image: " << exc.what() );
						userIcon = SurfaceRef(); // null icon
					}

					// Create and add the tweet
					mBuffer.pushFront( Tweet( content, username, userIcon ) );

					// Update last post ID for pagination
					if( posts.size() > 0 && it == posts.rbegin() ) {
						mLastPostId = post["id"].get<string>();
					}
				}
				catch( const Exception& exc ) {
					CI_LOG_W( "Exception parsing individual post: " << exc.what() );
				}
			}
		}
		catch( const Exception& exc ) {
			CI_LOG_W( "Exception fetching Mastodon posts: " << exc.what() );
			// Put an error message in the stream
			mBuffer.pushFront( Tweet( "Mastodon API query failed", "error", SurfaceRef() ) );
		}

		// Wait before next query (Mastodon rate limits are more generous than Twitter's old API)
		if( ! mCanceled ) {
			ci::sleep( 5000 ); // wait 5 seconds between requests
		}
	}
}

Json queryMastodon( const std::string& hashtag, const std::string& sinceId )
{
	string urlStr = "https://mastodon.social/api/v1/timelines/tag/" + Url::encode( hashtag ) + "?limit=20";
	if( ! sinceId.empty() ) {
		urlStr += "&since_id=" + Url::encode( sinceId );
	}

	Url	   url( urlStr );
	string jsonString = loadString( loadUrl( url ) );
	return Json::parse( jsonString );
}

void findAndReplaceAll( std::string& data, const std::string& toSearch, const std::string& replaceStr )
{
	size_t pos = data.find( toSearch );
	while( pos != std::string::npos ) {
		data.replace( pos, toSearch.size(), replaceStr );
		pos = data.find( toSearch, pos + replaceStr.size() );
	}
}

string stripHtmlTags( const string& htmlText )
{
	string result = htmlText;

	// Replace common HTML entities
	findAndReplaceAll( result, "&lt;", "<" );
	findAndReplaceAll( result, "&gt;", ">" );
	findAndReplaceAll( result, "&amp;", "&" );
	findAndReplaceAll( result, "&quot;", "\"" );
	findAndReplaceAll( result, "&#39;", "'" );
	findAndReplaceAll( result, "&nbsp;", " " );

	// Remove HTML tags - simple approach for this demo
	size_t startPos = 0;
	while( ( startPos = result.find( "<", startPos ) ) != string::npos ) {
		size_t endPos = result.find( ">", startPos );
		if( endPos != string::npos ) {
			result.erase( startPos, endPos - startPos + 1 );
		}
		else {
			break;
		}
	}

	// Clean up extra whitespace
	findAndReplaceAll( result, "\n", " " );
	findAndReplaceAll( result, "\r", " " );
	while( result.find( "  " ) != string::npos ) {
		findAndReplaceAll( result, "  ", " " );
	}

	return result;
}

bool MastadonHashtagStream::hasTweetAvailable()
{
	return mBuffer.isNotEmpty();
}

Tweet MastadonHashtagStream::getNextTweet()
{
	Tweet result;
	mBuffer.popBack( &result );
	return result;
}
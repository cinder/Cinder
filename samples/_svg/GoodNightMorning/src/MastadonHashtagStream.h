#pragma once

#include "cinder/Surface.h"
#include "cinder/ConcurrentCircularBuffer.h"
#include "cinder/Thread.h"

#include <string>

class Tweet {
  public:
	Tweet() {}
	Tweet( const std::string& phrase, const std::string& user, const ci::SurfaceRef& icon )
		: mPhrase( phrase )
		, mUser( user )
		, mIcon( icon )
	{
	}

	const std::string&	  getPhrase() const { return mPhrase; }
	const std::string&	  getUser() const { return mUser; }
	const ci::SurfaceRef& getIcon() const { return mIcon; }

  private:
	std::string	   mPhrase, mUser;
	ci::SurfaceRef mIcon;
};

class MastadonHashtagStream {
  public:
	MastadonHashtagStream( const std::string& hashtag );
	~MastadonHashtagStream();

	bool  hasTweetAvailable();
	Tweet getNextTweet();

  protected:
	void serviceTweets();

	std::string							mHashtag;
	std::thread							mThread;
	ci::ConcurrentCircularBuffer<Tweet> mBuffer;
	bool								mCanceled;
	std::string							mLastPostId;
};
//
//	InstagramStream.h
//	InstagramTest
//
//	Created by Greg Kepler on 6/9/12.
//	Copyright (c) 2012 The Barbarian Group. All rights reserved.
//


#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/Surface.h"
#include "cinder/ConcurrentCircularBuffer.h"
#include "cinder/Thread.h"
#include <string>


class Instagram {
  public:
	Instagram() {}
	Instagram( const std::string &user, const std::string &imageUrl, const ci::Surface &image ) :
				mUser( user ), mImageUrl( imageUrl ), mImage (image){}
	
	const std::string&	getUser()			const { return mUser; }
	const std::string&	getImageUrl()		const { return mImageUrl; }
	const ci::Surface&	getImage()			const { return mImage; }

	bool				isNull() const { return mImageUrl.empty(); }
	
  private:
	std::string		mUser, mImageUrl;
	ci::Surface		mImage;
};



class InstagramStream {
  public:
	// popular images
	InstagramStream( const std::string &clientId );									   
	
	 // images with a specific tag
	InstagramStream( const std::string &searchPhrase, const std::string &clientId );
	InstagramStream( const std::string &searchPhrase, const int &minId, const int &maxId, const std::string &clientId );
	
	// Search for media in a given area.
	InstagramStream( ci::Vec2f loc, float dist, int minTs, int maxTs, std::string clientId);
	InstagramStream( ci::Vec2f loc, float dist, std::string clientId);
	InstagramStream( ci::Vec2f loc, std::string clientId);
	
	~InstagramStream();
	
	bool		hasInstagramAvailable();
	Instagram	getNextInstagram();
	bool		isConnected();
	
  protected:
	void		startThread( std::string url );
	void		serviceGrams( std::string url );
	
	std::string								mSearchPhrase;
	std::shared_ptr<std::thread>			mThread;
	ci::ConcurrentCircularBuffer<Instagram> mBuffer;
	bool									mCanceled;
	bool									mIsConnected;
	std::string								mClientId;
};
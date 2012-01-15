//
//  qbUpdatePool.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//
#pragma once

#include <vector>

//
// Add to update pool...
//
//_qb.updatePoolAdd( &mSourceSyphon );
//_qb.updatePoolRemove( &mSourceMovie );

namespace cinder { namespace qb {
	
	//
	// Updateable Object (VIRTUAL)
	class qbUpdateObject {
	public:
		virtual void update() = 0;
	};
	
	//
	// The update Pool
	class qbUpdatePool {
	public:

		void add( qbUpdateObject *o )
		{
			std::vector<qbUpdateObject*>::iterator it;
			for ( it=mObjects.begin() ; it < mObjects.end(); it++ )
				if (*it == o)
					return;		// already updating
			mObjects.push_back(o);
		}
		void remove( qbUpdateObject *o )
		{
			std::vector<qbUpdateObject*>::iterator it;
			for ( it=mObjects.begin() ; it < mObjects.end(); it++ )
				if (*it == o)
				{
					mObjects.erase (it);
					return;
				}
		}
		void update() {
			std::vector<qbUpdateObject*>::iterator it;
			for ( it=mObjects.begin() ; it < mObjects.end(); it++ )
				(*it)->update();
		}
		
	private:
		std::vector<qbUpdateObject*>	mObjects;
	};
	
} } // cinder::qb



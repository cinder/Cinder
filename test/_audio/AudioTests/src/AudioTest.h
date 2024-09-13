#pragma once

#include "cinder/Vector.h"

using AudioTestRef = std::shared_ptr<class AudioTest>;

class AudioTest {
public:

	virtual void setup()	{}
	virtual void resize()	{}
	
	virtual void update()	{}
	virtual void draw()		{}
	virtual void updateUI()	{}

	const std::string&	getName() const	{ return mName; }
	void				setName( const std::string &name ) { mName = name; }

protected:
	std::string mName;
};

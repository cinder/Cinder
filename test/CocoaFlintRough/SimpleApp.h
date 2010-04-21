#ifndef SIMPLE_APP_H
#define SIMPLE_APP_H

#include "flint/app/AppFlintView.h"
#include "flint/Color.h"

class SimpleApp : public fli::app::AppFlintView {
 public:
	SimpleApp::SimpleApp();

	fli::app::Renderer* prepareRenderer();
	
	void update();
	void draw();
	void keyDown( fli::app::KeyEvent event );
	
	fli::Colorf	getColor() const { return mColor; }
	void		setColor( const fli::Color &aColor );
	int			getSubdivision() const { return mSubdivision; }
	void		setSubdivision( int aSubdivision );
	
 protected:
	fli::Color	mColor, mBackgroundColor;
	int			mSubdivision;
	float		mAnimation;
};


#endif // SIMPLE_APP_H
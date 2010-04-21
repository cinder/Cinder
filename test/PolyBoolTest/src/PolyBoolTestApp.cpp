#include "flint/app/AppBasic.h"
#include "PolyBoolean.h"
using namespace fli;
using namespace fli::app;

using std::vector;

// We'll create a new Flint Application by deriving from the BasicApp class
class PolyBoolTestApp : public AppBasic {
 public:
	virtual Renderer* prepareRenderer() { return new Renderer2D(); }
	void setup();

	void keyDown( KeyEvent event );	
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	
	void drawPoly( cairo::Context &ctx, const vector<vector<Vec2f> > &points, const ColorA &color, bool fill );
	void draw();

	void calculateBoolean();
	
	fli::Color		mColor, mBackgroundColor;
	Vec2f			mCurrentPoint;
	vector<vector<Vec2f> >	mSubject, mClip;
	vector<vector<Vec2f> >	mResult;
	vector<Vec2f>	*mCurrentPoly;
	bool			mBuildingPoly2;
	bool			mRenderFilled;
	int				mBooleanMode;
};

void PolyBoolTestApp::setup()
{
	mBackgroundColor = Color( 0.25, 0.25f, 0.25f );
	mBuildingPoly2 = false;
	mRenderFilled = false;
	mBooleanMode = 0;

	mSubject.push_back( vector<Vec2f>() );
	mCurrentPoly = &mSubject[0];
//	mPoints1.push_back( Vec2f( 50, 50 ) );
//	mPoints1.push_back( Vec2f( 250, 50 ) );	
//	mPoints1.push_back( Vec2f( 250, 250 ) );
//	mPoints1.push_back( Vec2f( 50, 250 ) );
//
//	mPoints2.push_back( Vec2f( 75, 75 ) );
//	mPoints2.push_back( Vec2f( 220, 90 ) );	
//	mPoints2.push_back( Vec2f( 210, 350 ) );
//	mPoints2.push_back( Vec2f( 40, 230 ) );
}

void PolyBoolTestApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
		case KeyEvent::KEY_RETURN:
			if( event.isShiftDown() ) {
				if( mBuildingPoly2 ) {
					mClip.push_back( vector<Vec2f>() );
					mCurrentPoly = &mClip[mClip.size()-1];
				}
				else {
					mSubject.push_back( vector<Vec2f>() );
					mCurrentPoly = &mSubject[mSubject.size()-1];				
				}
			}
			else {
				if( mBuildingPoly2 )
					calculateBoolean();
				else {
					mBuildingPoly2 = true;
					mClip.push_back( vector<Vec2f>() );
					mCurrentPoly = &mClip[0];
				}
			}
		break;
		case KeyEvent::KEY_f:
			mRenderFilled = ! mRenderFilled;
		break;
		case KeyEvent::KEY_x:
			mSubject.clear();
			mClip.clear();
			mResult.clear();
			mSubject.push_back( vector<Vec2f>() );
			mCurrentPoly = &mSubject[0];
			mBuildingPoly2 = false;
		break;
		case KeyEvent::KEY_m:
			mBooleanMode = ( mBooleanMode + 1 ) % 4;
			if( ( ! mSubject.empty() ) && ( ! mClip.empty() ) )
				calculateBoolean();
		break;
	}
}

void PolyBoolTestApp::mouseDown( MouseEvent event )
{
	mCurrentPoly->push_back( event.getVec2f() );
}

void PolyBoolTestApp::mouseDrag( MouseEvent event )
{
	(*mCurrentPoly)[mCurrentPoly->size()-1] = event.getVec2f();
}

void PolyBoolTestApp::drawPoly( cairo::Context &ctx, const vector<vector<Vec2f> > &poly, const ColorA &color, bool fill )
{
	ctx.setSourceColor( color );
	ctx.newPath();
	for( vector<vector<Vec2f> >::const_iterator polyIt = poly.begin(); polyIt != poly.end(); ++polyIt ) {
		if( polyIt->size() > 1 ) {
			ctx.moveTo( (*polyIt)[0] );
			for( int i = 1; i < polyIt->size(); ++i )
				ctx.lineTo( (*polyIt)[i] );
			ctx.lineTo( (*polyIt)[0] );
		}
	}
	if( fill )
		ctx.fill();
	else
		ctx.stroke();		
}

void PolyBoolTestApp::draw()
{
	cairo::Context ctx( getCairoSurface() );
	ctx.setFillRule( cairo::FILL_RULE_EVEN_ODD );
	
	ctx.setSourceColor( mBackgroundColor );
	ctx.paint();
	
	drawPoly( ctx, mSubject, ColorA( 1.0f, 0.0f, 0.0f, 0.5f ), mRenderFilled );
	drawPoly( ctx, mClip, ColorA( 0.0f, 1.0f, 0.0f, 0.5f ), mRenderFilled );
	
	ctx.setSourceColor( ColorA( 0.0, 0.0, 1.0, 0.9f ) );
	ctx.newPath();	
	if( ! mResult.empty() ) {
		for( vector<vector<Vec2f> >::const_iterator polyIt = mResult.begin(); polyIt != mResult.end(); ++polyIt ) {
			ctx.moveTo( (*polyIt)[0] );
			for( int i = 1; i < (*polyIt).size(); ++i )
				ctx.lineTo( (*polyIt)[i] );
			ctx.lineTo( (*polyIt)[0] );
		}
	}
	if( mRenderFilled )
		ctx.fill();
	else
		ctx.stroke();
}

void PolyBoolTestApp::calculateBoolean()
{	
	mResult = calculateIntersection( mSubject, mClip, mBooleanMode );
}

FLI_APP_BASIC( PolyBoolTestApp )

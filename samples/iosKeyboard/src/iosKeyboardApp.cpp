#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct TextView {
	TextView() : mIsSelected( false ), mPadding( 6.0f ) {}

	Rectf mBounds;
	string mText;
	string mPlacerholderText;
	bool mIsSelected;
	float mPadding;

	Rectf getTextBounds() const { return Rectf( mBounds.x1 + mPadding, mBounds.y1 + mPadding, mBounds.x2 - mPadding, mBounds.y2 - mPadding ); }
};

class iosKeyboardApp : public AppCocoaTouch {
  public:
	void setup();
	void touchesBegan( TouchEvent event );
	void keyDown( KeyEvent event );
	void update();
	void draw();

	void layoutTextViews();
	void drawTextView( const TextView &textView );
	void processNumerical( const KeyEvent &event );
	void processMultiline( const KeyEvent &event );

	TextView mNumericalTextView;
	TextView mMultiLineTextView;

	Anim<float> mViewYOffset;
	gl::TextureFontRef mTexFont;
};

void iosKeyboardApp::setup()
{
	mViewYOffset = 0.0f;

	layoutTextViews();

	getSignalKeyboardWillShow().connect( [this] {
		console() << "signal keyboardWillShow" << endl;
	} );

	getSignalKeyboardWillHide().connect( [this] {
		console() << "signal keyboardWillHide" << endl;

		timeline().apply( &mViewYOffset, 0.0f, 0.3f, EaseInOutCubic() );
	} );

}

void iosKeyboardApp::touchesBegan( TouchEvent event )
{

	mNumericalTextView.mIsSelected = false;
	mMultiLineTextView.mIsSelected = false;

	Vec2f pos = event.getTouches().front().getPos();
	pos.y -= mViewYOffset;

	if( mNumericalTextView.mBounds.contains( pos ) ) {
		mNumericalTextView.mIsSelected = true;
		showKeyboard( KeyboardOptions().type( KeyboardType::NUMERICAL ) );
	}
	else if( mMultiLineTextView.mBounds.contains( pos ) ) {
		mMultiLineTextView.mIsSelected = true;
		showKeyboard();

		timeline().apply( &mViewYOffset, -100.0f, 0.3f, EaseInOutCubic() );
	}
	else
		hideKeyboard();
}

void iosKeyboardApp::keyDown( KeyEvent event )
{
	console() << "key char: " << event.getChar() << endl;

	if( mNumericalTextView.mIsSelected )
		processNumerical( event );
	else
		processMultiline( event );
}

void iosKeyboardApp::processNumerical( const KeyEvent &event )
{
	if( event.getCode() == KeyEvent::KEY_RETURN )
			hideKeyboard();

	if( event.getCode() == KeyEvent::KEY_BACKSPACE && ! mNumericalTextView.mText.empty() )
		mNumericalTextView.mText.pop_back();
	else if( isdigit( event.getChar() ) ) {
		mNumericalTextView.mText.push_back( event.getChar() );
		Rectf fitRect = mNumericalTextView.getTextBounds();
		Vec2f size = mTexFont->measureString( mNumericalTextView.mText );

		if( size.x > fitRect.getWidth() ) {
			console() << "OVERFLOW" << endl;
			mNumericalTextView.mText.pop_back();
		}
	}

}

void iosKeyboardApp::processMultiline( const KeyEvent &event )
{
	if( event.getCode() == KeyEvent::KEY_BACKSPACE && ! mMultiLineTextView.mText.empty() )
		mMultiLineTextView.mText.pop_back();
	else
		mMultiLineTextView.mText.push_back( event.getChar() );
}

void iosKeyboardApp::update()
{
}

void iosKeyboardApp::draw()
{
	gl::clear( Color::gray( 0.85f ) );

	gl::pushMatrices();
	gl::translate( 0.0f, mViewYOffset );

	drawTextView( mNumericalTextView );
	drawTextView( mMultiLineTextView );

	gl::popMatrices();

//	gl::pushMatrices();
//	gl::translate( 70.0f, 70.0f );
//	gl::rotate( getElapsedSeconds() * 30.0f );
//	gl::color( 0.6f, 0.0f, 0.0f );
//	gl::drawSolidRect( Rectf( -30.0f, -30.0f, 30.0f, 30.0f ) );
//	gl::popMatrices();
}

void iosKeyboardApp::drawTextView( const TextView &textView )
{
	gl::color( Color( "PowderBlue" ) );
	gl::drawSolidRect( textView.mBounds );

	Color borderColor = textView.mIsSelected ? Color( "DodgerBlue" ) : Color( "DarkGray" );
	gl::color( borderColor );
	gl::drawStrokedRect( textView.mBounds );

	Rectf fitRect = textView.getTextBounds();
	Vec2f offset( 0.0f, mTexFont->getAscent() );

	if( textView.mText.empty() && ! textView.mIsSelected ) {
		gl::color( Color::gray( 0.6f ) );

		mTexFont->drawStringWrapped( textView.mPlacerholderText, fitRect, offset );

	}
	else {
		gl::color( Color::gray( 0.6f ) );

		mTexFont->drawStringWrapped( textView.mText, fitRect, offset );
	}

}

void iosKeyboardApp::layoutTextViews()
{
	mTexFont = gl::TextureFont::create( Font( "Helvetica", 18.0f ), gl::TextureFont::Format().enableMipmapping().premultiply() );
	gl::enableAlphaBlending( true );

	const float kLineHeight = 24.0f;
	Rectf rect( getWindowWidth() * 0.2f, getWindowCenter().y - kLineHeight * 2.0f, getWindowWidth() * 0.8f, getWindowCenter().y - kLineHeight );
	mNumericalTextView.mBounds = rect;
	mNumericalTextView.mPlacerholderText = "enter digits";

	rect.y1 = getWindowCenter().y;
	rect.y2 = getWindowCenter().y + kLineHeight * 3.0f;

	mMultiLineTextView.mBounds = rect;
	mMultiLineTextView.mPlacerholderText = "enter text";
}

CINDER_APP_COCOA_TOUCH( iosKeyboardApp, RendererGl() )
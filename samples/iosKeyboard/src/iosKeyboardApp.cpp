#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct TextView {
	TextView() : mIsSelected( false ), mPadding( 4.0f ) {}

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
	Font mFont;
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

		// Sometimes the keyboard can be dismissed by the user without going through hideKeyboard. Using this signal,
		// you have a consistent place to animate the view contents back to the normal position.
		timeline().apply( &mViewYOffset, 0.0f, 0.3f, EaseInOutCubic() );
	} );

	// You can further customize the way the iOS virtual keyboard looks by directly manipulating the managed UITextField:
	// (note: requires that you build this source file as Obj-C++).
	getkeyboardTextView().keyboardAppearance = UIKeyboardAppearanceAlert;
}

void iosKeyboardApp::touchesBegan( TouchEvent event )
{

	mNumericalTextView.mIsSelected = false;
	mMultiLineTextView.mIsSelected = false;

	Vec2f pos = event.getTouches().front().getPos();
	pos.y -= mViewYOffset;

	if( mNumericalTextView.mBounds.contains( pos ) ) {
		mNumericalTextView.mIsSelected = true;

		// show the numerical keyboard. Make sure the keyboard's string and our text view's string match.
		showKeyboard( KeyboardOptions().type( KeyboardType::NUMERICAL ).initialString( mNumericalTextView.mText ) );
	}
	else if( mMultiLineTextView.mBounds.contains( pos ) ) {
		mMultiLineTextView.mIsSelected = true;

		// this is the default keyboard type, but disable close on return.
		showKeyboard( KeyboardOptions().closeOnReturn( false ).initialString( mMultiLineTextView.mText ) );

		// the keyboard is going to block the multi-line text view, so animate the view contents up.
		timeline().apply( &mViewYOffset, -100.0f, 0.3f, EaseInOutCubic() );
	}
	else
		hideKeyboard();
}

void iosKeyboardApp::keyDown( KeyEvent event )
{
	if( mNumericalTextView.mIsSelected )
		processNumerical( event );
	else
		processMultiline( event );
}

// manually build a string of numerical digits, filtering out everything else
void iosKeyboardApp::processNumerical( const KeyEvent &event )
{
	if( event.getCode() == KeyEvent::KEY_BACKSPACE && ! mNumericalTextView.mText.empty() )
		mNumericalTextView.mText.pop_back();
	else if( isdigit( event.getChar() ) ) {
		mNumericalTextView.mText.push_back( event.getChar() );
		Rectf fitRect = mNumericalTextView.getTextBounds();
		TextBox tbox = TextBox().font( mFont ).text( mNumericalTextView.mText ).size( TextBox::GROW, TextBox::GROW );
		Vec2f size = tbox.measure();

		if( size.x > fitRect.getWidth() ) {
			console() << "OVERFLOW" << endl;
			mNumericalTextView.mText.pop_back();
		}
	}
}

// Don't return on enter here, which allows the KEY_RETURN to be interpreted as a newline.
// Instead, grab the internal keyboard string, which can also handle complex character sequences, such as kanji or emoji
void iosKeyboardApp::processMultiline( const KeyEvent &event )
{
	string str = getKeyboardString();
	mMultiLineTextView.mText = str;
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
}

void iosKeyboardApp::drawTextView( const TextView &textView )
{
	gl::color( Color( "PowderBlue" ) );
	gl::drawSolidRect( textView.mBounds );

	Color borderColor = textView.mIsSelected ? Color( "DodgerBlue" ) : Color( "DarkGray" );
	gl::color( borderColor );
	gl::drawStrokedRect( textView.mBounds );

	Rectf fitRect = textView.getTextBounds();
	Vec2f offset( 0.0f, mFont.getAscent() );

	TextBox tbox = TextBox().font( mFont ).size( fitRect.getWidth(), fitRect.getHeight() ).premultiplied();

	if( textView.mText.empty() && ! textView.mIsSelected ) {
		tbox.color( Color::gray( 0.6f ) ).text( textView.mPlacerholderText );
	}
	else {
		tbox.color( Color( "FireBrick" ) ).text( textView.mText );
	}
	gl::color( Color::white() );
	gl::draw( tbox.render(), fitRect.getUpperLeft() );
}

void iosKeyboardApp::layoutTextViews()
{
	mFont = Font( "Helvetica", 18.0f );
	gl::enableAlphaBlending( true );

	const float kLineHeight = 26.0f;
	Rectf rect( getWindowWidth() * 0.2f, getWindowCenter().y - kLineHeight * 2.0f, getWindowWidth() * 0.8f, getWindowCenter().y - kLineHeight );
	mNumericalTextView.mBounds = rect;
	mNumericalTextView.mPlacerholderText = "enter digits";

	rect.y1 = getWindowCenter().y;
	rect.y2 = getWindowCenter().y + ( kLineHeight + mMultiLineTextView.mPadding ) * 4.0f;

	mMultiLineTextView.mBounds = rect;
	mMultiLineTextView.mPlacerholderText = "enter text";
}

CINDER_APP_COCOA_TOUCH( iosKeyboardApp, RendererGl() )
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Utilities.h"
#include "cinder/svg/Svg.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Text.h"
#include "cinder/Timeline.h"
#include "cinder/ip/Fill.h"
#include "cinder/ip/Blend.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"

#include "TweetStream.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class GoodNightMorningApp : public App {
  public:
	void prepareSettings( Settings *settings );
	void setup();
	void resize();
	void update();
	void draw();
	void drawTweets();

	gl::TextureRef	renderTweet( const Tweet &tweet, float width, const Color &textColor, float backgroundAlpha );
	void			newMorningTweet();
	void			newNightTweet();

	
	gl::TextureRef	mForegroundTex, mBackgroundTex;
	Rectf			mDrawBounds;
	svg::DocRef		mCityscapeSvg;
	Font			mHeaderFont, mFont;
	
	Anim<float>		mMorningTweetPos, mNightTweetPos;
	Anim<float>		mMorningTweetAlpha, mNightTweetAlpha;
	gl::TextureRef	mMorningTweetTex, mNightTweetTex;
	TweenRef<float>	mMorningTween, mNightTween;
	
	shared_ptr<TweetStream>		mMorningTweets, mNightTweets;
};

void GoodNightMorningApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 890, 500 );
}

void GoodNightMorningApp::setup()
{
	mCityscapeSvg = std::shared_ptr<svg::Doc>( new svg::Doc( loadAsset( "cityscape.svg" ) ) );
	mFont = Font( loadAsset( "Lato-Bold.ttf" ), 14 );
	mHeaderFont = Font( loadAsset( "Lato-Light.ttf" ), 14 );
	
	mMorningTweets = shared_ptr<TweetStream>( new TweetStream( "\"Good morning\"" ) );
	mNightTweets = shared_ptr<TweetStream>( new TweetStream( "\"Good night\"" ) );
	
	newMorningTweet();
	newNightTweet();
	
	gl::enableAlphaBlending();	
}

// Renders the tweet into a gl::Texture, using TextBox for type rendering
gl::TextureRef GoodNightMorningApp::renderTweet( const Tweet &tweet, float width, const Color &textColor, float backgroundAlpha )
{
	TextBox header = TextBox().font( mHeaderFont ).color( textColor ).text( "@" + tweet.getUser() );
	Surface headerSurface = header.render();
	TextBox phrase = TextBox().size( vec2( width - 48 - 4, TextBox::GROW ) ).font( mFont ).color( textColor ).text( tweet.getPhrase() );
	Surface textSurface = phrase.render();
	Surface result( textSurface.getWidth() + 56, std::max( headerSurface.getHeight() + textSurface.getHeight() + 10, 56 ), true );
	ip::fill( &result, ColorA( 1, 1, 1, backgroundAlpha ) );
	if( tweet.getIcon() )
		result.copyFrom( *tweet.getIcon(), tweet.getIcon()->getBounds(), ivec2( 4, 4 ) );
	ip::blend( &result, headerSurface, headerSurface.getBounds(), ivec2( result.getWidth() - 4 - headerSurface.getWidth(), textSurface.getHeight() + 6 ) );
	ip::blend( &result, textSurface, textSurface.getBounds(), ivec2( 56, 4 ) );
	return gl::Texture::create( result );
}

void GoodNightMorningApp::newMorningTweet()
{
	if( ! mMorningTweets->hasTweetAvailable() )
		return;		
	Tweet tweet = mMorningTweets->getNextTweet();

	// Setup tweens for the tweet's position and opacity
	mMorningTweetPos = 0.8f;
	timeline().apply( &mMorningTweetPos, 0.10f, 2.0f, EaseOutAtan() );
	mMorningTween = timeline().appendTo( &mMorningTweetPos, -0.3f, 2.0f, EaseInAtan() ).delay( 1 );
	mMorningTweetAlpha = 1.0f;
	timeline().apply( &mMorningTweetAlpha, 0.0f, 1.0f ).appendTo( &mMorningTweetPos, -1 );
	
	mMorningTweetTex = renderTweet( tweet, mDrawBounds.getWidth() * 0.3f, Color::black(), 0.25f );
}

void GoodNightMorningApp::newNightTweet()
{
	if( ! mNightTweets->hasTweetAvailable() )
		return;		
	Tweet tweet = mNightTweets->getNextTweet();

	// Setup tweens for the tweet's position and opacity
	mNightTweetPos = -0.2f;
	timeline().apply( &mNightTweetPos, 0.10f, 1.5f, EaseOutAtan() );
	mNightTween = timeline().appendTo( &mNightTweetPos, 1.0f, 2.0f, EaseInAtan() ).delay( 1 );
	mNightTweetAlpha = 1.0f;
	timeline().apply( &mNightTweetAlpha, 0.0f, 1.0f ).appendTo( &mNightTweetPos, -1 );
	
	mNightTweetTex = renderTweet( tweet, mDrawBounds.getWidth() * 0.3f, Color::white(), 0.1f );
}

void GoodNightMorningApp::update()
{
	if( (! mMorningTween) || mMorningTween->isComplete() ) // do we need a new morning tweet?
		newMorningTweet();
	if( (! mNightTween) || mNightTween->isComplete() ) // do we need a new night tweet?
		newNightTweet();
}

// Renders a given SVG group 'groupName' into a new gl::Texture
gl::TextureRef renderSvgGroupToTexture( const svg::Doc &doc, const std::string &groupName, const Rectf &rect, bool alpha )
{
	cairo::SurfaceImage srfImg( rect.getWidth(), rect.getHeight(), alpha );
	cairo::Context ctx( srfImg );
	ctx.scale( rect.getWidth() / doc.getWidth(), rect.getHeight() / doc.getHeight() );
	ctx.render( doc / groupName );
	return gl::Texture::create( srfImg.getSurface() );
}

void GoodNightMorningApp::resize()
{
	Rectf svgBounds = mCityscapeSvg->getBounds(); // get the native bounds of the SVG
	// fit the SVG proportionally into the window
	mDrawBounds = svgBounds.getCenteredFit( getWindowBounds(), true );
	
	// Our SVG has two root groups, "Foreground" and "Background". Tweets are rendered between these two layers
	// Render each group into its own texture
	mForegroundTex = renderSvgGroupToTexture( *mCityscapeSvg, "Foreground", mDrawBounds, true );
	mBackgroundTex = renderSvgGroupToTexture( *mCityscapeSvg, "Background", mDrawBounds, false );
}

void GoodNightMorningApp::drawTweets()
{
	gl::color( ColorA( 1, 1, 1, mMorningTweetAlpha ) );
	if( mMorningTweetTex )
		gl::draw( mMorningTweetTex, vec2( 0.1f, mMorningTweetPos ) * mDrawBounds.getSize() + mDrawBounds.getUpperLeft() );
	gl::color( ColorA( 1, 1, 1, mNightTweetAlpha ) );
	if( mNightTweetTex )
		gl::draw( mNightTweetTex, vec2( 0.6f, mNightTweetPos ) * mDrawBounds.getSize() + mDrawBounds.getUpperLeft() );
	gl::color( Color::white() ); // restore for future drawing
}

void GoodNightMorningApp::draw()
{
	// clear out the window with dark gray
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) ); 
	
	if( mBackgroundTex )
		gl::draw( mBackgroundTex, mDrawBounds );
	drawTweets();
	if( mForegroundTex )
		gl::draw( mForegroundTex, mDrawBounds );
}

CINDER_APP( GoodNightMorningApp, RendererGl )

#include "cinder/app/AppBasic.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Font.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
using namespace ci;
using namespace ci::app;
using namespace std;
#include <sstream>

class fontSampleApp : public AppBasic {
 public:
	void		setup();
	void		drawCharacterVerbose( cairo::Context &ctx, Vec2f where );
	void		draw();	

	void		keyDown( KeyEvent event ) { if( event.getChar() == 'f' ) { setRandomFont(); setRandomGlyph(); } else setRandomGlyph(); }
	void		mouseDown( MouseEvent event ) { writeImage( getHomeDirectory() + "wisteriaShot.jpg", copyWindowSurface() ); }
	
	void		setRandomFont();
	void		setRandomGlyph();
	
	Font			mFont;
	Shape2d			mShape;
	vector<string>	mFontNames;
};

void fontSampleApp::setup()
{
	// this block iterates all of the fonts available on the system
	mFontNames = Font::getNames();
	for( vector<string>::const_iterator fontName = mFontNames.begin(); fontName != mFontNames.end(); ++fontName ) {
#if 1
// uncomment to exercise code to determine font properties
		Font font( *fontName, 128 );
		// display some font properties
		std::stringstream ss;
		ss << font.getName() << " leading: " << font.getLeading() << " ascent: " << font.getAscent() << " descent: " << font.getDescent() << " glyphs: " << font.getNumGlyphs();
		ss << "Glyph 'a' maps to " << font.getGlyphChar( 'a' );
		vector<Font::Glyph> g = font.getGlyphs( "abc123" );
		ss << g.size();
		console() << ss.str();
#endif
	}
	
	setRandomFont();
	setRandomGlyph();
}

void fontSampleApp::setRandomFont()
{
	// select a random font from those available on the system
	mFont = Font( mFontNames[rand() % mFontNames.size()], 256 );

	// exercise the code which determines which glyphs map to a given string
	console() << "In this font, the string 'abc123' is glyphs: " << std::endl;
	vector<Font::Glyph> glyphs = mFont.getGlyphs( "abc123" );
	for( vector<Font::Glyph>::const_iterator it = glyphs.begin(); it != glyphs.end(); ++it )
		console() << *it << " ";
	console() << endl;
}

void fontSampleApp::setRandomGlyph()
{
	size_t glyphIndex = rand() % mFont.getNumGlyphs();
	try {
		mShape = mFont.getGlyphShape( glyphIndex );
	}
	catch( FontGlyphFailureExc &exc  ) {
		console() << "Looks like glyph " << glyphIndex << " doesn't exist in this font." << std::endl;
	}
}

// This is a callback functor designed to interact with Shape2d::iterate()
// It draws each of the segments of the path explicitly as well as drawing circles at each of the control points
struct VerboseCharDraw {
	VerboseCharDraw( cairo::Context &ctx ) : mCtx( ctx ) { }
	bool operator()( Path2d::SegmentType type, const Vec2f *points, const Vec2f *previousPoint )
	{
		const float radius = 3.0f;
		const ColorA dotColor( 0.5f, 0.75f, 1.0f, 0.4f );
		const ColorA lineColor( 0.5f, 1.0f, 0.5f, 0.8f );
		if( type == Path2d::MOVETO ) {
			mCtx.circle( points[0], radius );
		}
		else if( type == Path2d::LINETO ) {
			mCtx.setSource( dotColor );
			mCtx.circle( points[0], radius );
			mCtx.fill();
			mCtx.setSource( lineColor );
			mCtx.line( points[0], *previousPoint );
			mCtx.stroke();
		}
		else if( type == Path2d::QUADTO ) {
			mCtx.setSource( dotColor );
			mCtx.circle( points[0], radius );
			mCtx.circle( points[1], radius );
			mCtx.fill();
			mCtx.setSource( lineColor );
			mCtx.moveTo( *previousPoint ); mCtx.quadTo( points[0], points[1] );
			mCtx.stroke();
		}
		else if( type == Path2d::CUBICTO ) {
			mCtx.setSource( dotColor );
			mCtx.circle( points[0], radius );
			mCtx.circle( points[1], radius );
			mCtx.circle( points[2], radius );						
			mCtx.fill();
			mCtx.setSource( lineColor );
			mCtx.moveTo( *previousPoint ); mCtx.curveTo( points[0], points[1], points[2] );
			mCtx.stroke();
		}
		else if( type == Path2d::CLOSE ) {
			mCtx.setSource( lineColor );
			mCtx.line( points[0], *previousPoint );
			mCtx.stroke();
		}
		return true;
	}
	
	cairo::Context &mCtx;
};

void fontSampleApp::drawCharacterVerbose( cairo::Context &ctx, Vec2f where )
{
	cairo::Matrix prevMat;
	ctx.getMatrix( &prevMat );
	
	// draw it filled
	ctx.setSourceRgb( 1.0f, 1.0, 0.5f );
	ctx.translate( where );
	// Uncomment below to render the character filled
	// ctx.appendPath( mShape );
	// ctx.fill();
	
	VerboseCharDraw verb( ctx );
	mShape.iterate<VerboseCharDraw>( verb );
	
	ctx.setMatrix( prevMat );
}

void fontSampleApp::draw()
{
	cairo::Context ctx( cairo::createWindowSurface() );
	
	// clear the screen
	ctx.setSourceRgb( 0.2f, 0.2f, 0.2f );
	ctx.paint();
	
	// draw the current character including the control points as dots
	ctx.setSourceRgb( 1.0f, 1.0, 0.5f );
	drawCharacterVerbose( ctx, Vec2f( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f ) );
	
	// Render the name of the font in the font itself
	ctx.setFont( mFont );
	ctx.setFontSize( 18 );
	ctx.moveTo( 10, getWindowHeight() - 10 );
	ctx.setSourceRgb( 0.5f, 0.75f, 1.0f );
	ctx.showText( mFont.getFullName() );
	ctx.stroke();
	
	// draw a lower case 'a'
#if 0
	Shape2d aPath;
	ctx.setSourceRgba( 0.5f, 0.75f, 1.0f, 0.2f );
	mFont.getGlyphPath( mFont.getGlyph( 'a' ), &aPath );
	ctx.translate( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f );
	ctx.appendPath( aPath );
	ctx.fill();
#endif
}


CINDER_APP_BASIC( fontSampleApp, Renderer2d )
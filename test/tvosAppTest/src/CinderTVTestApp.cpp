#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"

namespace {

auto const VertexShader = R"vs(
precision highp float;

uniform mat4 ciModelViewProjection;
in vec4 ciPosition;
in vec2 ciTexCoord0;

out vec2 vTexCoord;

void main() {
	gl_Position = ciModelViewProjection * ciPosition;
	vTexCoord =   ciTexCoord0;
}

)vs";

auto const FragmentShader = R"fs(
precision highp float;
uniform float ciElapsedSeconds;
uniform vec2  uNormalizedCursorPos;
in vec2 vTexCoord;
out vec4 fColor;

#define PI 3.141592653

float ripple( vec2 center, float cycles, float span, float offset ) {
	float d = distance( gl_FragCoord.xy, center );
	return cos( (d * cycles * 2.0 * PI / span) + offset );
}

void main() {
	vec2 window_size = vec2( 1920, 1080 );
	float r = ripple( uNormalizedCursorPos * window_size, 8.0, window_size.x / 2.0, -ciElapsedSeconds * 2.0 );
	fColor = vec4( vTexCoord, 1.0, 1.0 ) * r;
}

)fs";

} // namespace

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderTVTestApp : public App {
  public:
	void setup() override;

	/// Mouse events are not yet connected to anything on tvOS.
	/// May create a custom event type for the touchpad input.
	void mouseDown( MouseEvent event ) override;
	void mouseMove( MouseEvent event ) override;
	/// Touch events also are not yet connected to anything on tvOS.
	void touchesBegan( TouchEvent event ) override;

	void update() override;
	void draw() override;
	void attemptWebAccess();
  private:
	gl::BatchRef		mBatch;
	gl::TextureFontRef	mFont;
};

void CinderTVTestApp::setup()
{
	auto nowish = (int)chrono::high_resolution_clock::now().time_since_epoch().count();
	ci::randSeed( nowish );

	auto rect = geom::Rect( Rectf( 0, 0, 1920, 1080 ) );
	mBatch = gl::Batch::create( rect, gl::GlslProg::create( VertexShader, FragmentShader ) );

	auto possibleFonts = Font::getNames();
	auto arbitraryFontName = possibleFonts[randInt( (int)possibleFonts.size() )];
	console() << "Creating a TextureFont using " << arbitraryFontName << endl;
	mFont = gl::TextureFont::create( Font( arbitraryFontName, 96.0f ) );

	attemptWebAccess();
}

void CinderTVTestApp::attemptWebAccess()
{
	try {
		auto webContent = loadString( loadUrl( Url( "https://www.google.com/search?q=volcano&tbm=isch" ) ) );
		console() << "Result from googs starts with: " << webContent.substr(0, 91) << endl;
	}
	catch( const std::exception &exc ) {
		CI_LOG_E( "Error contacting website: " << exc.what() );
	}
}

void CinderTVTestApp::touchesBegan(cinder::app::TouchEvent event)
{
	CI_LOG_I("Touch began: " << event.getTouches().front().getPos());
}

void CinderTVTestApp::mouseDown( MouseEvent event )
{
}

void CinderTVTestApp::mouseMove( MouseEvent event )
{
}

void CinderTVTestApp::update()
{
	auto t = -getElapsedSeconds() * 0.2f;
	auto x = cos( t );
	auto y = sin( t );
	auto rippleCenter = vec2( 0.5f ) + vec2( x, y ) * 0.25f;
	mBatch->getGlslProg()->uniform( "uNormalizedCursorPos", rippleCenter );
}

void CinderTVTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::ScopedBlendPremult blendScope;
	mBatch->draw();
	mFont->drawString( "Whatever", getWindowCenter() );
}

CINDER_APP( CinderTVTestApp, RendererGl( RendererGl::Options().version( 3, 0 ) ), [] ( App::Settings *settings ) {
	settings->setMultiTouchEnabled(false);
} )

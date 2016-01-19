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
	vTexCoord = ciTexCoord0;
}

)vs";

auto const FragmentShader = R"fs(
precision highp float;
uniform float ciElapsedSeconds;
uniform vec2  uNormalizedCursorPos;
in vec2 vTexCoord;
out vec4 fColor;

#define PI 3.141592653

float ripple(vec2 center, float cycles, float span, float offset) {
	float d = distance(gl_FragCoord.xy, center);
	return cos((d * cycles * 2.0 * PI / span) + offset);
}

void main() {
	vec2 window_size = vec2(1920, 1080);
	float r = ripple(uNormalizedCursorPos, 8.0, window_size.x / 2.0, -ciElapsedSeconds * 2.0);
	fColor = vec4(vTexCoord, 1.0, 1.0) * r;
}

)fs";

} // namespace

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderTVTestApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseMove( MouseEvent event ) override;
	void touchesBegan( TouchEvent event) override;
	void update() override;
	void draw() override;
  private:
	gl::BatchRef		_batch;
	gl::TextureFontRef	_font;
};

void CinderTVTestApp::setup()
{
	auto rect = geom::Rect(Rectf(0, 0, 1920, 1080));
	_batch = gl::Batch::create(rect, gl::GlslProg::create(VertexShader, FragmentShader));

	auto result = loadString(loadUrl(Url("https://www.google.com/search?q=volcano&tbm=isch")));
	console() << "Result from googs: " << result << endl;

	auto possible_fonts = Font::getNames();
	for (auto &font: possible_fonts) {
		console() << "Font name: " << font << endl;
	}

	auto ms = (int)chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
	ci::randSeed(ms);
	_font = gl::TextureFont::create(Font(possible_fonts[randInt((int)possible_fonts.size())], 96.0f));
}

void CinderTVTestApp::touchesBegan(cinder::app::TouchEvent event)
{
	CI_LOG_I("Touch event: " << event.getTouches()[0].getPos());
}

void CinderTVTestApp::mouseDown( MouseEvent event )
{
}

void CinderTVTestApp::mouseMove( MouseEvent event )
{
	auto pos = vec2(event.getPos()) / vec2(getWindowSize());
	pos.y = 1.0 - pos.y;
	_batch->getGlslProg()->uniform("uNormalizedCursorPos", pos);
}

void CinderTVTestApp::update()
{
}

void CinderTVTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::ScopedBlendPremult blend_scope;
	_batch->draw();
	_font->drawString("Whatever", getWindowCenter());
}

CINDER_APP( CinderTVTestApp, RendererGl(RendererGl::Options().version(3, 0)), [] (App::Settings *settings) {
	settings->setMultiTouchEnabled(false);
} )

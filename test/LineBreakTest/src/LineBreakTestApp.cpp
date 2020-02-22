#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Unicode.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

Font font;
vec2 pos;
int maxWidth = 230;

class LineBreakTestApp : public App {
  public:
	void setup();
	void mouseDrag( MouseEvent event );	
	void update();
	void draw();
}; 

bool lineWidth( const char *s, size_t len, size_t maxLen )
{
//return len <= 6;
	TextBox box;
	box.setFont( font );
	box.setText( string( s, len ) );
	return box.measure().x < maxLen;
}

void print( const char *s, size_t len )
{
/*	size_t lenBeforeBreak = 0;
	bool found = false;
	while( lenBeforeBreak < len ) {
		size_t tempLen = lenBeforeBreak;
		uint32_t nextChar = nextCharUtf8( s, &tempLen, len );
		if( nextChar == '\r' || nextChar == '\n' ) {
			found = true;
			break;
		}
		lenBeforeBreak = tempLen;
	}
	app::console() << "{" << string( s, lenBeforeBreak ) << ( (found)?"!":"" ) << "}" << std::endl;*/
	
	gl::drawString( string( s, len ), pos, ColorA::white(), font );
int asc = font.getAscent();
int desc = font.getDescent();
int lead = font.getLeading();
	pos.y += font.getLeading() + font.getAscent() + font.getDescent();
}

void LineBreakTestApp::setup()
{
	font = Font( "Arial", 24 );

	const char *s8 = "One sees great things from the valley";
	console() << s8 << std::endl;
	vector<uint8_t> utf8Breaks;
	calcLinebreaksUtf8( s8, &utf8Breaks );
	for( auto it = utf8Breaks.begin(); it != utf8Breaks.end(); ++it )
		console() << (int)*it;
	console() << std::endl;

	// the Mac treats wchar_t as a 32-bit value, so we can't just type-cast to uint16_t; can on the PC though
	vector<uint16_t> s16;
	for( int i = 0; i < strlen( s8 ); ++i )
		s16.push_back( s8[i] );
	console() << s8 << std::endl;	
	vector<uint8_t> utf16Breaks;
	calcLinebreaksUtf16( &s16[0], s16.size(), &utf16Breaks );
	for( auto it = utf16Breaks.begin(); it != utf16Breaks.end(); ++it )
		console() << (int)*it;
	console() << std::endl;
}

void LineBreakTestApp::mouseDrag( MouseEvent event )
{
	maxWidth = event.getPos().x;
}

void LineBreakTestApp::update()
{
}

void LineBreakTestApp::draw()
{
	// clear out the window with black
	gl::enableAlphaBlending();
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::color( ColorA::white() );

	pos.x = 0;
	pos.y = 10;
//	const char *s = "a just.";
	const char *s = "just some stuff\n\nthat\nis longer than one line. I mean substantially longer...";
//	const char *s = "消費増税\n\n法案をめぐる事前事前事前審査を行っていた民主党税調などの合同総会は２８日未明、「名目３％程度、実質２％程度」の経済成長率の数値目標を付則に盛り込んだ新たな修正案を了承し、前原誠司政調会長に対応を一任した。野内閣は３０日に閣議決定を行う。";
//	for( size_t l = 1; l < 30; ++l ) {
	//	std::cout << l << ":" << std::endl;
		lineBreakUtf8( s,std::bind( lineWidth, std::placeholders::_1, std::placeholders::_2, maxWidth ), print );
//	}
	gl::color( ColorA( 1, 0, 0, 0.5 ) );
	TextBox box;
	box.setSize( ivec2( maxWidth, TextBox::GROW ) );
	box.setFont( font );
	box.setText( s );
	gl::TextureRef t = gl::Texture2d::create( box.render() );
	gl::draw( t );
	gl::color( Color( 0, 1, 0 ) );
	gl::drawStrokedRect( Rectf( 0, 10, maxWidth, 800 ) );
}

CINDER_APP( LineBreakTestApp, RendererGl )

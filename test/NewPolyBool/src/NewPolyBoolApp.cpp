#include "flint/app/AppBasic.h"
#include "PolyBoolean.h"
#include "gpc.h"
using namespace fli;
using namespace fli::app;
#include <fstream>
#include <sstream>

using std::vector;
bool gRelativeScale = false;

// We'll create a new Flint Application by deriving from the BasicApp class
class PolyBoolTestApp : public AppBasic {
 public:
	virtual Renderer* prepareRenderer() { return new Renderer2D(); }
	void setup();

	void keyDown( KeyEvent event );	

	void extremeTest();
	void generateLetters( char subjectChar, char clipChar, float size );
	void drawPoly( cairo::Context &ctx, const vector<vector<Vec2f> > &points, const ColorA &color, bool fill );
	void draw();

	void centerPolys();
	void calculateBoolean();

	void writeSVG();
	
	fli::Color		mColor, mBackgroundColor;
	vector<vector<Vec2f> >	mSubject, mClip;
	vector<vector<Vec2f> >	mResult;
vector<Vec2f> mRawIntersections;	
	bool			mRenderFilled;
	std::string		mLastName;
	int				mBooleanMode;
	bool			mUseVatti, mDoneCalculating;
	fli::Path2D		mTextPath, mTextPath2;
};

void PolyBoolTestApp::setup()
{
	mBackgroundColor = Color( 0.25, 0.25f, 0.25f );
	mRenderFilled = false;
	mBooleanMode = 0;
	mUseVatti = false;
	mDoneCalculating = false;
}

void savePolygon( const std::string &path, vector<vector<Vec2f> > &poly )
{
	std::ofstream of( path.c_str() );
	of << poly.size() << std::endl;
	for( size_t s = 0; s < poly.size(); ++s ) {
		of << poly[s].size() << " 1" << std::endl;
		for( size_t p = 0; p < poly[s].size(); ++p )
			of << poly[s][p].x << " " << poly[s][p].y << std::endl;
	}
}

void vectorizePath( const Path2D &path, const Vec2f &offset, vector<vector<Vec2f> > *result )
{
	result->clear();

	for( size_t contCount = 0; contCount < path.getNumContours(); ++contCount ) {
		const Contour2D &contour( path.getContour( contCount ) );
		
		size_t point = 0;
		vector<Vec2f> resultPoly;
		for( size_t seg = 0; seg < contour.getNumSegments(); ++seg ) {
			switch( contour.getSegmentType( seg ) ) {
				case Contour2D::MOVETO:
					resultPoly.push_back( offset + contour.getPoint( point++ ) );
				break;
				case Contour2D::LINETO:
					resultPoly.push_back( offset + contour.getPoint( point++ ) );
				break;
				default:
					throw;
			}
		}
		if( resultPoly.size() > 1 ) {
			result->push_back( resultPoly );
		}
	}	
}

void PolyBoolTestApp::extremeTest()
{
	while( true ) {
		generateLetters( char(33 + rand() % (126-33)), char(33 + rand() % (126-33)), (float)(12 + rand() % 500) );
		calculateBoolean();
	}
}

void PolyBoolTestApp::generateLetters( char subjectChar, char clipChar, float size )
{
	const Vec2f offset( 0, 0 );
	Path2D tempPath;
	cairo::Context ctx( shared_ptr<cairo::SurfaceImage>( new cairo::SurfaceImage( 16, 16, false ) ) );
#if defined( FLI_MAC )
//	ctx.setFontFace( cairo::FontFace( "MyriadPro-Regular" ) );
//	ctx.setFontFace( cairo::FontFace( "Cracked" ) );
	ctx.setFontFace( cairo::FontFace( "Papyrus" ) );
#else
	ctx.selectFontFace( "Arial", fli::cairo::FONT_SLANT_NORMAL, fli::cairo::FONT_WEIGHT_NORMAL );
#endif
	ctx.setFontSize( size );
	
	std::cout << "" << subjectChar << " " << clipChar << " @ " << size << std::endl;
	
	ctx.textPath( (std::string() + subjectChar).c_str() );
	ctx.copyPathFlat( &tempPath );
	vectorizePath( tempPath, offset, &mSubject );
	ctx.newPath();
	ctx.textPath( (std::string() + clipChar).c_str() );
	ctx.copyPathFlat( &tempPath );
	vectorizePath( tempPath, offset, &mClip );

	mLastName = std::string() + subjectChar + "_" + clipChar;
}

void PolyBoolTestApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
		case KeyEvent::KEY_f:
			mRenderFilled = ! mRenderFilled;
		break;
		case KeyEvent::KEY_r:
			gRelativeScale = ! gRelativeScale;
		break;
		case KeyEvent::KEY_d:
			std::cout << "------------------" << std::endl;
			for( size_t p = 0; p < mResult.size(); ++p ) {
				std::cout << mResult[p].size() << std::endl;
				for( size_t v = 0; v < mResult[p].size(); ++v )
					std::cout << mResult[p][v] << std::endl;
			}
		break;
		case KeyEvent::KEY_m:
			mBooleanMode = ( mBooleanMode + 1 ) % 3;
			if( ( ! mSubject.empty() ) && ( ! mClip.empty() ) )
				calculateBoolean();
		break;
		case KeyEvent::KEY_o: {
			mDoneCalculating = false;
			std::string path = getOpenFilePath();
			if( ! path.empty() ) {
				mSubject = loadPolygon( path, 1 );
			}
			path = getOpenFilePath();
			if( ! path.empty() ) {
				mClip = loadPolygon( path, 1 );
			}
			if( gRelativeScale )
				centerPolys();
			calculateBoolean();
		}
		break;
		case KeyEvent::KEY_w: {
			std::string path = getSaveFilePath();
			if( ! path.empty() )
				savePolygon( path, mSubject );
			path = getSaveFilePath();
			if( ! path.empty() )
				savePolygon( path, mClip );
		}
		break;
		case KeyEvent::KEY_v:
			mUseVatti = ! mUseVatti;
			calculateBoolean();
		break;
		case KeyEvent::KEY_l:
			generateLetters( char(33 + rand() % (126-33)), char(33 + rand() % (126-33)), 384 );
			calculateBoolean();
		break;
		case KeyEvent::KEY_t:
			//generateLetters( 'q', 'q', 384 );
//			generateLetters( '!', 'f', 384 ); // papyrus
			generateLetters( 'p', '_', 384 ); // papyrus
			calculateBoolean();
		break;
		case KeyEvent::KEY_s:
			writeSVG();
		break;
		case KeyEvent::KEY_e:
			extremeTest();
		break;
	}
}

void PolyBoolTestApp::centerPolys()
{
	fli::Rect subjectBounding( mSubject[0] );
	for( size_t c = 1; c < mSubject.size(); ++c )
		subjectBounding.include( mSubject[c] );

	fli::Rect clipBounding( mClip[0] );
	for( size_t c = 1; c < mClip.size(); ++c )
		clipBounding.include( mClip[c] );
std::cout << "Subject bounds: " << subjectBounding << std::endl;
std::cout << "Clip bounds: " << clipBounding << std::endl;	
	Vec2f windowCenter = Vec2f::zero();
	Vec2f subjectOffset = -subjectBounding.getCenter();
	Vec2f clipOffset = -clipBounding.getCenter();
	float clipScale, subjectScale;
	if( gRelativeScale ) {
		if( subjectBounding.getHeight() > clipBounding.getHeight() ) {
			float scale = getWindowHeight() / subjectBounding.getHeight();
			subjectScale = scale;
			clipScale = subjectBounding.getHeight() / clipBounding.getHeight() * scale;
		}
		else {
			float scale = getWindowHeight() / clipBounding.getHeight();
			clipScale = scale;
			subjectScale = clipBounding.getHeight() / subjectBounding.getHeight() * scale;
		}
	}
	else {
		float scale = ( subjectBounding.getHeight() > clipBounding.getHeight() ) ? ( getWindowHeight() / subjectBounding.getHeight() ) : ( getWindowHeight() / clipBounding.getHeight() );
		clipScale = scale;
		subjectScale = scale;
	}

	for( size_t c = 0; c < mSubject.size(); ++c ) {
		for( size_t p = 0; p < mSubject[c].size(); ++p ) {
			mSubject[c][p] += subjectOffset;
			mSubject[c][p] *= subjectScale;
			mSubject[c][p] += windowCenter;
		}
	}

	for( size_t c = 0; c < mClip.size(); ++c ) {
		for( size_t p = 0; p < mClip[c].size(); ++p ) {
			mClip[c][p] += clipOffset;
			mClip[c][p] *= clipScale;
			mClip[c][p] += windowCenter;
		}
	}
}

void PolyBoolTestApp::drawPoly( cairo::Context &ctx, const vector<vector<Vec2f> > &poly, const ColorA &color, bool fill )
{
	ctx.setLineWidth( 0.5f );
	ctx.setSourceColor( color );
	ctx.newPath();
	if( fill ) {
		for( vector<vector<Vec2f> >::const_iterator polyIt = poly.begin(); polyIt != poly.end(); ++polyIt ) {
			if( polyIt->size() > 1 ) {
				ctx.moveTo( (*polyIt)[0] );
				for( int i = 1; i < polyIt->size(); ++i )
					ctx.lineTo( (*polyIt)[i] );
				ctx.lineTo( (*polyIt)[0] );
			}
		}
		ctx.fill();
	}
	else {
		for( vector<vector<Vec2f> >::const_iterator polyIt = poly.begin(); polyIt != poly.end(); ++polyIt ) {
			if( polyIt->size() > 1 ) {
				ctx.moveTo( (*polyIt)[0] );
				for( int i = 1; i < polyIt->size(); ++i ) {
					ctx.lineTo( (*polyIt)[i] );
				}
				ctx.lineTo( (*polyIt)[0] );
				ctx.stroke();
			}
		}
	}
}

void drawCircles( cairo::Context &ctx, const vector<Vec2f> &circles, bool fill )
{
	ctx.setSourceRgba( 0, 0.8, 0.7, 1.0f );
	for( vector<Vec2f>::const_iterator it = circles.begin(); it != circles.end(); ++it ) {
		ctx.circle( *it, 3.0f );
		if( fill )
			ctx.fill();
		else
			ctx.stroke();
	}
}

void myDrawText( cairo::Context &ctx, float fontSize, Vec2f where, std::string what )
{
#if defined( FLI_MAC )
	ctx.setFontFace( cairo::FontFace( "MyriadPro-Regular" ) );
#else
	ctx.setFontFace( cairo::FontFace( "Arial" ) );
#endif
	ctx.setFontSize( fontSize );
	ctx.moveTo( where );
	ctx.showText( what.c_str() );
}

void PolyBoolTestApp::draw()
{
	if( ! mDoneCalculating )
		return;

	cairo::Context ctx( getCairoSurface() );

	ctx.setFillRule( cairo::FILL_RULE_EVEN_ODD );
//	ctx.setFillRule( cairo::FILL_RULE_WINDING );
	
	ctx.setSourceColor( mBackgroundColor );
	ctx.paint();

	ctx.pushMatrix();
	ctx.translate( getWindowWidth() / 3.0f, getWindowHeight() / 1.5f );
	
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

	drawCircles( ctx, mRawIntersections, mRenderFilled );
	ctx.popMatrix();
	
	std::stringstream ss;
	size_t total = 0; for( size_t t = 0; t < mResult.size(); ++t ) total += mResult[t].size();
	ss << " " << mResult.size() << " c " << total << " v ";
	myDrawText( ctx, 18, Vec2f( 10, 28 ), ( (mUseVatti)?"Vatti":"Martinez" ) );
	myDrawText( ctx, 18, Vec2f( 10, getWindowHeight() - 10 ), ss.str() );
	ss.str( "" );
	ss << "" << mRawIntersections.size() << " int";
	myDrawText( ctx, 18, Vec2f( getWindowWidth() - 100, getWindowHeight() - 10 ), ss.str() );	

	
/*	ctx.translate( 200, 200 );
	ctx.newPath();
	ctx.appendPath( mTextPath );
	ctx.stroke();
	ctx.newPath();
	ctx.appendPath( mTextPath2 );
	ctx.stroke();	*/
}

void convertToGPCPoly( const vector<vector<Vec2f> > &poly, gpc_polygon *output )
{
	output->num_contours = 0;
	output->hole = (int*)malloc( 1 );
	output->contour = (gpc_vertex_list*)malloc( 1 );
	for( vector<vector<Vec2f> >::const_iterator polyIt = poly.begin(); polyIt != poly.end(); ++polyIt ) {
		vector<gpc_vertex> vertices;
		for( size_t i = 0; i < polyIt->size(); ++i ) {
			gpc_vertex vert;
			vert.x = (*polyIt)[i].x; vert.y = (*polyIt)[i].y;
			vertices.push_back( vert );
		}
		gpc_vertex_list contour;
		contour.num_vertices = polyIt->size();
		contour.vertex = &vertices[0];
		gpc_add_contour( output, &contour, 0 );
	}
}

void convertFromGPCPoly( const gpc_polygon &poly, vector<vector<Vec2f> > *result )
{
	result->clear();
std::cout << poly.num_contours << "!!!!!\n";
	for( int c = 0; c < poly.num_contours; ++c ) {
		result->push_back( vector<Vec2f>() );
		for( int p = 0; p < poly.contour[c].num_vertices; ++p ) {
			result->back().push_back( Vec2f( poly.contour[c].vertex[p].x, poly.contour[c].vertex[p].y ) );
		}
	}
}

void PolyBoolTestApp::calculateBoolean()
{	
	if( mUseVatti ) {
		gpc_polygon subjectGPC, clipGPC, resultGPC;
		convertToGPCPoly( mSubject, &subjectGPC );
		convertToGPCPoly( mClip, &clipGPC );
		gpc_polygon_clip( gpc_op( GPC_INT + mBooleanMode ), &subjectGPC, &clipGPC, &resultGPC );
		convertFromGPCPoly( resultGPC, &mResult );
		gpc_free_polygon( &subjectGPC );
		gpc_free_polygon( &clipGPC );
		gpc_free_polygon( &resultGPC );				
	}
	else {
		mRawIntersections.clear();
		mResult = calculateIntersection( mSubject, mClip, mBooleanMode, &mRawIntersections );
	}
		
	mDoneCalculating = true;
}

void PolyBoolTestApp::writeSVG()
{
	std::string name = "PolyBool_" + mLastName + ( ( mUseVatti ) ? "_Vatti" : "_Martinez" ) + ".svg";
	cairo::SurfaceSVG surface( "/Users/andrewfb/" + name, getWindowWidth(), getWindowHeight() );
	cairo::Context ctx( &surface );
	const bool fill = mRenderFilled;
	drawPoly( ctx, mSubject, ColorA( 1, 0, 0, 1 ), fill );
	drawPoly( ctx, mClip, ColorA( 0, 1, 0, 1 ), fill );
	drawPoly( ctx, mResult, ColorA( 0, 0, 1, 0.5 ), fill );
	drawCircles( ctx, mRawIntersections, fill );
}

FLI_APP_BASIC( PolyBoolTestApp )

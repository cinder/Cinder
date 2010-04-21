#include <iostream>
#include <map>
using std::map;
#include <string>
using std::string;
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "Resources.h"

#include "flint/app/App.h"
#include "flint/cairo/Cairo.h"
#include "flint/Fill.h"	

#define TEST_SVG
#define TEST_PDF
#define TEST_PNG

using namespace fli;

struct TestBase {
 virtual void run( fli::cairo::Context &ctx ) = 0;
};

// Arc
struct TestArc : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		double xc = 128.0;
		double yc = 128.0;
		double radius = 100.0;
		double angle1 = 45.0  * ( M_PI / 180.0 );  /* angles are specified */
		double angle2 = 180.0 * ( M_PI / 180.0 );  /* in radians           */

		ctx.setLineWidth( 10.0 );
		ctx.arc( xc, yc, radius, angle1, angle2 );
		ctx.stroke();

		/* draw helping lines */
		ctx.setSourceRgba( 1, 0.2, 0.2, 0.6 );
		ctx.setLineWidth( 6.0 );

		ctx.arc( xc, yc, 10.0, 0, 2*M_PI );
		ctx.fill();

		ctx.arc( xc, yc, radius, angle1, angle1 );
		ctx.lineTo( xc, yc);
		ctx.arc( xc, yc, radius, angle2, angle2 );
		ctx.lineTo( xc, yc );
		ctx.stroke();
	}
};

// Arc Negative
struct TestArcNegative : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		double xc = 128.0;
		double yc = 128.0;
		double radius = 100.0;
		double angle1 = 45.0  * ( M_PI / 180.0 );  /* angles are specified */
		double angle2 = 180.0 * ( M_PI / 180.0 );  /* in radians           */

		ctx.setSourceRgb( 0.0, 0.0, 0.0 );
		ctx.setLineWidth( 10.0 );
		ctx.arcNegative( xc, yc, radius, angle1, angle2 );
		ctx.stroke();

		/* draw helping lines */
		ctx.setSourceRgba( 1, 0.2, 0.2, 0.6 );
		ctx.setLineWidth( 6.0 );

		ctx.arc( xc, yc, 10.0, 0, 2*M_PI );
		ctx.fill();

		ctx.arc( xc, yc, radius, angle1, angle1 );
		ctx.lineTo( xc, yc);
		ctx.arc( xc, yc, radius, angle2, angle2 );
		ctx.lineTo( xc, yc );
		ctx.stroke();
	}
};

// clip
struct TestClip : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		ctx.setSourceRgb( 0, 0, 0 );
		ctx.arc( 128.0, 128.0, 76.8, 0, 2 * M_PI );
		ctx.clip();

		ctx.newPath();  // current path is not consumed by ctx.clip()
		ctx.rectangle( 0, 0, 256, 256 );
		ctx.fill();
		ctx.setSourceRgb( 0, 1, 0 );
		ctx.moveTo( 0, 0 );
		ctx.lineTo( 256, 256 );
		ctx.moveTo( 256, 0 );
		ctx.lineTo( 0, 256 );
		ctx.setLineWidth( 10.0 );
		ctx.stroke();
	}
};

// clip image
struct TestClipImage : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		int w, h;

		ctx.arc( 128.0, 128.0, 76.8, 0, 2 * M_PI );
		ctx.clip();
		ctx.newPath(); // path not consumed by clip()

//		fli::shared_ptr<fli::cairo::SurfaceImage> image( fli::cairo::SurfaceImage::createFromPng( "/Users/andrewfb/Code/dt/libdt_2.0/test/fullCairoTest/data/romedalen.png" ) );
		Surface beyonceSurface( loadImage( app::App::loadResource( "romedalen.png", RES_ROMEDALEN_PNG, "PNG" ) ), SurfaceConstraintsCairo() );
		fli::shared_ptr<fli::cairo::SurfaceImage> image( new cairo::SurfaceImage( beyonceSurface ) );
		w = image->getWidth();
		h = image->getHeight();

		ctx.scale( 256.0 / w, 256.0 / h );

		ctx.setSourceSurface( *image, 0, 0 );
		ctx.paint();
	}
};

// curve rectangle
struct TestCurveRectangle : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		double x0 = 25.6, y0 = 25.6, rect_width = 204.8, rect_height = 204.8, radius = 102.4;
		
		double x1,y1;
		x1 = x0 + rect_width;
		y1 = y0 + rect_height;
		if( ( ! rect_width ) || ( ! rect_height ) )
			return;
		
		ctx.moveTo( x0, y0 + radius );
        ctx.curveTo( x0 , y0, x0 , y0, x0 + radius, y0 );
        ctx.lineTo( x1 - radius, y0 );
        ctx.curveTo( x1, y0, x1, y0, x1, y0 + radius );
        ctx.lineTo( x1 , y1 - radius );
        ctx.curveTo( x1, y1, x1, y1, x1 - radius, y1 );
        ctx.lineTo( x0 + radius, y1 );
        ctx.curveTo( x0, y1, x0, y1, x0, y1- radius );
        
        ctx.closePath();

		ctx.setSourceRgb( 0.5, 0.5, 1 );
		ctx.fillPreserve();
		ctx.setSourceRgba( 0.5, 0, 0, 0.5 );
		ctx.setLineWidth( 10.0 );
		ctx.stroke();
	}
};

// curve to
struct TestCurveTo : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		double x=25.6, y=128.0;
		double x1=102.4, y1=230.4, x2=153.6, y2=25.6, x3=230.4, y3=128.0;

		ctx.moveTo( x, y );
		ctx.curveTo( x1, y1, x2, y2, x3, y3 );

		ctx.setLineWidth( 10.0 );
		ctx.stroke();

		ctx.setSourceRgba( 1, 0.2, 0.2, 0.6 );
		ctx.setLineWidth( 6.0 );
		ctx.moveTo( x, y );
		ctx.lineTo( x1, y1 );
		ctx.moveTo( x2, y2 );
		ctx.lineTo( x3, y3 );
		ctx.stroke();
	}
};

// dash
struct TestDash : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		double dashes[] = {50.0,  /* ink */ 10.0,  /* skip */ 10.0,  /* ink */ 10.0   /* skip*/ };
		int ndash  = sizeof( dashes ) / sizeof( dashes[0] );
		double offset = -50.0;

		ctx.setDash( dashes, ndash, offset );
		ctx.setLineWidth( 10.0 );

		ctx.moveTo( 128.0, 25.6 );
		ctx.lineTo( 230.4, 230.4 );
		ctx.relLineTo( -102.4, 0.0 );
		ctx.curveTo( 51.2, 230.4, 51.2, 128.0, 128.0, 128.0 );

		ctx.stroke();
	}
};

// fill and stroke
struct TestFillAndStroke : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		ctx.moveTo( 128.0, 25.6 );
		ctx.lineTo( 230.4, 230.4 );
		ctx.relLineTo( -102.4, 0.0 );
		ctx.curveTo( 51.2, 230.4, 51.2, 128.0, 128.0, 128.0 );
		ctx.closePath();

		ctx.moveTo( 64.0, 25.6 );
		ctx.relLineTo( 51.2, 51.2 );
		ctx.relLineTo( -51.2, 51.2 );
		ctx.relLineTo( -51.2, -51.2 );
		ctx.closePath();

		ctx.setLineWidth( 10.0 );
		ctx.setSourceRgb( 0, 0, 1 );
		ctx.fillPreserve();
		ctx.setSourceRgb( 0, 0, 0 );
		ctx.stroke();
	}
};

// fill style
struct TestFillStyle : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		ctx.setLineWidth( 6 );

		ctx.rectangle( 12, 12, 232, 70 );
		ctx.newSubPath();
		ctx.arc( 64, 64, 40, 0, 2 * M_PI );
		ctx.newSubPath();
		ctx.arcNegative( 192, 64, 40, 0, -2 * M_PI );

		ctx.setFillRule( fli::cairo::FILL_RULE_EVEN_ODD );
		ctx.setSourceRgb( 0, 0.7, 0 );
		ctx.fillPreserve();
		ctx.setSourceRgb( 0, 0, 0 );
		ctx.stroke();

		ctx.translate( 0, 128 );
		ctx.rectangle( 12, 12, 232, 70 );
		ctx.newSubPath();
		ctx.arc( 64, 64, 40, 0, 2 * M_PI );
		ctx.newSubPath();
		ctx.arcNegative( 192, 64, 40, 0, -2 * M_PI );

		ctx.setFillRule( fli::cairo::FILL_RULE_WINDING );
		ctx.setSourceRgb( 0, 0, 0.9);
		ctx.fillPreserve();
		ctx.setSourceRgb( 0, 0, 0);
		ctx.stroke();
	}
};

// gradient
struct TestGradient : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		shared_ptr<fli::cairo::Pattern> pat;

		pat = shared_ptr<fli::cairo::Pattern>( fli::cairo::Pattern::createLinear( 0.0, 0.0, 0.0, 256.0 ) );
		pat->addColorStopRgba( 1, 0, 0, 0, 1 );
		pat->addColorStopRgba( 0, 1, 1, 1, 1);
		ctx.rectangle( 0, 0, 256, 256 );
		ctx.setSource( pat.get() );
		ctx.fill();

		pat = shared_ptr<fli::cairo::Pattern>( fli::cairo::Pattern::createRadial( 115.2, 102.4, 25.6, 102.4, 102.4, 128.0 ) );
		pat->addColorStopRgba( 0, 1, 1, 1, 1 );
		pat->addColorStopRgba( 1, 0, 0, 0, 1 );
		ctx.setSource( pat.get() );
		ctx.arc( 128.0, 128.0, 76.8, 0, 2 * M_PI );
		ctx.fill();
	}
};

// image
struct TestImage : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		Surface beyonceSurface( loadImage( app::App::loadResource( "Beyonce.jpg", RES_BEYONCE_JPG, "JPG" ) ), SurfaceConstraintsCairo() );
		fli::shared_ptr<fli::cairo::SurfaceImage> image( new cairo::SurfaceImage( beyonceSurface ) );


		ctx.translate( 128.0, 128.0 );
		ctx.rotate( 45 * M_PI / 180 );
		ctx.scale( 256.0 / image->getWidth(), 256.0 / image->getHeight() );
		ctx.translate( -0.5 * image->getWidth(), -0.5 * image->getHeight() );

		ctx.setSourceSurface( *image, 0, 0 );
		ctx.paint();
	}
};

// image pattern
struct TestImagePattern : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		Surface beyonceSurface( loadImage( app::App::loadResource( "Beyonce.jpg", RES_BEYONCE_JPG, "JPG" ) ), SurfaceConstraintsCairo() );
		fli::shared_ptr<fli::cairo::SurfaceImage> image( new cairo::SurfaceImage( beyonceSurface ) );
		fli::shared_ptr<fli::cairo::Pattern> pattern( fli::cairo::Pattern::createForSurface( image.get() ) );		
		pattern->setExtend( fli::cairo::EXTEND_REPEAT );

		ctx.translate( 128.0, 128.0 );
		ctx.rotate( M_PI / 4 );
		ctx.scale( 1 / sqrt( 2.0 ), 1 / sqrt( 2.0 ) );
		ctx.translate( -128.0, -128.0 );

		fli::cairo::Matrix mtx;
		mtx.initScale( image->getWidth() / 256.0 * 5.0, image->getHeight() / 256.0 * 5.0 );
		pattern->setMatrix( &mtx );
		
		ctx.setSource( pattern.get() );
		ctx.rectangle( 0, 0, 256.0, 256.0 );
		ctx.fill();
	}
};

// multi segment caps
struct TestMultiSegmentCaps : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		ctx.moveTo( 50.0, 75.0 );
		ctx.lineTo( 200.0, 75.0 );

		ctx.moveTo( 50.0, 125.0 );
		ctx.lineTo( 200.0, 125.0 );

		ctx.moveTo( 50.0, 175.0 );
		ctx.lineTo( 200.0, 175.0 );

		ctx.setLineWidth( 30.0 );
		ctx.setLineCap( fli::cairo::LINE_CAP_ROUND );
		ctx.stroke();
	}
};

// set line cap
struct TestSetLineCap : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		ctx.setLineWidth( 30.0 );
		ctx.setLineCap( fli::cairo::LINE_CAP_BUTT ); /* default */
		ctx.moveTo( 64.0, 50.0 );
		ctx.lineTo( 64.0, 200.0 );
		ctx.stroke();
		ctx.setLineCap( fli::cairo::LINE_CAP_ROUND );
		ctx.moveTo( 128.0, 50.0 );
		ctx.lineTo( 128.0, 200.0 );
		ctx.stroke();
		ctx.setLineCap( fli::cairo::LINE_CAP_SQUARE );
		ctx.moveTo( 192.0, 50.0 );
		ctx.lineTo( 192.0, 200.0 );
		ctx.stroke();

		/* draw helping lines */
		ctx.setSourceRgb( 1, 0.2, 0.2 );
		ctx.setLineWidth( 2.56 );
		ctx.moveTo( 64.0, 50.0 );
		ctx.lineTo( 64.0, 200.0 );
		ctx.moveTo( 128.0, 50.0 );
		ctx.lineTo( 128.0, 200.0 );
		ctx.moveTo( 192.0, 50.0 );
		ctx.lineTo( 192.0, 200.0 );
		ctx.stroke();
	}
};

// set line join
struct TestSetLineJoin : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		ctx.setLineWidth( 40.96 );
		ctx.moveTo( 76.8, 84.48 );
		ctx.relLineTo( 51.2, -51.2 );
		ctx.relLineTo( 51.2, 51.2 );
		ctx.setLineJoin( fli::cairo::LINE_JOIN_MITER );
		ctx.stroke();

		ctx.moveTo( 76.8, 161.28 );
		ctx.relLineTo( 51.2, -51.2 );
		ctx.relLineTo( 51.2, 51.2 );
		ctx.setLineJoin( fli::cairo::LINE_JOIN_BEVEL );
		ctx.stroke();

		ctx.moveTo( 76.8, 238.08 );
		ctx.relLineTo( 51.2, -51.2 );
		ctx.relLineTo( 51.2, 51.2 );
		ctx.setLineJoin( fli::cairo::LINE_JOIN_ROUND );
		ctx.stroke();
	}
};

// text
struct TestText : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		ctx.selectFontFace( "Batang", fli::cairo::FONT_SLANT_NORMAL, fli::cairo::FONT_WEIGHT_BOLD );
		ctx.setFontSize( 90.0 );

		ctx.moveTo( 10.0, 135.0 );
		ctx.showText( "Hello" );

		ctx.moveTo( 70.0, 165.0 );

// This code crashes PDF on the Mac
//	fli::cairo::FontFace *otherFont = new fli::cairo::FontFace( "Baskerville" );
//		cairo_set_font_face( ctx.getCairo(), cf );

		ctx.textPath( "World" );
		ctx.setSourceRgb( 0.5, 0.5, 1 );
		ctx.fillPreserve();
		ctx.setSourceRgb( 0, 0, 0 );
		ctx.setLineWidth( 2.56 );
		ctx.stroke();

		// draw helping lines
		ctx.setSourceRgba( 1, 0.2, 0.2, 0.6 );
		ctx.arc( 10.0, 135.0, 5.12, 0, 2 * M_PI );
		ctx.closePath();
		ctx.arc( 70.0, 165.0, 5.12, 0, 2 * M_PI );
		ctx.fill();
	}
};

// text align center
struct TestTextAlignCenter : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		const std::string text = "cairo";
		double x,y;

		ctx.selectFontFace( "Sans", fli::cairo::FONT_SLANT_NORMAL, fli::cairo::FONT_WEIGHT_NORMAL );

		ctx.setFontSize( 52.0 );
		fli::cairo::TextExtents extents = ctx.textExtents( text );
		x = 128.0 - ( extents.width() / 2 + extents.xBearing() );
		y = 128.0 - ( extents.height() / 2 + extents.yBearing() );

		ctx.moveTo( x, y );
		ctx.showText( text );

		// draw helping lines
		ctx.setSourceRgba( 1, 0.2, 0.2, 0.6 );
		ctx.setLineWidth( 6.0 );
		ctx.arc( x, y, 10.0, 0, 2 * M_PI );
		ctx.fill();
		ctx.moveTo( 128.0, 0 );
		ctx.relLineTo( 0, 256 );
		ctx.moveTo( 0, 128.0 );
		ctx.relLineTo( 256, 0 );
		ctx.stroke();
	}
};

// text extents
struct TestTextExtents : public TestBase {
	virtual void run( fli::cairo::Context &ctx ) {
		const std::string text = "cairo";
		double x,y;

		ctx.selectFontFace( "Sans", fli::cairo::FONT_SLANT_NORMAL, fli::cairo::FONT_WEIGHT_NORMAL );

		ctx.setFontSize( 100.0 );
		fli::cairo::TextExtents extents = ctx.textExtents( text );
		x = 25.0;
		y = 150.0;

		ctx.moveTo( x, y );
		ctx.showText( text );

		// draw helping lines
		ctx.setSourceRgba( 1, 0.2, 0.2, 0.6 );
		ctx.setLineWidth( 6.0 );
		ctx.arc( x, y, 10.0, 0, 2 * M_PI );
		ctx.fill();
		ctx.moveTo( x, y );
		ctx.relLineTo( 0, -extents.height() );
		ctx.relLineTo( extents.width(), 0 );
		ctx.relLineTo( extents.xBearing(), -extents.yBearing() );
		ctx.stroke();
	}
};

int main( int argc, char **argv ) 
{
	map<string,TestBase*> tests;
	
	tests["arc"] = new TestArc();
	tests["arc_negative"] = new TestArcNegative();
	tests["clip"] = new TestClip();
	tests["clip_image"] = new TestClipImage();
	tests["curve_rectangle"] = new TestCurveRectangle();
	tests["curve_to"] = new TestCurveTo();
	tests["dash"] = new TestDash();
	tests["fill_and_stroke"] = new TestFillAndStroke();
	tests["fill_style"] = new TestFillStyle();
	tests["gradient"] = new TestGradient();
	tests["image"] = new TestImage();
	tests["image_pattern"] = new TestImagePattern();
	tests["multi_segment_caps"] = new TestMultiSegmentCaps();
	tests["set_line_cap"] = new TestSetLineCap();
	tests["set_line_join"] = new TestSetLineJoin();
	tests["text"] = new TestText();
	tests["text_align_center"] = new TestTextAlignCenter();
	tests["text_extents"] = new TestTextExtents();
#if defined( TEST_PNG )
	{
		std::cout << "-= Testing PNG =-" << std::endl;
		fli::cairo::SurfaceImage surface( 512, 512 );
		for( map<string,TestBase*>::const_iterator it = tests.begin(); it != tests.end(); ++it ) {
			fli::cairo::Context ctx( &surface );
			std::cout << "Running test: " << it->first << std::endl;
			fli::fill( &surface.getSurface(), fli::Color8u( 255, 255, 255 ) );
			it->second->run( ctx );
			surface.writeToPng( it->first + ".png" );
		}
	}
#endif

#if defined( TEST_PDF )
	{
		std::cout << "-= Testing PDF =-" << std::endl;
		fli::cairo::SurfacePDF surface( "tests.pdf", 512, 512 );
		fli::cairo::Context ctx( &surface );
		ctx.save();
		for( map<string,TestBase*>::const_iterator it = tests.begin(); it != tests.end(); ++it ) {
			std::cout << "Running test: " << it->first << std::endl;
			it->second->run( ctx );
			ctx.showPage();
			ctx.restore();
			ctx.save();
		}
	}
#endif

#if defined( TEST_SVG )
	{
		std::cout << "-= Testing SVG =-" << std::endl;
		for( map<string,TestBase*>::const_iterator it = tests.begin(); it != tests.end(); ++it ) {
			fli::cairo::SurfaceSVG surface( ( it->first + ".svg" ).c_str(), 512, 512 );
			fli::cairo::Context ctx( &surface );
			std::cout << "Running test: " << it->first << std::endl;
			it->second->run( ctx );
			ctx.showPage();
		}
	}
#endif

	return 0;
}

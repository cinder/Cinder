/*
 Copyright (c) 2010, The Cinder Project (http://libcinder.org)

 Portions Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/cairo/Cairo.h"
#include "cinder/svg/Svg.h"
#include "cinder/ip/Premultiply.h"
#include "cinder/Text.h"

#include <cairo.h>
#include <cairo-svg.h>
#include <cairo-pdf.h>
#include <cairo-ps.h>
#if defined( CINDER_COCOA )
	#include "cinder/app/App.h"
	#if defined( CINDER_MAC )
		#include <ApplicationServices/ApplicationServices.h>
	#endif
	#include <cairo-quartz.h>
	#include "cinder/cocoa/CinderCocoa.h"
#elif defined( CINDER_MSW )
	#include "cinder/app/App.h"
	#include <cairo-win32.h>

FILE _iob[] = {*stdin, *stdout, *stderr};

extern "C" FILE * __cdecl __iob_func(void)
{
    return _iob;
}
#endif

using std::vector;

namespace cinder {


SurfaceChannelOrder SurfaceConstraintsCairo::getChannelOrder( bool alpha ) const
{
	return ( alpha ) ? SurfaceChannelOrder::BGRA : SurfaceChannelOrder::BGRX;
}

ptrdiff_t SurfaceConstraintsCairo::getRowBytes( int requestedWidth, const SurfaceChannelOrder &sco, int elementSize ) const {
	return cairo_format_stride_for_width( sco.hasAlpha() ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24, requestedWidth );
}

} // namespace cinder

namespace cinder { namespace cairo {

class SvgRendererCairo;

const int32_t	FONT_SLANT_NORMAL	= CAIRO_FONT_SLANT_NORMAL;
const int32_t	FONT_SLANT_ITALIC	= CAIRO_FONT_SLANT_ITALIC;
const int32_t	FONT_SLANT_OBLIQUE	= CAIRO_FONT_SLANT_OBLIQUE;
const int32_t	FONT_WEIGHT_NORMAL	= CAIRO_FONT_WEIGHT_NORMAL;
const int32_t	FONT_WEIGHT_BOLD	= CAIRO_FONT_WEIGHT_BOLD;

const int32_t	FILL_RULE_WINDING	= CAIRO_FILL_RULE_WINDING;
const int32_t	FILL_RULE_EVEN_ODD	= CAIRO_FILL_RULE_EVEN_ODD;

const int32_t	EXTEND_NONE		= CAIRO_EXTEND_NONE;
const int32_t	EXTEND_REPEAT	= CAIRO_EXTEND_REPEAT;
const int32_t	EXTEND_REFLECT	= CAIRO_EXTEND_REFLECT;
const int32_t	EXTEND_PAD		= CAIRO_EXTEND_PAD;

const int32_t	LINE_CAP_BUTT	= CAIRO_LINE_CAP_BUTT;
const int32_t	LINE_CAP_ROUND	= CAIRO_LINE_CAP_ROUND;
const int32_t	LINE_CAP_SQUARE	= CAIRO_LINE_CAP_SQUARE;

const int32_t	LINE_JOIN_MITER	= CAIRO_LINE_JOIN_MITER;
const int32_t	LINE_JOIN_ROUND	= CAIRO_LINE_JOIN_ROUND;
const int32_t	LINE_JOIN_BEVEL	= CAIRO_LINE_JOIN_BEVEL;

const int32_t	HINT_METRICS_DEFAULT	= CAIRO_HINT_METRICS_DEFAULT;
const int32_t	HINT_METRICS_OFF		= CAIRO_HINT_METRICS_OFF;
const int32_t	HINT_METRICS_ON		= CAIRO_HINT_METRICS_ON;
const int32_t	HINT_STYLE_DEFAULT		= CAIRO_HINT_STYLE_DEFAULT;
const int32_t	HINT_STYLE_NONE		= CAIRO_HINT_STYLE_NONE;
const int32_t	HINT_STYLE_SLIGHT		= CAIRO_HINT_STYLE_SLIGHT;
const int32_t	HINT_STYLE_MEDIUM		= CAIRO_HINT_STYLE_MEDIUM;
const int32_t	HINT_STYLE_FULL		= CAIRO_HINT_STYLE_FULL;

const int32_t	ANTIALIAS_DEFAULT		= CAIRO_ANTIALIAS_DEFAULT;
const int32_t	ANTIALIAS_NONE			= CAIRO_ANTIALIAS_NONE;
const int32_t	ANTIALIAS_GRAY			= CAIRO_ANTIALIAS_GRAY;
const int32_t	ANTIALIAS_SUBPIXEL		= CAIRO_ANTIALIAS_SUBPIXEL;

const int32_t	SUBPIXEL_ORDER_DEFAULT	= CAIRO_SUBPIXEL_ORDER_DEFAULT;
const int32_t	SUBPIXEL_ORDER_RGB		= CAIRO_SUBPIXEL_ORDER_RGB;
const int32_t	SUBPIXEL_ORDER_BGR		= CAIRO_SUBPIXEL_ORDER_BGR;
const int32_t	SUBPIXEL_ORDER_VRGB	= CAIRO_SUBPIXEL_ORDER_VRGB;
const int32_t	SUBPIXEL_ORDER_VBGR	= CAIRO_SUBPIXEL_ORDER_VBGR;

const int32_t STATUS_SUCCESS					= CAIRO_STATUS_SUCCESS;
const int32_t STATUS_NO_AVAILABLE_MEMORY		= CAIRO_STATUS_NO_MEMORY;
const int32_t STATUS_INVALID_RESTORE			= CAIRO_STATUS_INVALID_RESTORE;
const int32_t STATUS_INVALID_POP_GROUP		= CAIRO_STATUS_INVALID_POP_GROUP;
const int32_t STATUS_NO_CURRENT_POINT		= CAIRO_STATUS_NO_CURRENT_POINT;
const int32_t STATUS_INVALID_MATRIX			= CAIRO_STATUS_INVALID_MATRIX;
const int32_t STATUS_INVALID_STATUS			= CAIRO_STATUS_INVALID_STATUS;
const int32_t STATUS_NULL_POINTER			= CAIRO_STATUS_NULL_POINTER;
const int32_t STATUS_INVALID_STRING			= CAIRO_STATUS_INVALID_STRING;
const int32_t STATUS_INVALID_PATH_DATA		= CAIRO_STATUS_INVALID_PATH_DATA;
const int32_t STATUS_READ_ERROR				= CAIRO_STATUS_READ_ERROR;
const int32_t STATUS_WRITE_ERROR				= CAIRO_STATUS_WRITE_ERROR;
const int32_t STATUS_SURFACE_FINISHED		= CAIRO_STATUS_SURFACE_FINISHED;
const int32_t STATUS_SURFACE_TYPE_MISMATCH	= CAIRO_STATUS_SURFACE_TYPE_MISMATCH;
const int32_t STATUS_PATTERN_TYPE_MISMATCH	= CAIRO_STATUS_PATTERN_TYPE_MISMATCH;
const int32_t STATUS_INVALID_CONTENT			= CAIRO_STATUS_INVALID_CONTENT;
const int32_t STATUS_INVALID_FORMAT			= CAIRO_STATUS_INVALID_FORMAT;
const int32_t STATUS_INVALID_VISUAL			= CAIRO_STATUS_INVALID_VISUAL;
const int32_t STATUS_FILE_NOT_FOUND			= CAIRO_STATUS_FILE_NOT_FOUND;
const int32_t STATUS_INVALID_DASH			= CAIRO_STATUS_INVALID_DASH;
const int32_t STATUS_INVALID_DSC_COMMENT		= CAIRO_STATUS_INVALID_DSC_COMMENT;
const int32_t STATUS_INVALID_INDEX			= CAIRO_STATUS_INVALID_INDEX;
const int32_t STATUS_CLIP_NOT_REPRESENTABLE	= CAIRO_STATUS_CLIP_NOT_REPRESENTABLE;


/////////////////////////////////////////////////////////////////////////////
// SurfaceBase
SurfaceBase::SurfaceBase( const SurfaceBase &other )
	: mWidth( other.mWidth ), mHeight( other.mHeight ), mCairoSurface( other.mCairoSurface ) 
{
	if( mCairoSurface ) {
		cairo_surface_reference( mCairoSurface );
	}
}

SurfaceBase& SurfaceBase::operator=( const SurfaceBase &other )
{
	mWidth = other.mWidth;
	mHeight = other.mHeight;
	cairo_surface_t *oldCairoSurface = mCairoSurface;
	mCairoSurface = other.mCairoSurface;
	if( mCairoSurface ) {
		cairo_surface_reference( mCairoSurface );
	}
	if( oldCairoSurface )
		cairo_surface_destroy( oldCairoSurface );
	return *this;
}

SurfaceBase::~SurfaceBase()
{
	if( mCairoSurface ) {
		cairo_surface_destroy( mCairoSurface ); 
	}
}

void SurfaceBase::flush()
{
	cairo_surface_flush( mCairoSurface );
}

/////////////////////////////////////////////////////////////////////////////
// SurfaceImage
SurfaceImage::SurfaceImage( int32_t width, int32_t height, bool hasAlpha ) 
	: SurfaceBase( width, height )
{
	if( hasAlpha ) {
		mCairoSurface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, width, height ); 
	}
	else {
		mCairoSurface = cairo_image_surface_create( CAIRO_FORMAT_RGB24, width, height );
	}

	initCinderSurface( hasAlpha, mCairoSurface );
}

SurfaceImage::SurfaceImage( const uint8_t *dataPtr, int32_t width, int32_t height, int32_t stride, bool hasAlpha ) 
	: SurfaceBase( width, height )
{
	if( hasAlpha ) {
		mCairoSurface = cairo_image_surface_create_for_data( const_cast<unsigned char*>( dataPtr ), CAIRO_FORMAT_ARGB32, width, height, stride );
	} 
	else {
		mCairoSurface = cairo_image_surface_create_for_data( const_cast<unsigned char*>( dataPtr ), CAIRO_FORMAT_RGB24, width, height, stride );
	}
	
	initCinderSurface( hasAlpha, mCairoSurface );
}

SurfaceImage::SurfaceImage( cinder::Surface ciSurface ) 
	: SurfaceBase( ciSurface.getWidth(), ciSurface.getHeight() )
{
	bool needsManualCopy = true;
	cairo_format_t format = ( ciSurface.getChannelOrder().hasAlpha() ) ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24;
	bool legalRowBytes = cairo_format_stride_for_width( format, ciSurface.getWidth() ) == ciSurface.getRowBytes();
	if( legalRowBytes && ( ciSurface.getChannelOrder() == cinder::SurfaceChannelOrder::BGRA ) ) {
		mCairoSurface = cairo_image_surface_create_for_data( const_cast<unsigned char*>( ciSurface.getData() ), CAIRO_FORMAT_ARGB32, ciSurface.getWidth(), ciSurface.getHeight(), ciSurface.getRowBytes() );
		needsManualCopy = false;
	} 
	else if( legalRowBytes && ( ciSurface.getChannelOrder() == cinder::SurfaceChannelOrder::BGRX ) ) {
		mCairoSurface = cairo_image_surface_create_for_data( const_cast<unsigned char*>( ciSurface.getData() ), CAIRO_FORMAT_RGB24, ciSurface.getWidth(), ciSurface.getHeight(), ciSurface.getRowBytes() );
		needsManualCopy = false;
	}
	else { // we can't natively represent this Surface configuration, so we'll just allocate one and manually copy it
		mCairoSurface = cairo_image_surface_create( ciSurface.hasAlpha() ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24, ciSurface.getWidth(), ciSurface.getHeight() );
	}
	
	initCinderSurface( ciSurface.hasAlpha(), mCairoSurface );

	if( needsManualCopy )
		mCinderSurface.copyFrom( ciSurface, ciSurface.getBounds(), ivec2( 0 ) );
	cairo_surface_mark_dirty( mCairoSurface );
}

SurfaceImage::SurfaceImage( ImageSourceRef imageSource )
	: SurfaceBase( imageSource->getWidth(), imageSource->getHeight() )
{
	mCairoSurface = cairo_image_surface_create( imageSource->hasAlpha() ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24, imageSource->getWidth(), imageSource->getHeight() );
	initCinderSurface( imageSource->hasAlpha(), mCairoSurface );
	writeImage( (ImageTargetRef)mCinderSurface, imageSource );
	cairo_surface_mark_dirty( mCairoSurface );
}

SurfaceImage::SurfaceImage( const SurfaceImage &other )
	: SurfaceBase( other )
{
	initCinderSurface( other.mCinderSurface.hasAlpha(), mCairoSurface );
}

SurfaceImage& SurfaceImage::operator=( const SurfaceImage &other )
{
	this->SurfaceBase::operator=( other );
	initCinderSurface( other.mCinderSurface.hasAlpha(), mCairoSurface );
	return *this;
}

uint8_t* SurfaceImage::getData() 
{
	return cairo_image_surface_get_data( mCairoSurface );
}

int32_t SurfaceImage::getStride() const 
{
	return cairo_image_surface_get_stride( mCairoSurface );
}

void SurfaceImage::markDirty()
{
	cairo_surface_mark_dirty( mCairoSurface );
}

void SurfaceImage::surfaceDeallocator( void *data )
{
	cairo_surface_t *cairoSurface = reinterpret_cast<cairo_surface_t*>( data );
	cairo_surface_destroy( cairoSurface );
}

void SurfaceImage::initCinderSurface( bool alpha, cairo_surface_t *cairoSurface )
{
	unsigned char *data = cairo_image_surface_get_data( cairoSurface );
	int stride = cairo_image_surface_get_stride( cairoSurface );

	if( alpha )
		mCinderSurface = Surface( data, mWidth, mHeight, stride, SurfaceChannelOrder::BGRA );
	else
		mCinderSurface = Surface( data, mWidth, mHeight, stride, SurfaceChannelOrder::BGRX );
}

/////////////////////////////////////////////////////////////////////////////
// SurfaceSvg
SurfaceSvg::SurfaceSvg( const fs::path &filePath, uint32_t width, uint32_t height )
	: SurfaceBase( width, height )
{
	mCairoSurface = cairo_svg_surface_create( filePath.string().c_str(), width, height );
}

SurfaceSvg::SurfaceSvg( const SurfaceSvg &other )
	: SurfaceBase( other )
{
}

/////////////////////////////////////////////////////////////////////////////
// SurfacePdf
SurfacePdf::SurfacePdf( const fs::path &filePath, double widthInPoints, double heightInPoints )
	: SurfaceBase( (int32_t)widthInPoints, (int32_t)heightInPoints )
{
	mCairoSurface = cairo_pdf_surface_create( filePath.string().c_str(), widthInPoints, heightInPoints ); 
}

SurfacePdf::SurfacePdf( const SurfacePdf &other )
	: SurfaceBase( other )
{
}

void SurfacePdf::setSize( double widthInPoints, double heightInPoints )
{
	cairo_pdf_surface_set_size( mCairoSurface, widthInPoints, heightInPoints );
}

/////////////////////////////////////////////////////////////////////////////
// SurfacePs
SurfacePs::SurfacePs( const fs::path &filePath, double widthInPoints, double heightInPoints, bool enableLevel3 )
	: SurfaceBase( (int32_t)widthInPoints, (int32_t)heightInPoints )
{
	mCairoSurface = cairo_ps_surface_create( filePath.string().c_str(), widthInPoints, heightInPoints ); 
	cairo_ps_surface_restrict_to_level( mCairoSurface, ( enableLevel3 ) ? CAIRO_PS_LEVEL_3 : CAIRO_PS_LEVEL_2 );
}

SurfacePs::SurfacePs( const SurfacePs &other )
	: SurfaceBase( other )
{
}

void SurfacePs::setSize( double widthInPoints, double heightInPoints )
{
	cairo_ps_surface_set_size( mCairoSurface, widthInPoints, heightInPoints );
}

void SurfacePs::dscBeginSetup()
{
	cairo_ps_surface_dsc_begin_setup( mCairoSurface );
}

void SurfacePs::dscBeginPageSetup()
{
	cairo_ps_surface_dsc_begin_page_setup( mCairoSurface );
}

void SurfacePs::dscComment( const char *comment )
{
	cairo_ps_surface_dsc_comment( mCairoSurface, comment );
}

/////////////////////////////////////////////////////////////////////////////
// SurfaceEps
SurfaceEps::SurfaceEps( const fs::path &filePath, double widthInPoints, double heightInPoints, bool enableLevel3 )
	: SurfaceBase( (int32_t)widthInPoints, (int32_t)heightInPoints )
{
	mCairoSurface = cairo_ps_surface_create( filePath.string().c_str(), widthInPoints, heightInPoints ); 
	cairo_ps_surface_set_eps( mCairoSurface, TRUE );
	cairo_ps_surface_restrict_to_level( mCairoSurface, ( enableLevel3 ) ? CAIRO_PS_LEVEL_3 : CAIRO_PS_LEVEL_2 );
}

SurfaceEps::SurfaceEps( const SurfaceEps &other )
	: SurfaceBase( other )
{
}

void SurfaceEps::setSize( double widthInPoints, double heightInPoints )
{
	cairo_ps_surface_set_size( mCairoSurface, widthInPoints, heightInPoints );
}

void SurfaceEps::dscBeginSetup()
{
	cairo_ps_surface_dsc_begin_setup( mCairoSurface );
}

void SurfaceEps::dscBeginPageSetup()
{
	cairo_ps_surface_dsc_begin_page_setup( mCairoSurface );
}

void SurfaceEps::dscComment( const char *comment )
{
	cairo_ps_surface_dsc_comment( mCairoSurface, comment );
}

/////////////////////////////////////////////////////////////////////////////
// SurfaceQuartz
#if defined( CINDER_COCOA )
SurfaceQuartz::SurfaceQuartz( CGContextRef cgContext, int32_t width, int32_t height )
	: SurfaceBase( width, height ), mCgContextRef( cgContext )
{
	mCairoSurface = cairo_quartz_surface_create_for_cg_context( cgContext, width, height ); 
}

SurfaceQuartz::SurfaceQuartz( const SurfaceQuartz &other )
	: SurfaceBase( other ), mCgContextRef( other.mCgContextRef )
{
}

CGContextRef SurfaceQuartz::getCgContextRef()
{
	return mCgContextRef;
}


/////////////////////////////////////////////////////////////////////////////
// SurfaceCgBitmapContext
SurfaceCgBitmapContext::SurfaceCgBitmapContext( int32_t width, int32_t height, bool alpha )
	: SurfaceBase( width, height )
{
	mSurface = cinder::Surface( width, height, alpha, SurfaceConstraintsCgBitmapContext() );
	mCgContextRef = cinder::cocoa::createCgBitmapContext( mSurface );
	// Need to flip this vertically since Quartz is lower-left origin
	::CGContextTranslateCTM( mCgContextRef, 0.0f, height );
	::CGContextScaleCTM( mCgContextRef, 1.0f, -1.0f );

	mCairoSurface = cairo_quartz_surface_create_for_cg_context( mCgContextRef, width, height ); 
}

SurfaceCgBitmapContext::SurfaceCgBitmapContext( const SurfaceCgBitmapContext &other )
	: SurfaceBase( other ), mCgContextRef( other.mCgContextRef ), mSurface( other.mSurface )
{
}

#endif // defined( CINDER_COCOA )

/////////////////////////////////////////////////////////////////////////////
// SurfaceGdi
#if defined( CINDER_MSW )
SurfaceGdi::SurfaceGdi( HDC hdc )
	: SurfaceBase(), mDc( hdc )
{
	mCairoSurface = cairo_win32_surface_create( hdc );

	RECT rect;
	::GetClipBox( hdc, &rect );
	mWidth = rect.right - rect.left;
	mHeight = rect.bottom - rect.top;
}

SurfaceGdi::SurfaceGdi( const SurfaceGdi &other )
	: SurfaceBase( other ), mDc( other.mDc )
{
}

#endif // defined( CINDER_MSW )

/////////////////////////////////////////////////////////////////////////////
// Matrix
Matrix::Matrix()
{
}

Matrix::Matrix( double xx_, double yx_, double xy_, double yy_, double x0_, double y0_ )
{
	xx = xx_;
	yx = yx_;
	xy = xy_;
	yy = yy_;
	x0 = x0_;
	y0 = y0_;
}

void Matrix::init( double xx_, double yx_, double xy_, double yy_, double x0_, double y0_ )
{
	xx = xx_;
	yx = yx_;
	xy = xy_;
	yy = yy_;
	x0 = x0_;
	y0 = y0_;
}

Matrix::Matrix( const mat3 &m )
{
	xx = m[0][0];
	yx = m[0][1];
	xy = m[1][0];
	yy = m[1][1];
	x0 = m[2][0];
	y0 = m[2][1];
}

void Matrix::initIdentity()
{
	cairo_matrix_init_identity( &getCairoMatrix() );
}

void Matrix::initTranslate( double tx, double ty )
{
	cairo_matrix_init_translate( &getCairoMatrix(), tx, ty );
}

void Matrix::initScale( double sx, double sy )
{
	cairo_matrix_init_scale( &getCairoMatrix(), sx, sy );
}

void Matrix::initRotate( double radians )
{
	cairo_matrix_init_rotate( &getCairoMatrix(), radians );
}

void Matrix::translate( double tx, double ty )
{
	cairo_matrix_translate( &getCairoMatrix(), tx, ty );
}

void Matrix::scale( double sx, double sy )
{
	cairo_matrix_scale( &getCairoMatrix(), sx, sy );
}

void Matrix::rotate( double radians )
{
	cairo_matrix_rotate( &getCairoMatrix(), radians );
}

int32_t	Matrix::invert()
{
	return static_cast<int32_t>( cairo_matrix_invert( &getCairoMatrix() ) );
}

const Matrix& Matrix::operator*=( const Matrix &rhs )
{
	cairo_matrix_t r;
	cairo_matrix_multiply( &r, &getCairoMatrix(), &rhs.getCairoMatrix() );
	init( r.xx, r.yx, r.xy, r.yy, r.x0, r.y0 );
	return *this;
}

vec2 Matrix::transformPoint( const vec2 &v ) const
{
	double x = v.x, y = v.y;
	cairo_matrix_transform_point( &getCairoMatrix(), &x, &y );
	return vec2( (float)x, (float)y );
}

vec2 Matrix::transformDistance( const vec2 &v ) const
{
	double x = v.x, y = v.y;
	cairo_matrix_transform_distance( &getCairoMatrix(), &x, &y );
	return vec2( (float)x, (float)y );
}

/////////////////////////////////////////////////////////////////////////////
// Pattern
Pattern::Pattern( const Pattern &other )
{
	cairo_pattern_t *oldPattern = mCairoPattern;
	mCairoPattern = other.mCairoPattern;
	if( mCairoPattern )
		cairo_pattern_reference( mCairoPattern );
	if( oldPattern )
		cairo_pattern_destroy( oldPattern );
}

Pattern::~Pattern()
{ 
	if( mCairoPattern )
		cairo_pattern_destroy( mCairoPattern );
}

Pattern& Pattern::operator=( const Pattern &other )
{
	cairo_pattern_t *oldPattern = mCairoPattern;
	mCairoPattern = other.mCairoPattern;
	if( mCairoPattern )
		cairo_pattern_reference( mCairoPattern );
	if( oldPattern )
		cairo_pattern_destroy( oldPattern );
	return *this;
}

void Pattern::setExtend( int extend )
{
	cairo_pattern_set_extend( mCairoPattern, static_cast<cairo_extend_t>( extend ) );
}

void Pattern::setExtendNone()
{
	cairo_pattern_set_extend( mCairoPattern, CAIRO_EXTEND_NONE );
}

void Pattern::setExtendRepeat()
{
	cairo_pattern_set_extend( mCairoPattern, CAIRO_EXTEND_REPEAT );
}

void Pattern::setExtendReflect()
{
	cairo_pattern_set_extend( mCairoPattern, CAIRO_EXTEND_REFLECT );
}

void Pattern::setExtendPad()
{
	cairo_pattern_set_extend( mCairoPattern, CAIRO_EXTEND_PAD );
}

int Pattern::getExtend() const
{
	return static_cast<int>( cairo_pattern_get_extend( mCairoPattern ) );
}

void Pattern::setFilter( int filter )
{
	cairo_pattern_set_filter( mCairoPattern, static_cast<cairo_filter_t>( filter ) );
}

int	Pattern::getFilter() const
{
	return static_cast<int>( cairo_pattern_get_filter( mCairoPattern ) );
}

void Pattern::setMatrix( const Matrix &matrix )
{
	cairo_pattern_set_matrix( mCairoPattern, &matrix.getCairoMatrix() );
}

Matrix Pattern::getMatrix() const
{
	Matrix result;
	cairo_pattern_get_matrix( mCairoPattern, &result.getCairoMatrix() );
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// Gradient
Gradient::Gradient( cairo_pattern_t *pattern )
	: Pattern( pattern )
{
}

void Gradient::addColorStopRgb( double offset, double red, double green, double blue )
{
	cairo_pattern_add_color_stop_rgb( mCairoPattern, offset, red, green, blue );
}

void Gradient::addColorStopRgba( double offset, double red, double green, double blue, double alpha )
{
	cairo_pattern_add_color_stop_rgba( mCairoPattern, offset, red, green, blue, alpha );
}

int Gradient::getColorStopCount()
{
	int result;
	cairo_pattern_get_color_stop_count( mCairoPattern, &result );
	return result;
}

void Gradient::getColorStopRgba( int index, double *offset, double *red, double *green, double *blue, double *alpha )
{
	cairo_pattern_get_color_stop_rgba( mCairoPattern, index, offset, red, green, blue, alpha );
}

/////////////////////////////////////////////////////////////////////////////
// PatternSolid
PatternSolid::PatternSolid( const ColorA &color )
	: Pattern( cairo_pattern_create_rgba( color.r, color.g, color.b, color.a ) )
{
}

PatternSolid::PatternSolid( const Color &color )
	: Pattern( cairo_pattern_create_rgb( color.r, color.g, color.b ) )
{
}

/////////////////////////////////////////////////////////////////////////////
// PatternSurface
PatternSurface::PatternSurface( SurfaceBase &surface )
	: Pattern( cairo_pattern_create_for_surface( surface.getCairoSurface() ) )
{
}

PatternSurface::PatternSurface( ci::Surface cinderSurface )
	: Pattern( cairo_pattern_create_for_surface( SurfaceImage( cinderSurface ).getCairoSurface() ) )
{
}

PatternSurface::PatternSurface( ImageSourceRef imageSource )
	: Pattern( cairo_pattern_create_for_surface( SurfaceImage( imageSource ).getCairoSurface() ) )
{
}

/////////////////////////////////////////////////////////////////////////////
// GradientLinear
GradientLinear::GradientLinear( double x0, double y0, double x1, double y1 )
	: Gradient( cairo_pattern_create_linear( x0, y0, x1, y1 ) )
{
}

GradientLinear::GradientLinear( vec2 point0, vec2 point1 )
	: Gradient( cairo_pattern_create_linear( point0.x, point0.y, point1.x, point1.y ) )
{
}

/////////////////////////////////////////////////////////////////////////////
// GradientRadial
GradientRadial::GradientRadial( double x0, double y0, double radius0, double x1, double y1, double radius1 )
	: Gradient( cairo_pattern_create_radial( x0, y0, radius0, x1, y1, radius1 ) )
{
}

GradientRadial::GradientRadial( const vec2 &center0, float radius0, const vec2 &center1, float radius1 )
	: Gradient( cairo_pattern_create_radial( center0.x, center0.y, radius0, center1.x, center1.y, radius1 ) )
{
}

/////////////////////////////////////////////////////////////////////////////
// FontOptions
FontOptions::FontOptions( cairo_font_options_t *aCairoFontOptions ) : mCairoFontOptions( aCairoFontOptions )
{
}

FontOptions::~FontOptions() 
{
	if( mCairoFontOptions ) {
		cairo_font_options_destroy( mCairoFontOptions );
	}
}

FontOptions* FontOptions::create()
{
	return new FontOptions( cairo_font_options_create() );
}

int32_t FontOptions::status()
{
	return static_cast<int32_t>( cairo_font_options_status( mCairoFontOptions ) );
}

void FontOptions::merge( const FontOptions *other )
{
	cairo_font_options_merge( mCairoFontOptions, const_cast<FontOptions *>( other )->getCairoFontOptions() );
}

unsigned long FontOptions::hash()
{
	return cairo_font_options_hash( mCairoFontOptions );
}

bool FontOptions::equal( const FontOptions *other )
{
	return static_cast<bool>( cairo_font_options_equal( mCairoFontOptions, const_cast<FontOptions *>( other )->getCairoFontOptions() ) != 0 );
}

void FontOptions::setAntiAlias( int32_t antialias )
{
	cairo_font_options_set_antialias( mCairoFontOptions, static_cast<cairo_antialias_t>( antialias ) );
}

int32_t FontOptions::getAntiAlias()
{
	return static_cast<int32_t>( cairo_font_options_get_antialias( mCairoFontOptions ) );
}

void FontOptions::setSubPixelOrder( int32_t subpixel_order )
{
	cairo_font_options_set_subpixel_order( mCairoFontOptions, static_cast<cairo_subpixel_order_t>( subpixel_order ) );
}

int32_t FontOptions::getSubPixelOrder()
{
	return static_cast<int32_t>( cairo_font_options_get_subpixel_order( mCairoFontOptions ) );
}

void FontOptions::setHintStyle( int32_t hint_style )
{
	cairo_font_options_set_hint_style( mCairoFontOptions, static_cast<cairo_hint_style_t>( hint_style ) );
}

int32_t FontOptions::getHintStyle()
{
	return static_cast<int32_t>( cairo_font_options_get_hint_style( mCairoFontOptions ) );
}

void FontOptions::setHintMetrics( int32_t hint_metrics )
{
	cairo_font_options_set_hint_metrics( mCairoFontOptions, static_cast<cairo_hint_metrics_t>( hint_metrics ) );
}

int32_t FontOptions::getHintMetrics()
{
	return static_cast<int32_t>( cairo_font_options_get_hint_metrics( mCairoFontOptions ) );
}

/////////////////////////////////////////////////////////////////////////////
// FontFace
FontFace::FontFace( cairo_font_face_t *aCairoFontFace ) : mCairoFontFace( aCairoFontFace ) 
{
}

FontFace::FontFace( const std::string &faceName )
{
#if defined( CAIRO_HAS_QUARTZ_FONT )
	mCairoFontFace = NULL; // in case we throw below
	CFStringRef nameRef = ::CFStringCreateWithCString( kCFAllocatorDefault, faceName.c_str(), kCFStringEncodingUTF8 );
	CGFontRef fontRef = ::CGFontCreateWithFontName( nameRef );
	if( ! fontRef )
		throw FontInvalidNameExc();
	mCairoFontFace = cairo_quartz_font_face_create_for_cgfont( fontRef );
	// Cairo releases the CGFontRef
	CFRelease( nameRef );
#elif defined( CINDER_MSW )
	HFONT hf;
    HDC hdc;
    long lfHeight;
    
    hdc = ::GetDC( NULL );
    lfHeight = -MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    ReleaseDC( NULL, hdc );

    hf = CreateFontA( lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, faceName.c_str() );
	mCairoFontFace = cairo_win32_font_face_create_for_hfont( hf );
#endif
}

FontFace::~FontFace()
{
	if( mCairoFontFace ) {
		cairo_font_face_destroy( mCairoFontFace );
	}
}

int32_t FontFace::getType() 
{
	return static_cast<int32_t>( cairo_font_face_get_type( mCairoFontFace ) );
}

/////////////////////////////////////////////////////////////////////////////
// ScaledFont
ScaledFont::ScaledFont( cairo_scaled_font_t *aCairoScaledFont ) : mCairoScaledFont( aCairoScaledFont )
{
}

ScaledFont::~ScaledFont()
{
	if( mCairoScaledFont ) {
		cairo_scaled_font_destroy( mCairoScaledFont );
	}
}

int32_t ScaledFont::status()
{
	return static_cast<int32_t>( cairo_scaled_font_status( mCairoScaledFont ) );
}

/*FontExtents ScaledFont::fontExtents( FontExtents *extents )
{
	FontExtents result;
	cairo_scaled_font_extents( mCairoScaledFont, result->getCairoFontExtents() );
}

void ScaledFont::textExtents( const char *utf8, TextExtents *extents )
{
	cairo_scaled_font_text_extents( mCairoScaledFont, utf8, extents->getCairoTextExtents() );
}

void ScaledFont::glyphExents( const Glyph *glyphs, int32_t num_glyphs )
{
	cairo_scaled_font_glyph_extents( mCairoScaledFont, const_cast<Glyph *>( glyphs )->getCairoGlyph(), num_glyphs );
}
*/

FontFace* ScaledFont::getFontFace()
{
	return new FontFace( cairo_scaled_font_get_font_face( mCairoScaledFont ) );
}

void ScaledFont::getFontOptions( FontOptions *options )
{
	cairo_scaled_font_get_font_options( mCairoScaledFont, options->getCairoFontOptions() );
}

void ScaledFont::getFontMatrix( Matrix *font_matrix )
{
	cairo_scaled_font_get_font_matrix( mCairoScaledFont, &font_matrix->getCairoMatrix() );	
}

void ScaledFont::getCtm( Matrix *ctm )
{
	cairo_scaled_font_get_ctm( mCairoScaledFont, &ctm->getCairoMatrix() );
}

int32_t ScaledFont::getType()
{
	return static_cast<int32_t>( cairo_scaled_font_get_type( mCairoScaledFont ) );
}

/////////////////////////////////////////////////////////////////////////////
// Glyph
/*Glyph::Glyph() 
{
	mCairoGlyph = std::shared_ptr<cairo_glyph_t>( new cairo_glyph_t );
}

Glyph::Glyph( cairo_glyph_t *aGlyph ) 
{
	mCairoGlyph = std::shared_ptr<cairo_glyph_t>( new cairo_glyph_t );
	*mCairoGlyph.get() = *aGlyph;
}

Glyph::~Glyph()
{}

const unsigned long& Glyph::index() const
{
	return mCairoGlyph->index;
}

unsigned long& Glyph::index()
{
	return mCairoGlyph->index;
}

const double& Glyph::x() const
{
	return mCairoGlyph->x;
}

double&	Glyph::x()
{
	return mCairoGlyph->x;
}

const double& Glyph::y() const
{
	return mCairoGlyph->y;
}

double& Glyph::y()
{
	return mCairoGlyph->y;
}


/////////////////////////////////////////////////////////////////////////////
// GlyphArray
GlyphArray::GlyphArray() 
{}


Glyph GlyphArray::getAtIndex( size_t idx )
{
	return Glyph( &mGlyphs[idx] );
}


void GlyphArray::append( Glyph &aGlyph )
{
	mGlyphs.push_back( *aGlyph.getCairoGlyph() );
}

void GlyphArray::clear() 
{
	mGlyphs.clear();
}
*/
/////////////////////////////////////////////////////////////////////////////
// FontExtents
FontExtents::FontExtents()
{
	mCairoFontExtents = std::shared_ptr<cairo_font_extents_t>( new cairo_font_extents_t );
}

FontExtents::FontExtents( cairo_font_extents_t *aCairoFontExtents )
{
	mCairoFontExtents = std::shared_ptr<cairo_font_extents_t>( new cairo_font_extents_t );
	*mCairoFontExtents.get() = *aCairoFontExtents;
}

FontExtents::~FontExtents()
{}
	
const double& FontExtents::ascent() const
{
	return mCairoFontExtents->ascent;
}

double& FontExtents::ascent()
{
	return mCairoFontExtents->ascent;
}

const double& FontExtents::descent() const
{
	return mCairoFontExtents->descent;
}

double& FontExtents::descent()
{
	return mCairoFontExtents->descent;
}

const double& FontExtents::height() const
{
	return mCairoFontExtents->height;
}

double& FontExtents::height()
{
	return mCairoFontExtents->height;
}

const double& FontExtents::maxXAdvance() const
{
	return mCairoFontExtents->max_x_advance;
}

double& FontExtents::maxXAdvance()
{
	return mCairoFontExtents->max_x_advance;
}

const double& FontExtents::maxYAdvance() const
{
	return mCairoFontExtents->max_y_advance;
}

double& FontExtents::maxYAdvance()
{
	return mCairoFontExtents->max_y_advance;
}

/////////////////////////////////////////////////////////////////////////////
// TextExtents
TextExtents::TextExtents() 
{
	mCairoTextExtents = std::shared_ptr<cairo_text_extents_t>( new cairo_text_extents_t );
}

TextExtents::TextExtents( cairo_text_extents_t *aCairoTextExtents ) 
{
	mCairoTextExtents = std::shared_ptr<cairo_text_extents_t>( new cairo_text_extents_t );
	*mCairoTextExtents.get() = *aCairoTextExtents;
}

TextExtents::~TextExtents()
{}

const double& TextExtents::xBearing() const 
{
	return mCairoTextExtents->x_bearing;
}

double& TextExtents::xBearing() 
{
	return mCairoTextExtents->x_bearing;
}

const double& TextExtents::yBearing() const 
{
	return mCairoTextExtents->y_bearing;
}

double& TextExtents::yBearing() 
{
	return mCairoTextExtents->y_bearing;
}

const double& TextExtents::width() const 
{
	return mCairoTextExtents->width;
}

double& TextExtents::width() 
{
	return mCairoTextExtents->width;
}

const double& TextExtents::height() const 
{
	return mCairoTextExtents->height;
}

double& TextExtents::height() 
{
	return mCairoTextExtents->height;
}

const double& TextExtents::xAdvance() const
{
	return mCairoTextExtents->x_advance;
}

double& TextExtents::xAdvance() 
{
	return mCairoTextExtents->x_advance;
}

const double& TextExtents::yAdvance() const
{
	return mCairoTextExtents->y_advance;
}	

double& TextExtents::yAdvance() 
{
	return mCairoTextExtents->y_advance;
}	


/////////////////////////////////////////////////////////////////////////////
// Context
Context::Context( const SurfaceBase &surface )
{
	mCairoSurface = surface.getCairoSurface();
	mCairo = cairo_create( mCairoSurface );
}

Context::Context( const Context &other )
{
	mCairo = other.mCairo;
	if( mCairo )
		cairo_reference( mCairo );
	mCairoSurface = other.mCairoSurface;
}

Context::~Context()
{
	if( mCairoSurface )
		cairo_surface_flush( mCairoSurface );
	if( mCairo )
		cairo_destroy( mCairo );
}

Context& Context::operator=( const Context &other )
{
	cairo_t *oldCairo = mCairo;
	mCairo = other.mCairo;
	if( mCairo )
		cairo_reference( mCairo );
	mCairoSurface = other.mCairoSurface;
		
	if( oldCairo )
		cairo_destroy( oldCairo );
		
	return *this;
}

cairo_t* Context::getCairo()
{
	return mCairo;
}

cairo_surface_t* Context::getCairoSuface()
{
	return mCairoSurface;
}

void Context::save()
{
	cairo_save( mCairo );
}

void Context::restore()
{
	cairo_restore( mCairo );
}

void Context::flush()
{
	cairo_surface_flush( mCairoSurface );
}
	
void Context::pushGroup( )
{
	cairo_push_group( mCairo );
}

void Context::pushGroupWithContent( int32_t content )
{
	cairo_push_group_with_content( mCairo, static_cast<cairo_content_t>( content ) );
}

Pattern* Context::popGroup()
{
	return new Pattern( cairo_pop_group( mCairo ) );
}

void Context::popGroupToSource()
{
	cairo_pop_group_to_source( mCairo );
}

void Context::setSourceRgb( double red, double green, double blue )
{
	cairo_set_source_rgb( mCairo, red, green, blue );
}

void Context::setSourceRgba( double red, double green, double blue, double alpha )
{
	cairo_set_source_rgba( mCairo, red, green, blue, alpha );
}

void Context::setSource( const Pattern &source )
{
	cairo_set_source( mCairo, const_cast<Pattern&>( source ).getCairoPattern() );
}
	
void Context::setSourceSurface( SurfaceBase &surface, double x, double y )
{
	cairo_set_source_surface( mCairo, surface.getCairoSurface(), x, y );
}

void Context::copySurface( const SurfaceBase &surface, const Area &srcArea, const ivec2 &dstOffset )
{
	cairo_set_source_surface( mCairo, const_cast<SurfaceBase&>( surface ).getCairoSurface(), dstOffset.x - srcArea.getX1(), dstOffset.y - srcArea.getY1() );
	cairo_rectangle( mCairo, dstOffset.x, dstOffset.y, srcArea.getWidth(), srcArea.getHeight() );
	cairo_fill( mCairo );
}

void Context::copySurface( const SurfaceBase &surface, const Area &srcArea, const Rectf &dstRect )
{
	if( ( dstRect.getWidth() == 0 ) || ( dstRect.getHeight() == 0 ) )
		return;

	save();
	cairo_set_source_surface( mCairo, const_cast<SurfaceBase&>( surface ).getCairoSurface(), 0, 0 );
	cairo_pattern_t *sourcePattern = cairo_get_source( mCairo );
	cairo_matrix_t m;
	cairo_matrix_init_identity( &m );
	cairo_matrix_scale( &m, srcArea.getWidth() / (float)dstRect.getWidth(), srcArea.getHeight() / (float)dstRect.getHeight() );
	cairo_matrix_translate( &m, srcArea.getX1() - dstRect.getX1(), srcArea.getY1() - dstRect.getY1() );
	cairo_pattern_set_matrix( sourcePattern, &m );
	cairo_rectangle( mCairo, dstRect.getX1(), dstRect.getY1(), dstRect.getWidth(), dstRect.getHeight() );
	cairo_fill( mCairo );
	restore();
}

Pattern* Context::getSource()
{
	return new Pattern( cairo_get_source( mCairo ) );
}

void Context::setAntiAlias( int32_t antialias )
{
	cairo_set_antialias( mCairo, static_cast<cairo_antialias_t>( antialias ) );
}

int32_t Context::getAntiAlias()
{
	return static_cast<int32_t>( cairo_get_antialias( mCairo ) );
}

void Context::setDash( const double *dashes, int numDashes, double offset )
{
	cairo_set_dash( mCairo, dashes, numDashes, offset );
}

void Context::setDash( const vector<double> &dashes, double offset )
{
	cairo_set_dash( mCairo, &dashes[0], (int)dashes.size(), offset );
}

void Context::unsetDash()
{
	cairo_set_dash( mCairo, 0, 0, 0 );
}

int	 Context::getDashCount() const
{
	return cairo_get_dash_count( mCairo );
}

void Context::getDash( double *dashes, double *offset )
{
	return cairo_get_dash( mCairo, dashes, offset );
}

void Context::setFillRule( int32_t fill_rule )
{
	cairo_set_fill_rule( mCairo, static_cast<cairo_fill_rule_t>( fill_rule ) );
}

int32_t Context::getFillRule()
{
	return static_cast<int32_t>( cairo_get_fill_rule( mCairo ) );
}

void Context::setLineCap( int32_t line_cap )
{
	cairo_set_line_cap( mCairo, static_cast<cairo_line_cap_t>( line_cap ) );
}

int32_t Context::getLineCap()
{
	return static_cast<int32_t>( cairo_get_line_cap( mCairo ) );
}

void Context::setLineJoin( int32_t line_join )
{
	cairo_set_line_join( mCairo, static_cast<cairo_line_join_t>( line_join ) );
}

int32_t Context::getLineJoin()
{
	return static_cast<int32_t>( cairo_get_line_join( mCairo ) );
}

void Context::setLineWidth( double width )
{
	cairo_set_line_width( mCairo, width );
}

double Context::getLineWidth() const
{
	return cairo_get_line_width( mCairo );
}

void Context::setMiterLimit( double limit )
{
	cairo_set_miter_limit( mCairo, limit );
}

double Context::getMiterLimit() const
{
	return cairo_get_miter_limit( mCairo );
}

void Context::setOperator( int32_t op )
{
	cairo_set_operator( mCairo, static_cast<cairo_operator_t>( op ) );
}

int32_t Context::getOperator()
{
	return static_cast<int32_t>( cairo_get_operator( mCairo ) );
}
	
void Context::setTolerance( double tolerance )
{
	cairo_set_tolerance( mCairo, tolerance );
}

double Context::getTolerance() const
{
	return cairo_get_tolerance( mCairo );
}

void Context::clip()
{
	cairo_clip( mCairo );
}

void Context::clipPerserve()
{
	cairo_clip_preserve( mCairo );
}

void Context::clipExtents( double *x1, double *y1, double *x2, double *y2 )
{
	cairo_clip_extents( mCairo, x1, y1, x2, y2 );
}

void Context::resetClip()
{
	cairo_reset_clip( mCairo );
}

void Context::fill()
{
	cairo_fill( mCairo );
}

void Context::fillPreserve()
{
	cairo_fill_preserve( mCairo );
}

void Context::fillExtents( double *x1, double *y1, double *x2, double *y2 )
{
	cairo_fill_extents( mCairo, x1, y1, x2, y2 );
}

bool Context::inFill( double x, double y )
{
	return static_cast<bool>( cairo_in_fill( mCairo, x, y ) != 0 );
}

void Context::mask( Pattern *pattern )
{
	cairo_mask( mCairo, pattern->getCairoPattern() );
}

void Context::maskSurface( SurfaceBase *surface, double surface_x, double surface_y )
{
	cairo_mask_surface( mCairo, surface->getCairoSurface(), surface_x, surface_y );
}

void Context::paint()
{
	cairo_paint( mCairo );
}

void Context::paintWithAlpha( double alpha )
{
	cairo_paint_with_alpha( mCairo, alpha );
}

void Context::stroke()
{
	cairo_stroke( mCairo );
}

void Context::strokePreserve()
{
	cairo_stroke_preserve( mCairo );
}

void Context::strokeExtents( double *x1, double *y1, double *x2, double *y2 )
{
	cairo_stroke_extents( mCairo, x1, y1, x2, y2 );
}

bool Context::inStroke( double x, double y )
{
	return static_cast<bool>( cairo_in_stroke( mCairo, x, y ) != 0 );
}

void Context::copyPage()
{
	cairo_copy_page( mCairo );
}

void Context::showPage()
{
	cairo_show_page( mCairo );
}

// These methods are from the Path class that have been added here for convenience 

void convertCairoToCinderPath( cairo_path_t *path, cinder::Shape2d *resultPath )
{
	for( int i=0; i < path->num_data; i += path->data[i].header.length ) {
         cairo_path_data_t *data = &path->data[i];
         switch( data->header.type ) {
			case CAIRO_PATH_MOVE_TO:
				resultPath->moveTo( (float)data[1].point.x, (float)data[1].point.y );
			break;
			case CAIRO_PATH_LINE_TO:
				resultPath->lineTo( (float)data[1].point.x, (float)data[1].point.y );
			break;
			case CAIRO_PATH_CURVE_TO:
				resultPath->curveTo( (float)data[1].point.x, (float)data[1].point.y, 
					(float)data[2].point.x, (float)data[2].point.y, (float)data[3].point.x, (float)data[3].point.y );
			break;
			case CAIRO_PATH_CLOSE_PATH:
				resultPath->close();
			break;
         }
     }
}

void Context::copyPath( cinder::Shape2d *resultPath )
{
	resultPath->clear();	
	cairo_path_t *path = cairo_copy_path( mCairo );
	convertCairoToCinderPath( path, resultPath );
	cairo_path_destroy( path );
}

void Context::copyPathFlat( cinder::Shape2d *resultPath )
{
	resultPath->clear();	
	cairo_path_t *path = cairo_copy_path_flat( mCairo );
	convertCairoToCinderPath( path, resultPath );
	cairo_path_destroy( path );
}

void Context::getCurrentPoint( double *x, double *y ) const
{
	cairo_get_current_point( mCairo, x, y );
}

vec2 Context::getCurrentPoint() const
{
	double x, y;
	cairo_get_current_point( mCairo, &x, &y );
	return vec2( (float)x, (float)y );
}

void Context::newPath()
{	
	cairo_new_path( mCairo );
}

void Context::newSubPath()
{
	cairo_new_sub_path( mCairo );
}

void Context::closePath()
{
	cairo_close_path( mCairo );
}

void Context::arc( double xc, double yc, double radius, double angle1, double angle2 )
{
	cairo_arc( mCairo, xc, yc, radius, angle1, angle2 );
}

void Context::arcNegative( double xc, double yc, double radius, double angle1, double angle2 )
{
	cairo_arc_negative( mCairo, xc, yc, radius, angle1, angle2 );
}

void Context::quadTo( double x1, double y1, double x2, double y2 )
{
	double x0, y0;
	cairo_get_current_point( mCairo, &x0, &y0 );
	cairo_curve_to( mCairo, x0 + (x1 - x0) / 3.0f * 2.0f, y0 + (y1 - y0) / 3.0f * 2.0f, x1 + (x2 - x1) / 3.0f, y1 + (y2 - y1) / 3.0f, x2, y2 );
}

void Context::curveTo( double x1, double y1, double x2, double y2, double x3, double y3 )
{
	cairo_curve_to( mCairo, x1, y1, x2, y2, x3, y3 );
}

void Context::lineTo( double x, double y )
{
	cairo_line_to( mCairo, x, y );
}

void Context::moveTo( double x, double y )
{
	cairo_move_to( mCairo, x, y );
}

void Context::rectangle( double x, double y, double width, double height )
{
	cairo_rectangle( mCairo, x, y, width, height );
}

void Context::rectangle( const vec2 &upperLeft, const vec2 &lowerRight )
{ 
	float width = lowerRight.x - upperLeft.x;
	float height = lowerRight.y - upperLeft.y;
	rectangle( upperLeft.x - width * 0.5f, upperLeft.y - height * 0.5f, width, height );
}

void Context::roundedRectangle( const Rectf &r, float cornerRadius )
{
	// derived from formula due to Helton Moraes
	cairo_arc( mCairo, r.x1 + cornerRadius, r.y1 + cornerRadius, cornerRadius, 2*(M_PI/2), 3*( M_PI/2) );
	cairo_arc( mCairo, r.x2 - cornerRadius, r.y1 + cornerRadius, cornerRadius, 3*(M_PI/2), 4*(M_PI/2) );
	cairo_arc( mCairo, r.x2 - cornerRadius, r.y2 - cornerRadius, cornerRadius, 0*(M_PI/2), 1*(M_PI/2) );
	cairo_arc( mCairo, r.x1 + cornerRadius, r.y2 - cornerRadius, cornerRadius, 1*(M_PI/2), 2*(M_PI/2) );
	cairo_close_path( mCairo );
}

/*
void Context::glyphPath( const cairo_glyph_t *glyphs, int num_glyphs )
{

}
*/

void Context::textPath( const char *utf8 )
{
	cairo_text_path( mCairo, utf8 );
}

void Context::glyphPath( const std::vector<std::pair<uint16_t,vec2> > &glyphs )
{
	cairo_glyph_t *cairoGlyphs = new cairo_glyph_t[glyphs.size()];
	for( size_t g = 0; g < glyphs.size(); ++g ) {
		cairoGlyphs[g].index = glyphs[g].first;
		cairoGlyphs[g].x = glyphs[g].second.x;
		cairoGlyphs[g].y = glyphs[g].second.y;
	}
	cairo_glyph_path( mCairo, &cairoGlyphs[0], glyphs.size() );
	delete [] cairoGlyphs;
}

void Context::glyphPath( uint16_t index, const vec2 &offset )
{
	cairo_glyph_t glyph;
	glyph.index = index;
	glyph.x = offset.x;
	glyph.y = offset.y;
	cairo_glyph_path( mCairo, &glyph, 1 );
}

void Context::relCurveTo( double dx1, double dy1, double dx2, double dy2, double dx3, double dy3 )
{
	cairo_rel_curve_to( mCairo, dx1, dy1, dx2, dy2, dx3, dy3 );
}

void Context::relLineTo( double dx, double dy )
{
	cairo_rel_line_to( mCairo, dx, dy );
}

void Context::relMoveTo( double dx, double dy )
{
	cairo_rel_move_to( mCairo, dx, dy );
}

void Context::appendPath( const cinder::Shape2d &path )
{
	for( size_t contCount = 0; contCount < path.getNumContours(); ++contCount ) {
		const Path2d &contour( path.getContour( contCount ) );
		appendPath( contour );
	}
}

void Context::appendPath( const cinder::Path2d &path )
{
	size_t point = 0;
	if( path.empty() )
		return;
	moveTo( path.getPoint( point++ ) );
	for( size_t seg = 0; seg < path.getNumSegments(); ++seg ) {
		switch( path.getSegmentType( seg ) ) {
			case Path2d::MOVETO:
			break;
			case Path2d::LINETO:
				lineTo( path.getPoint( point++ ) );
			break;
			case Path2d::QUADTO: {
				const vec2 &spl0( path.getPoint( point - 1 ) ); const vec2 &spl1( path.getPoint( point + 0 ) ); const vec2 &spl2( path.getPoint( point + 1 ) );
				curveTo( spl0 + (spl1 - spl0) / 3.0f * 2.0f, spl1 + (spl2 - spl1) / 3.0f, spl2 );
				point += 2;
			}
			break;
			case Path2d::CUBICTO:
				curveTo( path.getPoint( point ), path.getPoint( point + 1 ), path.getPoint( point + 2 ) );
				point += 3;
			break;
			case Path2d::CLOSE:
				closePath();
			break;
		}
	}
}

void Context::circle( double dx, double dy, double radius )
{
	newSubPath();
	arc( dx, dy, radius, 0, 2 * 3.14159265 );
}

// Transformation functions
void Context::translate( double tx, double ty )
{
	cairo_translate( mCairo, tx, ty );
}

void Context::scale( double sx, double sy )
{
	cairo_scale( mCairo, sx, sy );
}

void Context::rotate( double radians )
{
	cairo_rotate( mCairo, radians );
}

void Context::transform( const Matrix &aMatrix )
{
	cairo_transform( mCairo, &aMatrix.getCairoMatrix() );
}

void Context::transform( const mat3 &matrix )
{
	cairo_transform( mCairo, &cairo::Matrix( matrix ).getCairoMatrix() );
}

void Context::setMatrix( const Matrix &aMatrix )
{
	cairo_set_matrix( mCairo, &aMatrix.getCairoMatrix() );
}

void Context::setMatrix( const mat3 &matrix )
{
	cairo_set_matrix( mCairo, &cairo::Matrix( matrix ).getCairoMatrix() );
}

void Context::getMatrix( Matrix *aMatrix )
{
	cairo_get_matrix( mCairo, &aMatrix->getCairoMatrix() );
}

mat3 Context::getMatrix() const
{
	cairo_matrix_t temp;
	cairo_get_matrix( mCairo, &temp );

	return mat3( temp.xx, temp.yx, temp.x0, temp.xy, temp.yy, temp.y0, 0, 0, 1 );

//	return mat3( (float)temp.xx, (float)temp.yx, (float)temp.xy, (float)temp.yy, (float)temp.x0, (float)temp.y0 );
}

void Context::identityMatrix()
{
	cairo_identity_matrix( mCairo );
}

void Context::userToDevice( double *x, double *y )
{
	cairo_user_to_device( mCairo, x, y );
}

void Context::userToDeviceDistance( double *dx, double *dy )
{
	cairo_user_to_device_distance( mCairo, dx, dy );
}

void Context::deviceToUser( double *x, double *y )
{
	cairo_device_to_user( mCairo, x, y );
}

void Context::deviceToUserDistance( double *dx, double *dy )
{
	cairo_device_to_user_distance( mCairo, dx, dy );
}

// Text/font functions
void Context::selectFontFace( const std::string &family, int32_t slant, int32_t weight )
{
	cairo_select_font_face( mCairo, family.c_str(), static_cast<cairo_font_slant_t>( slant ), static_cast<cairo_font_weight_t>( weight ) );
}

void Context::setFontSize( double size )
{
	cairo_set_font_size( mCairo, size );
}

void Context::setFontMatrix( const Matrix &matrix )
{
	cairo_set_font_matrix( mCairo, &matrix.getCairoMatrix() );
}

void Context::getFontMatrix( Matrix *matrix )
{
	cairo_get_font_matrix( mCairo, &matrix->getCairoMatrix() );
}

void Context::setFontOptions( const FontOptions *options )
{
	cairo_set_font_options( mCairo, const_cast<FontOptions*>( options )->getCairoFontOptions() );
}

void Context::getFontOptions( FontOptions *options )
{
	cairo_get_font_options( mCairo, options->getCairoFontOptions() );
}

void Context::setFont( const cinder::Font &font )
{
#if defined( CINDER_COCOA )
	cairo_font_face_t *cairoFont = cairo_quartz_font_face_create_for_cgfont( font.getCgFontRef() );
#elif defined( CINDER_MSW )
	cairo_font_face_t *cairoFont = cairo_win32_font_face_create_for_logfontw( const_cast<LOGFONTW*>( (const LOGFONTW*)font.getLogfont() ) );
#endif
	cairo_set_font_face( mCairo, cairoFont );
	cairo_set_font_size( mCairo, font.getSize() );
	cairo_font_face_destroy( cairoFont );
}

void Context::setFontFace( const FontFace &font_face )
{
	cairo_set_font_face( mCairo, font_face.getCairoFontFace() );
}

FontFace* Context::getFontFace()
{
	return new FontFace( cairo_get_font_face( mCairo ) );
}

void Context::setScaledFont( const ScaledFont *scaled_font )
{
	cairo_set_scaled_font( mCairo, const_cast<ScaledFont *>( scaled_font )->getCairoScaledFont() );
}

ScaledFont*	Context::getScaledFont()
{
	return new ScaledFont( cairo_get_scaled_font( mCairo ) );
}

void Context::showText( const std::string &s )
{
	cairo_show_text( mCairo, s.c_str() );
}

void Context::textPath( const std::string &s )
{
	cairo_text_path( mCairo, s.c_str() );
}

/*void Context::showGlyphs( const GlyphArray &glyphs )
{
	cairo_show_glyphs( mCairo, (cairo_glyph_t *)&glyphs.mGlyphs[0], (int)glyphs.mGlyphs.size() );
}*/

FontExtents Context::fontExtents()
{
	FontExtents result;
	cairo_font_extents( mCairo, result.getCairoFontExtents() );
	return result;
}

TextExtents Context::textExtents( const std::string &s )
{
	TextExtents result;
	cairo_text_extents( mCairo, s.c_str(), result.getCairoTextExtents() );
	return result;
}

#if defined( CINDER_MSW )
cairo::SurfaceGdi createWindowSurface()
{
	return cairo::SurfaceGdi( cinder::app::App::get()->getRenderer()->getDc() );
}
#elif defined( CINDER_MAC )
cairo::SurfaceQuartz createWindowSurface()
{
	auto cgContext = cinder::app::App::get()->getRenderer()->getCgContext();
	auto height = cinder::app::getWindowHeight();
	CGContextTranslateCTM( cgContext, 0.0, height );
	CGContextScaleCTM( cgContext, 1.0, -1.0 );
	return cairo::SurfaceQuartz( cgContext, cinder::app::getWindowWidth(), height );
}
#endif

/*void Context::glyphExtents( const GlyphArray &glyphs, int num_glyphs, TextExtents *extents )
{
	cairo_glyph_extents( mCairo, (cairo_glyph_t *)&glyphs.mGlyphs[0], (int)glyphs.mGlyphs.size(), extents->getCairoTextExtents() );
}
*/

std::string	Context::statusToString() const
{
	return std::string( cairo_status_to_string( cairo_status( mCairo ) ) );
}

bool isSingular( const mat3& m )
{
	return fabs( m[0][0] * m[1][1] - m[1][0] * m[0][1] ) <= 0.000001f;
}

/////////////////////////////////////////////////////////////////////////////
// SvgRendererCairo
class SvgRendererCairo : public svg::Renderer {
  public:
	SvgRendererCairo( cairo::Context &ctx ) : svg::Renderer(), mCtx( ctx ) { 
		mMatrixStack.push_back( mCtx.getMatrix() );
		// An illegal (non-singular) matrix is irrecoverable in Cairo,
		// but something we need to handle. This variable records it,
		// see W3C Test coords-trans-09-t.svg
		mMatrixStackContainsIllegal = false;

		mFillStack.push_back( svg::Paint( Color::black() ) );
		mStrokeStack.push_back( svg::Paint() );
		mFillOpacityStack.push_back( 1.0f );
		mStrokeOpacityStack.push_back( 1.0f );
		mGroupOpacityStack.push_back( 1.0f );
		
		mStrokeWidthStack.push_back( 1.0f );
		mCtx.setLineWidth( mStrokeWidthStack.back() );
		mFillRuleStack.push_back( cairo::FILL_RULE_WINDING );
		mCtx.setFillRule( mFillRuleStack.back() );
		mLineCapStack.push_back( cairo::LINE_CAP_BUTT );
		mCtx.setFillRule( mLineCapStack.back() );
		mLineJoinStack.push_back( cairo::LINE_JOIN_MITER );
		mCtx.setLineJoin( mLineJoinStack.back() );
		
		pushTextPen( vec2( 0 ) );
		mTextRotationStack.push_back( 0 );		
	}

	bool	shouldRender( const svg::Node &node ) const {
		return (! mMatrixStackContainsIllegal) && (( ! mFillStack.back().isNone() ) || ( ! mStrokeStack.back().isNone() ) );
	}
	
	void	pushGroup( const svg::Group &group, float opacity ) {
		mGroupOpacityStack.push_back( opacity );
		if( opacity < 1.0f )
			mCtx.pushGroup();
	}

	void	popGroup() {
		if( mGroupOpacityStack.back() < 1.0f ) {
			mCtx.popGroupToSource();
			mCtx.paintWithAlpha( mGroupOpacityStack.back() );
		}
		mGroupOpacityStack.pop_back();
	}
	
	bool	prepareSourceFill( const svg::Node &node ) { return prepareSource( node, mFillStack, mFillOpacityStack ); }
	bool	prepareSourceStroke( const svg::Node &node ) { return prepareSource( node, mStrokeStack, mStrokeOpacityStack ); }
	bool	prepareSource( const svg::Node &node, const std::vector<svg::Paint> &paintStack, const std::vector<float> &opacityStack ) {
		if( paintStack.back().isNone() )
			return false;
		else if( paintStack.back().isLinearGradient() ) {
			cairo::GradientLinear grad( paintStack.back().getCoords0(), paintStack.back().getCoords1() );
			prepareGradientSource( node, paintStack.back(), grad, opacityStack.back() );
			return true;
		}
		else if( paintStack.back().isRadialGradient() ) {
			cairo::GradientRadial grad( paintStack.back().getCoords1(), 0, paintStack.back().getCoords0(), paintStack.back().getRadius() );
			prepareGradientSource( node, paintStack.back(), grad, opacityStack.back() );
			return true;
		}
		else { // solid color or other
			ColorA color( paintStack.back().getColor() ); color.a = opacityStack.back();
			mCtx.setSource( color );
			return true;
		}
	}
	void	prepareGradientSource( const svg::Node &node, const svg::Paint &paint, cairo::Gradient &grad, float opacity ) {
		if( paint.useObjectBoundingBox() ) {
			Rectf bb = node.getBoundingBox();
			cairo::Matrix m; m.initIdentity();
			float invWidth = ( bb.getWidth() != 0 ) ? ( 1 / bb.getWidth() ) : 0;
			float invHeight = ( bb.getHeight() != 0 ) ? ( 1 / bb.getHeight() ) : 0;
			m.scale( invWidth, invHeight );
			m.translate( -bb.x1, -bb.y1 );
			if( paint.specifiesTransform() ) {
				m *= cairo::Matrix( paint.getTransform() );
			}
			grad.setMatrix( m );
		}
		else if( paint.specifiesTransform() ) {
			cairo::Matrix m2( paint.getTransform() );
			m2.invert();
			grad.setMatrix( m2 );
		}
		for( size_t s = 0; s < paint.getNumColors(); ++s ) {
			ColorA color = paint.getColor( s );
			color.a *= opacity;
			grad.addColorStop( paint.getOffset( s ), color );
		}
		mCtx.setSource( grad );		
	}

	void	strokeAndFill( const svg::Node &node ) {
		if( prepareSourceFill( node ) ) {
			if( ! mStrokeStack.back().isNone() )
				mCtx.fillPreserve();
			else
				mCtx.fill();
		}
		if( prepareSourceStroke( node ) ) {
			mCtx.stroke();
		}	
	}

	void	drawPath( const svg::Path &path ) {
		if( ! shouldRender( path ) )
			return;
		mCtx.appendPath( path.getShape2d() );
		strokeAndFill( path );
	}

	void	drawPolyline( const svg::Polyline &polyline ) {
		if( ! shouldRender( polyline ) )
			return;
		const ci::PolyLine2f &polyLine = polyline.getPolyLine();
		if( ! polyLine.getPoints().empty() ) {
			mCtx.moveTo( polyLine.getPoints()[0] );
			for( size_t p = 1; p < polyLine.getPoints().size(); ++p )
				mCtx.lineTo( polyLine.getPoints()[p] );
			if( polyLine.isClosed() )
				mCtx.closePath();
			strokeAndFill( polyline );
		}			
	}

	void	drawPolygon( const svg::Polygon &polygon ) {
		if( ! shouldRender( polygon ) )
			return;
		const ci::PolyLine2f &polyLine = polygon.getPolyLine();
		if( ! polyLine.getPoints().empty() ) {
			mCtx.moveTo( polyLine.getPoints()[0] );
			for( size_t p = 1; p < polyLine.getPoints().size(); ++p )
				mCtx.lineTo( polyLine.getPoints()[p] );
			if( polyLine.isClosed() )
				mCtx.closePath();
			strokeAndFill( polygon );
		}			
	}

	void	drawLine( const svg::Line &line ) {
		if( ! shouldRender( line ) )
			return;	
		if( prepareSourceStroke( line ) ) {
			mCtx.line( line.getPoint1(), line.getPoint2() );
			mCtx.stroke();
		}
	}

	void	drawRect( const svg::Rect &rect ) {
		if( ! shouldRender( rect ) )
			return;	
		mCtx.rectangle( rect.getRect() );
		strokeAndFill( rect );
	}

	void	drawCircle( const svg::Circle &circle ) {
		if( ! shouldRender( circle ) )
			return;
		mCtx.circle( circle.getCenter(), circle.getRadius() );
		strokeAndFill( circle );
	}

	void	drawEllipse( const svg::Ellipse &ellipse ) {
		if( ! shouldRender( ellipse ) )
			return;	
		if( ( ellipse.getRadiusX() < 0 ) || ( ellipse.getRadiusY() < 0 ) )
			return;
		mCtx.save();
		mCtx.translate( ellipse.getCenter() );
		mCtx.scale( ellipse.getRadiusX(), ellipse.getRadiusY() );
		mCtx.arc( 0, 0, 1, 0, 2 * M_PI );
		mCtx.restore();
		strokeAndFill( ellipse );
	}

	void	drawImage( const svg::Image &image ) {
		if( ! shouldRender( image ) )
			return;
		mCtx.save();
		std::shared_ptr<Surface8u> surface = image.getSurface();
		Surface8u premultCopy( surface->getWidth(), surface->getHeight(), surface->hasAlpha(), SurfaceConstraintsCairo() );
		premultCopy.copyFrom( *surface, surface->getBounds() );
		ip::premultiply( &premultCopy );
		mCtx.copySurface( cairo::SurfaceImage( premultCopy ), surface->getBounds(), image.getRect() );
		mCtx.restore();
	}

	void	drawTextSpan( const svg::TextSpan &span ) {
		if( ! shouldRender( span ) )
			return;
					
#if 0
		/*std::vector<std::pair<uint16_t,vec2> > glyphs = span.getGlyphMeasures();
		
		vec2 curPoint = mCtx.getCurrentPoint();
		for( size_t g = 0; g < glyphs.size(); ++g ) {
			mCtx.save();
			mCtx.translate( glyphs[g].second.x + curPoint.x, curPoint.y );
			mCtx.appendPath( font->getGlyphShape( glyphs[g].first ) );
			mCtx.restore();				
		}*/
		mCtx.appendPath( span.getShape() );
#else		
		std::shared_ptr<Font> font = span.getFont();
		mCtx.setFont( *font );

		mCtx.moveTo( mTextPenStack.back() );
		// we can use a text path when the rotate is empty
		if( fabs(mTextRotationStack.back()) < 0.0001f ) {
			mCtx.textPath( span.getString() );
			mTextPenStack.back() = mCtx.getCurrentPoint();
		}
		else {
			mCtx.save();
			cairo::Matrix fontMatrix, oldFontMatrix, rotationMatrix;
			mCtx.getFontMatrix( &oldFontMatrix );
			rotationMatrix.initRotate( toRadians( mTextRotationStack.back() ) );
			fontMatrix = oldFontMatrix;
			fontMatrix *= rotationMatrix;
			mCtx.setFontMatrix( fontMatrix );
			TextBox tbox = TextBox().font( *font ).text( span.getString() );
			std::vector<std::pair<uint16_t,vec2> > glyphs = tbox.measureGlyphs();
			vec2 curPoint = mCtx.getCurrentPoint();
			for( size_t g = 0; g < glyphs.size(); ++g ) {
				mCtx.save();
				mCtx.translate( glyphs[g].second.x + curPoint.x, /*glyphs[g].second.y + */curPoint.y );
				glyphs[g].second.x = 0;
				glyphs[g].second.y = 0;
				mCtx.glyphPath( glyphs[g].first, vec2( 0 ) );
				mCtx.restore();				
			}
			mTextPenStack.back() = mCtx.getCurrentPoint();
			mCtx.restore();
		}
#endif
		strokeAndFill( span );
	}

	void	pushMatrix( const mat3 &top ) {
		mat3 m = mMatrixStack.back() * top;
		mMatrixStack.push_back( m );
		// verify the matrix is non-singular
		if( ! isSingular( m ) ) {
			mCtx.setMatrix( m );
			//mCtx.transform( mMatrixStack.back() );
		}
		else {
			mMatrixStackContainsIllegal = true;
		}
	}
	void	popMatrix() {
		mMatrixStack.pop_back();
		mMatrixStackContainsIllegal = false;
		for( size_t s = 0; s < mMatrixStack.size(); ++s ) {
			const mat3 &m = mMatrixStack[s];
			if( isSingular( m ) ) {
				mMatrixStackContainsIllegal = true;
				break;
			}
		}
		
		if( ! mMatrixStackContainsIllegal ) {
			mCtx.setMatrix( mMatrixStack.back() );
		}
	}
	
	void	pushStyle( const svg::Style &style ) {}	
	void	popStyle() {}
	
	void	pushStroke( const svg::Paint &paint ) { mStrokeStack.push_back( paint ); }
	void	popStroke() { mStrokeStack.pop_back(); }
	void	pushFill( const svg::Paint &paint ) { mFillStack.push_back( paint ); }
	void	popFill() { mFillStack.pop_back(); }
	void	pushFillOpacity( float opacity ) { mFillOpacityStack.push_back( opacity ); }
	void	popFillOpacity() { mFillOpacityStack.pop_back(); }
	void	pushStrokeOpacity( float opacity ) { mStrokeOpacityStack.push_back( opacity ); }
	void	popStrokeOpacity() { mStrokeOpacityStack.pop_back(); }

	void	pushStrokeWidth( float width ) { mStrokeWidthStack.push_back( width ); mCtx.setLineWidth( width ); }
	void	popStrokeWidth() { mStrokeWidthStack.pop_back(); mCtx.setLineWidth( mStrokeWidthStack.back() ); }
	void	pushFillRule( svg::FillRule rule ) {
			mFillRuleStack.push_back( rule == svg::FILL_RULE_EVENODD ? cairo::FILL_RULE_EVEN_ODD : cairo::FILL_RULE_WINDING );
			mCtx.setFillRule( mFillRuleStack.back() );
	}
	void	popFillRule() { mFillRuleStack.pop_back(); mCtx.setFillRule( mFillRuleStack.back() ); }	
	void	pushLineCap( svg::LineCap lineCap ) {
			if( lineCap == cairo::LINE_CAP_BUTT ) mLineCapStack.push_back( cairo::LINE_CAP_BUTT );
			else if( lineCap == cairo::LINE_CAP_ROUND ) mLineCapStack.push_back( cairo::LINE_CAP_ROUND );
			else if( lineCap == cairo::LINE_CAP_SQUARE ) mLineCapStack.push_back( cairo::LINE_CAP_SQUARE );
			mCtx.setLineCap( mLineCapStack.back() );
	}
	void	popLineCap() { mLineCapStack.pop_back(); mCtx.setLineCap( mLineCapStack.back() ); }	
	void	pushLineJoin( svg::LineJoin lineJoin ) {
			if( lineJoin == cairo::LINE_JOIN_MITER ) mLineJoinStack.push_back( cairo::LINE_JOIN_MITER );
			else if( lineJoin == cairo::LINE_JOIN_ROUND ) mLineJoinStack.push_back( cairo::LINE_JOIN_ROUND );
			else if( lineJoin == cairo::LINE_JOIN_BEVEL ) mLineJoinStack.push_back( cairo::LINE_JOIN_BEVEL );
			mCtx.setLineJoin( mLineJoinStack.back() );
	}
	void	popLineJoin() { mLineJoinStack.pop_back(); mCtx.setLineJoin( mLineJoinStack.back() ); }		

	void	pushTextPen( const vec2 &penPos ) { mTextPenStack.push_back( penPos ); mCtx.moveTo( penPos ); }
	void	popTextPen() { mTextPenStack.pop_back(); mCtx.moveTo( mTextPenStack.back() ); }
	void	pushTextRotation( float rotation ) { mTextRotationStack.push_back( rotation ); }
	void	popTextRotation() { mTextRotationStack.pop_back(); }

	std::vector<mat3>			mMatrixStack;
	bool						mMatrixStackContainsIllegal;
	std::vector<svg::Paint>		mFillStack, mStrokeStack;
	std::vector<float>			mFillOpacityStack, mStrokeOpacityStack;
	std::vector<float>			mGroupOpacityStack;
	std::vector<float>			mStrokeWidthStack;
	std::vector<int32_t>		mFillRuleStack;
	std::vector<int32_t>		mLineCapStack;
	std::vector<int32_t>		mLineJoinStack;

	std::vector<vec2>			mTextPenStack;
	std::vector<float>			mTextRotationStack;

	cairo::Context				&mCtx;
	bool						mRenderingDisabled;
	float						mGroupOpacity;
};

void Context::render( const svg::Node &node, const std::function<bool(const svg::Node&, svg::Style *)> &visitor )
{
	SvgRendererCairo ren( *this );
	if( visitor )
		ren.setVisitor( visitor );
	node.render( ren );
}

} } // namespace cinder::cairo


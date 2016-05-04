/*
 Copyright (c) 2010, The Barbarian Group
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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Surface.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"
#include "cinder/BSpline.h"
#include "cinder/Shape2d.h"
#include "cinder/Font.h"
#include "cinder/ImageIo.h"
#include "cinder/Matrix.h"
#include "cinder/Function.h"
#include "cinder/svg/Svg.h"

#if defined( CINDER_COCOA_TOUCH )
	#include <CoreGraphics/CoreGraphics.h>
#elif defined( CINDER_MAC )
	#include <ApplicationServices/ApplicationServices.h>
#endif

#include <string>
#include <vector>
#include <iomanip>

// Forward declarations used by our cairo wrappers 
struct _cairo_surface;
typedef struct _cairo_surface cairo_surface_t;

struct _cairo;
typedef struct _cairo cairo_t;

struct cairo_path; 
typedef struct cairo_path cairo_path_t;

struct _cairo_pattern;
typedef struct _cairo_pattern cairo_pattern_t;

/*
struct _cairo_rectangle;
typedef struct _cairo_rectangle cairo_rectangle_t;

struct _cairo_rectangle_list;
typedef struct _cairo_rectangle_list cairo_rectangle_list_t;
*/

struct _cairo_font_options;
typedef struct _cairo_font_options cairo_font_options_t;

struct _cairo_matrix;
typedef struct _cairo_matrix cairo_matrix_t;

struct _cairo_font_face;
typedef struct _cairo_font_face cairo_font_face_t;

struct _cairo_scaled_font;
typedef struct _cairo_scaled_font cairo_scaled_font_t;

/*struct _cairo_glyph;
typedef struct _cairo_glyph cairo_glyph_t;*/

struct _cairo_text_extents;
typedef struct _cairo_text_extents cairo_text_extents_t;

struct _cairo_font_extents;
typedef struct _cairo_font_extents cairo_font_extents_t;

namespace cinder { namespace cairo {
/////////////////////////////////////////////////////////////////////////////
// SurfaceBase
class SurfaceBase {
 public: 
	SurfaceBase() : mCairoSurface( 0 ) {}
	SurfaceBase( const SurfaceBase &other );
	SurfaceBase& operator=( const SurfaceBase &other );
	
	virtual ~SurfaceBase();
	
	cairo_surface_t*	getCairoSurface() const { return mCairoSurface; }
	
	int32_t getWidth() const { return mWidth; }
	int32_t getHeight() const { return mHeight; }
	float	getAspectRatio() const { return getWidth() / (float)getHeight(); }
	Area	getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }
	ivec2	getSize() const { return ivec2( getWidth(), getHeight() ); }
	
	void	flush();
	
 protected:
	SurfaceBase( int32_t aWidth, int32_t aHeight ) : mWidth( aWidth ), mHeight( aHeight ), mCairoSurface( 0 ) {}
	
	cairo_surface_t		*mCairoSurface; 
	int32_t				mWidth, mHeight;
}; 

/////////////////////////////////////////////////////////////////////////////
// SurfaceImage
class SurfaceImage : public SurfaceBase {
 public:
	SurfaceImage() : SurfaceBase() {}
	SurfaceImage( const SurfaceImage &other );	
	SurfaceImage( int32_t width, int32_t height, bool hasAlpha = false );
	SurfaceImage( const uint8_t *dataPtr, int32_t width, int32_t height, int32_t stride, bool hasAlpha = false );
	//! Creates a copy of \a ciSurface
	SurfaceImage( cinder::Surface ciSurface );
	SurfaceImage( ImageSourceRef imageSource );

	SurfaceImage& operator=( const SurfaceImage &other );
	
	uint8_t*		getData();
	const uint8_t*	getData() const { return getData(); }
	int32_t			getStride() const;

	cinder::Surface&		getSurface() { return mCinderSurface; }
	const cinder::Surface&	getSurface() const { return mCinderSurface; }

	//! Call this when modifying the Surface's pixels outside of Cairo
	void			markDirty();

 protected:
	void		initCinderSurface( bool alpha, cairo_surface_t *cairoSurface );
	static void surfaceDeallocator( void *data );
	
	cinder::Surface		mCinderSurface;
};

/////////////////////////////////////////////////////////////////////////////
// SurfaceSvg
class SurfaceSvg : public SurfaceBase {
 public:
	SurfaceSvg() : SurfaceBase() {}
	SurfaceSvg( const fs::path &filePath, uint32_t aWidth, uint32_t aHeight );
	SurfaceSvg( const SurfaceSvg &other );
};

/////////////////////////////////////////////////////////////////////////////
// SurfacePdf
class SurfacePdf : public SurfaceBase {
 public:
	SurfacePdf() : SurfaceBase() {}
	SurfacePdf( const fs::path &filePath, double widthInPoints, double heightInPoints );
	SurfacePdf( const SurfacePdf &other );
		
	void	setSize( double widthInPoints, double heightInPoints );
}; 

/////////////////////////////////////////////////////////////////////////////
// SurfacePs
class SurfacePs : public SurfaceBase {
 public:
	SurfacePs() : SurfaceBase() {}
	SurfacePs( const fs::path &filePath, double widthInPoints, double heightInPoints, bool enableLevel3 = true );
	SurfacePs( const SurfacePs &other );
		
	void	setSize( double widthInPoints, double heightInPoints );
	/** \brief Indicates that subsequent calls to dscComment() should direct comments to the Setup section of the PostScript output.
		This function should be called at most once per surface, and must be called before any call to dscBeginPageSetup() and before any drawing is performed to the surface. **/
	void	dscBeginSetup();
	/** \brief Indicates that subsequent calls to dscComment() should direct comments to the PageSetup section of the PostScript output.
		This function call is only needed for the first page of a surface. It should be called after any call to dscBeginSetup() and before any drawing is performed to the surface.**/;
	void	dscBeginPageSetup();
	//! Emit a comment into the PostScript output for the given surface. 
	void	dscComment( const std::string &comment ) { dscComment( comment.c_str() ); }
	//! Emit a comment into the PostScript output for the given surface. 
	void	dscComment( const char *comment );
}; 

/////////////////////////////////////////////////////////////////////////////
// SurfaceEps
class SurfaceEps : public SurfaceBase {
 public:
	SurfaceEps() : SurfaceBase() {}
	SurfaceEps( const fs::path &filePath, double widthInPoints, double heightInPoints, bool enableLevel3 = true );
	SurfaceEps( const SurfaceEps &other );
		
	void	setSize( double widthInPoints, double heightInPoints );
	/** \brief Indicates that subsequent calls to dscComment() should direct comments to the Setup section of the PostScript output.
		This function should be called at most once per surface, and must be called before any call to dscBeginPageSetup() and before any drawing is performed to the surface. **/
	void	dscBeginSetup();
	/** \brief Indicates that subsequent calls to dscComment() should direct comments to the PageSetup section of the PostScript output.
		This function call is only needed for the first page of a surface. It should be called after any call to dscBeginSetup() and before any drawing is performed to the surface.**/;
	void	dscBeginPageSetup();
	//! Emit a comment into the PostScript output for the given surface. 
	void	dscComment( const std::string &comment ) { dscComment( comment.c_str() ); }
	//! Emit a comment into the PostScript output for the given surface. 
	void	dscComment( const char *comment );
}; 

/////////////////////////////////////////////////////////////////////////////
// SurfaceQuartz
#if defined( CINDER_COCOA )
class SurfaceQuartz : public SurfaceBase {
 public:
	SurfaceQuartz() : SurfaceBase() {}
	SurfaceQuartz( CGContextRef cgContext, int32_t width, int32_t height );
	SurfaceQuartz( const SurfaceQuartz &other );
	
	CGContextRef	getCgContextRef();
	
 protected:
	CGContextRef	mCgContextRef;
}; 

/////////////////////////////////////////////////////////////////////////////
// SurfaceCgBitmapContext
class SurfaceCgBitmapContext : public SurfaceBase {
 public:
	SurfaceCgBitmapContext() : SurfaceBase() {}
	SurfaceCgBitmapContext( int32_t width, int32_t height, bool alpha );
	SurfaceCgBitmapContext( const SurfaceCgBitmapContext &other );

	cinder::Surface&			getSurface() { return mSurface; }
	const cinder::Surface&		getSurface() const { return mSurface; } 

	CGContextRef				getCgContextRef() { return mCgContextRef; }
	
 protected:
	cinder::Surface		mSurface;
	CGContextRef		mCgContextRef;
};
#endif // defined( CINDER_COCOA )

/////////////////////////////////////////////////////////////////////////////
// SurfaceGdi
#if defined( CINDER_MSW )
class SurfaceGdi : public SurfaceBase {
  public:
	SurfaceGdi() : SurfaceBase() {}
	SurfaceGdi( HDC hdc );
	SurfaceGdi( const SurfaceGdi &other );
	
	HDC		getDc() const { return mDc; }	
	
  protected:  
	HDC		mDc;
}; 
#endif // defined( CINDER_MSW )

/////////////////////////////////////////////////////////////////////////////
// Matrix
class Matrix
{
  public:
	Matrix();
	Matrix( double xx, double yx, double xy, double yy, double x0, double y0 );
	Matrix( const mat3 &m );

	// This is a sort of horrible technique, but we will replace this with the ci::Matrix32 that will exist one day
	cairo_matrix_t&			getCairoMatrix() { return *reinterpret_cast<cairo_matrix_t*>( &(this->xx) ); }
	const cairo_matrix_t&	getCairoMatrix() const { return *reinterpret_cast<const cairo_matrix_t*>( &(this->xx) ); }

	void		init( double xx, double yx, double xy, double yy, double x0, double y0 );
	void		initIdentity();
	void		initTranslate( double tx, double ty );
	void		initScale( double sx, double sy );
	void		initRotate( double radians );
	void		translate( double tx, double ty );
	void		scale( double sx, double sy );
	void		rotate( double radians );
	int32_t		invert();

	//! Transforms the point \a v by the matrix.
	vec2	transformPoint( const vec2 &v ) const;
	//! Transforms the distance vector \a v by the matrix. This is similar to cairo_matrix_transform_point() except that the translation components of the transformation are ignored
	vec2	transformDistance( const vec2 &v ) const;

	const Matrix& 	operator*=( const Matrix &rhs );

	// this is designed to mimic cairo_matrix_t exactly
    double xx; double yx;
    double xy; double yy;
    double x0; double y0;
	
	friend std::ostream& operator<<( std::ostream &lhs, const Matrix &rhs ) 
	{
		lhs << "|" << std::setw( 12 ) << std::setprecision( 5 ) << rhs.xx << " " << rhs.yx << "|" << std::endl;
		lhs << "|" << std::setw( 12 ) << std::setprecision( 5 ) << rhs.xy << " " << rhs.yy << "|" << std::endl;
		lhs << "|" << std::setw( 12 ) << std::setprecision( 5 ) << rhs.x0 << " " << rhs.y0 << "|" << std::endl;
		return lhs;
	}	
};

class Pattern {
  public:
  	//! Creates a Pattern from a cairo_pattern_t pointer. Does not automatically increment the cairo_pattern_t reference count.
	Pattern( cairo_pattern_t *aPattern ) : mCairoPattern( aPattern ) {}	
	Pattern( const Pattern &other );
	~Pattern();
	
	Pattern&	operator=( const Pattern &other );
	
	cairo_pattern_t* getCairoPattern() { return mCairoPattern; }

	void		setExtend( int extend );
	void		setExtendNone();
	void		setExtendRepeat();
	void		setExtendReflect();
	void		setExtendPad();
	int			getExtend() const;

	void		setFilter( int filter );
	int			getFilter() const;
	
	void		setMatrix( const Matrix &matrix );
	Matrix		getMatrix() const;

  protected:
	Pattern() : mCairoPattern( 0 ) {}

	cairo_pattern_t *mCairoPattern;
};

class PatternSolid : public Pattern {
  public:
	PatternSolid( const ColorA &color );
	PatternSolid( const Color &color );
};

class PatternSurface : public Pattern {
  public:
	//! Initializes with a null PatternSurface
	PatternSurface() : Pattern() {}
	PatternSurface( SurfaceBase &surface );
	PatternSurface( cinder::Surface cinderSurface );
	PatternSurface( ImageSourceRef imageSource );
};

class Gradient : public Pattern {
  public:
	void		addColorStopRgb( double offset, double red, double green, double blue );
	void		addColorStopRgba( double offset, double red, double green, double blue, double alpha );
	void		addColorStop( double offset, const Color &color ) { addColorStopRgb( offset, (double)color.r, (double)color.g, (double)color.b ); }
	void		addColorStop( double offset, const ColorA &color ) { addColorStopRgba( offset, (double)color.r, (double)color.g, (double)color.b, (double)color.a ); }
	//! Returns the number of color stops specified in the given gradient pattern.
	int			getColorStopCount();
	void		getColorStopRgba( int index, double *offset, double *red, double *green, double *blue, double *alpha );

  protected:
	Gradient( cairo_pattern_t *pattern );
};

class GradientRadial : public Gradient {
  public:
	GradientRadial( double x0, double y0, double radius0, double x1, double y1, double radius1 );
	GradientRadial( const vec2 &center0, float radius0, const vec2 &center1, float radius1 );	
};

class GradientLinear : public Gradient {
  public:
	GradientLinear( double x0, double y0, double x1, double y1 );
	GradientLinear( vec2 point0, vec2 point1 );
};

class FontOptions 
{
public:
	FontOptions() { mCairoFontOptions = 0; }
	FontOptions( cairo_font_options_t *aCairoFontOptions );
	virtual ~FontOptions();
	
	cairo_font_options_t* getCairoFontOptions() { return mCairoFontOptions; }
	
	FontOptions*	create();
	
	int32_t			status();
	void			merge( const FontOptions *other );
	unsigned long	hash(); 
	bool			equal( const FontOptions *other );
	void			setAntiAlias( int32_t antialias );
	int32_t			getAntiAlias();
	void			setSubPixelOrder( int32_t subpixel_order );
	int32_t			getSubPixelOrder();
	void			setHintStyle( int32_t hint_style );
	int32_t			getHintStyle();
	void			setHintMetrics( int32_t hint_metrics );
	int32_t			getHintMetrics();
	
private:
	cairo_font_options_t	*mCairoFontOptions;
};

class FontFace 
{
public:
	FontFace() { mCairoFontFace = 0; }
	FontFace( const std::string &fontName );
	FontFace( cairo_font_face_t *aCairoFontFace );
	virtual ~FontFace();
	
	cairo_font_face_t* getCairoFontFace() const { return mCairoFontFace; }
	
	int32_t		getType();
	
private:
	cairo_font_face_t	*mCairoFontFace;
};

class ScaledFont 
{
public:
	ScaledFont() { mCairoScaledFont = 0; }
	ScaledFont( cairo_scaled_font_t *aCairoScaledFont );
	virtual ~ScaledFont();
	
	cairo_scaled_font_t* getCairoScaledFont() { return mCairoScaledFont; }
	
	int32_t		status();
//	void		fontExtents( class FontExtents *extents );
//	void		textExtents( const char *utf8, class TextExtents *extents );
	//void		glyphExents( const class Glyph *glyphs, int32_t num_glyphs ); // Needs to change to accept arrays
	FontFace*	getFontFace();
	void		getFontOptions( FontOptions *options );
	void		getFontMatrix( Matrix *font_matrix );
	void		getCtm( Matrix *ctm);
	int32_t		getType();
	
private:
	cairo_scaled_font_t	*mCairoScaledFont;
};

/*class Glyph
{
public:
	Glyph();
	Glyph( cairo_glyph_t *aCairoGlyph );
	virtual ~Glyph();
	
	cairo_glyph_t* getCairoGlyph() { return mCairoGlyph.get(); }
	
	const unsigned long&	index() const;
	unsigned long&			index();
	const double&			x() const;
	double&					x();
	const double&			y() const;
	double&					y();
	
private:
	std::shared_ptr<cairo_glyph_t>	mCairoGlyph;
};

// This stores a std::vector of cairo_glyph_t
class GlyphArray 
{
public:
	GlyphArray();
	
	Glyph	getAtIndex( size_t idx );
	void	append( Glyph &aGlyph );
	void	clear();

private:
	std::vector<cairo_glyph_t>	mGlyphs;
	
	friend class Context;
};
*/
class FontExtents
{
public:
	FontExtents();
	FontExtents( cairo_font_extents_t *aCairoFontExtents );
	virtual ~FontExtents();
	
	cairo_font_extents_t* getCairoFontExtents() { return mCairoFontExtents.get(); }
	
	const double&	ascent() const;
	double&			ascent();
	const double&	descent() const;
	double&			descent();
	const double&	height() const;
	double&			height();
	const double&	maxXAdvance() const;
	double&			maxXAdvance();
	const double&	maxYAdvance() const;
	double&			maxYAdvance();
		
private:
	std::shared_ptr<cairo_font_extents_t>	mCairoFontExtents;
};

class TextExtents
{
public:
	TextExtents();
	TextExtents( cairo_text_extents_t *aCairoTextExtents );
	virtual ~TextExtents();
	
	cairo_text_extents_t* getCairoTextExtents() { return mCairoTextExtents.get(); }
	
    const double&	xBearing() const;
    double&			xBearing();
    const double&	yBearing() const;
    double&			yBearing();
    const double&	width() const;
    double&			width();
    const double&	height() const;
    double&			height();
    const double&	xAdvance() const;
    double&			xAdvance();
    const double&	yAdvance() const;	
    double&			yAdvance();	
	
private:
	std::shared_ptr<cairo_text_extents_t>	mCairoTextExtents;
};

/////////////////////////////////////////////////////////////////////////////
// Context
class Context 
{
 public:
	Context() : mCairo( 0 ) {}
	Context( const SurfaceBase &surface ); 
	Context( const Context &other );
	~Context();

	Context& operator=( const Context &other );
	
	//! Returns the native cairo context pointer
	cairo_t*			getCairo();
	//! Returns the native cairo surface pointer
	cairo_surface_t*	getCairoSuface();

	void		save();
	void		restore();
	void		flush();

	void		pushGroup();

	void		pushGroupWithContent( int32_t content );
	Pattern*	popGroup();
	void		popGroupToSource();

	void		setSourceRgb( double red, double green, double blue ); 
	void		setSourceRgba( double red, double green, double blue, double alpha );
	void		setSource( const Color &col ) { setSourceRgb( col.r, col.g, col.b ); }
	void		setSource( const ColorAf &col ) { setSourceRgba( col.r, col.g, col.b, col.a ); }
	void		setSource( const Pattern &source ); 
	void		setSourceSurface( SurfaceBase &surface, double x, double y );
	Pattern*	getSource();

	void		copySurface( const SurfaceBase &surface, const Area &srcArea, const ivec2 &dstOffset = ivec2( 0 ) );
	void		copySurface( const SurfaceBase &surface, const Area &srcArea, const Rectf &dstRect );	

	void		setAntiAlias( int32_t antialias );
	int32_t		getAntiAlias();

	void		setDash( const double *dashes, int numDashes, double offset );
	void		setDash( const std::vector<double> &dashes, double offset = 0 );
	void		unsetDash();
	int			getDashCount() const; 
	void		getDash( double *dashes, double *offset );

	void		setFillRule( int32_t fill_rule );
	int32_t		getFillRule();
	void		setLineCap( int32_t line_cap );
	int32_t		getLineCap();
	void		setLineJoin( int32_t line_join );
	int32_t		getLineJoin();
	void		setLineWidth( double width );
	double		getLineWidth() const;
	void		setMiterLimit( double limit );
	double		getMiterLimit() const;
	
	void		setOperator( int32_t op );
	int32_t		getOperator();

	void		setTolerance( double tolerance );
	double		getTolerance() const;
	void		clip();
	void		clipPerserve();
	void		clipExtents( double *x1, double *y1, double *x2, double *y2 );
	void		resetClip();

	// TODO: This stuff needs to be reviewed before we implement.
//	enum        cairo_rectangle_t;
//	enum        cairo_rectangle_list_t;
//	void        cairo_rectangle_list_destroy    (cairo_rectangle_list_t *rectangle_list);
//	cairo_rectangle_list_t* cairo_copy_clip_rectangle_list(cairo_t *cr);

	void		fill();
	void		fillPreserve();
	void		fillExtents( double *x1, double *y1, double *x2, double *y2 );
	bool		inFill( double x, double y );

//	void        cairo_mask                      (cairo_t *cr, cairo_pattern_t *pattern);
//	void        cairo_mask_surface              (cairo_t *cr, cairo_surface_t *surface, double surface_x, double surface_y);
	void		mask( Pattern *pattern );
	void		maskSurface( SurfaceBase *surface, double surface_x, double surface_y );

	void		paint();
	void		paintWithAlpha( double alpha );
	void		stroke();
	void		strokePreserve();
	void		strokeExtents( double *x1, double *y1, double *x2, double *y2 );
	bool		inStroke( double x, double y );
	void		copyPage();
	void		showPage();
		
	/** Copy the current path to \a resultPath **/
	void		copyPath( cinder::Shape2d *resultPath );
	/** Copy a flattened, curve-free version of the current path to \a resultPath **/
	void		copyPathFlat( cinder::Shape2d *resultPath );
	/*void		pathDestroy( class Path *path );
	void		appendPath( const class Path *path );
	*/
	void		getCurrentPoint( double *x, double *y ) const;
	vec2		getCurrentPoint() const;
	void		newPath();
	void		newSubPath();
	void		closePath();
	void        arc( double xc, double yc, double radius, double angle1, double angle2 );
	void        arc( const vec2 &center, double radius, double angle1, double angle2 ) { arc( center.x, center.y, radius, angle1, angle2 ); }
	void        arcNegative( double xc, double yc, double radius, double angle1, double angle2 );
	void        arcNegative( const vec2 &center, double radius, double angle1, double angle2 ) { arcNegative( center.x, center.y, radius, angle1, angle2 ); }
	void        quadTo( double x1, double y1, double x2, double y2 );
	void        quadTo( const vec2 &v1, const vec2 &v2 ) { quadTo( (double)v1.x, (double)v1.y, (double)v2.x, (double)v2.y ); }
	void        curveTo( double x1, double y1, double x2, double y2, double x3, double y3 );
	void        curveTo( const vec2 &v1, const vec2 &v2, const vec2 &v3 ) { curveTo( (double)v1.x, (double)v1.y, (double)v2.x, (double)v2.y, (double)v3.x, (double)v3.y ); }
	void		line( const vec2 &v1, const vec2 &v2 ) { moveTo( v1 ); lineTo( v2 ); }
	void        lineTo( double x, double y );
	void        lineTo( const vec2 &v ) { lineTo( (double)v.x, (double)v.y ); }
	void        moveTo( double x, double y );
	void        moveTo( const vec2 &v ) { moveTo( (double)v.x, (double)v.y ); }
	void        rectangle( double x, double y, double width, double height );
	void        rectangle( const Rectf &r ) { rectangle( r.x1, r.y1, r.getWidth(), r.getHeight() ); }
	void        rectangle( const vec2 &upperLeft, const vec2 &lowerRight );
	void		roundedRectangle( const Rectf &r, float cornerRadius );
	//void        glyphPath( const cairo_glyph_t *glyphs, int num_glyphs );
	void        textPath( const char *utf8 );
	void        relCurveTo( double dx1, double dy1, double dx2, double dy2, double dx3, double dy3 );
	void        relLineTo( double dx, double dy );
	void        relMoveTo( double dx, double dy );
	void		appendPath( const cinder::Shape2d &path );
	void		appendPath( const cinder::Path2d &path );
	void		circle( double dx, double dy, double radius );
	void		circle( const vec2 &v, double radius ) { circle( (double)v.x, (double)v.y, radius ); }

	//! Renders an svg::Node, with an optional visitor function to modify the style per-node.
	void		render( const svg::Node &node, const std::function<bool(const svg::Node&, svg::Style *)> &visitor = std::function<bool(const svg::Node&, svg::Style *)>() );

// Transformation functions
	void        translate( double tx, double ty );
	void        translate( const vec2 &v ) { translate( (double)v.x, (double)v.y ); }
	void        scale( double sx, double sy );
	void        rotate( double radians );
	void        transform( const Matrix &aMatrix );
	void        transform( const mat3 &matrix );
	void        setMatrix( const Matrix &aMatrix );
	void		setMatrix( const mat3 &matrix );
	void        getMatrix( Matrix *aMatrix );
	mat3		getMatrix() const;
	void        identityMatrix();
	void        userToDevice( double *x, double *y );
	void        userToDeviceDistance( double *dx, double *dy );
	void        deviceToUser( double *x, double *y );
	void        deviceToUserDistance( double *dx, double *dy );
	
// Text/font functions
	void		setFont( const cinder::Font &font );
	void        selectFontFace( const std::string &family, int32_t slant, int32_t weight );
	void        setFontSize( double size );
	void        setFontMatrix( const Matrix &matrix );
	void        getFontMatrix( Matrix *matrix );
	void        setFontOptions( const FontOptions *options );
	void        getFontOptions( FontOptions *options );
	void        setFontFace( const FontFace &font_face );
	FontFace*	getFontFace();
	void        setScaledFont( const ScaledFont *scaled_font );
	ScaledFont*	getScaledFont();
	void        showText( const std::string &s );
	void		textPath( const std::string &s );
	//! Renders glyphs as returned by TextBox::measureGlyphs()
	void		glyphPath( const std::vector<std::pair<uint16_t,vec2> > &glyphs );
	void		glyphPath( uint16_t index, const vec2 &offset );
	//void        showGlyphs( const Glyph *glyphs, int num_glyphs );							// glyphs is an array of cairo_glyph_t
	//void		showGlyphs( const GlyphArray &glyphs );
	FontExtents	fontExtents();
	TextExtents	textExtents( const std::string &s );
	//void        glyphExtents( const Glyph *glyphs, int num_glyphs, TextExtents *extents );	// glyphs is an array of cairo_glyph_t
	//void		glyphExtents( const GlyphArray &glyphs, int num_glyphs, TextExtents *extents );

	std::string	statusToString() const;

  protected:
	cairo_t						*mCairo;
	cairo_surface_t				*mCairoSurface;
};

#if defined( CINDER_COCOA )
SurfaceQuartz	createWindowSurface();
#elif defined( CINDER_MSW )
SurfaceGdi		createWindowSurface();
#endif

// CONSTANTS
extern const int32_t	FONT_SLANT_NORMAL;
extern const int32_t	FONT_SLANT_ITALIC;
extern const int32_t	FONT_SLANT_OBLIQUE;
extern const int32_t	FONT_WEIGHT_NORMAL;
extern const int32_t	FONT_WEIGHT_BOLD;

extern const int32_t	FILL_RULE_WINDING;
extern const int32_t	FILL_RULE_EVEN_ODD;

extern const int32_t	EXTEND_NONE;
extern const int32_t	EXTEND_REPEAT;
extern const int32_t	EXTEND_REFLECT;
extern const int32_t	EXTEND_PAD;

extern const int32_t	LINE_CAP_BUTT;
extern const int32_t	LINE_CAP_ROUND;
extern const int32_t	LINE_CAP_SQUARE;

extern const int32_t	LINE_JOIN_MITER;
extern const int32_t	LINE_JOIN_ROUND;
extern const int32_t	LINE_JOIN_BEVEL;

extern const int32_t	HINT_METRICS_DEFAULT;
extern const int32_t	HINT_METRICS_OFF;
extern const int32_t	HINT_METRICS_ON;
extern const int32_t	HINT_STYLE_DEFAULT;
extern const int32_t	HINT_STYLE_NONE;
extern const int32_t	HINT_STYLE_SLIGHT;
extern const int32_t	HINT_STYLE_MEDIUM;
extern const int32_t	HINT_STYLE_FULL;

extern const int32_t	ANTIALIAS_DEFAULT;
extern const int32_t	ANTIALIAS_NONE;
extern const int32_t	ANTIALIAS_GRAY;
extern const int32_t	ANTIALIAS_SUBPIXEL;

extern const int32_t	SUBPIXEL_ORDER_DEFAULT;
extern const int32_t	SUBPIXEL_ORDER_RGB;
extern const int32_t	SUBPIXEL_ORDER_BGR;
extern const int32_t	SUBPIXEL_ORDER_VRGB;
extern const int32_t	SUBPIXEL_ORDER_VBGR;

extern const int32_t	STATUS_SUCCESS;
extern const int32_t	STATUS_NO_AVAILABLE_MEMORY;
extern const int32_t	STATUS_INVALID_RESTORE;
extern const int32_t	STATUS_INVALID_POP_GROUP;
extern const int32_t	STATUS_NO_CURRENT_POINT;
extern const int32_t	STATUS_INVALID_MATRIX;
extern const int32_t	STATUS_INVALID_STATUS;
extern const int32_t	STATUS_NULL_POINTER;
extern const int32_t	STATUS_INVALID_STRING;
extern const int32_t	STATUS_INVALID_PATH_DATA;
extern const int32_t	STATUS_READ_ERROR;
extern const int32_t	STATUS_WRITE_ERROR;
extern const int32_t	STATUS_SURFACE_FINISHED;
extern const int32_t	STATUS_SURFACE_TYPE_MISMATCH;
extern const int32_t	STATUS_PATTERN_TYPE_MISMATCH;
extern const int32_t	STATUS_INVALID_CONTENT;
extern const int32_t	STATUS_INVALID_FORMAT;
extern const int32_t	STATUS_INVALID_VISUAL;
extern const int32_t	STATUS_FILE_NOT_FOUND;
extern const int32_t	STATUS_INVALID_DASH;
extern const int32_t	STATUS_INVALID_DSC_COMMENT;
extern const int32_t	STATUS_INVALID_INDEX;
extern const int32_t	STATUS_CLIP_NOT_REPRESENTABLE;

} } // namespace cinder::cairo

namespace cinder {

class SurfaceConstraintsCairo : public cinder::SurfaceConstraints {
 public:
	virtual SurfaceChannelOrder getChannelOrder( bool alpha ) const;
	virtual ptrdiff_t			getRowBytes( int requestedWidth, const SurfaceChannelOrder &sco, int elementSize ) const;
};

} // namespace cinder

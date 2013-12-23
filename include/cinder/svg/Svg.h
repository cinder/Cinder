/*
 Copyright (c) 2012, The Cinder Project
 All rights reserved.
 
 This code is designed for use with the Cinder C++ library, http://libcinder.org

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
#include "cinder/Xml.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Shape2d.h"
#include "cinder/PolyLine.h"
#include "cinder/Exception.h"
#include "cinder/MatrixAffine2.h"
#include "cinder/Surface.h"
#include "cinder/Font.h"
#include "cinder/Function.h"

#include <map>
#include <boost/noncopyable.hpp>

namespace cinder { namespace svg {

typedef enum { FILL_RULE_NONZERO, FILL_RULE_EVENODD } FillRule;
typedef enum { LINE_CAP_BUTT, LINE_CAP_ROUND, LINE_CAP_SQUARE } LineCap;
typedef enum { LINE_JOIN_MITER, LINE_JOIN_ROUND, LINE_JOIN_BEVEL } LineJoin;
typedef enum { WEIGHT_100, WEIGHT_200, WEIGHT_300, WEIGHT_400, WEIGHT_NORMAL = WEIGHT_400, WEIGHT_500, WEIGHT_600, WEIGHT_700, WEIGHT_BOLD = WEIGHT_700, WEIGHT_800, WEIGHT_900 } FontWeight;

class Node;
class Group;
class Rect;
class Circle;
class Path;
class TextSpan;
class Style;
class Line;
class Ellipse;
class Polyline;
class Polygon;
class Image;
class ExcChildNotFound;

typedef std::function<bool(const Node&, svg::Style *)> RenderVisitor;

//! Base class from which Renderers are derived.
class Renderer {
  public:
	Renderer() {}
	
	virtual ~Renderer() {}

	void	setVisitor( const std::function<bool(const Node&, svg::Style *)> &visitor );
	
	virtual	void	pushGroup( const Group &group, float opacity ) {}	
	virtual void	popGroup() {}	
	virtual void	drawPath( const svg::Path &path ) {}
	virtual void	drawPolyline( const svg::Polyline &polyline ) {}
	virtual void	drawPolygon( const svg::Polygon &polygon ) {}
	virtual void	drawLine( const svg::Line &line ) {}
	virtual void	drawRect( const svg::Rect &rect ) {}
	virtual void	drawCircle( const svg::Circle &circle ) {}
	virtual void	drawEllipse( const svg::Ellipse &ellipse ) {}
	virtual void	drawImage( const svg::Image &image ) {}
	virtual void	drawTextSpan( const svg::TextSpan &span ) {}

	virtual void	pushMatrix( const MatrixAffine2f &m ) {}
	virtual void	popMatrix() {}
	virtual void	pushStyle( const svg::Style &style ) {}
	virtual void	popStyle( const svg::Style &style ) {}	
	virtual void	pushFill( const class Paint &paint ) {}
	virtual void	popFill() {}
	virtual void	pushStroke( const class Paint &paint ) {}
	virtual void	popStroke() {}
	virtual void	pushFillOpacity( float opacity ) {}
	virtual void	popFillOpacity() {}
	virtual void	pushStrokeOpacity( float opacity ) {}
	virtual void	popStrokeOpacity() {}
	virtual void	pushStrokeWidth( float width ) {}
	virtual void	popStrokeWidth() {}
	virtual void	pushFillRule( FillRule rule ) {}
	virtual void	popFillRule() {}	
	virtual void	pushLineCap( LineCap lineCap ) {}
	virtual void	popLineCap() {}	
	virtual void	pushLineJoin( LineJoin lineJoin ) {}
	virtual void	popLineJoin() {}
	virtual void	pushTextPen( const Vec2f &penPos ) {}
	virtual void	popTextPen() {}
	virtual void	pushTextRotation( float rotation ) {}
	virtual void	popTextRotation() {}

	bool		visit( const Node &node, svg::Style *style ) const {
		if( mVisitor )
			return (*mVisitor)( node, style );
		else
			return true;
	}
	
  protected:
  	// this is a shared_ptr to work around a bug in Clang 4.0
	std::shared_ptr<std::function<bool(const Node&, svg::Style *)> >		mVisitor;
	
	friend class svg::Node;
};

//! SVG Value/Unit pair
class Value {
  public:
	enum Unit { USER, PX, PERCENT, PT, PC, MM, CM, INCH, EM, EX };
	
	Value() : mValue( 0 ), mUnit( USER ) {}
	Value( float value, Unit unit = USER ) : mValue( value ), mUnit( unit ) {}

	float		asUser( float percentOf = 100, float dpi = 72, float fontSize = 12, float fontXHeight = 7 ) const;

	bool		isUser() const { return mUnit == USER; }
	bool		isPercent() const { return mUnit == PERCENT; }
	bool		isPixels() const { return mUnit == PX; }

	static Value		parse( const char **sInOut );
	static Value		parse( const std::string &s );
	
	Unit		mUnit;
	float		mValue;
};

//! SVG Paint specification for fill or stroke, including solids and gradients
class Paint {
  public:
	enum { NONE, COLOR, LINEAR_GRADIENT, RADIAL_GRADIENT };
	
	Paint();
	Paint( uint8_t type );
	Paint( const ColorA8u &color );
	
	static Paint 	parse( const char *value, bool *specified, const Node *parentNode );
	
	bool			isNone() const { return mType == NONE; }
	bool			isLinearGradient() const { return mType == LINEAR_GRADIENT; }
	bool			isRadialGradient() const { return mType == RADIAL_GRADIENT; }
	const ColorA8u&	getColor( size_t idx = 0 ) const { return mStops[idx].second; }
	float			getOffset( size_t idx ) const { return mStops[idx].first; }
	size_t			getNumColors() const { return mStops.size(); }
	
	// only apply to gradients
	Vec2f			getCoords0() const { return mCoords0; } // (x1,y1) on linear, (cx,cy) on radial
	Vec2f			getCoords1() const { return mCoords1; } // (x2,y2) on linear, (fx,fy) on radial
	float			getRadius() const { return mRadius; } // radial-only
	bool			useObjectBoundingBox() const { return mUseObjectBoundingBox; }
	bool			specifiesTransform() const { return mSpecifiesTransform; }
	MatrixAffine2f	getTransform() const { return mTransform; }
	
	uint8_t									mType;
	std::vector<std::pair<float,ColorA8u> >	mStops;
	
	Vec2f				mCoords0, mCoords1;
	float				mRadius;
	bool				mUseObjectBoundingBox;
	MatrixAffine2f		mTransform;
	bool				mSpecifiesTransform;
};

//! SVG Style for a node. Corresponds to SVG Styling: http://www.w3.org/TR/SVG/styling.html
class Style {
  public:
	Style();
	Style( const XmlTree &xml, const Node *parent );

	//! Returns a Style set appropriately for global defaults
	static Style	makeGlobalDefaults();
	//! Marks all styles as unspecified
	void			clear();

	bool				specifiesFill() const { return mSpecifiesFill; }
	void				unspecifyFill() { mSpecifiesFill = false; }
	const Paint&		getFill() const { return mFill; }
	void				setFill( const Paint &fill ) { mSpecifiesFill = true; mFill = fill; }
	static const Paint&	getFillDefault() { return sPaintBlack; }

	bool				specifiesStroke() const { return mSpecifiesStroke; }
	void				unspecifyStroke() { mSpecifiesStroke = false; }
	const Paint&		getStroke() const { return mStroke; }
	void				setStroke( const Paint &stroke ) { mSpecifiesStroke = true; mStroke = stroke; }
	static const Paint&	getStrokeDefault() { return sPaintNone; }

	bool			specifiesOpacity() const { return mSpecifiesOpacity; }
	void			unspecifyOpacity() { mSpecifiesOpacity = false; }
	float			getOpacity() const { return mOpacity; }
	void			setOpacity( float opacity ) { mSpecifiesOpacity = true; mOpacity = opacity; }
	static float	getOpacityDefault() { return 1.0f; }

	bool			specifiesStrokeOpacity() const { return mSpecifiesStrokeOpacity; }
	void			unspecifyStrokeOpacity() { mSpecifiesStrokeOpacity = false; }
	float			getStrokeOpacity() const { return mStrokeOpacity; }
	void			setStrokeOpacity( float strokeOpacity ) { mSpecifiesStrokeOpacity = true; mStrokeOpacity = strokeOpacity; }
	static float	getStrokeOpacityDefault() { return 1.0f; }

	bool			specifiesFillOpacity() const { return mSpecifiesFillOpacity; }
	void			unspecifyFillOpacity() { mSpecifiesFillOpacity = false; }
	float			getFillOpacity() const { return mFillOpacity; }
	void			setFillOpacity( float fillOpacity ) { mSpecifiesFillOpacity = true; mFillOpacity = fillOpacity; }
	static float	getFillOpacityDefault() { return 1.0f; }

	bool			specifiesStrokeWidth() const { return mSpecifiesStrokeWidth; }
	void			unspecifyStrokeWidth() { mSpecifiesStrokeWidth = false; }
	float			getStrokeWidth() const { return mStrokeWidth; }
	void			setStrokeWidth( float strokeWidth ) { mSpecifiesStrokeWidth = true; mStrokeWidth = strokeWidth; }	
	static float	getStrokeWidthDefault() { return 1.0f; }

	bool			specifiesFillRule() const { return mSpecifiesFillRule; }
	void			unspecifyFillRule() { mSpecifiesFillRule = false; }
	FillRule		getFillRule() const { return mFillRule; }
	void			setFillRule( FillRule fillRule ) { mSpecifiesFillRule = true; mFillRule = fillRule; }	
	static FillRule	getFillRuleDefault() { return svg::FILL_RULE_NONZERO; }

	bool			specifiesLineCap() const { return mSpecifiesLineCap; }
	void			unspecifyLineCap() { mSpecifiesLineCap = false; }
	LineCap			getLineCap() const { return mLineCap; }
	void			setLineCap( LineCap lineCap ) { mSpecifiesLineCap = true; mLineCap = lineCap; }	
	static LineCap	getLineCapDefault() { return svg::LINE_CAP_BUTT; }

	bool			specifiesLineJoin() const { return mSpecifiesLineJoin; }
	void			unspecifyLineJoin() { mSpecifiesLineJoin = false; }
	LineJoin		getLineJoin() const { return mLineJoin; }
	void			setLineJoin( LineJoin lineJoin ) { mSpecifiesLineJoin = true; mLineJoin = lineJoin; }	
	static LineJoin	getLineJoinDefault() { return svg::LINE_JOIN_MITER; }
	
	// fonts
	bool									specifiesFontFamilies() const { return mSpecifiesFontFamilies; }
	void									unspecifyFontFamilies() { mSpecifiesFontFamilies = false; }
	const std::vector<std::string>&			getFontFamilies() const { return mFontFamilies; }
	std::vector<std::string>&				getFontFamilies() { return mFontFamilies; }
	void									setFontFamily( const std::string &family ) { mSpecifiesFontFamilies = true; mFontFamilies.clear(); mFontFamilies.push_back( family ); }
	void									setFontFamilies( const std::vector<std::string> &families ) { mSpecifiesFontFamilies = true; mFontFamilies = families; }
	static const std::vector<std::string>&	getFontFamiliesDefault();
	
	bool			specifiesFontSize() const { return mSpecifiesFontSize; }
	void			unspecifyFontSize() { mSpecifiesFontSize = false; }
	Value			getFontSize() const { return mFontSize; }
	void			setFontSize( const Value &fontSize ) { mSpecifiesFontSize = true; mFontSize = fontSize; }
	static Value	getFontSizeDefault() { return Value( 12 ); }

	bool				specifiesFontWeight() const { return mSpecifiesFontWeight; }
	void				unspecifyFontWeight() { mSpecifiesFontWeight = false; }
	FontWeight			getFontWeight() const { return mFontWeight; }
	void				setFontWeight( FontWeight weight ) { mSpecifiesFontWeight = true; mFontWeight = weight; }
	static FontWeight	getFontWeightDefault() { return svg::WEIGHT_NORMAL; }

	bool			specifiesVisible() const { return mSpecifiesVisible; }
	bool			isVisible() const { return mVisible; }
	void			setVisible( bool visible ) { mSpecifiesVisible = true; mVisible = visible; }
	void			unspecifyVisible() { mSpecifiesVisible = false; }
	
	bool			isDisplayNone() const { return mDisplayNone; }
	void			setDisplayNone( bool displayNone ) { mDisplayNone = displayNone; }

	void 		startRender( Renderer &renderer, bool isNodeDrawable ) const;
	void 		finishRender( Renderer &renderer, bool isNodeDrawable ) const;

	void		parseStyleAttribute( const std::string &stylePropertyString, const Node *parent );
	bool		parseProperty( const std::string &key, const std::string &value, const Node *parent );

  protected:
	bool			mSpecifiesOpacity;
	float			mOpacity;
	bool			mSpecifiesFillOpacity, mSpecifiesStrokeOpacity;
	float			mFillOpacity, mStrokeOpacity;

	bool			mSpecifiesFill, mSpecifiesStroke;
	Paint			mFill, mStroke;
	bool			mSpecifiesStrokeWidth;
	float			mStrokeWidth;  	
	bool			mSpecifiesFillRule;
	FillRule		mFillRule;
	bool			mSpecifiesLineCap;
	LineCap			mLineCap;
	bool			mSpecifiesLineJoin;
	LineJoin		mLineJoin;
	
	// fonts
	bool			mSpecifiesFontFamilies, mSpecifiesFontSize, mSpecifiesFontWeight;
	std::vector<std::string>	mFontFamilies;
	Value			mFontSize;
	FontWeight		mFontWeight;
	
	// visibility
	bool			mSpecifiesVisible, mVisible, mDisplayNone;
	
  private:
  	static Paint	sPaintNone, sPaintBlack;
};

//! Base class for an element of an SVG Document
class Node {
  public:
	Node( const Node *parent ) : mParent( parent ),  mSpecifiesTransform( false ), mBoundingBoxCached( false ) {}
	virtual ~Node() {}
	
	//! Returns the svg::Doc this Node is an element of
	class Doc*			getDoc() const;
	//! Returns the immediate parent of this node
	const Node*			getParent() const { return mParent; }
	//! Returns the ID of this Node when present. 
	const std::string&	getId() const { return mId; }
	//! Returns a DOM-style path to this node.
	std::string			getDomPath() const;
	//! Returns the style elements defined on this Node but not inherited from ancestors.
	const Style&		getStyle() const { return mStyle; }
	//! Sets the style defined on this Node but not inherited from ancestors.
	void				setStyle( const Style &style ) { mStyle = style; }
	//! Returns the node's Style, including attributes inherited from its ancestors for attributes it does not specify
	Style				calcInheritedStyle() const;

	//! Returns whether the point \a pt is inside of the Node's shape.
	virtual bool	containsPoint( const Vec2f &pt ) const { return false; }
	
	//! Renders the node and its descendants.
	void			render( Renderer &renderer ) const;

	//! Finds the node with ID \a elementId amongst this Node's ancestors. Returns NULL on failure.
	virtual const Node*		findInAncestors( const std::string &elementId ) const;
	//! Finds the svg::Paint node with ID \a elementId amongst this Node's ancestors. Returns a default svg::Paint instance on failure.
	Paint					findPaintInAncestors( const std::string &paintName ) const;

	//! Returns whether this Node specifies a transformation
	bool			specifiesTransform() const { return mSpecifiesTransform; }
	//! Returns the local transformation of this node. Returns identity if the Node's transform isn't specified.
	MatrixAffine2f		getTransform() const { return mTransform; }
	//! Sets the local transformation of this node.
	void				setTransform( const MatrixAffine2f &transform ) { mTransform = transform; mSpecifiesTransform = true; }
	//! Removes the local transformation of this node, effectively making it the identity matrix.
	void				unspecifyTransform() { mSpecifiesTransform = false; }
	//! Returns the inverse of the local transformation of this node. Returns identity if the Node's transform isn't specified.
	MatrixAffine2f		getTransformInverse() const { return ( mSpecifiesTransform ) ? mTransform.invertCopy() : MatrixAffine2f::identity(); }
	//! Returns the absolute transformation of this node, which includes inherited transformations.
	MatrixAffine2f		getTransformAbsolute() const;
	//! Returns the inverse of the absolute transformation of this node, which includes inherited transformations.
	MatrixAffine2f		getTransformAbsoluteInverse() const { return getTransformAbsolute().invertCopy(); }

	//! Returns the local bounding box of the Node. Calculated and cached the first time it is requested.
	Rectf			getBoundingBox() const { if( ! mBoundingBoxCached ) { mBoundingBox = calcBoundingBox(); mBoundingBoxCached = true; } return mBoundingBox;  }
	//! Returns the absolute bounding box of the Node. Calculated and cached the first time it is requested.
	Rectf			getBoundingBoxAbsolute() const { return getBoundingBox().transformCopy( getTransformAbsolute() ); }

	//! Returns a Shape2d representing the node in local coordinates. Not supported for Text.
	virtual Shape2d	getShape() const { return Shape2d(); }
	//! Returns a Shape2d representing the node in absolute coordinates. Not supported for Text.
	Shape2d			getShapeAbsolute() const { return getShape().transformCopy( getTransformAbsolute() ); }

	//! Returns node's fill, or the first among its ancestors when it has none
	const Paint&	getFill() const;
	//! Returns node's stroke, or the first among its ancestors when it has none
	const Paint&	getStroke() const;
	//! Returns node's opacity, or the first among its ancestors when it has none
	float			getOpacity() const;
	//! Returns node's fill opacity, or the first among its ancestors when it has none
	float			getFillOpacity() const;
	//! Returns node's stroke opacity, or the first among its ancestors when it has none
	float			getStrokeOpacity() const;
	//! Returns node's fill rule, or the first among its ancestors when it has none
	FillRule		getFillRule() const;
	//! Returns node's line cap, or the first among its ancestors when it has none
	LineCap			getLineCap() const;
	//! Returns node's line join, or the first among its ancestors when it has none
	LineJoin		getLineJoin() const;
	//! Returns node's stroke width, or the first among its ancestors when it has none
	float			getStrokeWidth() const;
	//! Returns node's font families, or the first among its ancestors when it has none
	const std::vector<std::string>&		getFontFamilies() const;
	//! Returns node's font size, or the first among its ancestors when it has none
	Value			getFontSize() const;
	//! Returns whether this Node is visible, or the first among its ancestors when unspecified
	bool			isVisible() const;
	//! Returns whether the Display property of this Node is set to 'None', preventing rendering of the node and its children
	bool			isDisplayNone() const { return mStyle.isDisplayNone(); }


  protected:
	Node( const Node *parent, const XmlTree &xml );
	// returns whether this type of node directly renders anything. Everything but groups.
	virtual bool	isDrawable() const { return true; }

	void			startRender( Renderer &renderer, const Style &style ) const;
	void			finishRender( Renderer &renderer, const Style &style ) const;
	virtual void	renderSelf( Renderer &renderer ) const = 0;
	
	virtual Rectf	calcBoundingBox() const { return Rectf( 0, 0, 0, 0 ); }

	static Paint		parsePaint( const char *value, bool *specified, const Node *parentNode );
	static MatrixAffine2f	parseTransform( const std::string &value );
	static bool			parseTransformComponent( const char **c, MatrixAffine2f *result );
	
	static std::string	findStyleValue( const std::string &styleString, const std::string &key );
	void				parseStyle( const std::string &value );
    
  protected:
	const Node		*mParent;
	std::string		mId;
	Style			mStyle;
	bool			mSpecifiesTransform;
	MatrixAffine2f	mTransform;
	mutable bool	mBoundingBoxCached;
	mutable Rectf	mBoundingBox;
	
  private:
  	void			firstStartRender( Renderer &renderer ) const;

	friend class Group;
	friend class Use;
};

//! Base class for SVG Gradients. See SVG Gradients: http://www.w3.org/TR/SVG/pservers.html#Gradients
class Gradient : public Node {
  public:
  	Gradient( const Node *parent, const XmlTree &xml );
	
	class Stop {
	  public:
	  	Stop( const Node *parent, const XmlTree &xml );
		
		float		mOffset; // normalized 0-1
		ColorA8u	mColor;
		float		mOpacity;
		bool		mSpecifiesColor, mSpecifiesOpacity;
	};

	bool			useObjectBoundingBox() const { return mUseObjectBoundingBox; }
	bool			specifiesTransform() const { return mSpecifiesTransform; }
	
  protected:
	virtual void	renderSelf( Renderer &renderer ) const {}

	void 		parse( const Node *parent, const XmlTree &xml );
	void		copyAttributesFrom( const Gradient &rhs );
	Paint		asPaint() const;

  	std::vector<Stop>	mStops;
	Vec2f				mCoords0, mCoords1;
	bool				mUseObjectBoundingBox;
	bool				mSpecifiesTransform;
	MatrixAffine2f		mTransform;
};

//! SVG Linear gradient
class LinearGradient : public Gradient {
  public:
	LinearGradient( const Node *parent, const XmlTree &xml );
	
	Paint		asPaint() const;
	
  protected:
	void 		parse( const XmlTree &xml );
	
  	virtual bool	isDrawable() const { return false; }
};

//! SVG Radial gradient
class RadialGradient : public Gradient {
  public:
	RadialGradient( const Node *parent, const XmlTree &xml );
	
	Paint		asPaint() const;
	
  protected:
	void 		parse( const XmlTree &xml );

	virtual bool	isDrawable() const { return false; }
	float			mRadius;
};

//! SVG Circle element: http://www.w3.org/TR/SVG/shapes.html#CircleElement
class Circle : public Node {
  public:
	Circle( const Node *parent ) : Node( parent ) {}
	Circle( const Node *parent, const XmlTree &xml );
	
	Vec2f		getCenter() const { return mCenter; }
	float		getRadius() const { return mRadius; }

	virtual bool	containsPoint( const Vec2f &pt ) const { return pt.distanceSquared( mCenter ) < mRadius * mRadius; }

	virtual Shape2d	getShape() const;

  protected:	
	virtual void	renderSelf( Renderer &renderer ) const;
	virtual Rectf	calcBoundingBox() const { return Rectf( mCenter.x - mRadius, mCenter.y - mRadius, mCenter.x + mRadius, mCenter.y + mRadius ); }

	Vec2f		mCenter;
	float		mRadius;	
};

//! SVG Ellipse element: http://www.w3.org/TR/SVG/shapes.html#EllipseElement
class Ellipse : public Node {
  public:
	Ellipse( const Node *parent ) : Node( parent ) {}
	Ellipse( const Node *parent, const XmlTree &xml );
	
	Vec2f		getCenter() const { return mCenter; }
	float		getRadiusX() const { return mRadiusX; }
	float		getRadiusY() const { return mRadiusY; }

	bool 			containsPoint( const Vec2f &pt ) const;

	virtual Shape2d	getShape() const;

  protected:
	virtual void	renderSelf( Renderer &renderer ) const;
	virtual Rectf	calcBoundingBox() const { return Rectf( mCenter.x - mRadiusX, mCenter.y - mRadiusY, mCenter.x + mRadiusX, mCenter.y + mRadiusY ); }
  
	Vec2f		mCenter;
	float		mRadiusX, mRadiusY;
};

//! SVG Path element: http://www.w3.org/TR/SVG/paths.html#PathElement
class Path : public Node {
  public:
	Path( const Node *parent ) : Node( parent ) {}
	Path( const Node *parent, const XmlTree &xml );
	
	const Shape2d&		getShape2d() const { return mPath; }
	void				appendShape2d( Shape2d *appendTo ) const;

	virtual bool	containsPoint( const Vec2f &pt ) const { return mPath.contains( pt ); }

	virtual Shape2d	getShape() const { return mPath; }

  protected:
	virtual void	renderSelf( Renderer &renderer ) const;
	virtual Rectf	calcBoundingBox() const { return mPath.calcPreciseBoundingBox(); }
		
	Shape2d		mPath;
};

//! SVG Line element: http://www.w3.org/TR/SVG/shapes.html#LineElement
class Line : public Node {
  public:
	Line( const Node *parent ) : Node( parent ) {}
	Line( const Node *parent, const XmlTree &xml );
	
	const Vec2f&	getPoint1() const { return mPoint1; }
	const Vec2f&	getPoint2() const { return mPoint2; }

	virtual Shape2d	getShape() const;
	
  protected:
	virtual void	renderSelf( Renderer &renderer ) const;  
	virtual Rectf	calcBoundingBox() const { return Rectf( mPoint1, mPoint2 ); }	
	
	Vec2f		mPoint1, mPoint2;
};

//! SVG Rect element: http://www.w3.org/TR/SVG/shapes.html#RectElement
class Rect : public Node {
  public:
	Rect( const Node *parent ) : Node( parent ) {}
	Rect( const Node *parent, const XmlTree &xml );
	
	const Rectf&	getRect() const { return mRect; }

	virtual bool	containsPoint( const Vec2f &pt ) const { return mRect.contains( pt ); }	

	virtual Shape2d	getShape() const;

  protected:
	virtual void	renderSelf( Renderer &renderer ) const;
	virtual Rectf	calcBoundingBox() const { return mRect; }	
		
	Rectf			mRect;
};

//! SVG Polygon Element: http://www.w3.org/TR/SVG/shapes.html#PolygonElement
class Polygon : public Node {
  public:
	Polygon( const Node *parent ) : Node( parent ) {}
	Polygon( const Node *parent, const XmlTree &xml );

	const PolyLine2f&	getPolyLine() const { return mPolyLine; }
	PolyLine2f&			getPolyLine() { return mPolyLine; }

	virtual bool	containsPoint( const Vec2f &pt ) const { return mPolyLine.contains( pt ); }		

	virtual Shape2d	getShape() const;
		
  protected:
	virtual void	renderSelf( Renderer &renderer ) const;  
	virtual Rectf	calcBoundingBox() const { return Rectf( mPolyLine.getPoints() ); }
	
	PolyLine2f	mPolyLine;
};

//! SVG Polyline Element: http://www.w3.org/TR/SVG/shapes.html#PolylineElement
class Polyline : public Node {
  public:
	Polyline( const Node *parent ) : Node( parent ) {}
	Polyline( const Node *parent, const XmlTree &xml );

	const PolyLine2f&	getPolyLine() const { return mPolyLine; }
	PolyLine2f&			getPolyLine() { return mPolyLine; }

	virtual bool	containsPoint( const Vec2f &pt ) const { return mPolyLine.contains( pt ); }

	virtual Shape2d	getShape() const;

  protected:
	virtual void	renderSelf( Renderer &renderer ) const;
	virtual Rectf	calcBoundingBox() const { return Rectf( mPolyLine.getPoints() ); }
		
	PolyLine2f	mPolyLine;
};

//! SVG Use Element, which instantiates a different element: http://www.w3.org/TR/SVG/struct.html#UseElement
class Use : public Node {
  public:
	Use( const Node *parent, const XmlTree &xml );
	
	virtual bool	isDrawable() const { return false; }
	
	virtual Shape2d	getShape() const{ if( mReferenced ) return mReferenced->getShape(); else return Shape2d(); }

  protected:
	virtual void	renderSelf( Renderer &renderer ) const;  
	virtual Rectf	calcBoundingBox() const { if( mReferenced ) return mReferenced->getBoundingBox(); else return Rectf(0,0,0,0); }
	
	void parse( const XmlTree &xml );
	
	const Node		*mReferenced;
};

//! SVG Image Element. Represents an unpremultiplied bitmap. http://www.w3.org/TR/SVG/struct.html#ImageElement
class Image : public Node {
  public:
	Image( const Node *parent, const XmlTree &xml );

	const Rectf&						getRect() const { return mRect; }
	const std::shared_ptr<Surface8u>	getSurface() const { return mImage; }

	virtual bool	containsPoint( const Vec2f &pt ) const { return mRect.contains( pt ); }

  protected:
	virtual void	renderSelf( Renderer &renderer ) const;
	virtual Rectf	calcBoundingBox() const { return mRect; }
  
	static std::shared_ptr<Surface8u>	parseDataImage( const std::string &data );

	Rectf		mRect;
	fs::path	mFilePath;
	std::shared_ptr<Surface8u>	mImage;
};

typedef std::shared_ptr<TextSpan>	TextSpanRef;

//! SVG tspan Element. Generally owned by a svg::Text Node. http://www.w3.org/TR/SVG/text.html#TSpanElement
class TextSpan : public Node {
  public:
	class Attributes {
	  public:
		Attributes() {}
		Attributes( const XmlTree &xml );

		void 	startRender( Renderer &renderer ) const;
		void 	finishRender( Renderer &renderer ) const;

		std::vector<Value>	mX, mY;
		float				mDx, mDy;
		std::vector<Value>	mRotate;
		float				mTextLength;
		float				mLengthAdjust;
	};

	TextSpan( const Node *parent, const XmlTree &xml );
	TextSpan( const Node *parent, const std::string &spanString );
	
	const std::string&							getString() const { return mString; }
	const std::shared_ptr<Font>					getFont() const;
	//! Returns a vector of glyph IDs and positions for the string, ignoring rotation. Cached and lazily calculated.
	std::vector<std::pair<uint16_t,Vec2f> > 	getGlyphMeasures() const;
	Vec2f										getTextPen() const;
	float						getRotation() const;
	
  protected:
	virtual void	renderSelf( Renderer &renderer ) const;

	bool							mIgnoreAttributes; // TextSpans that are actually the contents of Text's attributes should be ignored
	Attributes						mAttributes;
	std::string						mString;
	mutable std::shared_ptr<Font>	mFont;
	mutable std::shared_ptr<std::vector<std::pair<uint16_t,Vec2f> > > mGlyphMeasures;
	mutable std::shared_ptr<Shape2d>	mShape;
	
	std::vector<TextSpanRef>		mSpans;
	
	friend class Text;
};

//! SVG Text element. http://www.w3.org/TR/SVG/text.html#TextElement
class Text : public Node {
  public:
  	Text( const Node *parent, const XmlTree &xml );

	Vec2f 	getTextPen() const;
	float	getRotation() const;
	
  protected:
	virtual void	renderSelf( Renderer &renderer ) const;

	TextSpan::Attributes		mAttributes;

	std::vector<TextSpanRef>	mSpans;
};

//! Represents a group of SVG elements. http://www.w3.org/TR/SVG/struct.html#Groups
class Group : public Node, private boost::noncopyable {
  public:
	Group( const Node *parent ) : Node( parent ) {}
	Group( const Node *parent, const XmlTree &xml );
	~Group();

	//! Recursively searches for a child element of type <tt>svg::T</tt> named \a id. Returns NULL on failure to find the object or if it is not of type T.
    template<typename T>
	const T*				find( const std::string &id ) { return dynamic_cast<const T*>( findNode( id ) ); }
	//! Recursively searches for a child element named \a id. Returns NULL on failure.
	const Node*				findNode( const std::string &id, bool recurse = true ) const;
	//! Recursively searches for a child element of type <tt>svg::T</tt> whose name contains \a idPartial. Returns NULL on failure to find the object or if it is not of type T.
    template<typename T>
	const T*				findByIdContains( const std::string &idPartial ) { return dynamic_cast<const T*>( findNodeByIdContains( idPartial ) ); }
	//! Recursively searches for a child element whose name contains \a idPartial. Returns NULL on failure. (null_ptr later?)
	const Node*				findNodeByIdContains( const std::string &idPartial, bool recurse = true ) const;
	virtual const Node*		findInAncestors( const std::string &elementId ) const;
	//! Returns a reference to the child named \a id. Throws svg::ExcChildNotFound if not found.
	const Node&				getChild( const std::string &id ) const;
	//! Returns a reference to the child named \a id. Throws svg::ExcChildNotFound if not found.
	const Node&				operator/( const std::string &id ) const { return getChild( id ); }
	
	//! Returns the merged Shape2d for all children of the group
	virtual Shape2d	getShape() const { return getMergedShape2d(); }

	//! Appends the merged Shape2d for the group to \a appentTo.	
	void					appendMergedShape2d( Shape2d *appendTo ) const;

	//! Returns a reference to the list of the Group's children.
	const std::list<Node*>&	getChildren() const { return mChildren; }
	//! Returns a reference to the list of the Group's children.
	std::list<Node*>&		getChildren() { return mChildren; }

  protected:
	Node*		nodeUnderPoint( const Vec2f &absolutePoint, const MatrixAffine2f &parentInverseMatrix ) const;
	Shape2d		getMergedShape2d() const;

	virtual void	renderSelf( Renderer &renderer ) const;
	virtual Rectf	calcBoundingBox() const;

	virtual bool	isDrawable() const { return false; }
	void 			parse( const XmlTree &xml );

	std::list<Node*>		mChildren;
	std::shared_ptr<Group>	mDefs;
};


typedef std::shared_ptr<Doc>	DocRef;
//! Represents an SVG Document. See SVG Document Structure http://www.w3.org/TR/SVG/struct.html
class Doc : public Group {
  public:
	Doc() : Group( 0 ), mWidth( 0 ), mHeight( 0 ) {}
	Doc( const fs::path &filePath );
	Doc( DataSourceRef dataSource, const fs::path &filePath = fs::path() );

	static DocRef	create( const fs::path &filePath );
	static DocRef	create( DataSourceRef dataSource, const fs::path &filePath = fs::path() );
	static DocRef	createFromSvgz( DataSourceRef dataSource, const fs::path &filePath = fs::path() );

	//! Returns the width of the document in pixels
	int32_t		getWidth() const { return mWidth; }
	//! Returns the height of the document in pixels
	int32_t		getHeight() const { return mHeight; }
	//! Returns the size of the document in pixels
	Vec2i		getSize() const { return Vec2i( getWidth(), getHeight() ); }	
	//! Returns the aspect ratio of the Doc (width / height)
	float		getAspectRatio() const { return getWidth() / (float)getHeight(); }
	//! Returns the bounds of the Doc (0,0,width,height)
	Area		getBounds() const { return Area( 0, 0, mWidth, mHeight ); }
	
	//! Returns the document's dots-per-inch. Currently hardcoded to 72.
	float		getDpi() const { return 72.0f; }
	
	//! Returns the top-most Node which contains \a pt. Returns NULL if no Node contains the point.
	Node*		nodeUnderPoint( const Vec2f &pt );
	
	//! Utility function to load an image relative to the document. Caches results.
	std::shared_ptr<Surface8u>	loadImage( fs::path relativePath );
  private:
  	void 	loadDoc( DataSourceRef source, fs::path filePath );

	virtual void		renderSelf( Renderer &renderer ) const;
  
	std::shared_ptr<XmlTree>	mXmlTree;
	std::map<fs::path,std::shared_ptr<Surface8u> >	mImageCache;
	
	fs::path		mFilePath;
	Area			mViewBox;
	int32_t			mWidth, mHeight;
};

//! SVG Exception base-class
class Exc : public Exception
{};

class ValueExc : public Exc
{};

class FloatParseExc : public Exc
{};

class PathParseExc : public Exc
{};

class TransformParseExc : public Exc
{};

class ExcChildNotFound : public Exc {
  public:
	ExcChildNotFound( const std::string &child ) throw();
  
	virtual const char* what() const throw() { return mMessage; }
  
  private:
	char mMessage[2048];
};


} } // namespace cinder::svg
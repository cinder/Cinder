//
//  qbSvg.cpp
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//

#include "qbSvg.h"
#include "qbCube.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"

#include <errno.h>

#define VERBOSE_SVG

#define SVG_xyz(x,y,z)		(SVG_Vec3f(Vec3f((x),(y),(z))))
#define SVG_Vec3f(v)		((v)-Vec3f(mViewBox.getX1(),mViewBox.getY1(),0))

namespace cinder { namespace qb {
	
	///////////////////////////////////
	//
	// SVG LOADER
	//	By Roger Sodre
	//	sep/2011
	//
	// SVG Reference: http://www.w3.org/TR/SVG/
	//
	// Instructions for generating the SVG file:
	//	- Edit on Adobe Illustrator
	//		Each Base Layer will be interpreted as a different layer
	//		You can use the Base Layer name as a hint for anything, like depth
	//		Inside layers, each element is a unique Poly
	//		Groups inside base layers are treated as a single Poly
	//		Layers inside base layers will be treated as groups (single Poly)
	//	- Save as.. SVG
	//		SVG Profiles:	"SVG 1.1"
	//		Images:			"Link"
	//		Check:			"Preserve Illustrator Editing Capabilities"
	//		(this will output a lot of useless data, but will preserve the layer hierarchy)
	//
	
	//
	// Return polys loaded
	int qbSvg::load( const DataSourceRef & _res, Vec2f destSize )
	{
		return this->parseSvg( XmlTree( _res ), destSize );
	}
	int qbSvg::load( const std::string & _f, Vec2f destSize )
	{
		return this->parseSvg( XmlTree( _f ), destSize );
	}
	
	//
	// Return polys loaded
	int qbSvg::parseSvg( const XmlTree  & _doc, Vec2f destSize ) {
		// grab SVG
		if ( ! _doc.hasChild("svg") )
		{
			printf(">>> QBSVG : doc is NOT SVG!\n");
			return 0;
		}
		XmlTree svg = _doc.getChild( "svg" );
		
		// Get doc data
		float w, h;
		if ( svg.hasAttribute( "viewBox" ) )
		{
			//
			// ATENCAO :: VIEWBOX NAO EH BOUNDS!!!!!!!
			//
			// <min-x>, <min-y>, <width> and <height>
			// http://www.w3.org/TR/SVG/coords.html#ViewBoxAttribute
			std::vector<std::string> vals = ci::split( svg.getAttributeValue<std::string>( "viewBox" ), " " );
			for (int n = 0 ; n < vals.size() ; n++)
				printf(">>> VIEWBOX  %d/%d =  %s\n",n,(int)vals.size(),vals[n].c_str());
			float x = atof(vals[0].c_str());
			float y = atof(vals[1].c_str());
			w = atof(vals[2].c_str());
			h = atof(vals[3].c_str());
			printf(">>> VIEWBOX  xywh  %.2f %.2f %.2f %.2f\n",x,y,w,h);
			mViewBox = Rectf( x, y, w, h );
		}
		else if ( svg.hasAttribute( "width" ) && svg.hasAttribute( "height" ) )
		{
			w = svg.getAttributeValue<float>( "width" );
			h = svg.getAttributeValue<float>( "height" );
			mViewBox = Rectf( 0, 0, w, h );
		}
		printf(">>> QBSVG : viewBox : %.2f, %.2f, %.2f, %.2f\n",mViewBox.getX1(),mViewBox.getX2(),mViewBox.getY1(),mViewBox.getY2());
		
		// Calc scale
		if ( destSize != Vec2f::zero() )
			mScale = Vec3f( destSize.x / w ,  destSize.y / h, 1.0f );
		else
			mScale = Vec3f::one();
		printf(">>> QBSVG : scale : %.4f %.4f\n",mScale.x,mScale.y);
		
		// Update viewbox
		mViewBox.x1 *= mScale.x;
		mViewBox.x2 *= mScale.x;
		mViewBox.y1 *= mScale.y;
		mViewBox.y2 *= mScale.y;

		// parse!!
		this->parseSvgLayer( svg );
		
		return (int) mPolys.size();
	}
	
	//
	// LAYER
	int lay = -1;
	void qbSvg::parseSvgLayer( const XmlTree  & _l ) {
		// get a name
		std::string layerName = "<" + _l.getTag() + ">";
		if ( _l.hasAttribute( "id" ) )
			layerName = _l.getAttributeValue<std::string>( "id" );
		mCurrLayerName = layerName;
		// is off??
		if ( _l.hasAttribute( "display" ) )
		{
			std::string display = _l.getAttributeValue<std::string>( "display" );
			if ( display == "none" )
			{
				printf(">>> QBSVG : %s : is OFF!\n",layerName.c_str());
				return;
			}
		}
		// Parse children
		std::list<XmlTree> children = _l.getChildren();
		std::list<XmlTree>::iterator p;
		for ( p = children.begin() ; p != children.end(); p++ )
		{
			qbPoly apoly;
			// get a name
			std::string tag = (*p).getTag();
			if ( (*p).hasAttribute( "id" ) )
				apoly.setName( (*p).getAttributeValue<std::string>( "id" ) );
			else
				apoly.setName( "<" + tag + ">" );
			// parse
			if ( tag == "g" || tag == "switch" )
			{
				this->parseSvgLayer( *p );
				mCurrLayerName = layerName;
			}
			else if ( tag == "polygon" )
				this->parseSvgPolygon( *p, true, &apoly );
			else if ( tag == "polyline" )
				this->parseSvgPolygon( *p, false, &apoly );
			else if ( tag == "rect" )
				this->parseSvgRect( *p, &apoly );
			else if ( tag == "line" )
				this->parseSvgLine( *p, &apoly );
			else if ( tag == "circle" )
				this->parseSvgCircle( *p, &apoly );
			else if ( tag == "ellipse" )
				this->parseSvgEllipse( *p, &apoly );
			else if ( tag == "path" )
				this->parseSvgPath( *p, &apoly );
			else
			{
#ifdef VERBOSE_SVG
				printf(">>> QBSVG : %s : %s : UNKNOWN TAG\n",mCurrLayerName.c_str(),apoly.getName().c_str());
#endif
				//continue;
			}
			// Add poly!
			if ( apoly.getType() != QBPOLY_NONE )
			{
				// set layer
				if ( lay == -1 || mCurrLayerName != mLastLayerName )
				{
					lay++;
					mLastLayerName = mCurrLayerName;
					printf(">>> QBSVG : NEW LAYER %d [%s]\n",lay,mCurrLayerName.c_str());
				}
				apoly.setLayerName( mCurrLayerName );
				apoly.setLayer( lay );
				// Add poly!
				this->addPoly( apoly );
			}
		}
	}
	//
	// <polygon fill="none" stroke="#ED1C24" stroke-width="2" stroke-miterlimit="10" points="1854.75,340.249 1954.734,340.249 2042.234,627.698 1767.245,627.698 	"/>
	// <polyline fill="none" stroke="#ED1C24" stroke-width="2" stroke-miterlimit="10" points="1854.75,340.249 1954.734,340.249 2042.234,627.698 1767.245,627.698 	"/>
	void qbSvg::parseSvgPolygon( const XmlTree  & _p, bool _closed, qbPoly *apoly ) {
		std::vector<std::string> points = ci::split( _p.getAttributeValue<std::string>( "points" ), " \t\n\r" );
#ifdef VERBOSE_SVG
		printf(">>> QBSVG : %s : %s : POLYGON (%d vertices)...\n",mCurrLayerName.c_str(),apoly->getName().c_str(),(int)points.size());
#endif
		for (int p = 0 ; p < points.size() ; p++)
		{
			std::vector<std::string> vals = ci::split( points[p], "," );
			printf("    %d : points (%d) = [%s]\n",p,(int)vals.size(),points[p].c_str());
			if (vals.size() == 2)
			{
				float x = atof(vals[0].c_str()) * mScale.x;
				float y = atof(vals[1].c_str()) * mScale.y;
				apoly->addVertex( SVG_xyz( x, y, 0.0f ) );
			}
		}
		apoly->finishOptimized();
		if ( _closed )
			apoly->close();
	}
	
	//
	// <rect x="137" y="571.495" width="749.999" height="125.005"/>
	void qbSvg::parseSvgRect( const XmlTree  & _p, qbPoly *apoly ) {
		try {
			float x = _p.getAttributeValue<float>( "x" ) * mScale.x;
			float y = _p.getAttributeValue<float>( "y" ) * mScale.y;
			float w = _p.getAttributeValue<float>( "width" ) * mScale.x;
			float h = _p.getAttributeValue<float>( "height" ) * mScale.y;
#ifdef VERBOSE_SVG
			printf(">>> QBSVG : %s : %s : RECT xy %.1f %.1f  wh %.1f %.1f\n",mCurrLayerName.c_str(),apoly->getName().c_str(),x,y,w,h);
#endif
			/*
			apoly->addVertex( SVG_xyz(x,y,0) );
			apoly->addVertex( SVG_xyz(x+w,y,0) );
			apoly->addVertex( SVG_xyz(x,y+h,0) );
			apoly->addVertex( SVG_xyz(x+w,y+h,0) );
			apoly->close();
			apoly->finishOptimized();
			 */
			apoly->makeRect( SVG_xyz(x,y,0), w, h );
		} catch ( XmlTree::Exception ) {
			printf(">>> QBSVG : RECT exception\n");
		}
	}
	
	//
	// <line fill="none" stroke="#000000" stroke-miterlimit="10" x1="33.5" y1="338.5" x2="55.5" y2="445.5"/>
	void qbSvg::parseSvgLine( const XmlTree  & _p, qbPoly *apoly ) {
		try {
			float x1 = _p.getAttributeValue<float>( "x1" ) * mScale.x;
			float y1 = _p.getAttributeValue<float>( "y1" ) * mScale.y;
			float x2 = _p.getAttributeValue<float>( "x2" ) * mScale.x;
			float y2 = _p.getAttributeValue<float>( "y2" ) * mScale.y;
#ifdef VERBOSE_SVG
			printf(">>> QBSVG : %s : %s : LINE xy %.1f %.1f .. w %.1f %.1f\n",mCurrLayerName.c_str(),apoly->getName().c_str(),x1,y1,x2,y2);
#endif
			apoly->addVertex( SVG_xyz( x1, y1, 0 ) );
			apoly->addVertex( SVG_xyz( x2, y2, 0 ) );
			apoly->finishOptimized();
		} catch ( XmlTree::Exception ) {
			printf(">>> QBSVG : LINE exception\n");
		}
	}
	
	// <circle fill="none" stroke="#000000" stroke-miterlimit="10" cx="282" cy="380" r="45.5"/>
	void qbSvg::parseSvgCircle( const XmlTree  & _p, qbPoly *apoly ) {
		try {
			float x = _p.getAttributeValue<float>( "cx" ) * mScale.x;
			float y = _p.getAttributeValue<float>( "cy" ) * mScale.y;
			float rx = _p.getAttributeValue<float>( "r" ) * mScale.x;
			float ry = _p.getAttributeValue<float>( "r" ) * mScale.y;
#ifdef VERBOSE_SVG
			printf(">>> QBSVG : %s : %s : CIRCLE xy %.1f %.1f  r %.1f %.1f\n",mCurrLayerName.c_str(),apoly->getName().c_str(),x,y,rx,ry);
#endif
			apoly->makeCircle( SVG_xyz(x,y,0), rx, ry );
		} catch ( XmlTree::Exception ) {
			printf(">>> QBSVG : CIRCLE exception\n");
		}
	}
	
	// <ellipse fill="none" stroke="#000000" stroke-miterlimit="10" cx="407" cy="377.5" rx="16" ry="57.5"/>
	void qbSvg::parseSvgEllipse( const XmlTree  & _p, qbPoly *apoly ) {
		try {
			float x = _p.getAttributeValue<float>( "cx" ) * mScale.x;
			float y = _p.getAttributeValue<float>( "cy" ) * mScale.y;
			float rx = _p.getAttributeValue<float>( "rx" ) * mScale.x;
			float ry = _p.getAttributeValue<float>( "ry" ) * mScale.y;
#ifdef VERBOSE_SVG
			printf(">>> QBSVG : %s : %s : ELLIPSE xy %.1f %.1f  rxy %.1f %.1f\n",mCurrLayerName.c_str(),apoly->getName().c_str(),x,y,rx,ry);
#endif
			apoly->makeCircle( SVG_xyz(x,y,0), rx, ry );
		} catch ( XmlTree::Exception ) {
			printf(">>> QBSVG : ELLIPSE exception\n");
		}
	}
	
	// From: http://www.w3.org/TR/SVG/paths.html#PathData
	// <path fill="none" stroke="#000000" stroke-miterlimit="10" d="M190.5,356.5c0,53.062,42.49,96,95,96"/>
	void qbSvg::parseSvgPath( const XmlTree  & _p, qbPoly *apoly ) {
		try {
#ifdef VERBOSE_SVG
			printf(">>> QBSVG : %s : %s : PATH...\n",mCurrLayerName.c_str(),apoly->getName().c_str());
#endif
			char cmd;
			Vec3f v, last_v;
			Vec3f c0, c1, last_c;
			std::string d = _p.getAttributeValue<std::string>( "d" );
			std::string values;
			std::string::iterator it;
			for ( it=d.begin() ; it < d.end(); it++ )
			{
				char c = *it;
				if ( isalpha(c) )
				{
					cmd = c;
					values.clear();
				}
				else
					values += c;
				// jump following spaces
				while ( isspace(*(it+1)) )
					it++;
				// Execure command!
				if ( isalpha(*(it+1)) || (it+1) == d.end() )
				{
					std::vector<float> vals = splitPathValues( values );
#ifdef VERBOSE_SVG
					printf("            %c = [%s] = %d vals (",cmd,values.c_str(),(int)vals.size());
					for (int n = 0 ; n < vals.size() ; n++)
						 printf("%s%.1f",(n==0?" ":", "),vals[n]);
					printf(" )\n");
#endif
					switch (cmd)
					{
						case 'm':		// Relative
						case 'M':		// Absolute
						case 'l':		// Relative
						case 'L':		// Absolute
							// http://www.w3.org/TR/SVG/paths.html#PathDataMovetoCommands
							// http://www.w3.org/TR/SVG/paths.html#PathDataLinetoCommands
							// 2 points: (x y)
							for (int n = 0 ; n < vals.size() / 2 ; n++)
							{
								v = Vec3f( vals[n+0], vals[n+1], 0 ) * mScale;
								if ( islower(cmd) )
									v += last_v;
								apoly->addVertex( SVG_Vec3f(v) );
								last_v = last_c = v;	// save last
							}
							break;
						case 'h':		// Relative
						case 'H':		// Absolute
							// http://www.w3.org/TR/SVG/paths.html#PathDataLinetoCommands
							// 1 point: (x)
							for (int n = 0 ; n < vals.size() / 1 ; n++)
							{
								v = Vec3f( vals[n+0] * mScale.x, last_v.y, 0 );
								if ( islower(cmd) )
									v.x += last_v.x;
								apoly->addVertex( SVG_Vec3f(v) );
								last_v = last_c = v;	// save last
							}
							break;
						case 'v':		// Relative
						case 'V':		// Absolute
							// http://www.w3.org/TR/SVG/paths.html#PathDataLinetoCommands
							// 1 point: (y)
							for (int n = 0 ; n < vals.size() / 1 ; n++)
							{
								v = Vec3f( last_v.x, vals[n+0] * mScale.y, 0 );
								if ( islower(cmd) )
									v.y += last_v.y;
								apoly->addVertex( SVG_Vec3f(v) );
								last_v = last_c = v;	// save last
							}
							break;
						case 'c':		// Relative
						case 'C':		// Absolute
							// http://www.w3.org/TR/SVG/paths.html#PathDataCubicBezierCommands
							// 6 points: (x1 y1 x2 y2 x y)
							for (int n = 0 ; n < vals.size() / 6 ; n++)
							{
								c0 = Vec3f( vals[n+0], vals[n+1], 0 ) * mScale;
								c1 = Vec3f( vals[n+2], vals[n+3], 0 ) * mScale;
								v = Vec3f( vals[n+4], vals[n+5], 0 ) * mScale;
								if ( islower(cmd) )
								{
									v += last_v;
									c0 += last_v;
									c1 += last_v;
								}
								apoly->makeCurve( SVG_Vec3f(last_v), SVG_Vec3f(v), c0, c1 );
								last_v = v;		// save last
								last_c = c1;	// save last
							}
							break;
						case 's':		// Relative
						case 'S':		// Absolute
							// http://www.w3.org/TR/SVG/paths.html#PathDataCubicBezierCommands
							// 4 points: (x2 y2 x y)
							for (int n = 0 ; n < vals.size() / 4 ; n++)
							{
								c0 = last_v - Vec3f( last_c.x-last_v.x, last_c.y-last_v.y, 0);	// reflection
								c1 = Vec3f( vals[n+0], vals[n+1], 0 ) * mScale;
								v = Vec3f( vals[n+2], vals[n+3], 0 ) * mScale;
								if ( islower(cmd) )
								{
									v += last_v;
									c1 += last_v;
								}
								apoly->makeCurve( SVG_Vec3f(last_v), SVG_Vec3f(v), c0, c1 );
								last_v = v;		// save last
								last_c = c1;	// save last
							}
							break;
						case 'z':
						case 'Z':
							// http://www.w3.org/TR/SVG/paths.html#PathDataClosePathCommand
							apoly->close();
							break;
						default:
							printf(">>> QBSVG : command [%c] NOT IMPLEMENTED!!!\n",cmd);
							break;
					}
				}
			}
			apoly->finishOptimized();
		} catch ( XmlTree::Exception ) {
			printf(">>> QBSVG : PATH exception\n");
		}
	}
	// Split Path values string
	// Values are separated by comma or minus (-) sign of next value
	// From: http://www.w3.org/TR/SVG/paths.html#PathData
	// <path fill="none" stroke="#000000" stroke-miterlimit="10" d="M190.5,356.5c0,53.062,42.49,96,95,96"/>
	std::vector<float> qbSvg::splitPathValues( std::string & _values )
	{
		std::vector<float> theValues;
		std::string v;
		std::string::iterator it;
		for ( it=_values.begin() ; it < _values.end(); it++ )
		{
			char c = *it;
			if ( isnumber(c) || c == '.' || (c == '-' && v.size()==0) )
				v += c;
			if ( c == ',' || 
				 c == ' ' || 
				( c == '-' && (v.size()>1 || (v.size()>0 && isnumber(v[0]))) ) || 
				(it+1) == _values.end() )
			{
				float fv = atof( v.c_str() );
				//printf("                 %.3f\n",fv);
				theValues.push_back(fv);
				v.clear();
				if ( c == '-' )
					v += c;
			}
		}
		return theValues;
	}
	

} } // cinder::qb



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

#include "cinder/gl/Texture.h"
#include "cinder/svg/Svg.h"
#include "cinder/Triangulate.h"

namespace cinder {

class CI_API SvgRendererGl : public svg::Renderer {
  public:
	SvgRendererGl() : svg::Renderer() { 
		mFillStack.push_back( svg::Paint( Color::black() ) );
		mStrokeStack.push_back( svg::Paint() );
		mFillOpacityStack.push_back( 1.0f );
		mStrokeOpacityStack.push_back( 1.0f );
		mStrokeWidthStack.push_back( 1.0f );
		glLineWidth( 1.0f );
		gl::pushModelMatrix();
		mFillRuleStack.push_back( svg::FILL_RULE_NONZERO );
	}
	
	~SvgRendererGl() {
		gl::popModelMatrix();
	}
  
	void	pushGroup( const svg::Group &group, float opacity ) {}
	
	void	drawPath( const svg::Path &path ) override {
		if( ! mFillStack.back().isNone() ) {
			gl::color( getCurFillColor() );
			Triangulator::Winding winding = ( mFillRuleStack.back() == svg::FILL_RULE_NONZERO ) ? Triangulator::WINDING_NONZERO : Triangulator::WINDING_ODD;
			gl::draw( Triangulator( path.getShape2d() ).calcMesh( winding ) );
		}
		if( ! mStrokeStack.back().isNone() ) {
			gl::color( getCurStrokeColor() );
			gl::draw( path.getShape2d() );
		}
	}

	void	drawPolygon( const svg::Polygon &polygon ) override {
		if( ! mFillStack.back().isNone() ) {
			gl::color( getCurFillColor() );
			Triangulator::Winding winding = ( mFillRuleStack.back() == svg::FILL_RULE_NONZERO ) ? Triangulator::WINDING_NONZERO : Triangulator::WINDING_ODD;
			gl::draw( Triangulator( polygon.getPolyLine() ).calcMesh( winding ) );

		}
		if( ! mStrokeStack.back().isNone() ) {
			gl::color( getCurStrokeColor() );
			gl::draw( polygon.getPolyLine() );
		}
	}

	void	drawPolyline( const svg::Polyline &polyline ) override {
		if( ! mFillStack.back().isNone() ) {
			gl::color( getCurFillColor() );
			Triangulator::Winding winding = ( mFillRuleStack.back() == svg::FILL_RULE_NONZERO ) ? Triangulator::WINDING_NONZERO : Triangulator::WINDING_ODD;
			gl::draw( Triangulator( polyline.getPolyLine() ).calcMesh( winding ) );

		}
		if( ! mStrokeStack.back().isNone() ) {
			gl::color( getCurStrokeColor() );
			gl::draw( polyline.getPolyLine() );
		}
	}

	void	drawLine( const svg::Line &line ) override {
		if( ! mStrokeStack.back().isNone() ) {
			gl::color( getCurStrokeColor() );
			gl::drawLine( line.getPoint1(), line.getPoint2() );
		}
	}

	void	drawRect( const svg::Rect &rect ) override {
		if( ! mFillStack.back().isNone() ) {
			gl::color( getCurFillColor() );
			gl::drawSolidRect( rect.getRect() );
		}
		if( ! mStrokeStack.back().isNone() ) {
			gl::color( getCurStrokeColor() );
			gl::drawStrokedRect( rect.getRect() );
		}
	}

	void	drawCircle( const svg::Circle &circle ) override {
		if( ! mFillStack.back().isNone() ) {
			gl::color( getCurFillColor() );
			gl::drawSolidCircle( circle.getCenter(), circle.getRadius() );
		}
		if( ! mStrokeStack.back().isNone() ) {
			gl::color( getCurStrokeColor() );
			gl::drawStrokedCircle( circle.getCenter(), circle.getRadius() );
		}
	}

	void	drawEllipse( const svg::Ellipse &ellipse ) override {
		if( ! mFillStack.back().isNone() ) {
			gl::color( getCurFillColor() );
			gl::drawSolidEllipse( ellipse.getCenter(), ellipse.getRadiusX(), ellipse.getRadiusY() );
		}
		if( ! mStrokeStack.back().isNone() ) {
			gl::color( getCurStrokeColor() );
			gl::drawStrokedEllipse( ellipse.getCenter(), ellipse.getRadiusX(), ellipse.getRadiusY() );
		}
	}	
    
	void	drawImage( const Surface8u &surface, const Rectf &drawRect ) {
		gl::color( Color::white() );
		gl::draw( gl::Texture::create( surface ), drawRect );
	}

	void	drawTextSpan( const svg::TextSpan &span ) override {
	
	}

	void	popGroup() override {}

	void	pushMatrix( const mat3 &m ) override {
		gl::pushModelMatrix();
		gl::multModelMatrix( transform2dTo3d( m ) );
	}
	void	popMatrix() override {
		gl::popModelMatrix();
	}
	
	void	pushFill( const svg::Paint &paint ) override { mFillStack.push_back( paint ); }
	void	popFill() override { mFillStack.pop_back(); }
	void	pushStroke( const svg::Paint &paint ) override { mStrokeStack.push_back( paint ); }
	void	popStroke() override { mStrokeStack.pop_back(); }
	void	pushFillOpacity( float opacity ) override { mFillOpacityStack.push_back( opacity ); }
	void	popFillOpacity() override { mFillOpacityStack.pop_back(); }
	void	pushStrokeOpacity( float opacity ) override { mStrokeOpacityStack.push_back( opacity ); }
	void	popStrokeOpacity() override { mStrokeOpacityStack.pop_back(); }

	ColorA	getCurFillColor() { ColorA result( mFillStack.back().getColor() ); result.a = mFillOpacityStack.back(); return result; }
	ColorA	getCurStrokeColor() { ColorA result( mStrokeStack.back().getColor() ); result.a = mStrokeOpacityStack.back(); return result; }


	void	pushStrokeWidth( float width ) override { mStrokeWidthStack.push_back( width ); glLineWidth( width ); }
	void	popStrokeWidth() override { mStrokeWidthStack.pop_back(); glLineWidth( mStrokeWidthStack.back() ); }
	void	pushFillRule( svg::FillRule rule ) override { mFillRuleStack.push_back( rule ); }
	void	popFillRule() override { mFillRuleStack.pop_back(); }	


	std::vector<svg::Paint>		mFillStack, mStrokeStack;
	std::vector<float>			mFillOpacityStack, mStrokeOpacityStack;
	std::vector<float>			mStrokeWidthStack;
	std::vector<svg::FillRule>	mFillRuleStack;
};

namespace gl {
inline void draw( const svg::Doc &svg )
{
	SvgRendererGl renderGl;
	svg.render( renderGl );
}
} // namespace gl

} // namespace cinder
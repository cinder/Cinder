/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/gl/platform.h"
#include "cinder/gl/Texture.h"

#include "cinder/GeomIo.h"
#include "cinder/PolyLine.h"
#include "cinder/Shape2d.h"
#include "cinder/Camera.h"
#include "cinder/Font.h"

// forward declarations
namespace cinder {
	class TriMesh;
	class Path2d;
	class Shape2d;
}

namespace cinder { namespace gl {

typedef std::shared_ptr<class VboMesh>			VboMeshRef;

//! Draws the VboMesh \a mesh. Consider a gl::Batch as a faster alternative. Optionally specify a \a first vertex index and a \a count of vertices.
void draw( const VboMeshRef &mesh, GLint first = 0, GLsizei count = -1 );
//! Draws a Texture2d \a texture, fitting it to \a dstRect. Ignores currently bound shader.
void draw( const Texture2dRef &texture, const Rectf &dstRect );
//! Draws a subregion \a srcArea of a Texture (expressed as upper-left origin pixels).
void draw( const Texture2dRef &texture, const Area &srcArea, const Rectf &dstRect );
void draw( const Texture2dRef &texture, const vec2 &dstOffset = vec2() );
void draw( const PolyLine2 &polyLine );
void draw( const PolyLine3 &polyLine );
//! Draws a Path2d \a pathd using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void draw( const Path2d &path, float approximationScale = 1.0f );
//! Draws a Shaped2d \a shaped using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void draw( const Shape2d &shape, float approximationScale = 1.0f );
//! Draws a TriMesh \a mesh at the origin. Currently only uses position and index information.
void draw( const TriMesh &mesh );
//! Draws a geom::Source \a source at the origin.
void draw( const geom::Source &source );

//! Draws a CubeMapTex \a texture inside \a rect with an equirectangular projection. If \a lod is non-default then a specific mip-level is drawn. Typical aspect ratio should be 2:1.
void drawEquirectangular( const gl::TextureCubeMapRef &texture, const Rectf &r, float lod = -1 );
//! Draws a CubeMapTex \a texture as a horizontal cross, fit inside \a rect. If \a lod is non-default then a specific mip-level is drawn. Typical aspect ratio should be 4:3.
void drawHorizontalCross( const gl::TextureCubeMapRef &texture, const Rectf &rect, float lod = -1 );
//! Draws a CubeMapTex \a texture as a vertical cross, fit inside \a rect. If \a lod is non-default then a specific mip-level is drawn. Typical aspect ratio should be 3:4.
void drawVerticalCross( const gl::TextureCubeMapRef &texture, const Rectf &rect, float lod = -1 );

//! Draws a solid (filled) Path2d \a path using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc. Performance warning: This routine tesselates the polygon into triangles. Consider using Triangulator directly.
void drawSolid( const Path2d &path2d, float approximationScale = 1.0f );
//! Draws a solid (filled) Shape2d \a shape using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc. Performance warning: This routine tesselates the polygon into triangles. Consider using Triangulator directly.
void drawSolid( const Shape2d &shape, float approximationScale = 1.0f );
void drawSolid( const PolyLine2 &polyLine );

//! Renders a solid cube centered at \a center of size \a size. Normals and created texture coordinates are generated.
void drawCube( const vec3 &center, const vec3 &size );
//! Renders a solid cube centered at \a center of size \a size. Each face is assigned a unique color.
void drawColorCube( const vec3 &center, const vec3 &size );
//! Renders a stroked cube centered at \a center of size \a size.
void drawStrokedCube( const vec3 &center, const vec3 &size );
//! Renders a stroked cube using \a box as the guide for center and size.
inline void drawStrokedCube( const ci::AxisAlignedBox &box ) { drawStrokedCube( box.getCenter(), box.getSize() ); }
//! Renders a solid \a sphere, subdivided on both longitude and latitude into \a subdivisions.
void drawSphere( const Sphere &sphere, int subdivisions = -1 );
//! Renders a solid sphere at \a center of radius \a radius, subdivided on both longitude and latitude into \a subdivisions.
void drawSphere( const vec3 &center, float radius, int subdivisions = -1 );
//! Draws a textured quad of size \a scale that is aligned with the vectors \a bbRight and \a bbUp at \a pos, rotated by \a rotationRadians around the vector orthogonal to \a bbRight and \a bbUp.
void drawBillboard( const vec3 &pos, const vec2 &scale, float rotationRadians, const vec3 &bbRight, const vec3 &bbUp, const Rectf &texCoords = Rectf( 0, 0, 1, 1 ) );
//! Renders a stroked representation of \a cam
void drawFrustum( const Camera &cam );
void drawCoordinateFrame( float axisLength = 1.0f, float headLength = 0.2f, float headRadius = 0.05f );
//! Draws a vector starting at \a start and ending at \a end. An arrowhead is drawn at the end of radius \a headRadius and length \a headLength.
void drawVector( const vec3 &start, const vec3 &end, float headLength = 0.2f, float headRadius = 0.05f );

//! Draws a line between points a and b
void drawLine( const vec3 &a, const vec3 &b );
void drawLine( const vec2 &a, const vec2 &b );

//! Draws \a texture on the XY-plane
void drawSolidRect( const Rectf &r, const vec2 &upperLeftTexCoord = vec2( 0, 1 ), const vec2 &lowerRightTexCoord = vec2( 1, 0 ) );
//! Draws a solid rounded rectangle centered around \a rect, with a corner radius of \a cornerRadius
void drawSolidRoundedRect( const Rectf &r, float cornerRadius, int numSegmentsPerCorner = 0,  const vec2 &upperLeftTexCoord = vec2( 0, 1 ), const vec2 &lowerRightTexCoord = vec2( 1, 0 ) );
//! Draws a filled circle centered around \a center with a radius of \a radius. Default \a numSegments requests a conservative (high-quality but slow) number based on radius.
void drawSolidCircle( const vec2 &center, float radius, int numSegments = -1 );
//! Draws a filled ellipse centered around \a center with an X-axis radius of \a radiusX and a Y-axis radius of \a radiusY. Default \a numSegments requests a conservative (high-quality but slow) number based on radius.
void drawSolidEllipse( const vec2 &center, float radiusX, float radiusY, int numSegments = -1 );

//! Draws a stroked rectangle with dimensions \a rect.
void drawStrokedRect( const Rectf &rect );
//! Draws a stroked rectangle centered around \a rect, with a line width of \a lineWidth
void drawStrokedRect( const Rectf &rect, float lineWidth );
//! Draws a stroked rounded rectangle centered around \a rect, with a corner radius of \a cornerRadius
void drawStrokedRoundedRect( const Rectf &rect, float cornerRadius, int numSegmentsPerCorner = 0 );
//! Draws a stroked circle centered around \a center with a radius of \a radius. Default \a numSegments requests a conservative (high-quality but slow) number based on radius.
void drawStrokedCircle( const vec2 &center, float radius, int numSegments = -1 );
//! Draws a stroked circle centered around \a center with a radius of \a radius and a line width of \a lineWidth. Default \a numSegments requests a conservative (high-quality but slow) number based on radius.
void drawStrokedCircle( const vec2 &center, float radius, float lineWidth, int numSegments = -1 );
//! Draws a stroked ellipse centered around \a center with an X-axis radius of \a radiusX and a Y-axis radius of \a radiusY.  Default \a numSegments requests a conservative (high-quality but slow) number based on radius.
void drawStrokedEllipse( const vec2 &center, float radiusX, float radiusY, int numSegments = -1 );

//! Draws a string \a str with its lower left corner located at \a pos. Optional \a font and \a color affect the style.
void drawString( const std::string &str, const vec2 &pos, const ColorA &color = ColorA( 1, 1, 1, 1 ), Font font = Font() );
//! Draws a string \a str with the horizontal center of its baseline located at \a pos. Optional \a font and \a color affect the style
void drawStringCentered( const std::string &str, const vec2 &pos, const ColorA &color = ColorA( 1, 1, 1, 1 ), Font font = Font() );
//! Draws a right-justified string \a str with the center of its  located at \a pos. Optional \a font and \a color affect the style
void drawStringRight( const std::string &str, const vec2 &pos, const ColorA &color = ColorA( 1, 1, 1, 1 ), Font font = Font() );

//! Renders a solid triangle.
void drawSolidTriangle( const vec2 &pt0, const vec2 &pt1, const vec2 &pt2 );
//! Renders a textured triangle.
void drawSolidTriangle( const vec2 &pt0, const vec2 &pt1, const vec2 &pt2, const vec2 &texPt0, const vec2 &texPt1, const vec2 &texPt2 );
//! Renders a textured triangle.
void drawSolidTriangle( const vec2 pts[3], const vec2 texCoord[3] = nullptr );
	
	
void	drawArrays( GLenum mode, GLint first, GLsizei count );
void	drawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices );

#if defined( CINDER_GL_HAS_DRAW_INSTANCED )
void	drawArraysInstanced( GLenum mode, GLint first, GLsizei count, GLsizei instanceCount );
void	drawElementsInstanced( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei instanceCount );
#endif

} } // namespace cinder::gl

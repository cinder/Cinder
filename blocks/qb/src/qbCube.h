//
//  qbCube.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//
#pragma once

#include "Cinder/gl/gl.h"
#include "cinder/gl/DisplayList.h"
#include "cinder/gl/GlslProg.h"

namespace cinder { namespace qb {
	
	class qbSourceSelector;
	class qbPolyVertex;
	class qbPoly;


//
// Quads
#define QUAD_VERTEX						4
#define QUAD_INDEX						4
#define QUAD_BORDER_INDEX				5
	
//
// cube data as quads
#define QB_CUBE_VERTICES				8	// unique vertices
#define QB_QUAD_VERTICES				4	// vertex per quad/face
#define QB_CUBE_FACES					6
#define QB_CUBE_INDICES_QUAD			(QB_CUBE_FACES*QB_QUAD_VERTICES)

// Wireframe cubes
#define QB_WIRE_CUBE_STRIP_ELEMENTS		24
	
	// Faces de um cubo
	// Deve estar alinhado com cubeVertex[]
	typedef enum {
		FACE_NONE = -1,
		FACE_X_POS,
		FACE_Y_POS,
		FACE_Z_POS,
		FACE_X_NEG,
		FACE_Y_NEG,
		FACE_Z_NEG,
		CUBE_FACE_COUNT
	} enumCubeFaces;
	
	//
	// Public drawers
	//
	// Solid, textured Cubes
	void drawCube();
	void drawCube( const GLfloat * tex );
	void drawCube( const Vec3f & center, const Vec3f & size );
	void drawCube( const Vec3f & center, const Vec3f & size, const GLfloat * tex );
	void drawCube( const Vec3f & center, const Vec3f & size, const Vec3f & rot );
	//
	// Source-textured cubes
	void drawCube( qbSourceSelector & src );
	void drawCube( qbSourceSelector & src, const Vec3f & center, const Vec3f & size, const Vec3f & rot=Vec3f::zero() );
	//
	// Wireframe Cubes
	void drawStrokedCube();
	void drawStrokedCube( const Vec3f & center, const Vec3f & size );
	void drawStrokedCube( const Vec3f & center, const Vec3f & size, const Vec3f & rot );
	//
	// Quads -- corrected by Bilinear Interpolation on shader
	void drawQuad( const Vec2f & v0, const Vec2f & v1, const Vec2f & v2, const Vec2f & v3 );
	void drawQuad( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Vec3f & v3 );
	void drawQuad( const Vec2f & v0, const Vec2f & v1, const Vec2f & v2, const Vec2f & v3, const Vec2f & uv );
	void drawQuad( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Vec3f & v3, const Vec2f & uv );
	void drawQuad( const Vec2f & v0, const Vec2f & v1, const Vec2f & v2, const Vec2f & v3, gl::GlslProg & shader, const int unit=0, const Vec2f & uv=Vec2f::one() );
	void drawQuad( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Vec3f & v3, gl::GlslProg & shader, const int unit=0, const Vec2f & uv=Vec2f::one() );
	void drawQuad( const gl::Texture & tex, const Vec2f & v0, const Vec2f & v1, const Vec2f & v2, const Vec2f & v3, gl::GlslProg & shader, const int unit=0 );
	void drawQuad( const gl::Texture & tex, const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Vec3f & v3, gl::GlslProg & shader, const int unit=0 );
	void drawQuad( qbSourceSelector & src, const Vec2f & v0, const Vec2f & v1, const Vec2f & v2, const Vec2f & v3, gl::GlslProg & shader, const int unit=0 );
	void drawQuad( qbSourceSelector & src, const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Vec3f & v3, gl::GlslProg & shader, const int unit=0 );
	void drawStrokedQuad( const Vec2f & v0, const Vec2f & v1, const Vec2f & v2, const Vec2f & v3 );
	//
	// Tris
	void drawTri( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2 );
	void drawTri(qbSourceSelector & src, const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, gl::GlslProg & shader, const int unit=0 );
	void drawStrokedTri( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2 );
	//
	// Source-textured quads
	void draw( qbSourceSelector & src );
	void draw( qbSourceSelector & src, const Vec2f & p );
	void draw( qbSourceSelector & src, const Vec2f & p, const Vec2f & sz );
	void draw( qbSourceSelector & src, const Rectf & rect );
	void draw( qbSourceSelector & src, const Area & area, const Rectf & rect );
	void drawInfo( qbSourceSelector & src, const Vec2f & p=Vec2f::zero() );
	//
	// Polys
	void drawSolidPoly( const std::vector<qbPolyVertex> & vs, const Rectf & b, const Vec2f & uv=Vec2f(1,1) );
	void drawStrokedPoly( const std::vector<qbPolyVertex> & vs, bool closed );
	//
	// QB Polys
	void drawSolidPoly( qbPoly & poly );
	void drawSolidPoly( qbPoly & poly, gl::GlslProg & shader, const int unit=0 );
	void drawSolidPoly( qbSourceSelector & src, qbPoly & poly, const int unit=0 );
	void drawSolidPoly( qbSourceSelector & src, qbPoly & poly, gl::GlslProg & shader, const int unit=0 );
	void drawStrokedPoly( qbPoly & poly );
	void drawPolyPoints( qbPoly & poly );
	//
	// Points
	void drawPoint( const Vec3f & v );
	void drawPoints( const std::vector<qbPolyVertex> & vs );
	
	
	
	// Dome ROGER
	void drawSolidRect( const Rectf &rect, const Rectf &tex );
	void drawSolidRectDiv( const Rectf &rect, int divs);
	void drawSolidRectDiv( const Vec2f center, const Vec2f size, int divs);
	void drawStrokedRectDiv( const Rectf &rect, int divs );
	void drawStrokedRectDiv( const Vec2f center, const Vec2f size, int divs );
	void drawCubeDiv( const Vec3f center, const Vec3f size, int divs, bool stroked=false);
	void drawStrokedCubeDiv( const Vec3f center, const Vec3f size, int divs );
	
	///////////////////////////////////
	//
	// GLOBALS (externals)
	//
	// Quads
	extern const GLfloat __qbQuadVertices2D[QB_QUAD_VERTICES][2];
	extern const GLfloat __qbQuadVertices3D[QB_QUAD_VERTICES][3];
	extern const GLfloat __qbQuadTexes[QB_QUAD_VERTICES][2];
	extern const GLfloat __qbQuadNormals[QB_QUAD_VERTICES][3];
	extern const GLubyte __qbQuadIndices[QB_QUAD_VERTICES];
	// Cubes
	extern const GLfloat __qbCubeVertices[QB_CUBE_VERTICES][3];
	extern const GLfloat __qbCubeVerticesStrip[QB_CUBE_FACES][QB_QUAD_VERTICES][3];
	extern const GLfloat __qbCubeVerticesQuad[QB_CUBE_FACES][QB_QUAD_VERTICES][3];
	extern const GLfloat __qbCubeTexesStrip[QB_CUBE_FACES][QB_QUAD_VERTICES][2];
	extern const GLfloat __qbCubeTexesQuad[QB_CUBE_FACES][QB_QUAD_VERTICES][2];
	extern const GLfloat __qbCubeNormals[QB_CUBE_FACES][QB_QUAD_VERTICES][3];
	extern const GLubyte __qbCubeIndices[QB_CUBE_FACES][QB_QUAD_VERTICES];
		
} } // cinder::qb


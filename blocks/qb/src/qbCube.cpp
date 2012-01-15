//
//	qbCube.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//
#include "qbCube.h"
#include "qbSource.h"
#include "qbPoly.h"

using namespace ci::gl;

#define AUTO_ALPHA	true

namespace cinder { namespace qb {
	
	////////////////////////////////////////////////
	//
	// GLOBALS
	//
	
	//
	// TRIs
	const GLfloat __qbTriNormals[3][3] = {
		{0,0,1},	{0,0,1},	{0,0,1},
	};
	const GLubyte __qbTriIndices[3] = {
		0,  1,  2
	};
	
	//
	// Quads as GL_TRIANGLE_STRIP
	// 0 > 1
	//   /
	// 2 > 3
	const GLfloat __qbQuadNormals[QB_QUAD_VERTICES][3] = {
		{0,0,1},	{0,0,1},	{0,0,1},	{0,0,1}
	};
	extern const GLfloat __qbQuadTexes[QB_QUAD_VERTICES][2] = {
		{ 0,0 },	{ 1,0 },	{ 0,1 },	{ 1,1 }
		//{ 0.25,0.25 },	{ 0.75,0.25 },	{ 0.25,0.75 },	{ 0.75,0.75 }	// miolo
	};
	const GLubyte __qbQuadIndices[QB_QUAD_VERTICES] = {
		0,  1,  2,  3
	};

	//
	//           4----5
	//           | -y |
	// 5----4----0----1----5
	// | -z | -x | +z | +x |
	// 7----6----2----3----7
	//           | +y |
	//           6----7
	// Texes like quads
	// 6 faces/quads * 4 vertices = 24 vertices per cube
	const GLfloat __qbCubeVertices[QB_CUBE_VERTICES][3] = {
		{ -0.5f, -0.5f, +0.5f }, { +0.5f, -0.5f, +0.5f }, { -0.5f, +0.5f, +0.5f }, { +0.5f, +0.5f, +0.5f },	// +Z front
		{ -0.5f, -0.5f, -0.5f }, { +0.5f, -0.5f, -0.5f }, { -0.5f, +0.5f, -0.5f }, { +0.5f, +0.5f, -0.5f }	// +Z back
	};
#define cv(i)	{__qbCubeVertices[i][0],__qbCubeVertices[i][1],__qbCubeVertices[i][2]}
	const GLfloat __qbCubeVerticesStrip[QB_CUBE_FACES][QB_QUAD_VERTICES][3] = {
		{	cv(1),		cv(5),		cv(3),		cv(7) },	// +X right
		{	cv(3),		cv(2),		cv(6),		cv(7) },	// +Y bottom
		{	cv(0),		cv(1),		cv(2),		cv(3) },	// +Z front
		{	cv(4),		cv(0),		cv(6),		cv(2) },	// -X left
		{	cv(4),		cv(5),		cv(0),		cv(1) },	// -Y top
		{	cv(5),		cv(4),		cv(7),		cv(6) }		// -Z back
	};
	const GLfloat __qbCubeVerticesQuad[QB_CUBE_FACES][QB_QUAD_VERTICES][3] = {
		{	cv(1),		cv(5),		cv(7),		cv(3) },	// +X right
		{	cv(2),		cv(3),		cv(7),		cv(6) },	// +Y bottom
		{	cv(0),		cv(1),		cv(3),		cv(2) },	// +Z front
		{	cv(4),		cv(0),		cv(2),		cv(6) },	// -X left
		{	cv(4),		cv(5),		cv(1),		cv(0) },	// -Y top
		{	cv(5),		cv(4),		cv(6),		cv(7) }		// -Z back
	};
	const GLfloat __qbCubeTexesStrip[QB_CUBE_FACES][QB_QUAD_VERTICES][2] = {
		{	{ 0, 0 },	{ 1, 0 },	{ 0, 1 },	{ 1, 1 } },		// +X right
		{	{ 0, 0 },	{ 1, 0 },	{ 0, 1 },	{ 1, 1 } },		// +Y front
		{	{ 0, 0 },	{ 1, 0 },	{ 0, 1 },	{ 1, 1 } },		// +Z up
		{	{ 0, 0 },	{ 1, 0 },	{ 0, 1 },	{ 1, 1 } },		// +X right
		{	{ 0, 0 },	{ 1, 0 },	{ 0, 1 },	{ 1, 1 } },		// -Y back
		{	{ 0, 0 },	{ 1, 0 },	{ 0, 1 },	{ 1, 1 } }		// -Z down
	};
	const GLfloat __qbCubeTexesQuad[QB_CUBE_FACES][QB_QUAD_VERTICES][2] = {
		{	{ 0, 0 },	{ 1, 0 },	{ 1, 1 },	{ 0, 1 } },		// +X right
		{	{ 0, 0 },	{ 1, 0 },	{ 1, 1 },	{ 0, 1 } },		// +Y front
		{	{ 0, 0 },	{ 1, 0 },	{ 1, 1 },	{ 0, 1 } },		// +Z up
		{	{ 0, 0 },	{ 1, 0 },	{ 1, 1 },	{ 0, 1 } },		// +X right
		{	{ 0, 0 },	{ 1, 0 },	{ 1, 1 },	{ 0, 1 } },		// -Y back
		{	{ 0, 0 },	{ 1, 0 },	{ 1, 1 },	{ 0, 1 } }		// -Z down
	};
	const GLfloat __qbCubeNormals[QB_CUBE_FACES][QB_QUAD_VERTICES][3] = {
		{	{+1,0,0},	{+1,0,0},	{+1,0,0},	{+1,0,0} },		// +X right
		{	{0,+1,0},	{0,+1,0},	{0,+1,0},	{0,+1,0} },		// +Y front
		{	{0,0,+1},	{0,0,+1},	{0,0,+1},	{0,0,+1} },		// +Z up
		{	{-1,0,0},	{-1,0,0},	{-1,0,0},	{-1,0,0} },		// -X right
		{	{0,-1,0},	{0,-1,0},	{0,-1,0},	{0,-1,0} },		// -Y back
		{	{0,0,-1},	{0,0,-1},	{0,0,-1},	{0,0,-1} }		// -Z down
	};
	const GLubyte __qbCubeIndices[QB_CUBE_FACES][QB_QUAD_VERTICES] = {
		{  0,  1,  2,  3 },		// +X right
		{  4,  5,  6,  7 },		// +Y bottom
		{  8,  9, 10, 11 },		// +Z front
		{ 12, 13, 14, 15 },		// -X left
		{ 16, 17, 18, 19 },		// -Y top
		{ 20, 21, 22, 23 }		// -Z back
	};
	//
	// Wireframe cubes as GL_LINES
	const GLfloat __qbWireCubeVerticesLines[QB_WIRE_CUBE_STRIP_ELEMENTS][3] = {
		cv(5),	cv(7),	cv(4),	cv(6),	cv(0),	cv(2),	cv(1),	cv(3),
		cv(4),	cv(5),	cv(0),	cv(1),	cv(2),	cv(3),	cv(6),	cv(7),
		cv(4),	cv(0),	cv(5),	cv(1),	cv(2),	cv(6),	cv(3),	cv(7)
	};
	const GLubyte __qbWireCubeIndices[QB_WIRE_CUBE_STRIP_ELEMENTS] = {
		0,  1,  2,  3 ,	4,  5,  6,  7,	8,	9,	10,	11,
		12,	13,	14,	15,	16,	17,	18,	19,	20,	21,	22,	23
	};
	
	////////////////////////////////////////////////
	//
	// QB OPTIMIZED CUBES (private)
	//
	// Draw UNIT Cube with Arrays
	void __qbDrawCubeAsQuads( const GLfloat * vertices, const GLfloat * tex )
	{
		ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
		ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );
		ClientBoolState normalArrayState( GL_NORMAL_ARRAY );
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, vertices );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, 0, tex );
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, 0, __qbCubeNormals );
		glDrawElements( GL_QUADS, QB_CUBE_INDICES_QUAD, GL_UNSIGNED_BYTE, __qbCubeIndices );
		//glDrawArrays( GL_QUADS, 0, QB_CUBE_INDICES_QUAD` );		// Draw sequentially, no indices
		//for (int f = 0 ; f < QB_CUBE_FACES ; f++)			// Draw face by face
		//	glDrawElements( GL_QUADS, QB_QUAD_VERTICES, GL_UNSIGNED_BYTE, &(__qbCubeIndices[f]) );
	};
	//
	// Draw SIZED Cube with Arrays
	void __qbDrawCubeAsQuads( const Vec3f &c, const Vec3f &sz, const GLfloat * tex )
	{
		GLfloat vertices[QB_CUBE_FACES][QB_QUAD_VERTICES][3];
		for (int f = 0 ; f < QB_CUBE_FACES ; f++)
			for (int v = 0 ; v < QB_QUAD_VERTICES ; v++ )
				for (int i = 0 ; i < 3 ; i++ )
					vertices[f][v][i] = c[i]+(__qbCubeVerticesQuad[f][v][i]*sz[i]);
		__qbDrawCubeAsQuads( (const GLfloat *) vertices, tex );
	};
	void __qbDrawCubeAsQuads( const Vec3f &c, const Vec3f &sz )
	{
		__qbDrawCubeAsQuads( c, sz, (const GLfloat *) __qbCubeTexesQuad );
	}
	//
	// Draw UNIT Cube as Display Lists
	void __qbDrawUnitCubeAsQuads( const GLfloat * tex )
	{
		__qbDrawCubeAsQuads( (const GLfloat *) __qbCubeVerticesQuad, tex );
	}
	void __qbDrawUnitCubeAsQuads()
	{
		__qbDrawCubeAsQuads( (const GLfloat *) __qbCubeVerticesQuad, (const GLfloat *) __qbCubeTexesQuad );
	}
	void __qbDrawUnitCubeAsQuadsDL()
	{
		// Initialize once
		static gl::DisplayList dl;
		if (dl == NULL)
		{
			dl = gl::DisplayList( GL_COMPILE );
			dl.newList();
			__qbDrawUnitCubeAsQuads();
			dl.endList();
			//__qbCubeDisplayList.setMaterial( aMaterial );
		}
		dl.draw();
	};
	//
	// Draw wireframe cubes
	// Draw as 2 GL_QUAD_STRIPS
	void __qbDrawStrokedCube( const GLfloat * vertices )
	{
		ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, vertices );
		glDrawElements( GL_LINES, QB_WIRE_CUBE_STRIP_ELEMENTS, GL_UNSIGNED_BYTE, __qbWireCubeIndices );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	};
	void __qbDrawStrokedCube( const Vec3f &c, const Vec3f &sz )
	{
		GLfloat vertices[QB_WIRE_CUBE_STRIP_ELEMENTS][3];
		for (int v = 0 ; v < QB_WIRE_CUBE_STRIP_ELEMENTS ; v++ )
			for (int i = 0 ; i < 3 ; i++ )
				vertices[v][i] = c[i]+(__qbWireCubeVerticesLines[v][i]*sz[i]);
		__qbDrawStrokedCube( (const GLfloat * )vertices );
	};
	void __qbDrawStrokedUnitCube()
	{
		__qbDrawStrokedCube( (const GLfloat * )__qbWireCubeVerticesLines );
	};

	
	////////////////////////////////////////////////
	//
	// QB PUBLIC (mimic cinder::gl)
	//
	void drawCube()
	{
		__qbDrawUnitCubeAsQuadsDL();
	}
	void drawCube( const GLfloat * tex )
	{
		__qbDrawUnitCubeAsQuads( tex );
	}
	void drawCube( const Vec3f & center, const Vec3f & size )
	{
		//__qbDrawCubeImpl( center, size, false );	// Cinder original
		__qbDrawCubeAsQuads( center, size );
	}
	void drawCube( const Vec3f & center, const Vec3f & size, const GLfloat * tex )
	{
		//__qbDrawCubeImpl( center, size, false );	// Cinder original
		__qbDrawCubeAsQuads( center, size, tex );
	}
	void drawCube( const Vec3f & center, const Vec3f & size, const Vec3f & rot )
	{
		glPushMatrix();
		gl::translate(center);
		gl::scale(size);
		gl::rotate(rot);
		__qbDrawUnitCubeAsQuadsDL();
		glPopMatrix();
	}
	//
	// QB Source Cubes
	void drawCube( qbSourceSelector & src )
	{
		if (AUTO_ALPHA) src.enableAlphaBlending();	// enable if it has alpha
		src.bind();
		__qbDrawUnitCubeAsQuads( src.getTexes() );
		src.unbind();
		if (AUTO_ALPHA) src.disableAlphaBlending();	// enable if it has alpha
	}
	void drawCube( qbSourceSelector & src, const Vec3f & center, const Vec3f & size, const Vec3f & rot )
	{
		if (src.getTexture())
		{
			glPushMatrix();
			gl::translate(center);
			gl::scale(size);
			gl::rotate(rot);
			qb::drawCube( src );
			glPopMatrix();
		}
	}
	//
	// Stoked cube
	void drawStrokedCube()
	{
		__qbDrawStrokedUnitCube();
	}
	void drawStrokedCube( const Vec3f & center, const Vec3f & size )
	{
		__qbDrawStrokedCube( center, size );
	}
	void drawStrokedCube( const Vec3f & center, const Vec3f & size, const Vec3f & rot )
	{
		glPushMatrix();
		gl::translate(center);
		gl::scale(size);
		gl::rotate(rot);
		__qbDrawStrokedUnitCube();
		glPopMatrix();
	}
	
	////////////////////////////////////////////////////
	//
	// QUADS as GL_TRIANGLE_STRIP
	//
	// 0 > 1	(0,0) > (1,0)
	//   /			  /
	// 2 > 3	(0,1) > (1,1)
	/*void drawQuad( const Vec2f & v0, const Vec2f & v1, const Vec2f & v2, const Vec2f & v3 )
	{
		ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
		ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );
		ClientBoolState normalArrayState( GL_NORMAL_ARRAY );
		GLfloat vertices[QB_QUAD_VERTICES][2];
		vertices[0][0] = v0.x;	vertices[0][1] = v0.y;
		vertices[1][0] = v1.x;	vertices[1][1] = v1.y;
		vertices[2][0] = v2.x;	vertices[2][1] = v2.y;
		vertices[3][0] = v3.x;	vertices[3][1] = v3.y;
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, 0, __qbQuadNormals );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, 0, __qbQuadTexes );
		glEnableClientState( GL_VERTEX_ARRAY );	 
		glVertexPointer( 2, GL_FLOAT, 0, vertices );
		glDrawElements( GL_TRIANGLE_STRIP, QB_QUAD_VERTICES, GL_UNSIGNED_BYTE, __qbQuadIndices );
	}*/
	void __qbDrawQuad( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Vec3f & v3, const GLfloat * tex )
	{
		ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
		ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );
		ClientBoolState normalArrayState( GL_NORMAL_ARRAY );
		GLfloat vertices[QB_QUAD_VERTICES][3];
		vertices[0][0] = v0.x;	vertices[0][1] = v0.y;	vertices[0][2] = v0.z;
		vertices[1][0] = v1.x;	vertices[1][1] = v1.y;	vertices[1][2] = v1.z;
		vertices[2][0] = v2.x;	vertices[2][1] = v2.y;	vertices[2][2] = v2.z;
		vertices[3][0] = v3.x;	vertices[3][1] = v3.y;	vertices[3][2] = v3.z;
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, 0, __qbQuadNormals );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, 0, tex );
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, vertices );
		glDrawElements( GL_TRIANGLE_STRIP, QB_QUAD_VERTICES, GL_UNSIGNED_BYTE, __qbQuadIndices );
	}
	void drawStrokedQuad( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Vec3f & v3 )
	{
		ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
		GLfloat vertices[QB_QUAD_VERTICES][3];
		vertices[0][0] = v0.x;	vertices[0][1] = v0.y;	vertices[0][2] = v0.z;
		vertices[1][0] = v1.x;	vertices[1][1] = v1.y;	vertices[1][2] = v1.z;
		vertices[2][0] = v3.x;	vertices[2][1] = v3.y;	vertices[2][2] = v3.z;
		vertices[3][0] = v2.x;	vertices[3][1] = v2.y;	vertices[3][2] = v2.z;
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, vertices );
		glDrawElements( GL_LINE_LOOP, QB_QUAD_VERTICES, GL_UNSIGNED_BYTE, __qbQuadIndices );
	}
	void drawQuad( const Vec2f & v0, const Vec2f & v1, const Vec2f & v2, const Vec2f & v3 )
	{
		__qbDrawQuad( Vec3f(v0,0), Vec3f(v1,0), Vec3f(v2,0), Vec3f(v3,0), (const GLfloat *) __qbQuadTexes  );
	}
	void drawQuad( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Vec3f & v3 )
	{
		__qbDrawQuad( v0, v1, v2, v3, (const GLfloat *) __qbQuadTexes );
	}
	// Perspective transform by shader
	// shader must be binded!!
	void drawQuad( const Vec2f & v0, const Vec2f & v1, const Vec2f & v2, const Vec2f & v3, const Vec2f & uv )
	{
		qb::drawQuad( Vec3f(v0,0), Vec3f(v1,0), Vec3f(v2,0), Vec3f(v3,0), uv  );
	}
	void drawQuad( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Vec3f & v3, const Vec2f & uv )
	{
		GLfloat tex[QB_QUAD_VERTICES][2];
		for (int v = 0 ; v < QB_QUAD_VERTICES ; v++ )
		{
			tex[v][0] = __qbQuadTexes[v][0] * uv.x;
			tex[v][1] = __qbQuadTexes[v][1] * uv.y;
		}
		__qbDrawQuad( v0, v1, v2, v3, (const GLfloat *) tex );
	}
	void drawQuad( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Vec3f & v3, gl::GlslProg & shader, const int unit, const Vec2f & uv )
	{
		GLfloat tex[QB_QUAD_VERTICES][2];
		for (int v = 0 ; v < QB_QUAD_VERTICES ; v++ )
		{
			tex[v][0] = __qbQuadTexes[v][0] * uv.x;
			tex[v][1] = __qbQuadTexes[v][1] * uv.y;
		}
		bool bilinear = !(v0.x==v2.x && v1.x==v3.x && v0.y==v1.y && v2.y==v3.y);	// perfect quad
		shader.uniform( "u_flagBi", (bool) bilinear );
		shader.uniform( "u_tex0_rect", (int) unit );
		if (bilinear)
		{
			shader.uniform( "u_v0", v0 );
			shader.uniform( "u_v1", v1 );
			shader.uniform( "u_v2", v2 );
			shader.uniform( "u_v3", v3 );
			shader.uniform( "u_t0", Vec2f( tex[0][0], tex[0][1] ) );
			shader.uniform( "u_t1", Vec2f( tex[1][0], tex[1][1] ) );
			shader.uniform( "u_t2", Vec2f( tex[2][0], tex[2][1] ) );
			shader.uniform( "u_t3", Vec2f( tex[3][0], tex[3][1] ) );
		}
		__qbDrawQuad( v0, v1, v2, v3, (const GLfloat *) tex );
	}
	void drawQuad( const Vec2f & v0, const Vec2f & v1, const Vec2f & v2, const Vec2f & v3, gl::GlslProg & shader, const int unit, const Vec2f & uv )
	{
		qb::drawQuad( Vec3f(v0,0), Vec3f(v1,0), Vec3f(v2,0), Vec3f(v3,0), shader, unit, uv );
	}
	//
	// Textured quads
	void drawQuad( const gl::Texture & tex, const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Vec3f & v3, gl::GlslProg & shader, const int unit )
	{
		if ( tex )
		{
			if (AUTO_ALPHA && tex.hasAlpha()) gl::enableAlphaBlending();
			tex.enableAndBind( unit );
			qb::drawQuad( v0, v1, v2, v3, shader, unit, Vec2f(tex.getMaxU(),tex.getMaxV()) );
			tex.unbind();
			if (AUTO_ALPHA && tex.hasAlpha()) gl::disableAlphaBlending();
		}
	}
	void drawQuad( const gl::Texture & tex, const Vec2f & v0, const Vec2f & v1, const Vec2f & v2, const Vec2f & v3, gl::GlslProg & shader, const int unit )
	{
		qb::drawQuad( tex, Vec3f(v0,0), Vec3f(v1,0), Vec3f(v2,0), Vec3f(v3,0), shader, unit );
	}
	//
	// Source quads
	void drawQuad( qbSourceSelector & src, const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Vec3f & v3, gl::GlslProg & shader, const int unit )
	{
		if (src.getTexture())
		{
			if (AUTO_ALPHA)	src.enableAlphaBlending();
			src.bind( unit );
			qb::drawQuad( v0, v1, v2, v3, shader, unit, src.getUV() );
			src.unbind();
			if (AUTO_ALPHA)	src.disableAlphaBlending();
		}
	}
	void drawQuad( qbSourceSelector & src, const Vec2f & v0, const Vec2f & v1, const Vec2f & v2, const Vec2f & v3, gl::GlslProg & shader, const int unit )
	{
		qb::drawQuad( src, Vec3f(v0,0), Vec3f(v1,0), Vec3f(v2,0), Vec3f(v3,0), shader, unit );
	}
	//
	// Source-textured quads
	// Replicate gl::Texture to qb::qbSource
	void draw( qbSourceSelector & src )
	{
		if (src.getTexture())
			qb::draw( src, 
					 Area(src.getTexture().getCleanBounds()), 
					 src.getTexture().getCleanBounds() );
	}
	void draw( qbSourceSelector & src, const Vec2f & p )
	{
		
		if (src.getTexture())
			qb::draw( src, 
					 Area(src.getTexture().getCleanBounds()), 
					 Rectf( p.x, p.y, p.x + src.getWidth(), p.y + src.getHeight() ) );
	}
	void draw( qbSourceSelector & src, const Vec2f & p, const Vec2f & sz )
	{
		if (src.getTexture())
			qb::draw( src, 
					 Area(src.getTexture().getCleanBounds()), 
					 Rectf( p.x, p.y, p.x + sz.x, p.y + sz.y ) );
	}
	void draw( qbSourceSelector & src, const Rectf & rect )
	{
		if (src.getTexture())
			qb::draw( src, 
					 Area(src.getTexture().getCleanBounds()), 
					 rect );
	}
	void draw( qbSourceSelector & src, const Area & area, const Rectf & rect )
	{
		if (src.getTexture())
		{
			if (AUTO_ALPHA)	src.enableAlphaBlending();
			src.bind();
			gl::draw( src.getTexture(), area, rect );
			src.unbind();
			if (AUTO_ALPHA)	src.disableAlphaBlending();
		}
	}
	void drawInfo( qbSourceSelector & src, const Vec2f & p )
	{
		if (src.getTexture())
			src.drawInfo( p );
	}
	
	
	
	
	////////////////////////////////////////////////////
	//
	// TRIs
	//
	void drawStrokedTri( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2 )
	{
		ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
		GLfloat vertices[3][3];
		vertices[0][0] = v0.x;	vertices[0][1] = v0.y;	vertices[0][2] = v0.z;
		vertices[1][0] = v1.x;	vertices[1][1] = v1.y;	vertices[1][2] = v1.z;
		vertices[2][0] = v2.x;	vertices[2][1] = v2.y;	vertices[2][2] = v2.z;
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, vertices );
		glDrawElements( GL_LINE_LOOP, 3, GL_UNSIGNED_BYTE, __qbTriIndices );
	}
	void __qbDrawTri( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const GLfloat * tex )
	{
		ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
		ClientBoolState normalArrayState( GL_NORMAL_ARRAY );
		ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );
		GLfloat vertices[3][3];
		vertices[0][0] = v0.x;	vertices[0][1] = v0.y;	vertices[0][2] = v0.z;
		vertices[1][0] = v1.x;	vertices[1][1] = v1.y;	vertices[1][2] = v1.z;
		vertices[2][0] = v2.x;	vertices[2][1] = v2.y;	vertices[2][2] = v2.z;
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, 0, __qbTriNormals );
		if (tex) glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		if (tex) glTexCoordPointer( 2, GL_FLOAT, 0, tex );
		glEnableClientState( GL_VERTEX_ARRAY );	 
		glVertexPointer( 3, GL_FLOAT, 0, vertices );
		glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, __qbTriIndices );
	}
	void __qbDrawTri( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2, const Rectf & b, const Vec2f & uv )
	{
		GLfloat tex[3][2];
		tex[0][0] = lmap( v0.x, b.x1, b.x2, 0.0f, uv.x );
		tex[0][1] = lmap( v0.y, b.y1, b.y2, 0.0f, uv.y );
		tex[1][0] = lmap( v1.x, b.x1, b.x2, 0.0f, uv.x );
		tex[1][1] = lmap( v1.y, b.y1, b.y2, 0.0f, uv.y );
		tex[2][0] = lmap( v2.x, b.x1, b.x2, 0.0f, uv.x );
		tex[2][1] = lmap( v2.y, b.y1, b.y2, 0.0f, uv.y );
		__qbDrawTri( v0, v1, v2, (const GLfloat *) tex );
	}
	//
	//  PUBLIC
	void drawTri( const Vec3f & v0, const Vec3f & v1, const Vec3f & v2 )
	{
		__qbDrawTri( v0, v1, v2, NULL );
	}
	
	
	////////////////////////////////////////////////////
	//
	// POLYS
	//
	void drawSolidPoly( const std::vector<qbPolyVertex> & vs, const Rectf & b, const Vec2f & uv )
	{
		glBegin(GL_POLYGON);
		for (int v = 0 ; v < vs.size() ; v++)
			glVertex3f( vs[v].x, vs[v].y, vs[v].z );
		glEnd();
		// TODO::Usar isto, que nao funciona nao sei porque...
		/*
		ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
		ClientBoolState normalArrayState( GL_NORMAL_ARRAY );
		ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );
		GLfloat vertices[vs.size()][3];
		GLfloat tex[vs.size()][2];
		GLfloat normals[vs.size()][3];
		GLfloat indices[vs.size()];
		for (int n = 0 ; n < vs.size() ; n++)
		{
			vertices[n][0] = vs[n].x;
			vertices[n][1] = vs[n].y;
			vertices[n][2] = vs[n].z;
			tex[n][0] = lmap( vs[n].x, b.x1, b.x2, 0.0f, uv.x );
			tex[n][1] = lmap( vs[n].y, b.y1, b.y2, 0.0f, uv.y );
			normals[n][0] = 1;
			normals[n][1] = 0;
			normals[n][2] = 1;
			indices[n] = n;
		}
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, 0, normals );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, 0, tex );
		glEnableClientState( GL_VERTEX_ARRAY );	 
		glVertexPointer( 3, GL_FLOAT, 0, vertices );
		glDrawElements( GL_POLYGON, vs.size(), GL_UNSIGNED_BYTE, indices );
		*/
	}
	void drawStrokedPoly( const std::vector<qbPolyVertex> & vs, bool closed )
	{
		glBegin( closed ? GL_LINE_LOOP : GL_LINE_STRIP );
		for (int v = 0 ; v < vs.size() ; v++)
			glVertex3f( vs[v].x, vs[v].y, vs[v].z );
		glEnd();
		// TODO::Usar isto, que nao funciona nao sei porque...
		/*
		ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
		GLfloat vertices[vs.size()][3];
		GLfloat indices[vs.size()];
		for (int n = 0 ; n < vs.size() ; n++)
		{
			vertices[n][0] = vs[n].x;
			vertices[n][1] = vs[n].y;
			vertices[n][2] = vs[n].z;
			indices[n] = n;
		}
		glEnableClientState( GL_VERTEX_ARRAY );	 
		glVertexPointer( 3, GL_FLOAT, 0, vertices );
		glDrawElements( GL_LINE_LOOP, vs.size(), GL_UNSIGNED_BYTE, indices );
		*/
	}
	
	
	
	////////////////////////////////////////////////////
	//
	// QB POLYS
	// 
	void drawSolidPoly( qbPoly & poly )
	{
		switch ( poly.getType() )
		{
			case QBPOLY_POINT:
				qb::drawPoints( poly.getVertices() );
				break;
			case QBPOLY_POLYLINE:
				qb::drawStrokedPoly( poly.getVertices(), false );
				break;
			case QBPOLY_POLYGON:
				qb::drawSolidPoly( poly.getVertices(), poly.getBounds(), Vec2f(1,1) );
				break;
			case QBPOLY_TRI:
				__qbDrawTri( poly.getVertex(0), poly.getVertex(1), poly.getVertex(2), poly.getBounds(), Vec2f(1,1) );
				break;
			case QBPOLY_QUAD:
				qb::drawQuad( poly.getVertex(0), poly.getVertex(1), poly.getVertex(2), poly.getVertex(3) );
				break;
			case QBPOLY_RECT:
			case QBPOLY_SQUARE:
				gl::drawSolidRect( poly.getBounds() );
				break;
		}
	}
	void drawSolidPoly( qbPoly & poly, gl::GlslProg & shader )
	{
		switch ( poly.getType() )
		{
			case QBPOLY_POINT:
				qb::drawPoints( poly.getVertices() );
				break;
			case QBPOLY_POLYLINE:
				qb::drawStrokedPoly( poly.getVertices(), false );
				break;
			case QBPOLY_TRI:
				shader.uniform( "u_flagBi", (bool) false );
				__qbDrawTri( poly.getVertex(0), poly.getVertex(1), poly.getVertex(2), poly.getBounds(), Vec2f(1,1) );
				break; 
			case QBPOLY_QUAD:
				qb::drawQuad( poly.getVertex(0), poly.getVertex(1), poly.getVertex(2), poly.getVertex(3), shader );
				break;
			default:
				shader.uniform( "u_flagBi", (bool) false );
				qb::drawSolidPoly( poly );
				break;
		}
	}
	void drawSolidPoly( qbSourceSelector & src, qbPoly & poly, const int unit )
	{
		if (src.getTexture())
		{
			if (AUTO_ALPHA)	src.enableAlphaBlending();
			src.bind( unit );

			switch ( poly.getType() )
			{
				case QBPOLY_POINT:
					qb::drawPoints( poly.getVertices() );
					break;
				case QBPOLY_POLYLINE:
					qb::drawStrokedPoly( poly.getVertices(), false );
					break;
				case QBPOLY_TRI:
					__qbDrawTri( poly.getVertex(0), poly.getVertex(1), poly.getVertex(2), poly.getBounds(), src.getUV() );
					break;
				case QBPOLY_QUAD:
					qb::drawQuad( poly.getVertex(0), poly.getVertex(1), poly.getVertex(2), poly.getVertex(3), src.getUV() );
					break;
				default:
					qb::drawSolidPoly( poly );
					break;
			}
			
			src.unbind();
			if (AUTO_ALPHA)	src.disableAlphaBlending();
		}
	}
	void drawSolidPoly( qbSourceSelector & src, qbPoly & poly, gl::GlslProg & shader, const int unit )
	{
		if (src.getTexture())
		{
			if (AUTO_ALPHA)	src.enableAlphaBlending();
			src.bind( unit );
			
			switch ( poly.getType() )
			{
				case QBPOLY_POINT:
					qb::drawPoints( poly.getVertices() );
					break;
				case QBPOLY_POLYLINE:
					qb::drawStrokedPoly( poly.getVertices(), false );
					break;
				case QBPOLY_TRI:
					shader.uniform( "u_flagBi", (bool) false );
					__qbDrawTri( poly.getVertex(0), poly.getVertex(1), poly.getVertex(2), poly.getBounds(), src.getUV() );
					break;
				case QBPOLY_QUAD:
					qb::drawQuad( poly.getVertex(0), poly.getVertex(1), poly.getVertex(2), poly.getVertex(3), shader, unit, src.getUV() );
					break;
				default:
					shader.uniform( "u_flagBi", (bool) false );
					qb::drawSolidPoly( poly );
					break;
			}
			
			src.unbind();
			if (AUTO_ALPHA)	src.disableAlphaBlending();
		}
	}
	//
	// Wireframe
	void drawStrokedPoly( qbPoly & poly )
	{
		switch ( poly.getType() )
		{
			case QBPOLY_POINT:
				qb::drawPoints( poly.getVertices() );
				break;
			case QBPOLY_TRI:
				qb::drawStrokedTri( poly.getVertex(0), poly.getVertex(1), poly.getVertex(2) );
				break;
			case QBPOLY_QUAD:
				qb::drawStrokedQuad( poly.getVertex(0), poly.getVertex(1), poly.getVertex(2), poly.getVertex(3) );
				break;
			case QBPOLY_RECT:
			case QBPOLY_SQUARE:
				gl::drawStrokedRect( poly.getBounds() );
				break;
			case QBPOLY_POLYLINE:
			case QBPOLY_POLYGON:
				qb::drawStrokedPoly( poly.getVertices(), poly.isClosed() );
				break;
		}
	}
	//
	// Poly Points
	void drawPolyPoints( qbPoly & poly )
	{
		qb::drawPoints( poly.getVertices() );
	}
	
	
	////////////////////////////////////////////////////
	//
	// POINTS
	// 
	void drawPoint( const Vec3f & v )
	{
		glBegin( GL_POINTS );
		glVertex3f( v.x, v.y, v.z );
		glEnd();
	}
	void drawPoints( const std::vector<qbPolyVertex> & vs )
	{
		glBegin( GL_POINTS );
		for (int v = 0 ; v < vs.size() ; v++)
			if ( ! vs[v].mDup )		// unique vertexes
				glVertex3f( vs[v].x, vs[v].y, vs[v].z );
		glEnd();
		// TODO::Usar isto, que nao funciona nao sei porque...
		/*
		 ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
		 GLfloat vertices[vs.size()][3];
		 GLfloat indices[vs.size()];
		 for (int n = 0 ; n < vs.size() ; n++)
		 {
		 vertices[n][0] = vs[n].x;
		 vertices[n][1] = vs[n].y;
		 vertices[n][2] = vs[n].z;
		 indices[n] = n;
		 }
		 glEnableClientState( GL_VERTEX_ARRAY );	 
		 glVertexPointer( 3, GL_FLOAT, 0, vertices );
		 glDrawElements( GL_POINTS, vs.size(), GL_UNSIGNED_BYTE, indices );
		 */
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	///////////////////////////////
	//
	// ROGER DOME
	//
	// TODO: REMAKE THIS!!!
	//
	void drawSolidRect( const Rectf &rect, const Rectf &tex )
	{
		glEnableClientState( GL_VERTEX_ARRAY );
		GLfloat verts[8];
		glVertexPointer( 2, GL_FLOAT, 0, verts );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		GLfloat texCoords[8];
		glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
		verts[0*2+0] = rect.getX2(); texCoords[0*2+0] = tex.getX2();
		verts[0*2+1] = rect.getY1(); texCoords[0*2+1] = tex.getY1();
		verts[1*2+0] = rect.getX1(); texCoords[1*2+0] = tex.getX1();
		verts[1*2+1] = rect.getY1(); texCoords[1*2+1] = tex.getY1();
		verts[2*2+0] = rect.getX2(); texCoords[2*2+0] = tex.getX2();
		verts[2*2+1] = rect.getY2(); texCoords[2*2+1] = tex.getY2();
		verts[3*2+0] = rect.getX1(); texCoords[3*2+0] = tex.getX1();
		verts[3*2+1] = rect.getY2(); texCoords[3*2+1] = tex.getY2();
		
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
	}
	
	//
	// Draw Subdivided quad on X/Y axis
	void drawSolidRectDiv( const Rectf &rect, int divs)
	{
	}
	void drawSolidRectDiv( const Vec2f center, const Vec2f size, int divs)
	{
		Vec2f origin = Vec2f( center.x-(size.x/2), center.y-(size.y/2) );
		Vec2f delta  = size / (float)divs;
		Rectf rect, tex;
		for (int y = 0 ; y < divs ; y++)
		{
			rect.y1 = origin.y + (y * delta.y);
			rect.y2 = rect.y1 + delta.y;
			tex.y1 =     y / (float)divs;
			tex.y2 = (y+1) / (float)divs;
			for (int x = 0 ; x < divs ; x++)
			{
				rect.x1 = origin.x + (x * delta.x);
				rect.x2 = rect.x1 + delta.x;
				tex.x1 =     x / (float)divs;
				tex.x2 = (x+1) / (float)divs;
				qb::drawSolidRect( rect, tex );
			}
		}
	}
	
	//
	// Draw Subdivided quad on X/Y axis
	void drawStrokedRectDiv( const Rectf &rect, int divs )
	{
		Vec2f size = Vec2f( (rect.x2-rect.x1), (rect.y2-rect.y1) );
		drawStrokedRectDiv( Vec2f(rect.x1, rect.y1) + size/2, size, divs );
	}
	void drawStrokedRectDiv( const Vec2f center, const Vec2f size, int divs )
	{
		Vec2f origin = Vec2f( center.x-(size.x/2), center.y-(size.y/2) );
		Vec2f delta  = size / (float)divs;
		float xx, yy;
		for (int x = 0 ; x <= divs ; x++)
		{
			xx = origin.x + (x * delta.x);
			glBegin( GL_LINE_STRIP );
			for (int y = 0 ; y <= divs ; y++)
			{
				yy = origin.y + (y * delta.y);
				glVertex2f(xx, yy);
			}
			glEnd();
		}
		for (int y = 0 ; y <= divs ; y++)
		{
			yy = origin.y + (y * delta.y);
			glBegin( GL_LINE_STRIP );
			for (int x = 0 ; x <= divs ; x++)
			{
				xx = origin.x + (x * delta.x);
				glVertex2f(xx, yy);
			}
			glEnd();
		}
	}
	
	/* JA MUDEI ISSO PRA ORDEM DO QB !!!
	 typedef enum {
		FACE_X_POS = 0,
		FACE_X_NEG,
		FACE_Y_POS,
		FACE_Y_NEG,
		FACE_Z_POS,
		FACE_Z_NEG
	} enumCubeFaces;
	 */
	const Vec3f cubeFaceCenter[] = {
		Vec3f(  0.5,  0,  0 ),
		Vec3f(  0,  0.5,  0 ),
		Vec3f(  0,  0,  0.5 ),
		Vec3f( -0.5,  0,  0 ),
		Vec3f(  0, -0.5,  0 ),
		Vec3f(  0,  0, -0.5 )
	};
	const Vec3f cubeFaceRot[] = {
		Vec3f( -90,  90,   0 ),
		Vec3f( -90,   0,   0 ),
		Vec3f(   0,   0,   0 ),
		Vec3f( -90, -90,   0 ),
		Vec3f( -90, 180,   0 ),
		Vec3f(   0,   0, 180 ),
	};
	// Draw cube with Subdivided quads
	void drawStrokedCubeDiv( Vec3f center, Vec3f size, int divs )
	{
		drawCubeDiv( center, size, divs, true);
	}
	void drawCubeDiv( Vec3f center, Vec3f size, int divs, bool stroked)
	{
		Vec3f c;
		Vec2f sz;
		for (int n = 0 ; n < 6 ; n++)
		{
			c = center;
			if (n == FACE_X_POS || n == FACE_X_NEG )
			{
				c.x += (size.x-1) / 2.0 * ( n == FACE_X_POS ? 1.0 : -1.0 );
				sz = size.yz();
			}
			if (n == FACE_Y_POS || n == FACE_Y_NEG )
			{
				c.y += (size.y-1) / 2.0 * ( n == FACE_Y_POS ? 1.0 : -1.0 );
				sz = size.xz();
			}
			if (n == FACE_Z_POS || n == FACE_Z_NEG )
			{
				c.z += (size.z-1) / 2.0 * ( n == FACE_Z_POS ? 1.0 : -1.0 );
				sz = size.xy();
			}
			glPushMatrix();
			glNormal3f(0,0,-1);
			gl::translate( c + cubeFaceCenter[n] );
			if (cubeFaceRot[n].x) glRotatef( cubeFaceRot[n].x, 1, 0, 0);
			if (cubeFaceRot[n].y) glRotatef( cubeFaceRot[n].y, 0, 1, 0);
			if (cubeFaceRot[n].z) glRotatef( cubeFaceRot[n].z, 0, 0, 1);
			if (stroked)
				qb::drawStrokedRectDiv( Vec2f::zero(), sz, divs );
			else
				qb::drawSolidRectDiv( Vec2f::zero(), sz, divs);
			glPopMatrix();
		}
	}



} } // cinder::qb
	

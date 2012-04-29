	// Globals
	//const gl::VboMesh __qbCubeMesh;
	gl::DisplayList __qbCubeDisplayLst;
	
	
	// Uptimied cube vertexes (8)
	// up: v1 v3  down: v5 v7
	//     v0 v2        v4 v6
	const GLubyte __qbCubeIndicesOtimized[QB_CUBE_FACES][QB_CUBE_FACE_VERTICES] = {
		{ 6, 2, 7, 3 },			// +X right
		{ 7, 3, 5, 1 },			// +Y front
		{ 0, 1, 2, 3 },			// +Z up
		{ 5, 1, 4, 0 },			// -X left
		{ 4, 0, 6, 2 },			// -Y back
		{ 4, 5, 6, 7 }			// -Z down
	};
	const GLfloat __qbCubeVerticesOtimized[QB_CUBE_VERTICES][3] = {
		{ -0.5f, -0.5f,  0.5f },	// up
		{ -0.5f,  0.5f,  0.5f },	//
		{  0.5f, -0.5f,  0.5f },	//
		{  0.5f,  0.5f,  0.5f },	//
		{ -0.5f, -0.5f, -0.5f },	// down
		{ -0.5f,  0.5f, -0.5f },	//
		{  0.5f, -0.5f, -0.5f },	//
		{  0.5f,  0.5f, -0.5f }		//
	};
	//
	// FUll cube, 6 faces * 4 vertices = 24
	const GLubyte __qbCubeIndices[QB_CUBE_FACES][QB_CUBE_FACE_VERTICES] = {
		{  0,  1,  2,  3 },		// +X right
		{  4,  5,  6,  7 },		// +Y front
		{  8,  9, 10, 11 },		// +Z up
		{ 12, 13, 14, 15 },		// -X left
		{ 16, 17, 18, 19 },		// -Y back
		{ 20, 21, 22, 23 }		// -Z down
	};
	const GLfloat __qbCubeVertices[QB_CUBE_FACES][QB_CUBE_FACE_VERTICES][3] = {
		{ {  0.5f, -0.5f, -0.5f }, {  0.5f, -0.5f,  0.5f }, {  0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f,  0.5f } },			// +X right
		{ {  0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f, -0.5f }, { -0.5f,  0.5f,  0.5f } },			// +Y front
		{ { -0.5f, -0.5f,  0.5f }, { -0.5f,  0.5f,  0.5f }, {  0.5f, -0.5f,  0.5f }, {  0.5f,  0.5f,  0.5f } },			// +Z up
		{ { -0.5f,  0.5f, -0.5f }, { -0.5f,  0.5f,  0.5f }, { -0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f,  0.5f } },			// -X left
		{ { -0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f,  0.5f }, {  0.5f, -0.5f, -0.5f }, {  0.5f, -0.5f,  0.5f } },			// -Y back
		{ { -0.5f, -0.5f, -0.5f }, { -0.5f,  0.5f, -0.5f }, {  0.5f, -0.5f, -0.5f }, {  0.5f,  0.5f, -0.5f } }			// -Z down
	};
	const GLfloat __qbCubeVerticesQuads[QB_CUBE_FACES][QB_CUBE_FACE_VERTICES][3] = {
		{ {  0.5f, -0.5f, -0.5f }, {  0.5f, -0.5f,  0.5f }, {  0.5f,  0.5f,  0.5f }, {  0.5f,  0.5f, -0.5f } },			// +X right
		{ {  0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f, -0.5f } },			// +Y front
		{ { -0.5f, -0.5f,  0.5f }, { -0.5f,  0.5f,  0.5f }, {  0.5f,  0.5f,  0.5f }, {  0.5f, -0.5f,  0.5f } },			// +Z up
		{ { -0.5f,  0.5f, -0.5f }, { -0.5f,  0.5f,  0.5f }, { -0.5f, -0.5f,  0.5f }, { -0.5f, -0.5f, -0.5f } },			// -X left
		{ { -0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f,  0.5f }, {  0.5f, -0.5f,  0.5f }, {  0.5f, -0.5f, -0.5f } },			// -Y back
		{ { -0.5f, -0.5f, -0.5f }, { -0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f, -0.5f }, {  0.5f, -0.5f, -0.5f } }			// -Z down
	};
	const GLfloat __qbCubeNormals[QB_CUBE_FACES][QB_CUBE_FACE_VERTICES][3] = {
		{  {1,0,0},  {1,0,0},  {1,0,0},  {1,0,0} },		// +X
		{  {0,1,0},  {0,1,0},  {0,1,0},  {0,1,0} },		// +Y
		{  {0,0,1},  {0,0,1},  {0,0,1},  {0,0,1} },		// +Z
		{ {-1,0,0}, {-1,0,0}, {-1,0,0}, {-1,0,0} },		// -X
		{ {0,-1,0}, {0,-1,0}, {0,-1,0}, {0,-1,0} },		// -Y
		{ {0,0,-1}, {0,0,-1}, {0,0,-1}, {0,0,-1} }		// -Z
	};
	const GLfloat __qbCubeTexes[QB_CUBE_FACES][QB_CUBE_FACE_VERTICES][2] = {
		{ { 0,1 },	{ 1,1 },	{ 1,0 },	{ 0,0 } },
		{ { 1,1 },	{ 1,0 },	{ 0,0 },	{ 0,1 } },
		{ { 0,1 },	{ 1,1 },	{ 1,0 },	{ 0,0 } },
		{ { 1,1 },	{ 1,0 },	{ 0,0 },	{ 0,1 } },
		{ { 1,0 },	{ 0,0 },	{ 0,1 },	{ 1,1 } },
		{ { 1,0 },	{ 0,0 },	{ 0,1 },	{ 1,1 } }
	};
	const GLfloat __qbCubeTexesQuads[QB_CUBE_FACES][QB_CUBE_FACE_VERTICES][2] = {
		{ { 0,1 },	{ 1,1 },	{ 0,0 },	{ 1,0 } },
		{ { 1,1 },	{ 1,0 },	{ 0,1 },	{ 0,0 } },
		{ { 0,1 },	{ 1,1 },	{ 0,0 },	{ 1,0 } },
		{ { 1,1 },	{ 1,0 },	{ 0,1 },	{ 0,0 } },
		{ { 1,0 },	{ 0,0 },	{ 1,1 },	{ 0,1 } },
		{ { 1,0 },	{ 0,0 },	{ 1,1 },	{ 0,1 } }
	};
	/*const float __qbCubeColors[QB_CUBE_FACES][QB_CUBE_FACE_VERTICES][3] = {
		{ 1, 0, 0 },		// +X = red
		{ 0, 1, 0 },		// +Y = green
		{ 0, 0, 1 },		// +Z = blue
		{ 0, 1, 1 },		// -X = cyan
		{ 1, 0, 1 },		// -Y = purple
		{ 1, 1, 0 }			// -Z = yellow
	};*/
	
	/////////////////////////////
	//
	// DIFFENTENT CUBE RENDERING
	//
	//
	// Direct Draw
	void qbDrawCubeDirectDL()
	{
		if (__qbCubeDisplayLst == NULL)
		{
			__qbCubeDisplayLst = gl::DisplayList( GL_COMPILE );
			__qbCubeDisplayLst.newList();
			qbDrawCubeDirect();
			__qbCubeDisplayLst.endList();
			//__qbCubeDisplayLst.setMaterial( aMaterial );
		}
		__qbCubeDisplayLst.draw();
	};
	void qbDrawCubeDirect()
	{
		for (int f = 0 ; f < QB_CUBE_FACES ; f++ )
		{
			glBegin( GL_TRIANGLE_STRIP );
			for (int v = 0 ; v < QB_CUBE_FACE_VERTICES ; v++ )
			{
				int i = __qbCubeIndicesOtimized[f][v];
				glTexCoord2d( __qbCubeTexes[f][v][0], __qbCubeTexes[f][v][1] );
				glNormal3f( __qbCubeNormals[f][v][0], __qbCubeNormals[f][v][1], __qbCubeNormals[f][v][2] );
				glVertex3f( __qbCubeVerticesOtimized[i][0], __qbCubeVerticesOtimized[i][1], __qbCubeVerticesOtimized[i][2] );
			}
			glEnd();
		}
	};
	void qbDrawCubeDirect( const Vec3f &c, const Vec3f &sz )
	{
		for (int f = 0 ; f < QB_CUBE_FACES ; f++ )
		{
			glBegin( GL_TRIANGLE_STRIP );
			for (int v = 0 ; v < QB_CUBE_FACE_VERTICES ; v++ )
			{
				int i = __qbCubeIndicesOtimized[f][v];
				glTexCoord2d( __qbCubeTexes[f][v][0], __qbCubeTexes[f][v][1] );
				glNormal3f( __qbCubeNormals[f][v][0], __qbCubeNormals[f][v][1], __qbCubeNormals[f][v][2] );
				glVertex3f( c.x+__qbCubeVerticesOtimized[i][0]*sz.x, c.y+__qbCubeVerticesOtimized[i][1]*sz.y, c.z+__qbCubeVerticesOtimized[i][2]*sz.z );
			}
			glEnd();
		}
	};
	
	//
	// Arrays
	void qbDrawCubeArraysDL()
	{
		if (__qbCubeDisplayLst == NULL)
		{
			__qbCubeDisplayLst = gl::DisplayList( GL_COMPILE );
			__qbCubeDisplayLst.newList();
			qbDrawCubeArrays();
			__qbCubeDisplayLst.endList();
			//__qbCubeDisplayLst.setMaterial( aMaterial );
		}
		__qbCubeDisplayLst.draw();
	};
	void qbDrawCubeArraysDLQuads()
	{
		if (__qbCubeDisplayLst == NULL)
		{
			__qbCubeDisplayLst = gl::DisplayList( GL_COMPILE );
			__qbCubeDisplayLst.newList();
			qbDrawCubeArraysQuads();
			__qbCubeDisplayLst.endList();
			//__qbCubeDisplayLst.setMaterial( aMaterial );
		}
		__qbCubeDisplayLst.draw();
	};
	void qbDrawCubeArrays()
	{
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, 0, __qbCubeNormals );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, 0, __qbCubeTexes );
		glEnableClientState( GL_VERTEX_ARRAY );	 
		glVertexPointer( 3, GL_FLOAT, 0, __qbCubeVertices );
		for (int f = 0 ; f < QB_CUBE_FACES ; f++)
			glDrawElements( GL_TRIANGLE_STRIP, QB_CUBE_FACE_VERTICES, GL_UNSIGNED_BYTE, &(__qbCubeIndices[f]) );
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	 
		glDisableClientState( GL_NORMAL_ARRAY );
	};
	void qbDrawCubeArraysQuads()
	{
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, 0, __qbCubeNormals );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, 0, __qbCubeTexesQuads );
		glEnableClientState( GL_VERTEX_ARRAY );	 
		glVertexPointer( 3, GL_FLOAT, 0, __qbCubeVerticesQuads );
		glDrawElements( GL_QUADS, QB_CUBE_INDICES, GL_UNSIGNED_BYTE, __qbCubeIndices );
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	 
		glDisableClientState( GL_NORMAL_ARRAY );
	};
	void qbDrawCubeArrays( const Vec3f &c, const Vec3f &sz )
	{
		GLfloat vertices[QB_CUBE_FACES][QB_CUBE_FACE_VERTICES][3];
		for (int f = 0 ; f < QB_CUBE_FACES ; f++)
			for (int v = 0 ; v < QB_CUBE_FACE_VERTICES ; v++ )
			{
				vertices[f][v][0] = c.x+(__qbCubeVertices[f][v][0]*sz.x);
				vertices[f][v][1] = c.y+(__qbCubeVertices[f][v][1]*sz.y);
				vertices[f][v][2] = c.z+(__qbCubeVertices[f][v][2]*sz.z);
			}
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, 0, __qbCubeNormals );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, 0, __qbCubeTexes );
		glEnableClientState( GL_VERTEX_ARRAY );	 
		glVertexPointer( 3, GL_FLOAT, 0, vertices );
		for (int f = 0 ; f < QB_CUBE_FACES ; f++)
			glDrawElements( GL_TRIANGLE_STRIP, QB_CUBE_FACE_VERTICES, GL_UNSIGNED_BYTE, &(__qbCubeIndices[f]) );
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	 
		glDisableClientState( GL_NORMAL_ARRAY );
	};
	void qbDrawCubeArraysQuad( const Vec3f &c, const Vec3f &sz )
	{
		GLfloat vertices[QB_CUBE_FACES][QB_CUBE_FACE_VERTICES][3];
		for (int f = 0 ; f < QB_CUBE_FACES ; f++)
			for (int v = 0 ; v < QB_CUBE_FACE_VERTICES ; v++ )
			{
				vertices[f][v][0] = c.x+(__qbCubeVerticesQuads[f][v][0]*sz.x);
				vertices[f][v][1] = c.y+(__qbCubeVerticesQuads[f][v][1]*sz.y);
				vertices[f][v][2] = c.z+(__qbCubeVerticesQuads[f][v][2]*sz.z);
			}
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, 0, __qbCubeNormals );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, 0, __qbCubeTexesQuads );
		glEnableClientState( GL_VERTEX_ARRAY );	 
		glVertexPointer( 3, GL_FLOAT, 0, vertices );
		glDrawElements( GL_QUADS, QB_CUBE_INDICES, GL_UNSIGNED_BYTE, __qbCubeIndices );
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	 
		glDisableClientState( GL_NORMAL_ARRAY );
	};
	
	
	//
	// PUBLIC
	void drawCube( const Vec3f &center, const Vec3f &size )
	{
		//qb::qbDrawCubeImpl( center, size, false );
		//qb::qbDrawCubeDirect( center, size );
		//qb::qbDrawCubeArrays( center, size );
		//qb::qbDrawCubeArraysQuad( center, size );
		//return;
		
		glPushMatrix();
		gl::translate(center);
		gl::scale(size);
		//qb::qbDrawCubeDirect();
		qb::qbDrawCubeDirectDL();
		//qb::qbDrawCubeArraysDL();
		//qb::qbDrawCubeArraysDLQuads();
		//qb::qbDrawCubeArrays();
		//qb::qbDrawCubeArraysQuads();
		glPopMatrix();
	}
	
	void drawColorCube( const Vec3f &center, const Vec3f &size )
	{
		qb::qbDrawCubeImpl( center, size, false );
	}

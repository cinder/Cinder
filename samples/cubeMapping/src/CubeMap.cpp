/*
 *  CubeMap.cpp
 *  CubeMapping
 *
 *  Created by David Wicks on 2/27/09.
 *  Copyright 2009 The Barbarian Group. All rights reserved.
 *
 */

#ifndef	CUBE_MAP_H
#define CUBE_MAP_H
#include "CubeMap.h"

using ci::Surface8u;
using boost::shared_ptr;

CubeMap::CubeMap( GLsizei texWidth, GLsizei texHeight, const Surface8u &pos_x, const Surface8u &pos_y, const Surface8u &pos_z, const Surface8u &neg_x, const Surface8u &neg_y, const Surface8u &neg_z )
{	
	//create a texture object
	glGenTextures(1, &textureObject);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, textureObject);
	//assign the images to positions
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pos_x.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, neg_x.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pos_y.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, neg_y.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pos_z.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, neg_z.getData());
	//set filtering modes for scaling up and down
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void CubeMap::bindMulti( int pos )
{
	glActiveTexture(GL_TEXTURE0 + pos );
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, textureObject);
}

void CubeMap::bind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, textureObject);
}

void CubeMap::unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0 );
}

void CubeMap::enableFixedMapping()
{
	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP );
	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP );
	glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP );
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable( GL_TEXTURE_CUBE_MAP );
}

void CubeMap::disableFixedMapping()
{
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable( GL_TEXTURE_CUBE_MAP );
}


#endif

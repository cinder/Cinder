/*
 *  CubeMap.h
 *  CubeMapping
 *
 *  Created by David Wicks on 2/27/09.
 *  Copyright 2009 The Barbarian Group. All rights reserved.
 *
 */
 
#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

class CubeMap
{
	unsigned int textureObject;	
public:
	//this should be overloaded or generalized to allow different types of texture inputs
	CubeMap( GLsizei texWidth, GLsizei texHeight, const ci::Surface8u &pos_x, const ci::Surface8u &pos_y, const ci::Surface8u &pos_z, const ci::Surface8u &neg_x, const ci::Surface8u &neg_y, const ci::Surface8u &neg_z );
	void bind();
	void bindMulti( int loc );
	void unbind();
	static void enableFixedMapping();
	static void disableFixedMapping();
};
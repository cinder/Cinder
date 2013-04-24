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

#include "cinder/gl/gl.h"
#include "cinder/Color.h"

namespace cinder { namespace gl {

class Material {
 public:
	Material();
	Material( const ColorA &aAmbient, const ColorA &aDiffuse = ColorA( 0, 0, 0, 1 ), const ColorA &aSpecular = ColorA( 0, 0, 0, 1 ),
			float aShininess = 1.0f, const ColorA &aEmission = ColorA( 0, 0, 0, 1 ), GLenum aFace = GL_FRONT );

	void	apply() const;
	void	setAmbient( const ColorA &aAmbient ) { mAmbient = aAmbient; }
	void	setDiffuse( const ColorA &aDiffuse ) { mDiffuse = aDiffuse; }
	void	setSpecular( const ColorA &aSpecular ) { mSpecular = aSpecular; }
	void	setShininess( float aShininess ) { mShininess = aShininess; }
	void	setEmission( const ColorA &aEmission ) { mEmission = aEmission; }
	void	setFace( GLenum aFace ) { mFace = aFace; }

	ColorA	getAmbient() const { return mAmbient; }
	ColorA	getDiffuse() const { return mDiffuse; }
	ColorA	getSpecular() const { return mSpecular; }
	float	getShininess() const { return mShininess; }
	ColorA	getEmission() const { return mEmission; }
	GLenum	getFace() const { return mFace; }

 protected:
	ColorA			mAmbient;
	ColorA			mDiffuse;
	ColorA			mSpecular;
	float			mShininess;
	ColorA			mEmission;
	GLenum			mFace;
};

} } // namesapce

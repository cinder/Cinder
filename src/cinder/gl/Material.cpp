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

#include "cinder/gl/Material.h"

namespace cinder { namespace gl {

Material::Material()
	: mAmbient( Color( 1.0f, 0.5f, 0.25f ) ), mDiffuse( Color( 0, 0, 0 ) ), mSpecular( Color( 0, 0, 0 ) ), mShininess( 1.0f ), mEmission( Color( 0, 0, 0 ) ), mFace( GL_FRONT )
{
}

Material::Material( const Color &aAmbient, const Color &aDiffuse, const Color &aSpecular,
			float aShininess, const Color &aEmission, GLenum aFace )
	: mAmbient( aAmbient ), mDiffuse( aDiffuse ), mSpecular( aSpecular ), mShininess( aShininess ), mEmission( aEmission ), mFace( aFace )
{
}

void Material::apply() const
{
	glMaterialfv( mFace, GL_AMBIENT, mAmbient );
	glMaterialfv( mFace, GL_DIFFUSE, mDiffuse );
	glMaterialfv( mFace, GL_SPECULAR, mSpecular );
	glMaterialfv( mFace, GL_EMISSION, mEmission );
	glMaterialf( mFace, GL_SHININESS, mShininess );
}

} } // namespace
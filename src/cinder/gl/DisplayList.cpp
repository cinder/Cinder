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

#include "cinder/gl/DisplayList.h"
#include "cinder/gl/gl.h"

namespace cinder { namespace gl {

DisplayList::Obj::~Obj()
{
	if( mID > 0 )
		glDeleteLists( mID, 1 );		
}

DisplayList::DisplayList( GLint aMode )
	: mObj( shared_ptr<Obj>( new Obj ) )
{
	mObj->mModelMatrix.setToIdentity();
	mObj->mID = glGenLists( 1 );
	mObj->mMode = aMode;
}

void DisplayList::newList()
{
	glNewList( mObj->mID, mObj->mMode );
}

void DisplayList::endList()
{
	glEndList();
}

void DisplayList::draw() const
{
	if( mObj->mMaterial )
		mObj->mMaterial->apply();

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
		gl::multModelView( mObj->mModelMatrix );
		glCallList( mObj->mID );
	glPopMatrix();
}

} } // namespace cinder::gl

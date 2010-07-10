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

#include "cinder/Matrix.h"
#include "cinder/gl/Material.h"

namespace cinder {
namespace gl {

class DisplayList {
 protected:
	struct Obj;
	
 public:
 	DisplayList() {}
	DisplayList( GLint aMode/* = GL_COMPILE*/ );	

	void	newList();
	void	endList();
	int		getID() const { return mObj->mID; }
	
	void	draw() const;
	
	Matrix44f&			getModelMatrix() { return mObj->mModelMatrix; }
	const Matrix44f&	getModelMatrix() const { return mObj->mModelMatrix; }	
	
	void			setMaterial( const Material &aMaterial ) { mObj->mMaterial = shared_ptr<Material>( new Material( aMaterial ) ); }
	Material&		getMaterial() { return *(mObj->mMaterial); }

	//@{
	//! Emulates shared_ptr-like behavior
	typedef shared_ptr<Obj> DisplayList::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &DisplayList::mObj; }
	void reset() { mObj.reset(); }
	//@}

 protected:
 	struct Obj {
		Obj() : mID( 0 ) {}
		~Obj();
	
		GLuint					mID;
		GLint					mMode;
		Matrix44f				mModelMatrix;
		shared_ptr<Material>	mMaterial;		
	};
 
	shared_ptr<Obj>		mObj;
};

} // namespace gl
} // namespace cinder

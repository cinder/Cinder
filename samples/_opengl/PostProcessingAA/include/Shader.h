/*
 Copyright (c) 2014, Paul Houx - All rights reserved.
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

#include <exception>
#include <string>

#include "cinder/Filesystem.h"
#include "cinder/gl/GlslProg.h"

typedef std::shared_ptr<class Shader> ShaderRef;

class Shader
{
public:
	Shader( void );
	//! Creates a shader. Loads "[name].vert", "[name].geom" and/or "[name].frag" from the assets folder.
	Shader( const std::string& name );
	//! Creates a shader. Loads the supplied files from the assets folder.
	Shader( const std::string& name, const std::string& vert, const std::string& frag, const std::string& geom = std::string() );
	//!
	~Shader( void );

	static ShaderRef     create();
	static ShaderRef     create( const std::string& name );

	//Shader&              enableLiveEdit( bool enabled = true ) { mIsLiveEditable = enabled; return *this; }

	ci::gl::GlslProgRef  program() { return mGlslProg; }

	template<typename T>
	void                 uniform( const std::string& name, T value ) { mGlslProg->uniform( name, value ); }

protected:
	const ci::fs::path&  getPath() const;

	void                 load();
	std::string          parseShader( const ci::fs::path& path, bool optional = true, int level = 0 );
	std::string          parseVersion( const std::string& code );

private:
	std::string          mName;

	std::string          mVertexFile;
	std::string          mFragmentFile;
	std::string          mGeometryFile;

	bool                 mHasGeometryShader;
	bool                 mIsLiveEditable;

	mutable ci::fs::path mPath;

	unsigned int         mGlslVersion;
	ci::gl::GlslProgRef  mGlslProg;
};
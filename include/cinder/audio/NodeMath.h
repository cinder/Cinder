/*
Copyright (c) 2014, The Cinder Project

This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/audio/Node.h"
#include "cinder/audio/Param.h"
#include "cinder/CinderMath.h"

namespace cinder { namespace audio {

// Typedefs for shared_ptr's to all MathNode types
typedef std::shared_ptr<class MathNode>			MathNodeRef;
typedef std::shared_ptr<class AddNode>			AddNodeRef;
typedef std::shared_ptr<class SubtractNode>		SubtractNodeRef;
typedef std::shared_ptr<class MultiplyNode>		MultiplyNodeRef;
typedef std::shared_ptr<class DivideNode>		DivideNodeRef;

//! Base class for an arithmetic based Node.
class CI_API MathNode : public Node {
  public:
	//! Sets the current value to a constant \a value.
	void setValue( float value )	{ mParam.setValue( value ); }
	//! Returns the current value.
	float getValue() const			{ return mParam.getValue(); }

	//! Returns a pointer to the Param that can be used to animate the value.
	Param* getParam()				{ return &mParam; }

  protected:
	MathNode( float initialValue, const Format &format );

	Param	mParam;
};

//! Node for performing an addition operation on its input.
class CI_API AddNode : public MathNode {
  public:
	AddNode( const Format &format = Format() ) : MathNode( 0, format )	{}
	AddNode( float initialValue, const Format &format = Format() )	: MathNode( initialValue, format )	{}

protected:
	void process( Buffer *buffer ) override;
};

//! Node for performing a subtraction operation on its input.
class CI_API SubtractNode : public MathNode {
  public:
	SubtractNode( const Format &format = Format() ) : MathNode( 0, format )	{}
	SubtractNode( float initialValue, const Format &format = Format() )	: MathNode( initialValue, format )	{}

  protected:
	void process( Buffer *buffer ) override;
};

//! Node for performing a multiplication operation on its input.
class CI_API MultiplyNode : public MathNode {
  public:
	MultiplyNode( const Format &format = Format() ) : MathNode( 0, format )	{}
	MultiplyNode( float initialValue, const Format &format = Format() )	: MathNode( initialValue, format )	{}

  protected:
	void process( Buffer *buffer ) override;
};

//! Node for performing a division operation on its input.
class CI_API DivideNode : public MathNode {
  public:
	DivideNode( const Format &format = Format() ) : MathNode( 0, format )	{}
	DivideNode( float initialValue, const Format &format = Format() )	: MathNode( initialValue, format )	{}

  protected:
	void process( Buffer *buffer ) override;
};

} } // namespace cinder::audio

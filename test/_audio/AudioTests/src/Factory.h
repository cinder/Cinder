/*
 Copyright (c) 2014-15, Richard Eakin - All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided
 that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
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

#include "cinder/Exception.h"

#include <map>
#include <memory>
#include <functional>

namespace mason {

//! Exception thrown when Factory cannot build an object for the requested key
class FactoryException : public cinder::Exception {
  public:
	FactoryException( const std::string &key )
		: Exception( "no Builder for key: " + key )
	{}
};

//! Allows for constructing shared_ptr<T> objects by string, passing \a Args to T's constructor.
template <typename T, typename... Args>
class Factory {
  public:
	//! Register a builder of type \a Y, that can be built by calling `build( key )`. \note Y must inherit from T
	template<typename Y>
	void registerBuilder( const std::string &key )
	{
		static_assert( std::is_base_of<T, Y>::value, "Y must inherit from T" );

		mBuilderMap[key] = Builder<Y>();
	}

	//! Builds an object that inherits from T, which is associated with \a key via registerBuilder(), returning it in a shared_ptr.
	std::shared_ptr<T>	build( const std::string &key, const Args&... args )
	{
		auto builderFnIt = mBuilderMap.find( key );
		if( builderFnIt == mBuilderMap.end() )
			throw FactoryException( key );

		return builderFnIt->second( args... );
	}

	std::vector<std::string>	getAllKeys() const
	{
		std::vector<std::string> result;
		for( const auto &mp : mBuilderMap ) {
			result.push_back( mp.first );
		}

		return result;
	}

  private:
	// Helper functor for building objects registered with Factory and returning them in a shared_ptr.
	template<typename Y>
	struct Builder {
		//! returns a shared_ptr of a newly constructed T object
		std::shared_ptr<T> operator()( const Args&... args )
		{
			return std::shared_ptr<T>( new Y( args... ) );
		}
	};

	typedef std::function<std::shared_ptr<T>( Args... )>		BuilderFn;

	std::map<std::string, BuilderFn> mBuilderMap;
};

} // namespace mason

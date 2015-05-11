/*
 Copyright (c) 2015, The Cinder Project

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

#include "cinder/Signals.h"

namespace cinder { namespace signals {

Connection::Connection()
	: mLink( nullptr ), mPriority( 0 )
{
}

Connection::Connection( const std::shared_ptr<detail::Disconnector> &disconnector, detail::SignalLinkBase *link, int priority )
	: mDisconnector( disconnector ), mLink( link ), mPriority( priority )
{
}

bool Connection::disconnect()
{
	auto disconnector = mDisconnector.lock();
	if( disconnector ) {
		auto link = mLink;
		mLink = nullptr;
		return disconnector->disconnect( link, mPriority );
	}

	return false;
}

bool Connection::isConnected() const
{
	return ! mDisconnector.expired() && mLink;
}

void Connection::disable()
{
	if( mLink ) {
		mLink->disable();
	}
}

void Connection::enable()
{
	if( mLink ) {
		mLink->enable();
	}
}

bool Connection::isEnabled() const
{
	if( mLink ) {
		return mLink->isEnabled();
	}
	return false;
}

ScopedConnection::ScopedConnection()
{
}

ScopedConnection::~ScopedConnection()
{
	disconnect();
}

ScopedConnection::ScopedConnection( const Connection &other )
	: Connection( other )
{
}

ScopedConnection::ScopedConnection( ScopedConnection &&other )
	: Connection( std::move( other ) )
{
}

ScopedConnection::ScopedConnection( Connection &&other )
	: Connection( std::move( other ) )
{
}

ScopedConnection& ScopedConnection::operator=( const Connection &rhs )
{
	disconnect();
	Connection::operator=( rhs );
	return *this;
}

ScopedConnection& ScopedConnection::operator=( ScopedConnection &&rhs )
{
	Connection::operator=( std::move( rhs ) );
	return *this;
}

namespace detail {

Disconnector::Disconnector( SignalBase *signal )
	: mSignal( signal )
{
}

bool Disconnector::disconnect( SignalLinkBase *link, int priority )
{
	return mSignal->disconnect( link, priority );
}

} } } // namespace cinder::signals::detail
